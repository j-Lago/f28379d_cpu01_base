/*
 * control.cpp
 *
 *  Created on: Aug 24, 2022
 *      Author: j-Lago
 */

#include "control.h"

volatile float fan_duty = 0.6f; //velocidade do fan quando inv está ligado
volatile float fan_idle = 0.2; //velocidade do fan quando inv está desligado

volatile float i_dq_p_ref [2] = {7.0f, 0.0f}; //refs correntes seq+
volatile float i_dq_n_ref [2] = {0.0f, 0.0f}; //refs correntes seq-
volatile float kn = 1.5f;   // ganho compensação seq-
volatile float w0L = 377.0f * 0.001f * 3.0f*0;  //ganho desacoplamento cruzado

bool en_seqn = false;
bool auto_seqn = true;
Togi togi_v_al;
Togi togi_v_be;
Togi togi_i_al;
Togi togi_i_be;


//PI ctrl_i_dp;
//PI ctrl_i_qp;
//PI ctrl_i_dn;
//PI ctrl_i_qn;


TFZ4 ctrl_i_dp;
TFZ4 ctrl_i_qp;
TFZ4 ctrl_i_dn;
TFZ4 ctrl_i_qn;

TFZ5 notch_vdn;
TFZ5 notch_vqn;

pll_s pll;
//pll_f pll;

float cis_n[2]; // [cos, -sin]
float cis[2];   // [cos,  sin]

float m_dq_p0 [2] = {0,0};
float m_dq_n0 [2] = {0,0};

float v_dq_n_base [2] = {0,0};
bool set_seqn_base = false;

float v_ab[2];      // [va, vb]
float v_albe[2];    // [valpha, vbeta]
float v_albe_p[2];
float v_albe_n[2];
float v_dq_p[2];
float v_dq_n[2];

float i_uv[2];
float i_albe[2];
float i_albe_p[2];
float i_albe_n[2];
float i_dq_p[2];
float i_dq_n[2];




float m_dq_p[2];
float m_dq_n[2];
float m_albe_p[2];
float m_albe_n[2];
float m_albe[2];
float m_abc[3];     // [ma, mb, mc] (para modulador PWM)


float inv_vdc_2 = 0.0f;

void control_setup()
{
    float fa = 20000.0f;

    togi_set(&togi_v_al, fa, 1.4f, 0.2f);
    togi_set(&togi_v_be, fa, 1.4f, 0.2f);
    togi_set(&togi_i_al, fa, 1.4f, 0.2f);
    togi_set(&togi_i_be, fa, 1.4f, 0.2f);

    const float num_p[] = {8.0883725,-23.9831351,23.8050353,-7.90929123};
    const float den_p[] = {1,-2.98692774,2.98661463,-0.999686892};
    const float num_n[] = {1.98242227,-5.93381841,5.93165143,-1.98024394};
    const float den_n[] = {1,-2.9756811,2.95703401,-0.981352917};
    const float num_6[] = {0.995024406,-3.97092525,5.95180483,-3.97092525,0.995024406};
    const float den_6[] = {1,-3.98084111,5.95179242,-3.9610094,0.990061221};


    tfz4_set(&ctrl_i_dp, num_p, den_p);
    tfz4_set(&ctrl_i_qp, num_p, den_p);
    tfz4_set(&ctrl_i_dn, num_n, den_n);
    tfz4_set(&ctrl_i_qn, num_n, den_n);

    tfz5_set(&notch_vdn, num_6, den_6);
    tfz5_set(&notch_vqn, num_6, den_6);

    pll_set(&pll, fa, 377.0f, 0.40824829046386301636621401245098f, 0.03f );
    //pllf_set(&pll, fa, 377.0f, 0.40824829046386301636621401245098f, 0.03f, num_pll, den_pll );

}

void control()
{
    //--Variáveis de controle----------------------------------------------------------------------------------




    //--Variáveis de entrada-----------------------------------------------------------------------------------
    v_ab[0] = (2.0f * adc.vrs - adc.vts ) *0.33333333333333333f;
    v_ab[1] = -v_ab[0] - ((-adc.vrs+2.0f * adc.vts)*0.33333333333333333f);
    i_uv[0] = adc.iu;
    i_uv[1] = adc.iv;


    //--Malhas de controle-------------------------------------------------------------------------------------

    // tensao seq. pos e pll
    // tensao seq. pos
    transform_ab_albe(v_albe, v_ab);
    togi_step(&togi_v_al, pll.wf, v_albe[0]);
    togi_step(&togi_v_be, pll.wf, v_albe[1]);
    transform_albe_pos(v_albe_p, togi_v_al.out, togi_v_be.out);
    transform_albe_neg(v_albe_n, togi_v_al.out, togi_v_be.out);
    transform_albe_dq(v_dq_p, cis, v_albe_p);

    pll_step(&pll, v_dq_p[1]);
    //pllf_step(&pll, v_dq_p[1]);

    cis[0] = cos(pll.th);
    cis[1] = sin(pll.th);
    cis_n[0] = cis[0];
    cis_n[1] = -cis[1];

    // tensao de sequência negativa e referência de corrente seq-
    transform_albe_dq(v_dq_n, cis_n, v_albe_n);

    if(set_seqn_base){
        set_seqn_base = false;
        v_dq_n_base[0] = v_dq_n[0];
        v_dq_n_base[1] = v_dq_n[1];
    }

    if(auto_seqn){
        if(en_seqn){
            tfz5_step(&notch_vdn,  kn * (v_dq_n[1] - v_dq_n_base[1]));
            tfz5_step(&notch_vqn, -kn * (v_dq_n[0] - v_dq_n_base[0]));
            i_dq_n_ref[0] = notch_vdn.out;
            i_dq_n_ref[1] = notch_vqn.out;
            //i_dq_n_ref[0] =  kn * (v_dq_n[1] - v_dq_n_base[1]);
            //i_dq_n_ref[1] = -kn * (v_dq_n[0] - v_dq_n_base[0]);
        }
        else{
            i_dq_n_ref[0] = 0.0f;
            i_dq_n_ref[1] = 0.0f;
            tfz5_reset(&notch_vdn);
            tfz5_reset(&notch_vqn);
        }
    }

    inv_vdc_2 = 0.0f;
        if(adc.vdc != 0 )
            inv_vdc_2 = -2.0f / adc.vdc;

    // malha corrente seq+
    transform_ab_albe(i_albe, i_uv);
    togi_step(&togi_i_al, pll.wf, i_albe[0]);
    togi_step(&togi_i_be, pll.wf, i_albe[1]);
    transform_albe_neg(i_albe_n, togi_i_al.out, togi_i_be.out);

    i_albe_p[0] = i_albe[0] - i_albe_n[0];
    i_albe_p[1] = i_albe[1] - i_albe_n[1];
    transform_albe_dq(i_dq_p, cis, i_albe_p);

    //malha corrente seq-
    transform_albe_dq(i_dq_n, cis_n, i_albe_n);


    if(pwm.en){

        //pi_step(&ctrl_i_dp, i_dq_p_ref[0] - i_dq_p[0]);
        //pi_step(&ctrl_i_qp, i_dq_p_ref[1] - i_dq_p[1]);
        //pi_step(&ctrl_i_dn, i_dq_n_ref[0] - i_dq_n[0]);
        //pi_step(&ctrl_i_qn, i_dq_n_ref[1] - i_dq_n[1]);

        tfz4_step(&ctrl_i_dp, i_dq_p_ref[0] - i_dq_p[0]);
        tfz4_step(&ctrl_i_qp, i_dq_p_ref[1] - i_dq_p[1]);
        tfz4_step(&ctrl_i_dn, i_dq_n_ref[0] - i_dq_n[0]);
        tfz4_step(&ctrl_i_qn, i_dq_n_ref[1] - i_dq_n[1]);
    }
    else{
        //pi_reset(&ctrl_i_dp);
        //pi_reset(&ctrl_i_qp);
        //pi_reset(&ctrl_i_dn);
        //pi_reset(&ctrl_i_qn);

        tfz4_reset(&ctrl_i_dp);
        tfz4_reset(&ctrl_i_qp);
        tfz4_reset(&ctrl_i_dn);
        tfz4_reset(&ctrl_i_qn);

        m_dq_p0[0] = v_dq_p[0] * inv_vdc_2; // + m_dq_n[0];
        m_dq_p0[1] = v_dq_p[1] * inv_vdc_2; // + m_dq_n[1];
    }



    m_dq_p[0] = (ctrl_i_dp.out - w0L * i_dq_p[1] ) * inv_vdc_2 + m_dq_p0[0];
    m_dq_p[1] = (ctrl_i_qp.out + w0L * i_dq_p[0] ) * inv_vdc_2 + m_dq_p0[1];
    transform_dq_albe(m_albe_p, cis, m_dq_p);

    m_dq_n[0] = (ctrl_i_dn.out + w0L * i_dq_n[1]) * inv_vdc_2 + m_dq_n0[0];
    m_dq_n[1] = (ctrl_i_qn.out - w0L * i_dq_n[0]) * inv_vdc_2 + m_dq_n0[1];
    transform_dq_albe(m_albe_n, cis_n, m_dq_n);

    m_albe[0] = m_albe_p[0] + m_albe_n[0];
    m_albe[1] = m_albe_p[1] + m_albe_n[1];
    transform_albe_abc(m_abc, m_albe);


    pwm.setComps(m_abc);



    if(pwm.en)
        fan.dutycicle = fan_duty;
    else
        fan.dutycicle = fan_idle;


}

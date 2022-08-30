/*
 * control.cpp
 *
 *  Created on: Aug 24, 2022
 *      Author: j-Lago
 */

#include "control.h"

volatile float fan_duty = 0.6f; //velocidade do fan
volatile float i_dq_p_ref [2] = {3.0f, 0.0f}; //refs correntes seq+
volatile float i_dq_n_ref [2] = {0.0f, 0.0f}; //refs correntes seq-
volatile float kn = 4.0f;   // ganho compensação seq-
volatile float w0L = 377.0f * 0.001f * 3.0f;  //ganho desacoplamento cruzado

Togi togi_v_al;
Togi togi_v_be;
Togi togi_i_al;
Togi togi_i_be;
PI pi_i_dp;
PI pi_i_qp;
PI pi_i_dn;
PI pi_i_qn;
pll_s pll;

void control_setup()
{
    float fa = 20000.0f;

    togi_set(&togi_v_al, fa, 1.4f, 0.2f);
    togi_set(&togi_v_be, fa, 1.4f, 0.2f);
    togi_set(&togi_i_al, fa, 1.4f, 0.2f);
    togi_set(&togi_i_be, fa, 1.4f, 0.2f);
    pi_set(&pi_i_dp, fa, 8.1649f, 0.0072f,  0.0f, 0.0f);
    pi_set(&pi_i_qp, fa, 8.1649f, 0.0072f,  0.0f, 0.0f);
    pi_set(&pi_i_dn, fa, 4.0f, 0.05f,  0.0f, 0.0f);
    pi_set(&pi_i_qn, fa, 4.0f, 0.05f,  0.0f, 0.0f);
    pll_set(&pll, fa, 377.0f, 0.40824829046386301636621401245098f, 0.03f );

}

void control()
{
    //--Variáveis de controle----------------------------------------------------------------------------------

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

    float cis[2];   // [cos,  sin]
    float cis_n[2]; // [cos, -sin]

    float m_dq_p0 [2] = {0,0};
    float m_dq_n0 [2] = {0,0};
    float m_dq_p[2];
    float m_dq_n[2];
    float m_albe_p[2];
    float m_albe_n[2];
    float m_albe[2];
    float m_abc[3];     // [ma, mb, mc] (para modulador PWM)


    //--Variáveis de entrada-----------------------------------------------------------------------------------
    v_ab[0] = (2.0f * adc.vrs - adc.vts ) *0.33333333333333333f;
    v_ab[1] = -v_ab[0] - ((-adc.vrs+2.0f * adc.vts)*0.33333333333333333f);
    i_uv[0] = adc.iu;
    i_uv[1] = adc.iv;


    //--Malhas de controle-------------------------------------------------------------------------------------

    // tensao seq. pos e pll
    transform_ab_albe(v_albe, v_ab);
    togi_step(&togi_v_al, pll.wf, v_albe[0]);
    togi_step(&togi_v_be, pll.wf, v_albe[1]);
    transform_albe_pos(v_albe_p, togi_v_al.out, togi_v_be.out);
    transform_albe_neg(v_albe_n, togi_v_al.out, togi_v_be.out);
    transform_albe_dq(v_dq_p, cis, v_albe_p);

    pll_step(&pll, v_dq_p[1]);
    cis[0] = cos(pll.th);
    cis[1] = sin(pll.th);
    cis_n[0] = cis[0];
    cis_n[1] = -cis[1];

    // tensao de sequência negativa e referência de corrente seq-
    transform_albe_dq(v_dq_n, cis_n, v_albe_n);
    i_dq_n_ref[0] = kn * v_dq_n[1];
    i_dq_n_ref[1] = -kn * v_dq_n[0];

    if(pwm.en){
        // malha corrente seq+
        transform_ab_albe(i_albe, i_uv);
        togi_step(&togi_i_al, pll.wf, i_albe[0]);
        togi_step(&togi_i_be, pll.wf, i_albe[1]);
        transform_albe_neg(i_albe_n, togi_i_al.out, togi_i_be.out);

        i_albe_p[0] = i_albe[0] - i_albe_n[0];
        i_albe_p[1] = i_albe[1] - i_albe_n[1];
        transform_albe_dq(i_dq_p, cis, i_albe_p);
        pi_step(&pi_i_dp, i_dq_p_ref[0] - i_dq_p[0]);
        pi_step(&pi_i_qp, i_dq_p_ref[1] - i_dq_p[1]);
        m_dq_p[0] = pi_i_dp.out + m_dq_p0[0] - w0L * i_dq_p[1];
        m_dq_p[1] = pi_i_qp.out + m_dq_p0[1] + w0L * i_dq_p[0];
        transform_dq_albe(m_albe_p, cis, m_dq_p);

        //malha corrente seq-
        transform_albe_dq(i_dq_n, cis_n, i_albe_n);
        pi_step(&pi_i_dn, i_dq_n_ref[0] - i_dq_n[0]);
        pi_step(&pi_i_qn, i_dq_n_ref[1] - i_dq_n[1]);
        m_dq_n[0] = pi_i_dn.out + m_dq_n0[0] + w0L * i_dq_n[1];
        m_dq_n[1] = pi_i_qn.out + m_dq_n0[1] - w0L * i_dq_n[0];
        transform_dq_albe(m_albe_n, cis_n, m_dq_n);

        m_albe[0] = m_albe_p[0] + m_albe_n[0]*0;
        m_albe[1] = m_albe_p[1] + m_albe_n[1]*0;
        transform_albe_abc(m_abc, m_albe);
    }
    else
    {
        m_dq_p0[0] = v_dq_p[0] + m_dq_n[0];
        m_dq_p0[1] = v_dq_p[1] + m_dq_n[1];
    }

    pwm.setComps(m_abc);

    if(pwm.en)
        fan.dutycicle = fan_duty;
    else
        fan.dutycicle = 0.0f;


}

/*
 * controle.cpp
 *
 *  Created on: Aug 22, 2022
 *      Author: j-Lago
 */

#include "F28x_Project.h"
#include "globals.h"
#include "pwm.h"
#include "cla.h"
#include "adc.h"
#include "f_controle.h"

//#include "signals.h"

static const float sqrt3 = 1.7320508075688772935274463415059f;

static const float fa = 20000.0f; //Frequência de amostragem
static const float Ta = 1.0 / fa; //Período de amostragem
static const float KSQRT3_6 = 0.28867513459481288225457439025098f;
static const float W0 =  376.99111843077518861551720599354f;
//static const float PI = 3.1415926535897932384626433832795f;

//Ganhos dos controladores da simula��o disponibilizada:
static const float K1_PLL = 0.3000f;
static const float K2_PLL = -0.2995f;
static const float K1_PISN = 0.0600f;
static const float K2_PISN = -0.0594f;
static const float K1_PISP = 0.0600;
static const float K2_PISP = 0.0594f;
static const float K_SOGI = 0.7f;
float compseqn = 0.01f;

// velocidade do fan
float duty = 0.6;

//Vari�veis para o SOGI:
float v0[] = {0, 0, 0};
float v90[] = {0, 0, 0};
float v0_[] = {0, 0, 0};
float v90_[] = {0, 0, 0};
float vg[] = {0,0,0};

//
float vsn[] = {0, 0, 0}; //Tens�o de seq- //Tens�o obtida pelo SOGI
float vdn, vqn = 0; //Tens�o direta(d)/ quadratura(q) de seq-
float idp, iqp = 0; //Corrente direta(d)/ quadratura(q) de seq+

//controladores
float PI_dn, PI_qn, PI_dn_, PI_qn_ = 0; //Sinal de chegada ao PI do controle de seq-
float PI_dp, PI_qp, PI_dp_, PI_qp_ = 0; //Sinal de chegada ao PI do controle de seq+
float dn, qn, dn_, qn_ = 0; //Componente direta(d)/ quadratura(q) de sa�da do controle de seq-
float dp, qp, dp_, qp_ = 0; //Componente direta(d)/ quadratura(q) de sa�da do controle de seq+
float an, bn, cn = 0; //Sinal modulador de seq- do PWM
float ap, bp, cp = 0; //Sinal modulador de seq+ do PWM
float aw, aw_ = 0; //Sinal anterior ao da frequ�ncia no PLL

float int1[] = {0, 0, 0}; //Integrador para v0 do SOGI
float int1_[] = {0, 0, 0};
float int2[] = {0, 0, 0}; //Integrador para v90 do SOGI
float int2_[] = {0, 0, 0};

float vq, vd, vq_, ia, ib, ic = 0.0;
float th, th_=0.0; //Theta inicial.
float w, w_ = 60.0; //Frequ�ncia inicial.

float ipref = 2.5f; //Referencial de corrente a ser adicionada no controle de seq+
float iqref = 0.0f; //

float vdnref = 0.0f; //Referencial de corrente a ser adicionada no controle de seq+
float vqnref = 0.0f; //

float vdc; //Tens�o no link CC
bool enn = false;
bool enp = false; //enn: Liga controle seq- //enp: Liga controle seq+

float m_abc[3] = {0.0f, 0.0f, 0.0f};
float m_dq[2] = {1.0f, 0.0f};

bool zero_cross = false;
bool semiciclo = false;


void pv_control(void){
    float sin_th = sin(th);
    float cos_th = cos(th);

    //PLL:
    //Desconsiderando as componentes de seq0:
    vd =  -adc.vts*sqrt(3.0f)/3.0f*sin_th + (2.0f*adc.vrs-adc.vts)/3.0f*cos_th;
    vq =   adc.vts*sqrt(3.0f)/3.0f*cos_th + (2.0f*adc.vrs-adc.vts)/3.0f*sin_th;

    //PI
    aw = K1_PLL*vq + K2_PLL*vq_ + aw_;
    w = aw+W0;

    th = (w + w_)*Ta/2.0 + th_; //Demonstrado na planilha
    if(th > 2*fcla_PI){
        th -= 2*fcla_PI;
        //logic3.set();
    }else{
        //logic3.clear();
    }




    //atualiza modulador

    float cos_2 = 0.5f * cos_th;
    float sin_2 = 0.5f * sin_th;
    float cossqrt3_2 = 0.86602540378443864676372317075294f * cos_th;
    float sinsqrt3_2 = 0.86602540378443864676372317075294f * sin_th;



    //-------------------------------------------------------------------------------------------------
    //Controle seq+:
    //Transformada de park seq+:
    ia = -adc.iu;
    ib = -adc.iv;
    ic = -ia -ib;
    enp = pwm.en;


    idp =  (-ic+ib)*sqrt(3.0f)/3.0f*sin_th + (2.0f*ia-ib-ic)/3.0f*cos_th;
    iqp = -(-ic+ib)*sqrt(3.0f)/3.0f*cos_th + (2.0f*ia-ib-ic)/3.0f*sin_th;



    if(!enp){ //Desativa a inje��o de corrente de seq+ quando enp estiver em zero
        dp = 0.8;
        qp = -0.22;
        PI_dp = 0.0f;
        PI_qp = 0.0f;
    }else{
        //PI:
        PI_dp = ipref-idp;
        PI_qp = iqref-iqp;
        dp = K1_PISP*PI_dp  - K2_PISP*PI_dp_ + dp_;
        qp = K1_PISP*PI_qp  - K2_PISP*PI_qp_ + qp_;
    }


    //-------------------------------------------------------------------------------------------------
    //Desconsiderando as componentes de seq0:
    vg[0] = -1.0f/3.0f*adc.vts + 2.0/3.0*adc.vrs;
    vg[1] = -1.0f/3.0f*adc.vts - 1.0/3.0*adc.vrs;
    vg[2] =  2.0f/3.0f*adc.vts - 1.0/3.0*adc.vrs;

    //SOGI
    for(int i=0; i<3; i++){

        int1[i] = ((vg[i] - v0_[i])*K_SOGI - v90_[i])* w; //Demonstrado na planilha
        v0[i] = (int1[i] + int1_[i])*Ta/2.0 + v0_[i];
        int2[i] = v0[i]*w;
        v90[i] = (int2[i] + int2_[i])*Ta/2.0 + v90_[i];

        //Atualiza vari�veis:
        int1_[i] = int1[i];
        int2_[i] = int2[i];
        v0_[i] = v0[i];
        v90_[i] = v90[i];
    }

    //Controle seq-:
    //Componente de seq- da tens�o:
    //**************************Aparentemente no Artigo est� invertido seq+ com seq-**************************
    vsn[0] = -(v90[2]-v90[1])*KSQRT3_6 + (v0[0] - 0.5*v0[1] - 0.5*v0[2])*1.0/3.0; //Demonstrado na planilha
    vsn[1] = -(v90[0]-v90[2])*KSQRT3_6 + (v0[1] - 0.5*v0[0] - 0.5*v0[2])*1.0/3.0;
    vsn[2] = -(v90[1]-v90[0])*KSQRT3_6 + (v0[2] - 0.5*v0[1] - 0.5*v0[0])*1.0/3.0;

    //Transformada de park seq-:
    vdn =  (-vsn[2]+vsn[1])*sqrt(3.0)/3.0*sin(-th) + (2.0*vsn[0]-vsn[1]-vsn[2])/3.0*cos(-th);
    vqn = -(-vsn[2]+vsn[1])*sqrt(3.0)/3.0*cos(-th) + (2.0*vsn[0]-vsn[1]-vsn[2])/3.0*sin(-th);
    //-------------------------------------------------------------------------------------------------



    if(!enn){ //Desativa a inje��o de corrente de seq- quando enn estiver em zero
            dn = 0.0f;
            qn = 0.0f;
            PI_dn = 0.0f;
            PI_qn = 0.0f;
        }else{
            //PI: seq-
            PI_dn = vdnref-vdn;
            PI_qn = vqnref-vqn;
            dn = compseqn*PI_dn;
            qn = compseqn*PI_qn;
        }


    //-------------------------------------------------------------------------------------------------
    // atualiza modulador
    if(enp){ //true = manual; false = automatico
        m_dq[0] = dp;
        m_dq[1] = qp;
    }else{
        m_dq[0] = cla_dq[0];
        m_dq[1] = cla_dq[1];
    }


    //Transformada inversa de park seq+:
    ap = cos_th*m_dq[0]              + sin_th*m_dq[1];
    bp = (-cos_2+sinsqrt3_2)*m_dq[0] - (sin_2 + cossqrt3_2)*m_dq[1];
    cp = (-cos_2-sinsqrt3_2)*m_dq[0] + (-sin_2 + cossqrt3_2)*m_dq[1];

    //Transformada inversa de park seq-:
    an =  qn * (-sin_th) +  dn * cos_th;
    bn = ((- qn + sqrt3 * dn) * (-sin_th) - (sqrt3 * qn +  dn) * cos_th)*0.5f;
    cn = ((- qn - sqrt3 * dn) * (-sin_th) + (sqrt3 * qn -  dn) * cos_th)*0.5f;


    m_abc[0] = ap + an;
    m_abc[1] = bp + bn;
    m_abc[2] = cp + cn;

    pwm.setComps(m_abc);

    if(pwm.en)
            fan.dutycicle = duty;
        else
            fan.dutycicle = 0.0f;
    //-------------------------------------------------------------------------------------------------


    //Atualiza vari�veis:
    //Todas as var�aveis que tem a sua correspondente no intante anterior dever� ser atualizada
    w_ = w; //Frequ�ncia
    th_ = th; //Theta
    vq_ = vq; //Tens�o da rede em quadratura
    aw_ = aw; //Sinal anterior ao da frequ�ncia no PLL
    dn_ = dn; //Componente quadratura(q) de sa�da do controle de seq-
    qn_ = qn; //Componente direta(d) de sa�da do controle de seq-
    dp_ = dp; //Componente quadratura(q) de sa�da do controle de seq+
    qp_ = qp; //Componente direta(d) de sa�da do controle de seq+
    PI_dn_ = PI_dn; //Sinal de chegada ao PI do controle de seq-
    PI_qn_ = PI_qn; //Sinal de chegada ao PI do controle de seq-
    PI_dp_ = PI_dp; //Sinal de chegada ao PI do controle de seq+
    PI_qp_ = PI_qp; //Sinal de chegada ao PI do controle de seq+
    vq_ = vq; //Tens�o da rede em quadratura
    aw_ = aw; //Sinal anterior ao da frequ�ncia no PLL
}




/*
 * controle.cpp
 *
 *  Created on: May 31, 2022
 *      Author: jacksonlago
 */

#include "F28x_Project.h"
#include "globals.h"
#include "pwm.h"
#include "cla.h"
#include "adc.h"
#include "controle.h"

static const float fa = 20000.0f; //Frequência de amostragem
static const float Ta = 1.0 / fa; //Período de amostragem
static const float KSQRT3_6 = 0.28867513459481288225457439025098f;
static const float W0 =  376.99111843077518861551720599354f;
//static const float PI = 3.1415926535897932384626433832795f;

//Ganhos dos controladores da simulação disponibilizada:
static const float K1_PLL = 0.3000f;
static const float K2_PLL = -0.2995f;
static const float K1_PISN = 0.0600f;
static const float K2_PISN = -0.0594f;
static const float K1_PISP = 0.0600;
static const float K2_PISP = 0.0594f;
static const float K_SOGI = 0.7f;


//Variáveis para o SOGI:
float v0[] = {0, 0, 0};
float v90[] = {0, 0, 0};
float v0_[] = {0, 0, 0};
float v90_[] = {0, 0, 0};

//
float vsn[] = {0, 0, 0}; //Tensão de seq- //Tensão obtida pelo SOGI
float vdn, vqn = 0; //Tensão direta(d)/ quadratura(q) de seq-
float idp, iqp = 0; //Corrente direta(d)/ quadratura(q) de seq+

//controladores
float PI_dn, PI_qn, PI_dn_, PI_qn_ = 0; //Sinal de chegada ao PI do controle de seq-
float PI_dp, PI_qp, PI_dp_, PI_qp_ = 0; //Sinal de chegada ao PI do controle de seq+
float dn, qn, dn_, qn_ = 0; //Componente direta(d)/ quadratura(q) de saída do controle de seq-
float dp, qp, dp_, qp_ = 0; //Componente direta(d)/ quadratura(q) de saída do controle de seq+
float an, bn, cn = 0; //Sinal modulador de seq- do PWM
float ap, bp, cp = 0; //Sinal modulador de seq+ do PWM
float aw, aw_ = 0; //Sinal anterior ao da frequência no PLL

float int1[] = {0, 0, 0}; //Integrador para v0 do SOGI
float int1_[] = {0, 0, 0};
float int2[] = {0, 0, 0}; //Integrador para v90 do SOGI
float int2_[] = {0, 0, 0};

float vq, vd, vq_, ia, ib, ic = 0.0;
float th, th_=0.0; //Theta inicial.
float w, w_ = 60.0; //Frequência inicial.

float ipref = 2.5f; //Referencial de corrente a ser adicionada no controle de seq+
float iqref = 0.0f; //

float vdc; //Tensão no link CC
bool enn = false;
bool enp = false; //enn: Liga controle seq- //enp: Liga controle seq+

float m_abc[3] = {0.0f, 0.0f, 0.0f};
float m_dq[2] = {1.0f, 0.0f};

bool zero_cross = false;
bool semiciclo = false;


void controle(void){
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
    if(th > 2*PI){
        th -= 2*PI;
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



    if(!enp){ //Desativa a injeção de corrente de seq+ quando enp estiver em zero
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





    //-------------------------------------------------------------------------------------------------
    // atualiza modulador
    if(enp){ //true = manual; false = automatico
        m_dq[0] = dp;
        m_dq[1] = qp;
    }else{
        m_dq[0] = cla_dq[0];
        m_dq[1] = cla_dq[1];
    }

    m_abc[1] = cos_th*m_dq[0]              + sin_th*m_dq[1];
    m_abc[0] = (-cos_2+sinsqrt3_2)*m_dq[0] - (sin_2 + cossqrt3_2)*m_dq[1];
    m_abc[2] = (-cos_2-sinsqrt3_2)*m_dq[0] + (-sin_2 + cossqrt3_2)*m_dq[1];
    pwm.setComps(m_abc);
    //-------------------------------------------------------------------------------------------------


    //Atualiza variáveis:
    //Todas as varíaveis que tem a sua correspondente no intante anterior deverá ser atualizada
    w_ = w; //Frequência
    th_ = th; //Theta
    vq_ = vq; //Tensão da rede em quadratura
    aw_ = aw; //Sinal anterior ao da frequência no PLL
    dn_ = dn; //Componente quadratura(q) de saída do controle de seq-
    qn_ = qn; //Componente direta(d) de saída do controle de seq-
    dp_ = dp; //Componente quadratura(q) de saída do controle de seq+
    qp_ = qp; //Componente direta(d) de saída do controle de seq+
    PI_dn_ = PI_dn; //Sinal de chegada ao PI do controle de seq-
    PI_qn_ = PI_qn; //Sinal de chegada ao PI do controle de seq-
    PI_dp_ = PI_dp; //Sinal de chegada ao PI do controle de seq+
    PI_qp_ = PI_qp; //Sinal de chegada ao PI do controle de seq+
    vq_ = vq; //Tensão da rede em quadratura
    aw_ = aw; //Sinal anterior ao da frequência no PLL
}


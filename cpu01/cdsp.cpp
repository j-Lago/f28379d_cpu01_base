/*
 * cdsp.cpp
 *
 *  Created on: Aug 24, 2022
 *      Author: jacks
 */

#include "cdsp.h"


//--Integrator---------------------------------------------------------------------------------------------
void integrator_set(struct Integrator* self, float sample_frequency, float in0, float out0)
{
    self->k = 1.0 / (2.0 * sample_frequency);
    self->in = in0;
    self->out = out0;
}

void integrator_step(struct Integrator* self, float input)
{
    self->out = (input + self->in) * self->k + self->out;
    self->in = input;
}
//---------------------------------------------------------------------------------------------------------



//--Integrator3--------------------------------------------------------------------------------------------
void integrator3_set(struct Integrator3* self, float sample_frequency)
{
    self->k = 1.0 / (12.0 * sample_frequency);
    self->mem[0] = 0.0f;
    self->mem[1] = 0.0f;
    self->mem[2] = 0.0f;
    self->out = 0.0f;
}

void integrator3_step(struct Integrator3* self, float input)
{
    float ans = self->k * input + self->mem[0];

    self->out = self->mem[0] * 23.0f - self->mem[1] * 16.0f + self->mem[2] * 5.0f;

    self->mem[0] = ans;
    self->mem[1] = self->mem[0];
    self->mem[2] = self->mem[1];
}
//---------------------------------------------------------------------------------------------------------



//--PI-----------------------------------------------------------------------------------------------------
void pi_set(struct PI* self, float fa, float kp, float Ti, float in0, float out0)
{
    float temp1 = 2.0f * fa * Ti;
    float temp2 = kp / temp1;
    self->k0 = (1.0f + temp1) * temp2;
    self->k1 = (1.0f - temp1) * temp2;
    self->in = in0;
    self->out = out0;
}

void pi_reset(struct PI* self, float in0, float out0)
{
    self->in = in0;
    self->out = out0;
}

void pi_step(struct PI* self, float input)
{
    self->out = self->k0 * input + self->k1 * self->in + self->out;
    self->in = input;
}
//---------------------------------------------------------------------------------------------------------



//--TOGI---------------------------------------------------------------------------------------------------
void togi_set(struct Togi* self, float fa, float ks, float kt)
{
    integrator3_set(&self->int0, fa);
    integrator3_set(&self->int90, fa);
    self->ks = ks;
    self->kt = kt;
    self->out[0] = self->int0.out;
    self->out[1] = self->int90.out;
}

void togi_step(struct Togi* self, float w, float input)
{
    float tin = input - self->intt.out - self->int0.out;
    float ans = tin * self->ks;
    ans -= self->int90.out;
    ans *= w;
    integrator3_step(&self->int0, ans);

    ans = self->int0.out * w;
    integrator3_step(&self->int90, ans);

    ans = tin * self->kt * w;
    integrator3_step(&self->intt, ans);

    self->out[0] = self->int0.out;
    self->out[1] = self->int90.out;
}
//---------------------------------------------------------------------------------------------------------



//--Transformadas------------------------------------------------------------------------------------------

/*
 * transform_ab_albe: Transformada de Clarke (dims: 2 <- 2)
 *
 * out: albe[2] = [alfa, beta]
 * in: ab[2] = [a, b]
 *
 * | alfa |   | sqrt(3/2)       0    |   | a |
 * |      | = |                      | * |   |
 * | beta |   | 1/sqrt(2)    sqrt(2) |   | b |
 *
 */
void transform_ab_albe(float* albe, float* ab)
{
    albe[0] = f_sqrt3_sqrt2 * ab[0];
    albe[1] = f_1_sqrt2 * ab[0] + f_sqrt2 * ab[1];
}

/*
 * transform_albe_abc: Transformada Inversa de Clarke (dims: 3 <- 2)
 *
 * out: acb[2] = [a, b, c]
 * in: albe[2] = [alfa, beta]
 *
 * | a |   |  sqrt(2/3)       0    |   |      |
 * |   |   |                       |   | alfa |
 * | b | = | -1/sqrt(6)    sqrt(2) | * |      |
 * |   |   |                       |   | beta |
 * | c |   | -1/sqrt(6)   -sqrt(2) |   |      |
 *
 */
void transform_albe_abc(float* abc, float* albe)
{
    abc[0] = f_sqrt2_sqrt3 * albe[0];
    abc[1] = -f_1_sqrt6 * albe[0] + f_1_sqrt2 * albe[1];
    abc[2] = -f_1_sqrt6 * albe[0] - f_1_sqrt2 * albe[1];
}

/*
 * transform_albe_dq: Transformada de Parke (dims: 2 <- 2)
 *
 * out:  dq[2] = [    d   ,   q     ]
 * in:  cis[2] = [ cos(th), sin(th) ]
 * in: albe[2] = [  alfa  ,  beta   ]
 *
 * | d |   |  cos(th)  sin(th)  |   | alfa |
 * |   | = |                    | * |      |
 * | q |   | -sin(th)  cos(th)  |   | beta |
 *
 */
void transform_albe_dq(float* dq, float* cis, float* albe)
{
    dq[0] = cis[0] * albe[0] + cis[1] * albe[1];
    dq[1] = -cis[1] * albe[0] + cis[0] * albe[1];
}


/*
 * transform_dq_albe: Transformada Inversa de Parke (dims: 2 <- 2)
 *
 * out: albe[2] = [  alfa  ,  beta   ]
 * in:   cis[2] = [ cos(th), sin(th) ]
 * in:    dq[2] = [    d   ,    q    ]
 *
 * | alfa |   | cos(th)  -sin(th)  |   | d |
 * |      | = |                    | * |   |
 * | beta |   | sin(th)   cos(th)  |   | q |
 *
 */
void transform_dq_albe(float* albe, float* cis, float* dq)
{
    albe[0] = cis[0] * dq[0] - cis[1] * dq[1];
    albe[1] = cis[1] * dq[0] + cis[0] * dq[1];
}


/*
 * transform_albe_pos:
 * Transformada Componentes Simétrica: seq. positiva  (dims: 2 <- [2;2])
 *
 * out: albe_p[2] = [ alfa_p ,  beta_p ] = [ alfa_p , beta_p ]
 * in:   al090[2] = [  alfa  , -j*alfa ] = [ alfa_0 , alfa_90 ]
 * in:   be090[2] = [  beta  , -j*beta ] = [ beta_0 , beta_90 ]
 *
 *                                      | alfa_0  |
 * | alfa_p | = | 1/2   0  -1/2  0  | * | alfa_90 |
 * | beta_p |   |  0   1/2   0  1/2 |   | beta_0  |
 *                                      | beta_90 |
 *
 */
void transform_albe_pos(float* albe_p, float* al090, float* be090)
{
    albe_p[0] = 0.5f * (al090[0] - be090[1]);
    albe_p[1] = 0.5f * (al090[1] + be090[0]);
}


/*
 * transform_albe_neg:
 * Transformada Componentes Simétrica: seq. negativa (dims: 2 <- [2;2] )
 *
 * out: albe_n[2] = [ alfa_n ,  beta_n ] = [ alfa_n , beta_n ]
 * in:   al090[2] = [  alfa  , -j*alfa ] = [ alfa_0 , alfa_90 ]
 * in:   be090[2] = [  beta  , -j*beta ] = [ beta_0 , beta_90 ]
 *
 *                                      | alfa_0  |
 * | alfa_p | = | 1/2   0    0  1/2 | * | alfa_90 |
 * | beta_p |   |  0  -1/2  1/2  0  |   | beta_0  |
 *                                      | beta_90 |
 *
 */
void transform_albe_neg(float* albe_n, float* al090, float* be090)
{
    albe_n[0] = 0.5f * (al090[0] + be090[1]);
    albe_n[1] = 0.5f * (-al090[1] + be090[0]);
}
//---------------------------------------------------------------------------------------------------------



//--PLL----------------------------------------------------------------------------------------------------
void pll_set(struct pll_s* self, float fa, float w0, float kp, float Ti)
{

    self->kp = kp;
    self->ki = 1.0/Ti;

    integrator_set(&self->int_w, fa, 0.0f, w0);
    integrator_set(&self->int_th, fa, 0.0f, 0.0f);

    self->wf = self->int_w.out;
    self->th = 0.0f;
}

void pll_step(struct pll_s* self, float input)
{
    float kpin = input * self->kp;
    integrator_step(&self->int_w, kpin * self->ki);
    self->wf = self->int_w.out;
    integrator_step(&self->int_th, self->wf + kpin);

    if (self->int_th.out >= f_2pi)
        self->int_th.out -= f_2pi;

    if (self->int_th.out < 0)
        self->int_th.out += f_2pi;

   self->th = self->int_th.out;
}

void pllf_set(struct pll_f* self, float fa, float w0, float kp, float Ti, float* num, float* den)
{
    tfz3_set(&self->notch, num, den);
    self->kp = kp;
    self->ki = 1.0/Ti;

    integrator_set(&self->int_w, fa, 0.0f, w0);
    integrator_set(&self->int_th, fa, 0.0f, 0.0f);

    self->wf = self->int_w.out;
    self->th = 0.0f;
}

void pllf_step(struct pll_f* self, float input)
{
    tfz3_step(&self->notch, input);
    float kpin = self->notch.out * self->kp;
    integrator_step(&self->int_w, kpin * self->ki);
    self->wf = self->int_w.out;
    integrator_step(&self->int_th, self->wf + kpin);

    if (self->int_th.out >= f_2pi)
        self->int_th.out -= f_2pi;

    if (self->int_th.out < 0)
        self->int_th.out += f_2pi;

   self->th = self->int_th.out;
}
//---------------------------------------------------------------------------------------------------------





void tfz4_set(struct TFZ4* self, float* num, float* den)
{
    float temp = 1.0f / den[0];
    self->n0 = num[0]*temp;
    self->n1 = num[1]*temp;
    self->n2 = num[2]*temp;
    self->n3 = num[3]*temp;
    self->d1 = den[1]*temp;
    self->d2 = den[2]*temp;
    self->d3 = den[3]*temp;
    self->in1 = 0.0f;
    self->in2 = 0.0f;
    self->in3 = 0.0f;
    self->out = 0.0f;
    self->out1 = 0.0f;
    self->out2 = 0.0f;
    self->out3 = 0.0f;
}


void tfz4_reset(struct TFZ4* self)
{
    self->in1 = 0.0f;
    self->in2 = 0.0f;
    self->in3 = 0.0f;
    self->out = 0.0f;
    self->out1 = 0.0f;
    self->out2 = 0.0f;
    self->out3 = 0.0f;
}

void tfz4_step(struct TFZ4* self, float input)
{
    self->out3 = self->out2;
    self->out2 = self->out1;
    self->out1 = self->out;

        self->out =         self->n0 * input
                + self->n1 * self->in1
                + self->n2 * self->in2
                + self->n3 * self->in3
                 - self->out1 * self->d1
                 - self->out2 * self->d2
                 - self->out3 * self->d3;

    self->in3 = self->in2;
    self->in2 = self->in1;
    self->in1 = input;
}



void tfz3_set(struct TFZ3* self, float* num, float* den)
{
    float temp = 1.0f / den[0];
    self->n0 = num[0]*temp;
    self->n1 = num[1]*temp;
    self->n2 = num[2]*temp;

    self->d1 = den[1]*temp;
    self->d2 = den[2]*temp;

    self->in1 = 0.0f;
    self->in2 = 0.0f;

    self->out = 0.0f;
    self->out1 = 0.0f;
    self->out2 = 0.0f;

}


void tfz3_reset(struct TFZ3* self)
{

    self->in1 = 0.0f;
    self->in2 = 0.0f;

    self->out = 0.0f;
    self->out1 = 0.0f;
    self->out2 = 0.0f;

}

void tfz3_step(struct TFZ3* self, float input)
{

    self->out2 = self->out1;
    self->out1 = self->out;

        self->out =         self->n0 * input
                + self->n1 * self->in1
                + self->n2 * self->in2
                 - self->out1 * self->d1
                 - self->out2 * self->d2;

    self->in2 = self->in1;
    self->in1 = input;
}


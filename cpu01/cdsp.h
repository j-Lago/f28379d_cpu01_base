/*
 * cdsp.h
 *
 *  Created on: Aug 24, 2022
 *      Author: j-Lago
 */

#pragma once


#define f_sqrt2_sqrt3 0.81649658092772603273242802490196f
#define f_sqrt3_sqrt2 1.2247448713915890490986420373529f
#define f_1_sqrt2 0.70710678118654752440084436210485f
#define f_1_sqrt6 0.40824829046386301636621401245098f
#define f_sqrt2 1.4142135623730950488016887242097f
#define f_2pi 6.283185307179586476925286766559f
#define f_pi 3.1415926535897932384626433832795f

/*
 * Integrator: First order integrator (tusting)
 *
 * Função de transferência:
 *
 *            y    1
 *            - =  -
 *            x    s
 *
 * Eq. diferenças:
 *
 *              1
 *      y[n] = ---- * ( x[n] + x[n-1] ) + y[n-1]       , fa = frequência de amostragem
 *             2*fa
 */
struct Integrator
{
    float k;
    float in;
    float out;
};

void integrator_set(struct Integrator* self, float sample_frequency, float in0, float out0);
void integrator_step(struct Integrator* self, float input);


/*
 * Integrator3: Thrird order integrator
 *
 * Função de transferência:
 *
 *            y    1
 *            - =  -
 *            x    s
 *
 * Eq. diferenças:
 *
 *      y[n] = 5*a[n-3] - 16*a[n-2] + 23*a[n-1]
 *      a[n] = 1 / (12*fa) *  x[n] - a[n-1]           , fa = frequência de amostragem
 *
 */
struct Integrator3
{
    float k;
    float mem[3];
    float out;
};

void integrator3_set(struct Integrator3* self, float sample_frequency);
void integrator3_step(struct Integrator3* self, float input);



/*
 * PI: Controlador Proporcional Integral
 *
 *    Função transferência:
 *
 *      y         (1 + sTi)                     1
 *      -  = kp * ---------  =    kp + kp/Ti * ---
 *      x            sTi                        s
 *
 *
 *    Eq. diferenças:
 *
 *      y[n] = k0 * x[n] + k1 * x[n-1] ) + y[n-1]
 *
 *                 1 + 2*fa*Ti                              1 - 2*fa*Ti
 *      k0 = kp * -------------               , k1 = kp * --------------
 *                   2*fa*Ti                                  2*fa*Ti
 *
 *
 *    Discretização trapezoidal:
 *
 *                  z-1
 *      s = 2*fa * -----                      , fa = frequência de amostragem
 *                  z+1
 *
 */
struct PI
{
    float k0;
    float k1;

    float in;
    float out;
};

void pi_set(struct PI* self, float fa, float kp, float Ti, float in0, float out0);
void pi_reset(struct PI* self, float in0=0.0f, float out0=0.0f);
void pi_step(struct PI* self, float input);


/*
 * Togi: third order generalized integrator
 */
struct Togi
{
    float ks;
    float kt;
    Integrator3 int0;
    Integrator3 int90;
    Integrator3 intt;
    float out[2];
};

void togi_set(struct Togi* self, float fa, float ks, float kt);
void togi_step(struct Togi* self, float w, float input);



void transform_ab_albe(float* albe, float* ab);
void transform_albe_abc(float* abc, float* albe);
void transform_albe_dq(float* dq, float* cis, float* albe);
void transform_dq_albe(float* albe, float* cis, float* dq);

void transform_albe_pos(float* albe_p, float* al090, float* be090);
void transform_albe_neg(float* albe_n, float* al090, float* be090);

/*
 * pll_s: Proprtional integral controller for phase-locked loop
 */
struct pll_s
{
    float kp;
    float ki;
    Integrator int_w;
    Integrator int_th;
    float wf;
    float th;
};

void pll_set(struct pll_s* self, float fa, float w0, float kp, float Ti);
void pll_step(struct pll_s* self, float input);




/*
 * Controlador 3 ordem
 *
 *    Função transferência em s:
 *
 *      y          (1 + s*z0) * (1 + s*z1) * (1 + s*z2)
 *      -  = k * -------------------------------------------
 *      x          (1 + s*p0) * (1 + s*p1) * (1 + s*p2)
 *
 *
 *    Função transferência em z (tustin):
 *
 *      y      n0 * z^3 + n1 * z^2 + n2 * z + n3
 *      -  =  -------------------------------------------
 *      x      d0 * z^3 + d1 * z^2 + d2 * z + d3
 *
 *
 */
struct TFZ4
{
    float n0;
    float n1;
    float n2;
    float n3;
    float d1;
    float d2;
    float d3;
        float in1;
        float in2;
        float in3;
    float out1;
    float out2;
    float out3;
        float out;
};
void tfz4_set(struct TFZ4* self, float* num, float* den);
void tfz4_reset(struct TFZ4* self);
void tfz4_step(struct TFZ4* self, float input);


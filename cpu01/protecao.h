/*
 * protecao.h
 *
 *  Created on: Aug 22, 2022
 *      Author: j-Lago
 */

#pragma once
#include "adc.h"
#include "pwm.h"

struct Protecao
{
    static const int errNch = 8;

    static const float lim_iu = 8.0f;
    static const float lim_iv = lim_iu;
    static const float lim_vp0 = 80.0f;
    static const float lim_v0n = lim_vp0;
    static const float lim_vrs = 150.0f;
    static const float lim_vts = lim_vrs;
    static const float lim_ic = 10.0f;
    static const float lim_ib = lim_ic;

    bool errors[errNch];
    float err_values[errNch];

    ADC& adc;
    PWM& pwm;

    Protecao(ADC& adc, PWM& pwm)
    : adc(adc), pwm(pwm)
    {
        clear();
    }

    void clear(void)
    {
        for (int k=0; k<errNch; k++){
            errors[k] = false;
            err_values[k] = 0;
        }
        pwm.clear();
    }

    void test()
    {
        //-- proteções --------------------------------------------------------------------
        errors[0] = fabsf(adc.iu) > lim_iu;
        errors[1] = fabsf(adc.iv) > lim_iv;
        errors[2] = fabsf(adc.vp0) > lim_vp0;
        errors[3] = fabsf(adc.v0n) > lim_v0n;
        errors[4] = fabsf(adc.vrs) > lim_vrs;
        errors[5] = fabsf(adc.vts) > lim_vts;
        errors[6] = fabsf(adc.ic) > lim_ic;
        errors[7] = fabsf(adc.ib) > lim_ib;

        for(int k = 0; k < errNch; k++){
            if(errors[k] and !pwm.fault){
                pwm.trip();
                err_values[0] = adc.iu;
                err_values[1] = adc.iv;
                err_values[2] = adc.vp0;
                err_values[3] = adc.v0n;
                err_values[4] = adc.vrs;
                err_values[5] = adc.vts;
                err_values[6] = adc.ic;
                err_values[7] = adc.ib;
                break;
            }
        }
    }

};

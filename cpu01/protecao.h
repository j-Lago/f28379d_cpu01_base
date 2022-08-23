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
    static const int errNch = Nmeasurs;
    static const float limits[errNch];  // definido em consts.cpp

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
        for (int k = 0; k < errNch; k++)
            errors[k] = fabsf(adc.measurs[k]) > limits[k];

        for(int m = 0; m < errNch; m++){
            if(errors[m] and !pwm.fault){
                pwm.trip();
                for(int n = 0; n < errNch; n++)
                    err_values[n] = adc.measurs[n];
                break;
            }
        }
    }

};

/*
 * protecao.h
 *
 *  Created on: Aug 22, 2022
 *      Author: j-Lago
 *
 */

#pragma once
#include "adc.h"
#include "pwm.h"

class Protecao
{
public:

    static const int errNch = Nmeasurs; // definido em adc.h
    static const float limits[errNch];  // definido em consts.cpp

    bool err_flags[errNch];
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
            err_flags[k] = false;
            err_values[k] = 0.0f;
        }
        pwm.clear();
    }

    void compare()
    {
        for (int m = 0; m < errNch; m++) // não unir com o loop de baixo para preencher todos os flas e não apenas o primeiro erro
            err_flags[m] = fabsf(adc.measurs[m]) > limits[m];

        for(int m = 0; m < errNch; m++){
            if(err_flags[m] and !pwm.fault){
                pwm.trip();
                for(int n = 0; n < errNch; n++)
                    err_values[n] = adc.measurs[n];
                break;
            }
        }
    }

};

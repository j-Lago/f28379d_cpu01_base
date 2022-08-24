/*
 * softPWM.h
 *
 *  Created on: Aug 24, 2022
 *      Author: j-Lago
 */

#pragma once

#include <stdint.h>
#include "gpio.h"

/*
 * PWM por software
 *
 *  frequência do softPWM deve ser uma fração da frequência com que o método softPWM::refresh é chamado
 *  softPWM::dutycicle deve ficar entre 0.0f e 1.0f
 *
 */
class softPWM
{
private:
    uint16_t comp = 0;
    uint16_t period  = 100;
    uint16_t counter = 0;
    OutPin pin;

public:
    float dutycicle;


    softPWM(uint16_t npin, uint16_t fsample, uint16_t fpwm, pin_active_logic active_logic = active_high, float duty = 0.5f)
    {
        pin.appendRegister(npin, active_logic);

        period = fsample / fpwm;
        counter = period;
        dutycicle = duty;
    }

    void set_dutycicle(float duty)
    {
        dutycicle = duty;
    }


    void refresh()
    {
        comp = period * dutycicle;

        if (--counter == 0 )
            counter = period;

        if(counter <= comp)
            pin.set();
        else
            pin.clear();
    }
};

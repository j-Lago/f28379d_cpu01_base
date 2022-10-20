/*
 * main.cpp
 *
 *  Created on: 24 de jan de 2022
 *      Author: j-Lago
 */

#include "F28x_Project.h"
#include "globals.h"
#include "gpio.h"
#include "pwm.h"
#include "adc.h"
#include "cla.h"
#include "parameters.h"
#include "control.h"
#include "scope.h"

extern Scope<SCOPE_BUFER_SIZE> scope;
extern float kn;

void hardware_setup();

void main(void)
{
    hardware_setup();
    //page0.par.loadDefaults();

    while(true){
        //page0.refresh();

        if(scope.state == full)//(raspi.ser.len() > 0)
        {
            PROBE_SET(3);   // probe: 3 - medicao de tempo comunicação

            scope.hmi->write_float32(&kn, 1, 4, 0);
            scope.send();

            PROBE_CLEAR(3); // probe: 3 - medicao de tempo comunicação
        }



    }
}


void hardware_setup()
{
    InitSysCtrl();
    InitGpio();
    InitPieCtrl();

    IER = 0x0000;
    IFR = 0x0000;
    InitPieVectTable();

    CLA_setup();
    OutPin::setup();
    PWM::setup();
    ADC::setup();
    SCI::setup();
    Encoder::setup();

    control_setup();

    CLA_start();
    PWM::start();
    ADC::start();
    SCI::start();
    Encoder::start();
    EnableInterrupts();
}

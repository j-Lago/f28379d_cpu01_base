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

extern Scope scope;
ScopeState test;

void hardware_setup();

void main(void)
{
    hardware_setup();
    //page0.par.loadDefaults();

    while(true){
        //page0.refresh();

        test = scope.state ;
        if(scope.state == full)//(raspi.ser.len() > 0)
        {
PROBE_SET(3);   // probe: 3 - medicao de tempo comunica��o
            scope.send();
PROBE_CLEAR(3); // probe: 3 - medicao de tempo comunica��o
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

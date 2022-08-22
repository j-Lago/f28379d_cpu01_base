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

void hardware_setup();

void main(void)
{
    hardware_setup();
    page0.par.loadDefaults();

    while(true)
    {
        page0.refresh();
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

    CLA_start();
    PWM::start();
    ADC::start();
    SCI::start();
    Encoder::start();
    EnableInterrupts();
}

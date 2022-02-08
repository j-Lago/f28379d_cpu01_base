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

void load_defaults();
void hardware_setup();

void main(void)
{
    hardware_setup();
    load_defaults();

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

    CLA_start();
    PWM::start();
    ADC::start();
    SCI::start();
    EnableInterrupts();
}


void load_defaults()
{
    // inicialização de variáveis do cla
    // TODO: linkar inicialização com valores padrão da tabela parameters_default.h

    cla_dq[0] = 1.0f;
    cla_dq[1] = 0.0f;
    cla_dir = 1.0f;
    cla_th = 0.0f;
}

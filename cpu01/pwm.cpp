/*
 * pwm.cpp
 *
 *  Created on: 10 de jan de 2022
 *      Author: j-Lago
 */

#include "F28x_Project.h"
#include "pwm.h"


void PWM::setComps(float a, float b, float c)
{
    this->a = a;
    this->b = b;
    this->c = c;
    update();
}

void PWM::setComps(float* abc)
{
    this->a = abc[0];
    this->b = abc[1];
    this->c = abc[2];
    update();
}

void PWM::update()
{
    if(fault)
        trip();

    EPwm1Regs.CMPA.bit.CMPA = (uint16_t)(GAIN * a + BIAS);
    EPwm2Regs.CMPA.bit.CMPA = (uint16_t)(GAIN * b + BIAS);
    EPwm3Regs.CMPA.bit.CMPA = (uint16_t)(GAIN * c + BIAS);
}

bool PWM::enable(void)
{
    if(!fault)
    {
        EALLOW;
        EPwm1Regs.TZCLR.bit.OST = 1;
        EPwm2Regs.TZCLR.bit.OST = 1;
        EPwm3Regs.TZCLR.bit.OST = 1;
        EPwm4Regs.TZCLR.bit.OST = 1;
        EPwm5Regs.TZCLR.bit.OST = 1;
        EPwm6Regs.TZCLR.bit.OST = 1;
        EDIS;
        en = true;
    }
    else
        en = false;

    return en;
}


void PWM::disable(void)
{
    EALLOW;
    EPwm1Regs.TZFRC.bit.OST = 1;
    EPwm2Regs.TZFRC.bit.OST = 1;
    EPwm3Regs.TZFRC.bit.OST = 1;
    EPwm4Regs.TZFRC.bit.OST = 1;
    EPwm5Regs.TZFRC.bit.OST = 1;
    EPwm6Regs.TZFRC.bit.OST = 1;
    EDIS;
    en = false;
}

void PWM::trip()
{
    disable();
    fault = true;
}

void PWM::clear()
{
    disable();
    fault = false;
}

void PWM::setup(void)
{
    EALLOW;
    GpioCtrlRegs.GPAPUD.bit.GPIO0 = 1;    // Disable pull-up on GPIO0 (EPWM1A)
    GpioCtrlRegs.GPAPUD.bit.GPIO1 = 1;    // Disable pull-up on GPIO1 (EPWM1B)
    GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 1;   // Configure GPIO0 as EPWM1A
    GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 1;   // Configure GPIO1 as EPWM1B

    GpioCtrlRegs.GPAPUD.bit.GPIO2 = 1;    // Disable pull-up on GPIO2 (EPWM2A)
    GpioCtrlRegs.GPAPUD.bit.GPIO3 = 1;    // Disable pull-up on GPIO3 (EPWM2B)
    GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 1;   // Configure GPIO2 as EPWM2A
    GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 1;   // Configure GPIO3 as EPWM2B

    GpioCtrlRegs.GPAPUD.bit.GPIO4 = 1;    // Disable pull-up on GPIO4 (EPWM3A)
    GpioCtrlRegs.GPAPUD.bit.GPIO5 = 1;    // Disable pull-up on GPIO5 (EPWM3B)
    GpioCtrlRegs.GPAMUX1.bit.GPIO4 = 1;   // Configure GPIO4 as EPWM3A
    GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 1;   // Configure GPIO5 as EPWM3B
    EDIS;

    EALLOW;
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 0;
    EDIS;


    EPwm1Regs.TBCTL.bit.CTRMODE = 3; // freeze counter
    EPwm2Regs.TBCTL.bit.CTRMODE = 3; // freeze counter
    EPwm3Regs.TBCTL.bit.CTRMODE = 3; // freeze counter

    EPwm1Regs.TBCTL.bit.PHSEN = TB_DISABLE;        // Enable phase loading
    EPwm2Regs.TBCTL.bit.PHSEN = TB_DISABLE;        // Enable phase loading
    EPwm3Regs.TBCTL.bit.PHSEN = TB_DISABLE;        // Enable phase loading

    EPwm1Regs.TBCTL.bit.SYNCOSEL = 1;
    EPwm2Regs.TBCTL.bit.SYNCOSEL = 1;
    EPwm2Regs.TBCTL.bit.SYNCOSEL = 1;



    EPwm1Regs.TBPRD = CMP_PORTADORA;                       // Set timer period
    EPwm2Regs.TBPRD = CMP_PORTADORA;                       // Set timer period
    EPwm3Regs.TBPRD = CMP_PORTADORA;                       // Set timer period



    EPwm1Regs.TBPHS.bit.TBPHS = 0x0000;           // Phase is 0
    EPwm2Regs.TBPHS.bit.TBPHS = 0x0000;           // Phase is 0
    EPwm3Regs.TBPHS.bit.TBPHS = 0x0000;           // Phase is 0

    EPwm1Regs.TBCTR = 0x0000;                     // Clear counter
    EPwm2Regs.TBCTR = 0x0000;                     // Clear counter
    EPwm3Regs.TBCTR = 0x0000;                     // Clear counter




    //EPwm1Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;       // Clock ratio to SYSCLKOUT
    //EPwm2Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;       // Clock ratio to SYSCLKOUT
    //EPwm3Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;       // Clock ratio to SYSCLKOUT

    EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;    // Load registers every ZERO
    EPwm1Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
    EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
    EPwm1Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

    EPwm2Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;    // Load registers every ZERO
    EPwm2Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
    EPwm2Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
    EPwm2Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

    EPwm3Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;    // Load registers every ZERO
    EPwm3Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
    EPwm3Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
    EPwm3Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;


    // Comparador
    EPwm1Regs.CMPA.bit.CMPA = 0;
    EPwm2Regs.CMPA.bit.CMPA = 0;
    EPwm3Regs.CMPA.bit.CMPA = 0;






    // Set actions
    EPwm1Regs.AQCTLA.bit.CAU = AQ_CLEAR;          // Clear PWM1A on event A, up count
    EPwm1Regs.AQCTLA.bit.CAD = AQ_SET;            // Set PWM1A on event A, down count
    EPwm1Regs.AQCTLB.bit.CAU = AQ_SET;            // Set PWM1B on event A, up count
    EPwm1Regs.AQCTLB.bit.CAD = AQ_CLEAR;          // Clear PWM1B on event A, down count

    EPwm2Regs.AQCTLA.bit.CAU = AQ_CLEAR;          // Clear PWM1A on event A, up count
    EPwm2Regs.AQCTLA.bit.CAD = AQ_SET;            // Set PWM1A on event A, down count
    EPwm2Regs.AQCTLB.bit.CAU = AQ_SET;            // Set PWM1B on event A, up count
    EPwm2Regs.AQCTLB.bit.CAD = AQ_CLEAR;          // Clear PWM1B on event A, down count

    EPwm3Regs.AQCTLA.bit.CAU = AQ_CLEAR;          // Clear PWM1A on event A, up count
    EPwm3Regs.AQCTLA.bit.CAD = AQ_SET;            // Set PWM1A on event A, down count
    EPwm3Regs.AQCTLB.bit.CAU = AQ_SET;            // Set PWM1B on event A, up count
    EPwm3Regs.AQCTLB.bit.CAD = AQ_CLEAR;          // Clear PWM1B on event A, down count



    //Trip Zone
    EALLOW;
    //EPwm1Regs.TZSEL.bit.OSHT1 = 1; // enable TZ1 as a one-shot event
    //EPwm2Regs.TZSEL.bit.OSHT1 = 1; // enable TZ1 as a one-shot event
    //EPwm3Regs.TZSEL.bit.OSHT1 = 1; // enable TZ1 as a one-shot event

    EPwm1Regs.TZCTL.bit.TZA = TZ_FORCE_LO; // PWM forçado para zero
    EPwm1Regs.TZCTL.bit.TZB = TZ_FORCE_LO; // PWM forçado para zero
    EPwm2Regs.TZCTL.bit.TZA = TZ_FORCE_LO; // PWM forçado para zero
    EPwm2Regs.TZCTL.bit.TZB = TZ_FORCE_LO; // PWM forçado para zer
    EPwm3Regs.TZCTL.bit.TZA = TZ_FORCE_LO; // PWM forçado para zero
    EPwm3Regs.TZCTL.bit.TZB = TZ_FORCE_LO; // PWM forçado para zer
    EDIS;


    // Tempo morto
    EPwm1Regs.DBCTL.bit.IN_MODE = 0;
    EPwm1Regs.DBRED.bit.DBRED = CMP_TEMPO_MORTO;
    EPwm1Regs.DBFED.bit.DBFED = CMP_TEMPO_MORTO;
    EPwm1Regs.DBCTL.bit.POLSEL = 2;
    EPwm1Regs.DBCTL.bit.OUT_MODE = 3;
    EPwm1Regs.DBCTL.bit.OUTSWAP = 0;

    EPwm2Regs.DBCTL.bit.IN_MODE = 0;
    EPwm2Regs.DBRED.bit.DBRED = CMP_TEMPO_MORTO;
    EPwm2Regs.DBFED.bit.DBFED = CMP_TEMPO_MORTO;
    EPwm2Regs.DBCTL.bit.POLSEL = 2;
    EPwm2Regs.DBCTL.bit.OUT_MODE = 3;
    EPwm2Regs.DBCTL.bit.OUTSWAP = 0;


    EPwm3Regs.DBCTL.bit.IN_MODE = 0;



    EPwm3Regs.DBRED.bit.DBRED = CMP_TEMPO_MORTO;
    EPwm3Regs.DBFED.bit.DBFED = CMP_TEMPO_MORTO;

    EPwm3Regs.DBCTL.bit.POLSEL = 2;
    EPwm3Regs.DBCTL.bit.OUT_MODE = 3;
    EPwm3Regs.DBCTL.bit.OUTSWAP = 0;



    //TRIP
    EALLOW;
    EPwm1Regs.TZFRC.bit.OST = 1;
    EPwm2Regs.TZFRC.bit.OST = 1;
    EPwm3Regs.TZFRC.bit.OST = 1;
    EPwm4Regs.TZFRC.bit.OST = 1;
    EPwm5Regs.TZFRC.bit.OST = 1;
    EPwm6Regs.TZFRC.bit.OST = 1;
    EDIS;



    //EPwm1Regs.ETSEL.bit.SOCAEN = 1;  // habilita sequenciador de conversão (SOCA)
    EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // inicia contador PWM no modo 'updown'
    EPwm2Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // inicia contador PWM no modo 'updown'
    EPwm3Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // inicia contador PWM no modo 'updown'

}

void PWM::start(void)
{
    EALLOW;
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 1;
    EDIS;
}


//singleton
PWM PWM::instance;
PWM& PWM::getInstance(){ return instance; }
PWM::PWM()
{
    clear();
    a = 0;
    b = 0;
    c = 0;
}

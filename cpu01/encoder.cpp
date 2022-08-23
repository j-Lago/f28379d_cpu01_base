/*
 * eqep.cpp
 *
 *  Created on: Jul 27, 2022
 *      Author: j-Lago
 */

/*
 * sci.cpp
 *
 *  Created on: 12 de jan de 2022
 *      Author: j-Lago
 */


#include "F28x_Project.h"
#include <stdio.h>
#include "globals.h"
#include "encoder.h"



void Encoder::setup(void)
{
    EALLOW;

    // Configura pinos GPIO020 e GPIO021 para medição canais A e B do encoder
    // utilizar conector QEP_A do Launchpad pois possui levelshift (encoder é de 5V)

    //EQEP1A
    GpioCtrlRegs.GPAPUD.bit.GPIO20 = 1;     // disable pull-up
    GpioCtrlRegs.GPAQSEL2.bit.GPIO20 = 0;   // sync to SYSCLK
    GpioCtrlRegs.GPAGMUX2.bit.GPIO20 = 0;   // config as EQEP1A
    GpioCtrlRegs.GPAMUX2.bit.GPIO20 = 1;    // config as EQEP1A

    //EQEP1B
    GpioCtrlRegs.GPAPUD.bit.GPIO21 = 1;     // disable pull-up
    GpioCtrlRegs.GPAQSEL2.bit.GPIO21 = 0;   // sync to SYSCLK
    GpioCtrlRegs.GPAGMUX2.bit.GPIO21 = 0;   // config as EQEP1B
    GpioCtrlRegs.GPAMUX2.bit.GPIO21 = 1;    // config as EQEP1B

    CpuSysRegs.PCLKCR4.bit.EQEP1 = 1;       // habilita clock do modulo QEP

    EDIS;

    EQep1Regs.QUPRD=200000000/2/Fvel;   // unit Timer for Fvel [Hz] at 200 MHz SYSCLKOUT
    EQep1Regs.QDECCTL.bit.QSRC=0;       // modo de contagem
    EQep1Regs.QDECCTL.bit.XCR=0;        // 2x resolution (falling and rising edges)
    EQep1Regs.QEPCTL.bit.FREE_SOFT=2;
    EQep1Regs.QEPCTL.bit.PCRM=00;       // QPOSCNT reset on index evnt
    EQep1Regs.QEPCTL.bit.UTE=1;         // unit Timer Enable
    EQep1Regs.QEPCTL.bit.QCLM=1;        // latch on unit time out
    EQep1Regs.QPOSMAX=0xffffffff;
    //EQep1Regs.QCAPCTL.bit.UPPS=3;     // 1/8 for unit position
    //EQep1Regs.QCAPCTL.bit.CCPS=7;     // 1/128 for CAP clock
}

void Encoder::start()
{
    EQep1Regs.QEPCTL.bit.QPEN=1;        // QEP enable
}

void Encoder::update_vel(){

    if(EQep1Regs.QFLG.bit.UTO==1){      // calculo da nova velocidade amostrado na frequencia Fvel

        pulse_count_ = pulse_count;
        pulse_count = EQep1Regs.QPOSLAT;

        vel = (pulse_count - pulse_count_) * pulsepT_to_radps;
        update_pos();

        EQep1Regs.QCLR.bit.UTO=1;   // clear __interrupt flag
    }
}

void Encoder::update_pos(){
    pos = ((pulse_count - pulse_zero) % PPR ) * pulse_to_rad;         // posição do eixo em rad
}

void Encoder::set_zero(void){
    pulse_zero = pulse_count;
    pos = 0.0f;
}



//singleton
Encoder Encoder::instance;
Encoder::Encoder(){}
Encoder& Encoder::getInstance(){ return instance; }




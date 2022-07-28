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

    //EQEP1A

    GpioCtrlRegs.GPAPUD.bit.GPIO20 = 1; // disable pull-up
    GpioCtrlRegs.GPAQSEL2.bit.GPIO20 = 0; // sync to SYSCLK
    GpioCtrlRegs.GPAGMUX2.bit.GPIO20 = 0; // config as EQEP1A
    GpioCtrlRegs.GPAMUX2.bit.GPIO20 = 1; // config as EQEP1A


    //EQEP1B
    GpioCtrlRegs.GPAPUD.bit.GPIO21 = 1; // disable pull-up
    GpioCtrlRegs.GPAQSEL2.bit.GPIO21 = 0; // sync to SYSCLK
    GpioCtrlRegs.GPAGMUX2.bit.GPIO21 = 0; // config as EQEP1B
    GpioCtrlRegs.GPAMUX2.bit.GPIO21 = 1; // config as EQEP1B

    CpuSysRegs.PCLKCR4.bit.EQEP1 = 1;

    EDIS;



    EQep1Regs.QUPRD=2000000;          // Unit Timer for 100Hz at
                                          // 200 MHz SYSCLKOUT

    EQep1Regs.QDECCTL.bit.QSRC=0;     //
    EQep1Regs.QDECCTL.bit.XCR=0;      // 2x resolution (cnt falling and
                                      //                rising edges)

    EQep1Regs.QEPCTL.bit.FREE_SOFT=2;
    EQep1Regs.QEPCTL.bit.PCRM=00;     // QPOSCNT reset on index evnt
    EQep1Regs.QEPCTL.bit.UTE=1;       // Unit Timer Enable
    EQep1Regs.QEPCTL.bit.QCLM=1;      // Latch on unit time out
    EQep1Regs.QPOSMAX=0xffffffff;

    //EQep1Regs.QCAPCTL.bit.UPPS=3;     // 1/8 for unit position
    //EQep1Regs.QCAPCTL.bit.CCPS=7;     // 1/128 for CAP clock
}

void Encoder::start()
{
    EQep1Regs.QEPCTL.bit.QPEN=1;      // QEP enable
    //EQep1Regs.QCAPCTL.bit.CEN=1;      // QEP Capture Enable

    //downsample_count = 0;
    //pulse_count = 0;
    //pulse_count_ = 0;
    //w = 0.0f;

}

void Encoder::calc(){

    //if(EQep1Regs.QFLG.bit.UTO==1){   // Unit Timeout event
    if (++downsample_count >= DOWNSAMPLE){
        int32_t delta; //polsos por período
        downsample_count = 0;

        if(EQep1Regs.QFLG.bit.UTO==1){   // Unit Timeout event
            pulse_count_ = pulse_count;
            pulse_count = EQep1Regs.QPOSLAT;

            //if (pulse_count>pulse_count_)
            //    delta = pulse_count - pulse_count_;
            //else
            //    delta = (0xFFFFFFFF-pulse_count_)+pulse_count;
            delta = pulse_count - pulse_count_;


            w = delta * pulsepT_to_radps;
        }
        EQep1Regs.QCLR.bit.UTO=1;   // Clear __interrupt flag
    }


}





//singleton
Encoder Encoder::instance;
Encoder::Encoder(){}
Encoder& Encoder::getInstance(){ return instance; }




/*
 * adc.cpp
 *
 *  Created on: 11 de jan de 2022
 *      Author: j-Lago
 */

#include "adc.h"
#include "globals.h"

/*
 * Interrup��o chamada quando convers�o do ADCA (iu) excecer limite predefinido em OVERCURRENT
 */
interrupt void adca_ppb_isr(void)
{
#ifndef DISABLE_PROTECTIONS
    pwm.trip(); //trip: hardware
#endif
    adc.iu_over =  (AdcaResultRegs.ADCRESULT0 - adc.offset[0]) * adc.gain[0];
    adc.iv_over =  (AdcbResultRegs.ADCRESULT0 - adc.offset[1]) * adc.gain[1];
    adc.vdc_over = (AdccResultRegs.ADCRESULT3 - adc.offset[6]) * adc.gain[6] + (AdcbResultRegs.ADCRESULT3 - adc.offset[7]) * adc.gain[7];
    AdcaRegs.ADCEVTCLR.bit.PPB1TRIPHI = 1;
    AdcaRegs.ADCEVTCLR.bit.PPB1TRIPLO = 1;

    PieCtrlRegs.PIEACK.all = PIEACK_GROUP10;
}
/*
 * Interrup��o chamada quando convers�o do ADCB (iv) excecer limite predefinido em OVERCURRENT
 */
interrupt void adcb_ppb_isr(void)
{
#ifndef DISABLE_PROTECTIONS
    pwm.trip(); //trip: hardware
#endif
    adc.iu_over =  (AdcaResultRegs.ADCRESULT0 - adc.offset[0]) * adc.gain[0];
    adc.iv_over =  (AdcbResultRegs.ADCRESULT0 - adc.offset[1]) * adc.gain[1];
    adc.vdc_over = (AdccResultRegs.ADCRESULT3 - adc.offset[6]) * adc.gain[6] + (AdcbResultRegs.ADCRESULT3 - adc.offset[7]) * adc.gain[7];

    AdcbRegs.ADCEVTCLR.bit.PPB1TRIPLO = 1;
    AdcbRegs.ADCEVTCLR.bit.PPB1TRIPHI = 1;

    PieCtrlRegs.PIEACK.all = PIEACK_GROUP10;
}

/*
 * tira offset e aplica ganho
 */
void ADC::read()
{
    iu =  (AdcaResultRegs.ADCRESULT0 - offset[0]) * gain[0];      // iu  -> pino 26
    iv =  (AdcbResultRegs.ADCRESULT0 - offset[1]) * gain[1];      // iv  -> pino 25

    ic =  -(AdcaResultRegs.ADCRESULT1 - offset[2]) * gain[2];     // ic  -> pino 29
    ib =  -(AdccResultRegs.ADCRESULT1 - offset[3]) * gain[3];     // ib  -> pino 64

    vrs = (AdcbResultRegs.ADCRESULT2 - offset[4]) * gain[4];      // vrs -> pino 28
    vts = (AdccResultRegs.ADCRESULT2 - offset[5]) * gain[5];      // vts -> pino 27

    vp0 = (AdccResultRegs.ADCRESULT3 - offset[6]) * gain[6];      // vp0 -> pino 24
    v0n = (AdcbResultRegs.ADCRESULT3 - offset[7]) * gain[7];      // v0n -> pino 23

    icf =  (AdcaResultRegs.ADCRESULT4 - offset[8]) * gain[8];     // ic2  -> pino 63
    ibf =  (AdcbResultRegs.ADCRESULT4 - offset[9]) * gain[9];     // ib2  -> pino 65

    vrsf = (AdccResultRegs.ADCRESULT5 - offset[10]) * gain[10];   // vrs2 -> pino 67
    vtsf = (AdcaResultRegs.ADCRESULT5 - offset[11]) * gain[11];   // vts2 -> pino 66

    pot = (AdcaResultRegs.ADCRESULT6 - offset[12]) * gain[12];    // DAC_A(JA3) -> pino 30

    vdc = vp0+v0n;
}


void ADC::setup()
{

    ADCresolution adc_res = bits12;
    ADCsignalmode adc_mode = single;

    EALLOW;

    PieVectTable.ADCA1_INT = &main_adc_isr; // mapeia a fun��o a ser chamada pela interrup��o do ADCA
    PieVectTable.ADCA_EVT_INT = &adca_ppb_isr; // mapeia a fun��o a ser chamada pela interrup��o do PPB (limites ADCa - iu)
    PieVectTable.ADCB_EVT_INT = &adcb_ppb_isr; // mapeia a fun��o a ser chamada pela interrup��o do PPB (limites ADCb - iv)

    AdcaRegs.ADCCTL2.bit.PRESCALE = 6; //set ADCCLK divider to /4
    AdcSetMode(ADC_ADCA, adc_res, adc_mode);
    AdcaRegs.ADCCTL1.bit.INTPULSEPOS = 1; //set pulse positions to late
    AdcaRegs.ADCCTL1.bit.ADCPWDNZ = 1; // power up the ADC

    AdcbRegs.ADCCTL2.bit.PRESCALE = 6; //set ADCCLK divider to /4
    AdcSetMode(ADC_ADCB, adc_res, adc_mode);
    AdcbRegs.ADCCTL1.bit.INTPULSEPOS = 1; //set pulse positions to late
    AdcbRegs.ADCCTL1.bit.ADCPWDNZ = 1; // power up the ADC

    AdccRegs.ADCCTL2.bit.PRESCALE = 6; //set ADCCLK divider to /4
    AdcSetMode(ADC_ADCC, adc_res, adc_mode);
    AdccRegs.ADCCTL1.bit.INTPULSEPOS = 1; //set pulse positions to late
    AdccRegs.ADCCTL1.bit.ADCPWDNZ = 1; // power up the ADC

    DELAY_US(10); //delay for 1ms to allow ADC time to power up
    EDIS;


    Uint16 acqps;
    //determine minimum acquisition window (in SYSCLKS) based on resolution
    if(ADC_RESOLUTION_12BIT == AdcaRegs.ADCCTL2.bit.RESOLUTION)
    {
        acqps = 14;  //75ns
    }
    else //resolution is 16-bit
    {
        acqps = 32; //63; //320ns
    }


    /*
     * +-----+------+------+----+--------------+------+
     * | con |sinal | pino |      ADCINxx      | SOCx |
     * +-----+------+------+----+----+----+----+------+
     * |     | v0n  |  23  |    |    |    | 14 | SOC3 |
     * |     | vp0  |  24  |    |    | c3 |    | SOC3 |
     * | JA2 +------+------+----+----+----+----+------+
     * |     | iu   |  26  | a3 |    |    |    | SOC0 |
     * |     | iv   |  25  |    | b3 |    |    | SOC0 |
     * +-----+------+------+----+----+----+----+------+
     * |     | vts1 |  27  |    |    | c2 |    | SOC2 |
     * |     | vrs1 |  28  |    | b2 |    |    | SOC2 |
     * |     +------+------+----+----+----+----+------+
     * |     | ic1  |  29  | a2 |    |    |    | SOC1 |
     * |     | ib1  |  64  |    |    | c5 |    | SOC1 |
     * | JA1 +------+------+----+----+----+----+------+
     * |     | ic2  |  63  |    |    |    | 15 | SOC4 |
     * |     | ib2  |  65  |    | b5 |    |    | SOC4 |
     * |     +------+------+----+----+----+----+------+
     * |     | vts2 |  66  | a5 |    |    |    | SOC5 |
     * |     | vrs2 |  67  |    |    | c4 |    | SOC5 |
     * +-----+------+------+----+----+----+----+------+
     * | JA3 | pot1 |  30  | a0 |    |    |    | SOC6 |
     * |     | pot2 |  70  | a1 |    |    |    |      |
     * +-----+------+------+----+----+----+----+------+
     */

    //
    //Select the channels to convert and end of conversion flag
    //
    EALLOW;
    //SOC0 canal A (iv)
    AdcaRegs.ADCSOC0CTL.bit.CHSEL = 03;       //SOC0 will convert pin A03
    AdcaRegs.ADCSOC0CTL.bit.ACQPS = acqps;    //sample window is 100 SYSCLK cycles
    AdcaRegs.ADCSOC0CTL.bit.TRIGSEL = 5;      //trigger on ePWM1 SOCA/C
    //SOC0 canal B (iu)
    AdcbRegs.ADCSOC0CTL.bit.CHSEL = 03;       //SOC0 will convert pin B03
    AdcbRegs.ADCSOC0CTL.bit.ACQPS = acqps;    //sample window is 100 SYSCLK cycles
    AdcbRegs.ADCSOC0CTL.bit.TRIGSEL = 5;      //trigger on ePWM1 SOCA/C

    //SOC3 canal C (vp0)
    AdccRegs.ADCSOC3CTL.bit.CHSEL = 03;       //SOC3 will convert pin C03
    AdccRegs.ADCSOC3CTL.bit.ACQPS = acqps;    //sample window is 100 SYSCLK cycles
    AdccRegs.ADCSOC3CTL.bit.TRIGSEL = 5;      //trigger on ePWM1 SOCA/C
    //SOC3 canal B (v0n)
    AdcbRegs.ADCSOC3CTL.bit.CHSEL = 14;       //SOC3 will convert pin A14
    AdcbRegs.ADCSOC3CTL.bit.ACQPS = acqps;    //sample window is 100 SYSCLK cycles
    AdcbRegs.ADCSOC3CTL.bit.TRIGSEL = 5;      //trigger on ePWM1 SOCA/C

    //SOC2 canal B (vrs)
    AdcbRegs.ADCSOC2CTL.bit.CHSEL = 02;       //SOC2 will convert pin B02
    AdcbRegs.ADCSOC2CTL.bit.ACQPS = acqps;    //sample window is 100 SYSCLK cycles
    AdcbRegs.ADCSOC2CTL.bit.TRIGSEL = 5;      //trigger on ePWM1 SOCA/C
    //SOC2 canal C (vts)
    AdccRegs.ADCSOC2CTL.bit.CHSEL = 02;       //SOC2 will convert pin C02
    AdccRegs.ADCSOC2CTL.bit.ACQPS = acqps;    //sample window is 100 SYSCLK cycles
    AdccRegs.ADCSOC2CTL.bit.TRIGSEL = 5;      //trigger on ePWM1 SOCA/C

    //SOC1 canal A (ic)
    AdcaRegs.ADCSOC1CTL.bit.CHSEL = 02;       //SOC1 will convert pin A02
    AdcaRegs.ADCSOC1CTL.bit.ACQPS = acqps;    //sample window is 100 SYSCLK cycles
    AdcaRegs.ADCSOC1CTL.bit.TRIGSEL = 5;      //trigger on ePWM1 SOCA/
    //SOC1 canal C (ib)
    AdccRegs.ADCSOC1CTL.bit.CHSEL = 05;       //SOC1 will convert pin A02
    AdccRegs.ADCSOC1CTL.bit.ACQPS = acqps;    //sample window is 100 SYSCLK cycles
    AdccRegs.ADCSOC1CTL.bit.TRIGSEL = 5;      //trigger on ePWM1 SOCA/

    //SOC5 canal C (vrs2)
    AdccRegs.ADCSOC5CTL.bit.CHSEL = 04;       //SOC5 will convert pin C04
    AdccRegs.ADCSOC5CTL.bit.ACQPS = acqps;    //sample window is 100 SYSCLK cycles
    AdccRegs.ADCSOC5CTL.bit.TRIGSEL = 5;      //trigger on ePWM1 SOCA/C
    //SOC5 canal A (vts2)
    AdcaRegs.ADCSOC5CTL.bit.CHSEL = 05;       //SOC5 will convert pin A03
    AdcaRegs.ADCSOC5CTL.bit.ACQPS = acqps;    //sample window is 100 SYSCLK cycles
    AdcaRegs.ADCSOC5CTL.bit.TRIGSEL = 5;      //trigger on ePWM1 SOCA/C

    //SOC4 canal A (ic2)
    AdcaRegs.ADCSOC4CTL.bit.CHSEL = 15;       //SOC4 will convert pin A15
    AdcaRegs.ADCSOC4CTL.bit.ACQPS = acqps;    //sample window is 100 SYSCLK cycles
    AdcaRegs.ADCSOC4CTL.bit.TRIGSEL = 5;      //trigger on ePWM1 SOCA/
    //SOC4 canal B (ib)
    AdcbRegs.ADCSOC4CTL.bit.CHSEL = 05;       //SOC4 will convert pin B05
    AdcbRegs.ADCSOC4CTL.bit.ACQPS = acqps;    //sample window is 100 SYSCLK cycles
    AdcbRegs.ADCSOC4CTL.bit.TRIGSEL = 5;      //trigger on ePWM1 SOCA/

    //SOC6 canal A (potenciometro)
    AdcaRegs.ADCSOC6CTL.bit.CHSEL = 00;       //SOC6 will convert pin A00
    AdcaRegs.ADCSOC6CTL.bit.ACQPS = acqps;    //sample window is 100 SYSCLK cycles
    AdcaRegs.ADCSOC6CTL.bit.TRIGSEL = 5;      //trigger on ePWM1 SOCA/

    //interrup��o de fim de convers�o
    AdcaRegs.ADCINTSEL1N2.bit.INT1SEL = 6;    //end of SOC2 will set INT1 flag
    AdcaRegs.ADCINTSEL1N2.bit.INT1E = 1;      //enable INT1 flag
    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;    //make sure INT1 flag is cleared

    //pos-processamento (sobrecorente iu)
    AdcaRegs.ADCPPB1CONFIG.bit.CONFIG = 0;  //PPB1 is associated with soc0
    AdcaRegs.ADCPPB1TRIPHI.bit.LIMITHI = LIMITHI_OVERCURRENTu;  //set high limits
    AdcaRegs.ADCPPB1TRIPLO.bit.LIMITLO = LIMITLO_OVERCURRENTu;    //set low limits
    AdcaRegs.ADCEVTINTSEL.bit.PPB1TRIPHI = 1;  //enable high limit events to generate interrupt
    AdcaRegs.ADCEVTINTSEL.bit.PPB1TRIPLO = 1;  //enable low limit events to generate interrupt
    AdcaRegs.ADCEVTCLR.bit.PPB1TRIPHI = 1; // limpa flag high limit
    AdcaRegs.ADCEVTCLR.bit.PPB1TRIPLO = 1; // limpa flag low limit

    //pos-processamento (sobrecorente iv)
    AdcbRegs.ADCPPB1CONFIG.bit.CONFIG = 0;  //PPB1 is associated with soc0
    AdcbRegs.ADCPPB1TRIPHI.bit.LIMITHI = LIMITHI_OVERCURRENTv;  //set high limits
    AdcbRegs.ADCPPB1TRIPLO.bit.LIMITLO = LIMITLO_OVERCURRENTv;    //set low limits
    AdcbRegs.ADCEVTINTSEL.bit.PPB2TRIPHI = 1;  //enable high limit events to generate interrupt
    AdcbRegs.ADCEVTINTSEL.bit.PPB2TRIPLO = 1;  //enable low limit events to generate interrupt
    AdcbRegs.ADCEVTCLR.bit.PPB2TRIPHI = 1; // limpa flag high limit
    AdcbRegs.ADCEVTCLR.bit.PPB2TRIPLO = 1; // limpa flag low limit


    // configura disparo SOC
    EALLOW;
    EPwm1Regs.ETSEL.bit.SOCAEN = 0;    // Disable SOC on A group
    EPwm1Regs.ETSEL.bit.SOCASEL = ET_CTR_PRD; // | ET_CTR_ZERO; //3;   // Select SOC on Zero e PRD
    EPwm1Regs.ETPS.bit.SOCAPRD = 1;    // Generate pulse on 1st event
    EDIS;

}

void ADC::start(void)
{
    IER |= M_INT1;    // adc
    IER |= M_INT10;   // ppb

    PieCtrlRegs.PIEIER1.bit.INTx1 = 1;
    PieCtrlRegs.PIEIER10.bit.INTx1 = 1;    //PPB ADCa
    PieCtrlRegs.PIEIER10.bit.INTx5 = 1;    //PPB ADCb

    EALLOW;
    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;  //clear INT1 flag
    EPwm1Regs.ETSEL.bit.SOCAEN = 1;         // habilita sequenciador de convers�o (SOCA) disparado pelo PWM
    EDIS;
}

void ADC::AdcSetMode(Uint16 adc, Uint16 resolution, Uint16 signalmode)
{
    Uint16 adcOffsetTrimOTPIndex; //index into OTP table of ADC offset trims
    Uint16 adcOffsetTrim;         //temporary ADC offset trim

    //re-populate INL trim
    CalAdcINL(adc);

    if(0xFFFF != *((Uint16*)GetAdcOffsetTrimOTP))
    {
        //offset trim function is programmed into OTP, so call it

        //calculate the index into OTP table of offset trims and call
        //function to return the correct offset trim
        adcOffsetTrimOTPIndex = 4*adc + 2*resolution + 1*signalmode;
        adcOffsetTrim = (*GetAdcOffsetTrimOTP)(adcOffsetTrimOTPIndex);
    }
    else
    {
        //offset trim function is not populated, so set offset trim to 0
        adcOffsetTrim = 0;
    }

    //Apply the resolution and signalmode to the specified ADC.
    //Also apply the offset trim and, if needed, linearity trim correction.
    switch(adc)
    {
        case ADC_ADCA:
            AdcaRegs.ADCCTL2.bit.RESOLUTION = resolution;
            AdcaRegs.ADCCTL2.bit.SIGNALMODE = signalmode;
            AdcaRegs.ADCOFFTRIM.all = adcOffsetTrim;
            if(ADC_RESOLUTION_12BIT == resolution)
            {
                //12-bit linearity trim workaround
                AdcaRegs.ADCINLTRIM1 &= 0xFFFF0000;
                AdcaRegs.ADCINLTRIM2 &= 0xFFFF0000;
                AdcaRegs.ADCINLTRIM4 &= 0xFFFF0000;
                AdcaRegs.ADCINLTRIM5 &= 0xFFFF0000;
            }
        break;
        case ADC_ADCB:
            AdcbRegs.ADCCTL2.bit.RESOLUTION = resolution;
            AdcbRegs.ADCCTL2.bit.SIGNALMODE = signalmode;
            AdcbRegs.ADCOFFTRIM.all = adcOffsetTrim;
            if(ADC_RESOLUTION_12BIT == resolution)
            {
                //12-bit linearity trim workaround
                AdcbRegs.ADCINLTRIM1 &= 0xFFFF0000;
                AdcbRegs.ADCINLTRIM2 &= 0xFFFF0000;
                AdcbRegs.ADCINLTRIM4 &= 0xFFFF0000;
                AdcbRegs.ADCINLTRIM5 &= 0xFFFF0000;
            }
        break;
        case ADC_ADCC:
            AdccRegs.ADCCTL2.bit.RESOLUTION = resolution;
            AdccRegs.ADCCTL2.bit.SIGNALMODE = signalmode;
            AdccRegs.ADCOFFTRIM.all = adcOffsetTrim;
            if(ADC_RESOLUTION_12BIT == resolution)
            {
                //12-bit linearity trim workaround
                AdccRegs.ADCINLTRIM1 &= 0xFFFF0000;
                AdccRegs.ADCINLTRIM2 &= 0xFFFF0000;
                AdccRegs.ADCINLTRIM4 &= 0xFFFF0000;
                AdccRegs.ADCINLTRIM5 &= 0xFFFF0000;
            }
        break;
        case ADC_ADCD:
            AdcdRegs.ADCCTL2.bit.RESOLUTION = resolution;
            AdcdRegs.ADCCTL2.bit.SIGNALMODE = signalmode;
            AdcdRegs.ADCOFFTRIM.all = adcOffsetTrim;
            if(ADC_RESOLUTION_12BIT == resolution)
            {
                //12-bit linearity trim workaround
                AdcdRegs.ADCINLTRIM1 &= 0xFFFF0000;
                AdcdRegs.ADCINLTRIM2 &= 0xFFFF0000;
                AdcdRegs.ADCINLTRIM4 &= 0xFFFF0000;
                AdcdRegs.ADCINLTRIM5 &= 0xFFFF0000;
            }
        break;
    }
}


void ADC::CalAdcINL(Uint16 adc)
{
    switch(adc)
    {
        case ADC_ADCA:
            if(0xFFFF != *((Uint16*)CalAdcaINL))
                (*CalAdcaINL)();    //trim function is programmed into OTP, so call it
            break;
        case ADC_ADCB:
            if(0xFFFF != *((Uint16*)CalAdcbINL))
                (*CalAdcbINL)();    //trim function is programmed into OTP, so call it
            break;
        case ADC_ADCC:
            if(0xFFFF != *((Uint16*)CalAdccINL))
                (*CalAdccINL)();    //trim function is programmed into OTP, so call it
            break;
        case ADC_ADCD:
            if(0xFFFF != *((Uint16*)CalAdcdINL))
                (*CalAdcdINL)();    //trim function is programmed into OTP, so call it
            break;
    }
}


//singleton
ADC ADC::instance;
ADC& ADC::getInstance(){ return instance; }
ADC::ADC()
{
    for(int k=0; k < Nmeasurs; k++)
        measurs[k] = 0.0f;
}



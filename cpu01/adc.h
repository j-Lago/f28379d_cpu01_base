/*
 * adc.h
 *
 *  Created on: 11 de jan de 2022
 *      Author: j-Lago
 */

#pragma once
#include "F28x_Project.h"
#include <stdint.h>

#include "pwm.h"
#include "cla.h"


#define Nmeasurs 13

enum ADCresolution {bits12=0, bits16=1};
enum ADCsignalmode {single=0, differential=1};


interrupt void main_adc_isr(void);
interrupt void adca_ppb_isr(void);
interrupt void adcb_ppb_isr(void);


class ADC
{

public:
    static const float offset[Nmeasurs];    // definido em consts.cpp
    static const float gain[Nmeasurs];      // definido em consts.cpp

    static const int LIMITHI_OVERCURRENTu;  // definido em consts.cpp
    static const int LIMITLO_OVERCURRENTu;  // definido em consts.cpp
    static const int LIMITHI_OVERCURRENTv;  // definido em consts.cpp
    static const int LIMITLO_OVERCURRENTv;  // definido em consts.cpp

    union
    {
        struct
        {
            float iu;
            float iv;
            float ic;
            float ib;
            float vrs;
            float vts;
            float vp0;
            float v0n;
            float vrsf;
            float vtsf;
            float icf;
            float ibf;
            float pot;
        };
        float measurs[Nmeasurs];
    };

    float iu_over;
    float iv_over;
    float vdc_over;

    float vdc;

    void read(void);
    static void setup(void);
    static void start(void);

private:
    static void AdcSetMode(uint16_t adc, uint16_t resolution, uint16_t signalmode);
    static void CalAdcINL(uint16_t adc);

//singleton
private:
    static ADC instance;
    ADC();
public:
    static ADC& getInstance();
    ADC(const ADC&) = delete;  // impede construtor de cópia

};


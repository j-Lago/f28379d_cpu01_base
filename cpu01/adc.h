/*
 * adc.h
 *
 *  Created on: 11 de jan de 2022
 *      Author: j-Lago
 */

#pragma once


enum ADCresolution {bits12=0, bits16=1};
enum ADCsignalmode {single=0, differential=1};


interrupt void adca1_isr(void);
interrupt void adca_ppb_isr(void);
interrupt void adcb_ppb_isr(void);

struct ADC
{
    static const float OVERCURRENT = 12.0f;
    static const float FUNDOESCALA_I = 20.0f;
    static const float FUNDOESCALA_VAC = 400.0f;
    static const float FUNDOESCALA_VDC = 20.0f;
    static const int LIMITHI_OVERCURRENT =  (int) ((1 + OVERCURRENT/FUNDOESCALA_I)*2047.5f);
    static const int LIMITLO_OVERCURRENT =  (int) ((1 - OVERCURRENT/FUNDOESCALA_I)*2047.5f);
    static const double km_DC = 0.00024420024420024420024f;  // 1 / 2^12
    static const double km_AC = 0.00048840048840048840048f;  // 1 / 2^11
    static const float km_i = FUNDOESCALA_I * km_AC;
    static const float km_vdc = FUNDOESCALA_VDC * km_DC;
    static const float km_vac = FUNDOESCALA_VAC * km_AC;

    float iu;
    float iv;
    float vp0;
    float v0n;
    float vrs;
    float vts;
    float ic;
    float ib;
    float vrsf;
    float vtsf;
    float icf;
    float ibf;
    float pot;

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


/*
 * adc.h
 *
 *  Created on: 11 de jan de 2022
 *      Author: j-Lago
 */

#pragma once



enum ADCresolution {bits12=0, bits16=1};
enum ADCsignalmode {single=0, differential=1};


interrupt void main_adc_isr(void);
interrupt void adca_ppb_isr(void);
interrupt void adcb_ppb_isr(void);

struct ADC
{
    // limite das proteções de conversão do ADC. Os limites definidos por software estão em proteções.h
    static const float OVERCURRENT = 12.0f;

    static const float km_iuvw = 0.006171422365f;
    static const float km_vdc = 0.08775538f;
    static const float km_vrst = 0.336042171336f;
    static const float km_iabc = 0.012085702131f;

    static const float FUNDOESCALA_I = 2047.5f * km_iuvw;
    static const int LIMITHI_OVERCURRENT =  (int) ((1 + OVERCURRENT/FUNDOESCALA_I)*2047.5f);
    static const int LIMITLO_OVERCURRENT =  (int) ((1 - OVERCURRENT/FUNDOESCALA_I)*2047.5f);

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


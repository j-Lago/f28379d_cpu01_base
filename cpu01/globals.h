/*
 * globals.h
 *
 *  Created on: 24 de jan de 2022
 *      Author: j-Lago
 */

#pragma once

#include "nextion_pages.h"
#include "gpio.h"
#include "adc.h"
#include "encoder.h"
#include "pwm.h"
#include "protecao.h"
#include "settings.h"
//#include "raspberrypi.h"



#ifdef ENABLE_PROBES
    #define PROBE_SET(ch)  probe_pins[ch].set()
    #define PROBE_CLEAR(ch)  probe_pins[ch].clear()
#else
    #define PROBE_SET(ch)  ;
    #define PROBE_CLEAR(ch)  ;
#endif

#define REFRESH_COMP 5000 // 20k/5k = 4 Hz (frequencia de atualização da hmi nextion e led)
#define COMM_REFRESH_COMP 10   // comunicação a 20k/20 = 1 kHz (envia até 17 bytes a 500 kbps)

extern OutPin led_vm;
extern OutPin led_az;

#ifdef ENABLE_PROBES
extern OutPin probe_pins[];
#endif

extern NextionP0 page0;
//extern rPiComm::Comm raspi;

extern ADC& adc;
extern PWM& pwm;
extern Encoder& enc;

extern Protecao prot;

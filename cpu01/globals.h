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
#include "pwm.h"

#define REFRESH_COMP 5000 // 4 Hz

extern OutPin led_vm;
extern OutPin led_az;
//extern OutPin logic0;
extern OutPin logic1;
extern OutPin logic2;
extern OutPin logic3;

//extern OutPin fan;

extern NextionP0 page0;
extern ADC& adc;
extern PWM& pwm;

extern PROT prot;

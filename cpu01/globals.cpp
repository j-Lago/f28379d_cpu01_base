/*
 * globals.cpp
 *
 *  Created on: 24 de jan de 2022
 *      Author: j-Lago
 */

#include "globals.h"
#include "F28x_Project.h"
#include "adc.h"
#include "encoder.h"
#include "pwm.h"
#include "protecao.h"
#include "softPWM.h"

// número máximo de instâncias de OutPin definido em gpio.h -> static const pin_list_cacity;
OutPin led_vm(34, active_low);
OutPin led_az(31, active_low);

softPWM fan(65, 20000, 500);

#ifdef ENABLE_PROBES
OutPin probe_pins[] = {32, 19, 18, 67, 111}; // pinos RELE1..4 na placa de controle
#endif

//NextionP0 page0;
rPiComm::Comm raspi;

ADC& adc = ADC::getInstance();
PWM& pwm = PWM::getInstance();
Encoder& enc = Encoder::getInstance();

Protecao prot(adc, pwm);

extern pll_s pll;
float teste0 = 0.0f;
float teste1 = 4.0f;
float teste2 = 0.9f;
float teste3 = 0.9f;
float teste4 = 0.9f;
float teste5 = 0.9f;
Scope<SCOPE_BUFER_SIZE> scope(&raspi, 0x10, &teste3, &teste4, &teste5);


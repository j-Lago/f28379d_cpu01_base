/*
 * globals.cpp
 *
 *  Created on: 24 de jan de 2022
 *      Author: j-Lago
 */

#include "globals.h"
#include "F28x_Project.h"



// número máximo de instâncias de OutPin definido em gpio.h -> static const pin_list_cacity;
OutPin led_vm(34, active_low);
OutPin led_az(31, active_low);
//OutPin logic0(32, active_high);
OutPin logic1(19, active_high);
OutPin logic2(18, active_high);
OutPin logic3(67, active_high);

NextionP0 page0;



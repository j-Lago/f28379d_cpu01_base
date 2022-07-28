/*
 * raspberrypi.cpp
 *
 *  Created on: Jul 25, 2022
 *      Author: jacks
 */


#include "F28x_Project.h"
#include "fixed_string.h"
#include "globals.h"
#include "sci.h"
#include "pwm.h"
#include "raspberrypi.h"



//--------------------------------------------------------------------------

const char *RaspberryPi::Comm::EOM = "ÿÿÿ";
SCI& RaspberryPi::Comm::port = SCI::getInstance();

void RaspberryPi::Comm::send(char c)                { port.push(c);          }
void RaspberryPi::Comm::send(const char *str)       { port.push(str);          }




/*
 * raspberrypi.h
 *
 *  Created on: Jul 25, 2022
 *      Author: j-Lago
 */


#pragma once

#include "sci.h"
#include "pwm.h"
#include "fixed_string.h"


namespace RaspberryPi
{
    class Comm
    {
    public:
        static const char* EOM; // End of Message characters
        static SCI& port;   // pointer to SCI (UART) object

    public:
        void send(char c);
        void send(const char *str);
    };

};

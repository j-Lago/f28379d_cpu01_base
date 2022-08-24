/*
 * gpio.h
 *
 *  Created on: 24 de jan de 2022
 *      Author: jacks
 */

#pragma once

#include "F28x_Project.h"
#include <vector>

//void gpio_setup(void);

enum pin_active_logic {active_low = 0, active_high = 1 };

class OutPin
{
public:
    static const uint16_t pin_list_capacity = 8;
    static uint16_t count;
    static uint16_t pin_list[];

    uint16_t gpio_num;
    pin_active_logic active_logic;

    OutPin(uint16_t gpio_num, pin_active_logic active_logic = active_high);
    OutPin(){};
    void appendRegister(uint16_t new_gpio_num, pin_active_logic new_active_logic);
    void set(void);
    void clear(void);
    void toggle(void);

    static void setup(void);

};


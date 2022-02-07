/*
 * gpio.cpp
 *
 *  Created on: 24 de jan de 2022
 *      Author: jacks
 */

#include "gpio.h"
#include "F28x_Project.h"
#include <vector>

uint16_t OutPin::count = 0;
uint16_t OutPin::pin_list[OutPin::pin_list_capacity];

void OutPin::setup(void)
{
    for(int k=0; k<count; k++)
    {
        GPIO_SetupPinMux(pin_list[k], GPIO_MUX_CPU1, 0);
        GPIO_SetupPinOptions(pin_list[k], GPIO_OUTPUT, GPIO_PUSHPULL);
    }
}

OutPin::OutPin(uint16_t gpio_num, pin_active_logic active_logic)
    :gpio_num(gpio_num), active_logic(active_logic)
{
    pin_list[count++] = gpio_num;
}

void OutPin::set()
{
    GPIO_WritePin(gpio_num, active_logic);
}

void OutPin::clear()
{
    GPIO_WritePin(gpio_num, !active_logic);
}

void OutPin::toggle()
{
    if(gpio_num < 32)
        GpioDataRegs.GPATOGGLE.all |= 1 << gpio_num;
    else if(gpio_num < 64)
        GpioDataRegs.GPBTOGGLE.all |= 1 << (gpio_num-32);
    else if(gpio_num < 96)
        GpioDataRegs.GPCTOGGLE.all |= 1 << (gpio_num-64);
}

/*
 * parameters.h
 *
 *  Created on: Jan 31, 2022
 *      Author: j-Lago
 */

#pragma once

#include "fixed_string.h"

enum ParType{read_only, read_write, error};

class Parameter
{
public:
    fixed_string<16> name;          //W???(read_write), R???(read_only), E???(error)
    fixed_string<16> um;            // unidade de medida
    fixed_string<32> description;
    ParType type;                   // {read_write, read_write, error}
    float min_value;
    float max_value;
    float default_value;
    float value;

    Parameter(void);
    Parameter(ParType type, const char* name,const char* um, float min, float max, float deflt, const char* description);
    bool set_value(float new_value);
};

/*
 * parameters.cpp
 *
 *  Created on: Jan 31, 2022
 *      Author: j-Lago
 */

#include "fixed_string.h"
#include "parameters.h"


Parameter::Parameter(ParType type, const char* name,const char* um, float min, float max, float deflt, const char* description)
    : type(type), min_value(min), max_value(max), default_value(deflt), value(deflt)
{
    this->um << um;
    this->name << name;
    this->description << description;
}

bool Parameter::set_value(float new_value)
{
    if(new_value > max_value)
    {
        value = max_value;
        return false;
    }

    if(new_value < min_value)
    {
        value = min_value;
        return false;
    }

    value = new_value;
    return true;
}




/*
 * parameters.cpp
 *
 *  Created on: Jan 31, 2022
 *      Author: j-Lago
 */

#include "fixed_string.h"
#include "parameters.h"
#include "cla.h"
#include "globals.h"
#include "parameters_table.h"

fParReadWrite ParList::parW[] = PARAMETERS_W;
fParReadOnly  ParList::parR[] = PARAMETERS_R;
fParError  ParList::parE[] = PARAMETERS_E;


uint16_t fParReadWrite::instance_count = 0;
uint16_t fParReadOnly::instance_count = 0;
uint16_t fParError::instance_count = 0;

const uint16_t ParList::len = sizeof(parW)/sizeof(parW[0]) + sizeof(parR)/sizeof(parR[0]) + sizeof(parE)/sizeof(parE[0]);
fParBase* ParList::par[ParList::len];

//------------------------------------------------------------------------

fParBase::fParBase(float* varptr, fixed_string<5> name, fixed_string<5> um, string32 description, float dfault)
    : varptr(varptr), dfault(dfault)
{
    this->name << name;
    this->um << um;
    this->description << description;
    //*varptr = dfault;
}

float fParBase::getValue(void) const { return *varptr; }
void fParBase::loadDefault(void) { *varptr = dfault; }
void fParBase::_forceSetValue(float new_value){ *varptr = new_value; }

//------------------------------------------------------------------------

fParError::fParError(float* varptr, fixed_string<5> name, fixed_string<5> um, string32 description, float dfault)
    : fParBase(varptr, name, um, description, dfault) { instance_count++; }

ParType fParError::getType(void) { return error; }

ParError fParError::setValue(float new_value){ return try_write; }

//------------------------------------------------------------------------


fParReadOnly::fParReadOnly(float* varptr, fixed_string<5> name, fixed_string<5> um, string32 description, float dfault)
    : fParBase(varptr, name, um, description, dfault) { instance_count++; }

ParType fParReadOnly::getType(void) { return read_only; }

ParError fParReadOnly::setValue(float new_value){ return try_write; }

//------------------------------------------------------------------------


fParReadWrite::fParReadWrite(float* varptr, fixed_string<5> name, fixed_string<5> um, string32 description, float dfault, float min, float max)
    : fParBase(varptr, name, um, description, dfault), min(min), max(max) { instance_count++; }

float fParReadWrite::getMin() {return min;}
float fParReadWrite::getMax() {return max;}

ParType fParReadWrite::getType(void) { return read_write; }

ParError fParReadWrite::setValue(float new_value)
{
    if(new_value > max)
    {
        _forceSetValue(max);
        return limit_max;
    }
    else if(new_value < min)
    {
        _forceSetValue(min);
        return limit_min;
    }

    _forceSetValue(new_value);
    return no_err;
}

//------------------------------------------------------------------------

fParBase* ParList::operator[](const uint16_t id) const
{
    return par[id];
}

void ParList::loadDefaults()
{
    for(uint16_t k=0; k<len ; k++)
        par[k]->loadDefault();
}

//singleton
ParList ParList::instance;
ParList& ParList::getInstance(){ return instance; }
ParList::ParList()
{

    uint16_t id_offset;

    id_offset = 0;
    for(uint16_t k=0; k < fParReadWrite::instance_count; k++)
        par[k+id_offset] = &parW[k];

    id_offset = fParReadWrite::instance_count;
    for(uint16_t k=0; k < fParReadOnly::instance_count; k++)
      par[k+id_offset] = &parR[k];

    id_offset += fParReadOnly::instance_count;
    for(uint16_t k=0; k < fParError::instance_count; k++)
      par[k+id_offset] = &parE[k];

    id_offset += fParError::instance_count;
}



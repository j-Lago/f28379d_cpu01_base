/*
 * parameters.h
 *
 *  Created on: Jan 31, 2022
 *      Author: j-Lago
 */

#pragma once

#include "F28x_Project.h"
#include "fixed_string.h"
#include <iterator>

enum ParType  {read_only, read_write, error};
enum ParError {no_err = 0, limit_min, limit_max, try_write};



class fParBase
{
private:
    float* varptr;
    float dfault;

public:
    fixed_string<5> name;
    fixed_string<5> um;
    fixed_string<32> description;

    fParBase(float* varptr, fixed_string<5> name, fixed_string<5> um, string32 description, float dfault);
    float getValue(void) const;
    void loadDefault(void);
    virtual ParError setValue(float new_value) = 0;
    virtual ParType getType(void) = 0;

protected:
    void _forceSetValue(float new_value);
};


class fParError : public fParBase
{
public:
    static uint16_t instance_count;

    fParError(float* varptr, fixed_string<5> name, fixed_string<5> um, string32 description, float dfault);
    virtual ParType getType(void);
    virtual ParError setValue(float new_value);
};


class fParReadOnly : public fParBase
{
public:
    static uint16_t instance_count;

    fParReadOnly(float* varptr, fixed_string<5> name, fixed_string<5> um, string32 description, float dfault);
    virtual ParType getType(void);
    virtual ParError setValue(float new_value);
};


class fParReadWrite : public fParBase
{
private:
    float min;
    float max;

public:
    static uint16_t instance_count;

    fParReadWrite(float* varptr, fixed_string<5> name, fixed_string<5> um, string32 description, float dfault, float min, float max);
    float getMin();
    float getMax();
    virtual ParType getType(void);
    virtual ParError setValue(float new_value);
};


class ParList
{
public:
    static fParReadWrite parW[];
    static fParReadOnly  parR[];
    static fParError     parE[];
    static const uint16_t len;
    static fParBase* par[];

    fParBase* operator[](const uint16_t id) const;
    void loadDefaults();

//singleton
private:
    static ParList instance;
    ParList();
public:
    static ParList& getInstance();
    ParList(const ParList&) = delete;  // impede construtor de cópia
};


/*
 * Nextion.cpp
 *
 *  Created on: 18 de jan de 2022
 *      Author: j-Lago
 */

#include "F28x_Project.h"
#include "fixed_string.h"
#include "globals.h"
#include "sci.h"
#include "parameters_default.h"
#include "pwm.h"
#include "nextion.h"


using namespace Nextion;


//--------------------------------------------------------------------------

const char *Comm::EOM = "ÿÿÿ";
SCI* Comm::port = &sci;

void Comm::send(const fixed_string<16>& s16)    { port->push(s16);          }
void Comm::send(const char* str)                { port->push(str);          }
void Comm::sendEOM(fixed_string<16>& s16)       { port->push(s16 << EOM);   }
void Comm::send(const fixed_string<32>& s32)    { port->push(s32);          }
void Comm::sendEOM(fixed_string<32>& s32)       { port->push(s32 << EOM);   }

//--------------------------------------------------------------------------

Component::Component(const char* name)  { this->objname << name; }
Component::Component(string16 name)     { this->objname << name; }

//--------------------------------------------------------------------------

uint16_t Page::active_page = 0;

uint16_t Page::activePage() { return active_page;   }

void Page::changePage(uint16_t id)
{
    active_page = id;
    fixed_string<16> s16;
    s16 << "page " << id;
    sendEOM(s16);
}

//--------------------------------------------------------------------------


Text32::Text32(const char* name)
    : Component(name)
{
    //this->name << name;
    refresh_color = false;
    txt_color = black;
}

Text32::Text32(string16 name)
    : Component(name)
{
    //this->name << name;
    refresh_color = false;
    txt_color = black;
}


fixed_string<32>& Text32::clear()
{
    text.clear();
    //color(black);
    return text;
}


fixed_string<32>& Text32::txt(const char* str)
{
    text.clear();
    text << str;
    return text;
}

fixed_string<32>& Text32::txt(string16 str)
{
    text.clear();
    text << str;
    return text;
}

fixed_string<32>& Text32::txt(string32 str)
{
    text.clear();
    text << str;
    return text;
}

void Text32::color(hmi_colors new_color)
{
    txt_color = new_color;
    refresh_color = true;
}



fixed_string<32>& Text32::operator<<(const char* str)
{
    text += str;
    return text;
}

fixed_string<32>& Text32::operator<<(string16 str)
{
    text << str;
    return text;
}

fixed_string<32>& Text32::operator<<(string32 str)
{
    text << str;
    return text;
}


void Text32::update(bool force)
{
    string16 temp;
    send(temp << objname << ".txt=\"");
    send(text);
    send("\"ÿÿÿ");

    if(force | refresh_color)
    {
        temp.clear();
        send(temp << objname << ".pco=" << txt_color << "ÿÿÿ" );
    }
}


//--------------------------------------------------------------------------


VarNum::VarNum(const char* name, int16_t value)
    : Component(name), value(value) {}
VarNum::VarNum(string16 name, int16_t value)
    : Component(name), value(value) {}

void VarNum::val(int16_t value)
{
    this->value = value;
}

uint16_t VarNum::val() const
{
    return value;
}

void VarNum::update(bool force)
{
    string32 temp;
    send(temp << objname << ".val=" << (uint16_t)value << "ÿÿÿ");
}


//--------------------------------------------------------------------------


Picture2::Picture2(const char* name, uint16_t pic1, uint16_t pic2, bool state)
    : Component(name), pic1(pic1), pic2(pic2), _state(state) {}

Picture2::Picture2(string16 name, uint16_t pic1, uint16_t pic2, bool state)
    : Component(name), pic1(pic1), pic2(pic2), _state(state) {}

void Picture2::state(int state)     { _state = (state > 0)? true : false;   }
bool Picture2::state() const        { return _state;                        }
bool Picture2::toggle()             { _state = ! _state; return _state;     }

void Picture2::update(bool force)
{
    string32 temp;
    send(temp << objname << ".pic=" <<  ( _state? pic1 : pic2) << "ÿÿÿ");
}


//--------------------------------------------------------------------------


Slider::Slider(const char* name, int16_t value)
    : Component(name), value(value) { refresh_value = true; }
Slider::Slider(string16 name, int16_t value)
    : Component(name), value(value) { refresh_value = true; }

void Slider::val(int16_t value, bool force)
{
    int16_t old_value = this->value;
    this->value = value;
    if(old_value != value | force)
        refresh_value = true;
}

uint16_t Slider::val() const
{
    return value;
}

void Slider::update(bool force)
{
    if(force | refresh_value)
    {
        string32 temp;
        send(temp << objname << ".val=" << (uint16_t)value << "ÿÿÿ");
        refresh_value = false;
    }
}

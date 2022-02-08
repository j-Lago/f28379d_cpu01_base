/*
 * nextion.h
 *
 *  Created on: 18 de jan de 2022
 *      Author: j-Lago
 */

#pragma once

#include "parameters_default.h"
#include "sci.h"
#include "pwm.h"
#include "fixed_string.h"


namespace Nextion
{

    enum hmi_colors{black=0,
                    white=65535,
                    gray=33808,
                    blue=31,
                    cornflowerblue=17139,
                    red=63488,
                    terracotta=45476,
                    green=1032,
                    velvetwine=39435,
                    royalblue=17244,
                    darkslateblue=16881,
                    sapphire=2711,
                    lightseagreen=9621,
                    darkmossgreen=19172,
                    kalegreen=15145,
                    olivegreen=21317,
                    };

    // classe base. Não deve ser instanciada.
    class Comm
    {
    public:
        static const char* EOM; // End of Message: nextion messages ends with three 0xff characters
        static SCI& port;   // pointer to SCI (UART) object

    public:
        void sendEOM(fixed_string<16>& s16); // add END to the message and sends it through sci
        void sendEOM(fixed_string<32>& s32); // add END to the message and sends it through sci
        void send(const fixed_string<16>& s16); // message the message as is through sci
        void send(const fixed_string<32>& s32); // message the message as is through sci
        void send(const char* str);
    };



    // classe base. Não deve ser instanciada.
    class Component : public Comm
    {
    public:
        string16 objname;

    public:
        Component(const char* name);
        Component(string16 name);

        virtual void update(bool force = false) = 0; //implementado nas classes filhas
    };



    class Page : public Comm
    {
        static uint16_t active_page;
        uint16_t activePage(void);
        void changePage(uint16_t);

        virtual void refresh(void) = 0; //implementado nas classes filhas
        virtual void decodeMessage(fixed_string<16> s16) = 0; //implementado nas classes filhas

    };


    class Text32 : Nextion::Component
    {
    private:
        bool refresh_color;
        uint16_t txt_color;
    public:
        fixed_string<32> text;

        Text32(const char* name);
        Text32(string16 name);
        fixed_string<32>& clear();

        fixed_string<32>& operator<<(const char *str);
        fixed_string<32>& operator<<(string16 name);
        fixed_string<32>& operator<<(string32 name);

        fixed_string<32>& txt(const char *str);
        fixed_string<32>& txt(string16 name);
        fixed_string<32>& txt(string32 name);

        void color(hmi_colors new_color);
        void update(bool force = false);
    };


    class VarNum : public Component
    {
    private:
        int16_t value;

    public:
        VarNum(const char* name, int16_t value = 0);
        VarNum(string16 name, int16_t value = 0);
        void val(int16_t value);
        uint16_t val() const;
        void update(bool force = false);
    };


    // 2 state picture (to implement 2 state button)
    class Picture2 : public Component
    {
    public:
        uint16_t pic1;
        uint16_t pic2;
        bool _state;

    public:
        Picture2(const char* name, uint16_t pic1, uint16_t pic2, bool state = false);
        Picture2(string16 name, uint16_t pic1, uint16_t pic2, bool state = false);

        void state(int state);
        bool state() const;
        bool toggle();

        void update(bool force = false);
    };



    class Slider : public Component
    {
    private:
        int16_t value;
        bool refresh_value;

    public:
        Slider(const char* name, int16_t value = 0);
        Slider(string16 name, int16_t value = 0);
        void val(int16_t value, bool force = false);
        uint16_t val() const;
        void update(bool force = false);
    };


};


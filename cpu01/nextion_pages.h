/*
 * nextion_pages.h
 *
 *  Created on: Feb 5, 2022
 *      Author: j-Lago
 */

#pragma once
#include "nextion.h"

using namespace Nextion;

enum hmi_mode {hmi_update, hmi_edit, hmi_err, hmi_refresh, hmi_slider, hmi_waiting, hmi_selection, hmi_init};
enum focus_error {limit_min, limit_max, try_edit, no_err};

class NextionP0 : public Nextion::Page
{
public:
    Text32 txt_log          = {"t5"};
    Text32 txt_input        = {"t0"};
    Text32 txt_name         = {"t1"};
    Text32 txt_description  = {"t2"};
    Text32 txt_error        = {"t3"};
    Text32 txt_unit         = {"t4"};
    VarNum num_state        = {"state", 1};
    Picture2 btn_reverse    = {"p2", 59, 60, true};
    Slider sld_slider       = {"h1"};

    VSI3fPWM* modulator;
    hmi_mode mode;
    focus_error last_err;

    string16 input;
    bool cursor_status;
    int16_t cursor_pos;

    bool pending_refresh;

    int16_t par_sel;
    static Parameter par[];

    NextionP0(VSI3fPWM* modulator);
    void inc_sel();
    void dec_sel();
    void inc_cursor();
    void dec_cursor();
    void toggle_state();
    void reverse_rotation();
    void char_insert(char key);
    void char_remove();

    void load_refresh();
    void periodic_refresh();
    void editing_refresh();
    void update_refresh();
    void sel_refresh();


    void refresh(void);
    void decodeMessage(fixed_string<16> s16_rx);
    void pressedKey(char key);
    void handleCommand(fixed_string<16> s16_rx);
    void handleSlider(fixed_string<16> s16_rx);
};


/*
 * nextion_pages.cpp
 *
 *  Created on: Feb 5, 2022
 *      Author: j-Lago
 */

#include "globals.h"
#include "nextion.h"

//using namespace Nextion;

Parameter NextionP0::par[PAR_SIZE] = PAR_INIT;
PWM& NextionP0::modulator = PWM::getInstance();


// Nextion page 0
NextionP0::NextionP0()
{
    mode = hmi_init;
    last_err = no_err;
    cursor_status = false;
    pending_refresh = true;
    cursor_pos = 0;
    par_sel = 0;

}

void NextionP0::inc_sel()
{
    if(++par_sel >= PAR_SIZE)
        par_sel = 0;
    sel_refresh();
}

void NextionP0::dec_sel()
{
    if(--par_sel < 0)
        par_sel = PAR_SIZE-1;
    sel_refresh();
}

void NextionP0::inc_cursor()
{
    bool cursor_status_ = cursor_status;
    cursor_status = true;

    if(!cursor_status_)
            cursor_pos = 0;
    else if(cursor_pos++ >= input.len-1)
        cursor_status = false;

    editing_refresh();
}

void NextionP0::dec_cursor()
{
    bool cursor_status_ = cursor_status;
        cursor_status = true;

    if(!cursor_status_)
        cursor_pos = input.len-1;
    else if(cursor_pos-- <= 0)
        cursor_status = false;

    editing_refresh();
}

void NextionP0::toggle_state()
{
    modulator.en = !modulator.en;
    load_refresh();
}

void NextionP0::reverse_rotation()
{
    if(cla_dir >= 0.0f)
        cla_dir = -1.0f;
    else
        cla_dir = 1.0f;

    load_refresh();
}

void NextionP0::char_insert(char key)
{
    if(cursor_status)
    {
        input.set(cursor_pos, key);
        inc_cursor();
    }
    else
        input << key;

    editing_refresh();
}

void NextionP0::char_remove()
{
    if(cursor_status)
    {
        input.remove(cursor_pos);
        inc_cursor();

        editing_refresh();
    }
    else
        input.backspace();

    editing_refresh();
}


void NextionP0::decodeMessage(fixed_string<16> s16_rx)
{

    if (s16_rx.len == 1)
    {
        pressedKey(s16_rx[0]);
    }
    else if (s16_rx.len == 2)
    {
        handleCommand(s16_rx);
    }
    else
    {
        handleSlider(s16_rx);
    }

}

void NextionP0::handleCommand(fixed_string<16> s16_rx)
{
    // toma a devida ação para cada mensagem recebida
    //------------------------------------------------------
    if      (s16_rx == "su") { dec_sel();           }
    else if (s16_rx == "sd") { inc_sel();           }
    else if (s16_rx == "sr") { inc_cursor();        }
    else if (s16_rx == "sl") { dec_cursor();        }
    //------------------------------------------------------
    else if (s16_rx == "io") { toggle_state();      }
    else if (s16_rx == "ir") { reverse_rotation();  }
    //------------------------------------------------------
    //else if (s16_rx == "p0") { changePage(0);       }
    //else if (s16_rx == "p1") { changePage(1);       }
    //else if (s16_rx == "p2") { changePage(2);       }
    //else if (s16_rx == "p3") { changePage(3);       }
    //else if (s16_rx == "p4") { changePage(4);       }
    else                     { /* ação para mensagem desconhecida */ }
    //------------------------------------------------------
}

void NextionP0::handleSlider(fixed_string<16> s16_rx)
{
    if(par[par_sel].type == read_only){
        mode = hmi_err;
        last_err = try_edit;
    }
    else{
        par[par_sel].set_value( ((float)s16_rx[2]) * 0.004f * (par[par_sel].max_value-par[par_sel].min_value) + par[par_sel].min_value);
        mode = hmi_slider;
    }

    pending_refresh = true;
}

void NextionP0::pressedKey(char key)
{
    if (input.len >= 31)
        input.clear();

    if      (key == 0x0d)         { update_refresh();                                  } // enter
    else if (key == 0x7f)         { input.clear(); editing_refresh();                  } // clear
    else if (key == 0x08)         { char_remove();                                     } // backspace
    else if (key>='0' & key<='9') { char_insert(key);                                  } // 0..9
    else if (key == '.')          { if(!input.contains('.')) char_insert(key);         } // .
    else if (key == '-')          { if     (input.char_cmp(0, '-')) input.set(0, '+');
                                    else if(input.char_cmp(0, '+')) input.set(0, '-');
                                    else                     input.push_left('-');
                                    editing_refresh();                                     } //

}



void NextionP0::periodic_refresh()
{
    if (mode == hmi_waiting)
    {
        mode = hmi_refresh;
        pending_refresh = true;
    }
    else if(mode == hmi_err)
        pending_refresh = true;
}

void NextionP0::editing_refresh()
{
    mode = hmi_edit;
    pending_refresh = true;
}

void NextionP0::update_refresh()
{
    mode = hmi_update;
    pending_refresh = true;
}

void NextionP0::load_refresh()
{
    mode = hmi_refresh;
    pending_refresh = true;
}

void NextionP0::sel_refresh()
{
    mode = hmi_selection;
    pending_refresh = true;
}



void NextionP0::refresh(void)
{
    if(pending_refresh)
    {
        txt_name.txt( par[par_sel].name );
        txt_description.txt(par[par_sel].description);
        txt_unit.txt(par[par_sel].um );


        if(mode == hmi_refresh | mode == hmi_slider | mode == hmi_selection | mode == hmi_init)
        {
            cursor_status = false;
            input.clear();
            input << par[par_sel].value; // load par value
        }
        else if(mode == hmi_update)
        {
            cursor_status = false;
            if(par[par_sel].type != read_only)
            {
                if (!par[par_sel].set_value(input.to_float()))
                {
                    mode = hmi_err;
                    last_err = (par[par_sel].value == par[par_sel].max_value) ? limit_max : limit_min;
                }
            }

            input.clear();
            input << par[par_sel].value;
        }

        // sinaliza erro se tentar editar vaiável somente leitura
        if( (mode == hmi_edit | mode == hmi_slider) & par[par_sel].type == read_only)
        {
            mode = hmi_err;
            last_err = try_edit;

            input.clear();
            input << par[par_sel].value;
        }

        // atualiza slider para modos diferentes de edit
        uint16_t h1val;
        static hmi_mode mode_;
        if(mode == hmi_update | mode == hmi_refresh | mode == hmi_err){
            h1val = (1.0f / (par[par_sel].max_value - par[par_sel].min_value) * (par[par_sel].value - par[par_sel].min_value) * 250.0f);
            sld_slider.val(h1val, (mode_ != mode & mode == hmi_err) | mode == hmi_update); // só atualiza slider quando há mudanças
            mode_ = mode;
        }

        // modo insert
        txt_input.txt(input);
        if(cursor_status)
            txt_input.text.set(cursor_pos, '_');

        // input field color
        switch(mode)
        {
                case hmi_err:   {txt_input.color(red);      }
        break; case hmi_edit:   {txt_input.color(blue);     }
        break; default:         {txt_input.color(black);    }
        }

        // parameter name and description color
        switch(par[par_sel].type)
        {
                case error       :  { txt_name.color(terracotta);       txt_description.color(terracotta);      }
         break; case read_write  :  { txt_name.color(darkslateblue);    txt_description.color(darkslateblue);   }
         break; default          :  { txt_name.color(kalegreen);        txt_description.color(kalegreen);       }
        }


        num_state.val(modulator.en);    // ayaliza status
        btn_reverse.state(cla_dir);   // botão direção

        // msg erro
        if(mode == hmi_err)
            switch(last_err)
            {
                   case limit_min   :  txt_error.txt("value < MIN");
            break; case limit_max   :  txt_error.txt("value > MAX");
            break; case try_edit    :  txt_error.txt("read only");
            }
        else
            txt_error.clear();


        /*
        // log
        txt_log.txt("log:\\r  ");
        switch(mode)
        {
                case hmi_update:    {txt_log << "value updated";          }
        break;  case hmi_edit:      {txt_log << "edit mode";              }
        break;  case hmi_err:       {txt_log << "input error";            }
        break;  case hmi_slider:    {txt_log << "slider changed";         }
        break;  case hmi_selection: {txt_log << "par selection";          }
        break;  case hmi_refresh:   {txt_log << "hmi_refresh";            }
        break;  case hmi_waiting:   {txt_log << "hmi_waiting";            }
        break;  case hmi_init:      {txt_log << "hmi_init";               }
        }

        txt_log << "\\r  par_id: " << (long) par_sel;
        txt_log.color( (mode == hmi_err) ? red : gray);

        if(mode != hmi_refresh)
            txt_log.update();
        */

        txt_name.update();
        txt_description.update();
        txt_unit.update();
        txt_error.update();
        txt_input.update();
        num_state.update();
        btn_reverse.update();
        sld_slider.update();

        if(mode != hmi_edit & mode != hmi_err)
            mode = hmi_waiting;

        port.dump();
        pending_refresh = false;

    }



}






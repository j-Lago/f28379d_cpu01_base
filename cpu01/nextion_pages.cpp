/*
 * nextion_pages.cpp
 *
 *  Created on: Feb 5, 2022
 *      Author: j-Lago
 */

#include "globals.h"
#include "nextion.h"
#include "cla.h"

PWM& NextionP0::modulator = PWM::getInstance();
ParList& NextionP0::par = ParList::getInstance();

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
    if(++par_sel >= ParList::len)
        par_sel = 0;
    sel_refresh();
}

void NextionP0::dec_sel()
{
    if(--par_sel < 0)
        par_sel = ParList::len -1;
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
    // toma a devida a��o para cada mensagem recebida
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
    else                     { /* a��o para mensagem desconhecida */ }
    //------------------------------------------------------
}

void NextionP0::handleSlider(fixed_string<16> s16_rx)
{
    if(par[par_sel]->getType() == read_only){
        mode = hmi_err;
        last_err = try_write;
    }
    else{
        float min = ((fParReadWrite*)par[par_sel])->getMin();
        float max = ((fParReadWrite*)par[par_sel])->getMax();
        par[par_sel]->setValue( ((float)s16_rx[2]) * 0.004f * (max-min ) + min);
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
                                    editing_refresh();                                 } //

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
        txt_name.txt( par[par_sel]->name );
        txt_description.txt(par[par_sel]->description);
        txt_unit.txt(par[par_sel]->um );


        if(mode == hmi_refresh | mode == hmi_slider | mode == hmi_selection | mode == hmi_init)
        {
            cursor_status = false;
            input.clear();
            input << par[par_sel]->getValue(); // load par value
        }
        else if(mode == hmi_update) // ENTER
        {
            cursor_status = false;
            if(par[par_sel]->getType() != read_only)
            {
                last_err = par[par_sel]->setValue(input.to_float());
                if (last_err != no_err)
                    mode = hmi_err;
            }

            input.clear();
            input << par[par_sel]->getValue();
        }

        // sinaliza erro se tentar editar vai�vel somente leitura
        if( (mode == hmi_edit | mode == hmi_slider) & par[par_sel]->getType() == read_only)
        {
            mode = hmi_err;
            last_err = try_write;

            input.clear();
            input << par[par_sel]->getValue();
        }

        // atualiza slider para modos diferentes de edit
        uint16_t h1val;
        static hmi_mode mode_;
        if(mode == hmi_update | mode == hmi_refresh | mode == hmi_err){
            if(par[par_sel]->getType() == read_write)
            {
                float min = ((fParReadWrite*)par[par_sel])->getMin();
                float max = ((fParReadWrite*)par[par_sel])->getMax();
                h1val = 250.0 * ( par[par_sel]->getValue() - min) / (max - min);
                sld_slider.val(h1val, (mode_ != mode & mode == hmi_err) | mode == hmi_update); // s� atualiza slider quando h� mudan�as
            }
            mode_ = mode;
        }

        if( mode == hmi_selection )
            if (par[par_sel]->getType() == read_write)
                sld_slider.enable();
            else
                sld_slider.disable();

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
        switch(par[par_sel]->getType())
        {
                case error       :  { txt_name.color(terracotta);       txt_description.color(terracotta);      }
         break; case read_write  :  { txt_name.color(darkslateblue);    txt_description.color(darkslateblue);   }
         break; default          :  { txt_name.color(kalegreen);        txt_description.color(kalegreen);       }
        }


        num_state.val(modulator.en);    // atuliza status
        btn_reverse.state(cla_dir);   // bot�o dire��o

        // msg erro
        if(mode == hmi_err)
            switch(last_err)
            {
               case limit_min   :  txt_error.txt("value < MIN");
               break; case limit_max   :  txt_error.txt("value > MAX");
               break; case try_write   :  txt_error.txt("read only");
               break; case no_err      :  txt_error.txt("no error");
            }
        else
            txt_error.clear();

        // log
        //txt_log.txt("log:\\r  ");
        //txt_log.text << h1val;
        //txt_log.update();

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






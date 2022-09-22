/*
 * scope.h
 *
 *  Created on: Sep 20, 2022
 *      Author: j-Lago
 */

#pragma once

#include "globals.h"
#include "raspberrypi.h"

typedef enum { empty, buffering, full, sending } ScopeState;

template<size_t buffer_size>
class Scope
{
public:
    int address;
    volatile ScopeState state;
    rPiComm::Comm* hmi;

    float buffer[3][buffer_size];

    float *chA_ptr;
    float *chB_ptr;
    float *chC_ptr;

    int count = 0;

    Scope(rPiComm::Comm *hmi, int address, float *chA_ptr, float *chB_ptr, float *chC_ptr)
    : hmi(hmi),
      address(address),
      chA_ptr(chA_ptr),
      chB_ptr(chB_ptr),
      chC_ptr(chC_ptr),
      state(empty)
    {}

    void sample()
    {
        if(state == buffering)
        {
            if(count < buffer_size)
            {
                //float ch[3] = {*chA_ptr, *chB_ptr, *chC_ptr};
                //hmi->write_float32(ch, 2, address, false);
                buffer[0][count] = *chA_ptr;
                buffer[1][count] = *chB_ptr;
                buffer[2][count] = *chC_ptr;
                count++;
            }else{
                state = full;
            }
        }
    }

    void send()
    {
        //for(int k=0; k<count; k++){
        //    hmi->write_float32(buffer[k], 3, address, true);
        //}
        char preamb[] = {'d', 0xB3, buffer_size, 0x10};
        hmi->write_raw(preamb, 4);

        hmi->write_raw((char*)buffer, 3*buffer_size*4);

        hmi->write_raw(preamb, 1);

        count = 0;
        state = empty;
    }

};

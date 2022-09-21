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


class Scope
{
public:
    int address;
    int buffer_size;
    volatile ScopeState state;
    rPiComm::Comm* hmi;

    float *chA_ptr;
    float *chB_ptr;

    int count = 0;

    Scope(rPiComm::Comm *hmi, int address, float *chA_ptr, float *chB_ptr, int buffer_size=1024)
    : hmi(hmi),
      buffer_size(buffer_size),
      address(address),
      chA_ptr(chA_ptr),
      chB_ptr(chB_ptr),
      state(empty)
    {}

    void sample()
    {
        if(state == buffering)
        {
            if(count < buffer_size)
            {
                float ch[2] = {*chA_ptr, *chB_ptr};
                hmi->write_float32(ch, 2, address, false);
                count++;
            }else{
                state = full;
            }
        }
    }

    void send()
    {
        hmi->ser.dump();
        count = 0;
        state = empty;
    }

};

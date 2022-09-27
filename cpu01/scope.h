/*
 * scope.h
 *
 *  Created on: Sep 20, 2022
 *      Author: j-Lago
 */

#pragma once

#include <stdint.h>
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

    union{
        float data_f[3][buffer_size];
        uint16_t  data_u[3*2*buffer_size];
    };


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

    bool sample()
    {
        if(state != buffering)
            return false;


        if(count < buffer_size)
        {
            data_f[0][count] = *chA_ptr;
            data_f[1][count] = *chB_ptr;
            data_f[2][count] = *chC_ptr;
            count++;
        }
        else
        {
            state = full;
        }

        return true;
    }

    void send()
    {
        //for(int k=0; k<count; k++){
        //    hmi->write_float32(buffer[k], 3, address, true);
        //}
        char preamb[] = {'d', 0xB3, buffer_size, 0x10};
        hmi->write_raw(preamb, 4);

        hmi->write_uint16_raw(data_u, 3*buffer_size*2);

        hmi->write_raw(preamb, 1);

        count = 0;
        state = empty;
    }

};

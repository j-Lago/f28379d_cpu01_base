/*
 * fifo.h
 *
 *  Created on: Jan 31, 2022
 *      Author: jacks
 */

#pragma once

#include "F28x_Project.h"

template<typename T, uint16_t size>
class FIFO
{
public:
    T buffer[size];
    int first;
    unsigned int len;

    FIFO()
    {
        len = 0;
        first = 0;
    }

    void push(T value)
    {
        if (len < size)
        {
            buffer[(first+len)%size] = value;
            len++;
        }
    }

    T pop()
    {
        T ret;
        if (len > 0)
        {
            ret = buffer[first];
            first = ++first % size;
            len--;
        }
        return ret;
    }
};

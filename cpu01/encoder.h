/*
 * eqep.h
 *
 *  Created on: Jul 27, 2022
 *      Author: j-Lago
 */

#pragma once

/*
 * EQEP
 *
 * contador de quadratura pra encoder modelo C38S6G5-1000B-G24N
 *
 */
class Encoder
{
public:
    static const uint16_t PPR = 4 * 1000;
    static const float Fvel = 1000.0f;       // frequencia em que a velocidade será acalculada
    static const float pulsepT_to_radps = (Fvel*6.283185307179586476925286766559f)/(PPR);

    int32_t pulse_count;       // contador de pulsos
    float w;                   // velocidade angular em rad/s

private:
    int32_t pulse_count_;      // valor anterior da contagem de pulsos
    int32_t delta;             //pulsos por período

public:
    static void setup(void);
    static void start(void);

    void calc(void);


//singleton
private:
    static Encoder instance;
    Encoder();
public:
    static Encoder& getInstance();
    Encoder(const Encoder&) = delete;  // impede construtor de cópia
};

/*
 * eqep.h
 *
 *  Created on: Jul 27, 2022
 *      Author: j-Lago
 */

#pragma once

/*
 * Encoder (EQEP)
 *
 * contador de quadratura para encoder modelo C38S6G5-1000B-G24N
 *
 */
class Encoder
{
private:
    static const uint16_t PPR = 4 * 1000;    // contagens por revolução (depende do modo de contagem tudas as bordas/subidas/etc)
    static const float Fvel = 1000.0f;       // frequencia em que a velocidade será calculada
    static const float pulse_to_rad = 6.283185307179586476925286766559f/PPR;
    static const float pulsepT_to_radps = Fvel*pulse_to_rad;

    int32_t pulse_count;    // contador de pulsos
    int32_t pulse_count_;   // valor anterior da contagem de pulsos
    int32_t pulse_zero;     // contagem na posição zero

public:
    float vel;              // velocidade angular [rad/s]
    float pos;              // posicao angular em [rad]

    static void setup(void);
    static void start(void);

    void update_vel(void);
    void update_pos(void);
    void set_zero(void);


//singleton
private:
    static Encoder instance;
    Encoder();
public:
    static Encoder& getInstance();
    Encoder(const Encoder&) = delete;  // impede construtor de cópia
};

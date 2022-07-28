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
    static const float FS = 20000.0f;       // frequência em que a função de calculo de velociade vai ser chamada (deve ser igual a da interrupção dos controladores)
    static const uint16_t DOWNSAMPLE = 10;  // downsample para calculo da velocidade (fs/downsample é a frequência em que o cálculo realmente será realizado)
    static const float pulsepT_to_radps = (FS*6.283185307179586476925286766559f)/(DOWNSAMPLE*PPR);

    int32_t pulse_count;       // contador de pulsos
    float w;                   // velocidade angular em rad/s

private:
    int32_t pulse_count_;      // valor anterior da contagem de pulsos
    uint16_t downsample_count;

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

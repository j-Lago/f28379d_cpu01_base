/*
 * pwm.h
 *
 *  Created on: 10 de jan de 2022
 *      Author: j-Lago
 */

#pragma once
#include "F28x_Project.h"
#include "settings.h"

/*
 * SI3fPWM prov� as configura��es do hardware e m�todos auxiliares para implementa��o de um modulador PWM para um VSI trif�sico.
 *
 *    A classe foi desenvolvida para funcionar da seguinte forma:
 *
 *      - a classe � um singleton, e a instancia deve ser obtida por getInstance;
 *
 *      - o m�todo setup() deve ser chamado na inicializa��o do programa;
 *
 *      - o m�todo start() deve ser chamado antes de entrar no loop infinito para iniciar o contador do PWM;
 *
 *      - a constante CMP_PORTADORA define frequ�ncia da portadora (do tipo triangular):
 *        fc = 200.000.000 / (16 * CMP_PORTADORA)
 *
 *      - a constante CMP_TEMPO_MORTO define o tempo morto inserido entre sinais complementares:
 *        deadtime = (8 * CMP_TEMPO_MORTO) / 200.000.000
 *
 *      - o m�todo enable() habilita os pulsos se o flag fault=false;
 *
 *      - o m�todo disable() interrompe os pulsos PWM;
 *
 *      - o m�todp trip() interrompe os pulsos PWM e seta o flag fault, impedindo subsequente atua��o de enable();
 *
 *      - o m�todo clear() limpa o flag fault;
 *
 *      - o m�todo setComps(float a, float b, float c), para valores de a,b e c entre -1 e 1, altera as raz�es
 *        c�clicas nas 3 fases, com efeito para o pr�ximo ciclo da portadora.
 *
 */
class PWM
{
private:
    static const float fsw = 10000.0f;  // frequencia de comuta��o 10 kHz

    static const uint16_t CMP_TEMPO_MORTO = 38; // CMP_TEMPO_MORTO*40ns: 38 -> 1.52us
    static const uint16_t CMP_PORTADORA = (uint16_t)(CPU_CLOCK / 16 / fsw);   // = 1250 para 10kHz

    static const float GAIN = (float)(CMP_PORTADORA)*0.5f - (float)CMP_TEMPO_MORTO*0.25f;
    static const float BIAS = (float)(CMP_PORTADORA)*0.5f + (float)(CMP_TEMPO_MORTO+1)*0.25f;

public:
    bool en;
    bool fault;
    float a;  // range: -1...+1
    float b;  // range: -1...+1
    float c;  // range: -1...+1

    static void setup(void);
    static void start(void);
    bool enable(void);
    void disable(void);
    void trip();
    void clear();
    void update();
    void setComps(float a, float b, float c);
    void setComps(float* abc);

//singleton
private:
    static PWM instance;
    PWM();
public:
    static PWM& getInstance();
    PWM(const PWM&) = delete;  // impede construtor de c�pia
};


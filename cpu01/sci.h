/*
 * sci.h
 *
 *  Created on: 12 de jan de 2022
 *      Author: j-Lago
 *
 *  Descrição: Configuração comunicação UART
 *
 *  - bauld rate = 115200 bps
 */


#pragma once

#include "fifo.h"
#include "fixed_string.h"


#if CPU_FRQ_200MHZ
 #ifndef CPU_CLOCK
  #define CPU_CLOCK 200000000
 #endif
#endif


interrupt void sciaRxFifoIsr(void);
interrupt void sciaTxFifoIsr(void);


/*
 * SCI provê as configurações do hardware e métodos auxiliares para implementação de comunicação UART full duplex
 *
 *    A classe foi desenvolvida para funcionar da seguinte forma:
 *
 *      - a classe é um singleton, e a instancia deve ser obtida por getInstance;
 *
 *      - o método setup() deve ser chamado na inicialização do programa (static method);
 *
 *      - o método start() deve ser chamado antes de entrar no loop infinito para habilitar as interrupções (static method);
 *
 *      - a constante BAUD_RATE define o baud rate em bps :
 *        (ScicRegs.SCIHBAUD e ScicRegs.SCILBAUD) -> baud_count = CPU_CLOCK / 8 / 8 / BAUD_RATE - 1
 *
 *      - configuração padrão: 1 stop bit, no parity, 8 char bits (ScicRegs.SCIFFCT);
 *
 *      - a classe trabalha com um buffer FIFO_TX auxiliar, de tamanho FIFO_TX_SIZE, adicionalmente ao FIFO
 *        de 16 posições do hardware. Os métodos auxiliares dessa classe não escrevem no FIFO_TX do hardware, mas
 *        sim no FIFO_TX implementado por software. A transferência do FIFO_TX software para o FIFO_TX hardware é feita
 *        pelo método dump(). Isso permite que dump() seja executado mais facilmente no loop infinito de main(), e
 *        o controle de escrita no FIFO_TX de hardware (de apenas 16 posições) não seja feito dentro de interrupções,
 *        possibilitando priorização de interrupções mais importante como controle/proteção/modulaçao em detrimento da
 *        comunicação (que é lenta, menos prioritária e é feita quando sobrar tempo).
 *
 *      - os métodos push() registram os dados a serem enviados no fifo_tx implementado em software;
 *
 *      - o método dumb() envia à porta serial os dados previamente adicionados ao fifo_tx;
 *
 */
class SCI
{
public:
    static const uint32_t BAUD_RATE = 250000; // ~250 kbps
    static const uint16_t FIFO_TX_SIZE = 256;

    FIFO<char, FIFO_TX_SIZE> fifo_tx;

    static void setup(void);
    static void start(void);

    void push(const fixed_string<16>& s16);
    void push(const fixed_string<32>& s32);
    void push(const char* str);
    void dump(void);

//singleton
private:
    static SCI instance;
    SCI();
public:
    static SCI& getInstance();
    SCI(const SCI&) = delete;  // impede construtor de cópia
};


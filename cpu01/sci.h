/*
 * sci.h
 *
 *  Created on: 12 de jan de 2022
 *      Author: j-Lago
 *
 *  Descri��o: Configura��o comunica��o UART
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
 * SCI prov� as configura��es do hardware e m�todos auxiliares para implementa��o de comunica��o UART full duplex
 *
 *    A classe foi desenvolvida para funcionar da seguinte forma:
 *
 *      - a classe � um singleton, e a instancia deve ser obtida por getInstance;
 *
 *      - o m�todo setup() deve ser chamado na inicializa��o do programa (static method);
 *
 *      - o m�todo start() deve ser chamado antes de entrar no loop infinito para habilitar as interrup��es (static method);
 *
 *      - a constante BAUD_RATE define o baud rate em bps :
 *        (ScicRegs.SCIHBAUD e ScicRegs.SCILBAUD) -> baud_count = CPU_CLOCK / 8 / 8 / BAUD_RATE - 1
 *
 *      - configura��o padr�o: 1 stop bit, no parity, 8 char bits (ScicRegs.SCIFFCT);
 *
 *      - a classe trabalha com um buffer FIFO_TX auxiliar, de tamanho FIFO_TX_SIZE, adicionalmente ao FIFO
 *        de 16 posi��es do hardware. Os m�todos auxiliares dessa classe n�o escrevem no FIFO_TX do hardware, mas
 *        sim no FIFO_TX implementado por software. A transfer�ncia do FIFO_TX software para o FIFO_TX hardware � feita
 *        pelo m�todo dump(). Isso permite que dump() seja executado mais facilmente no loop infinito de main(), e
 *        o controle de escrita no FIFO_TX de hardware (de apenas 16 posi��es) n�o seja feito dentro de interrup��es,
 *        possibilitando prioriza��o de interrup��es mais importante como controle/prote��o/modula�ao em detrimento da
 *        comunica��o (que � lenta, menos priorit�ria e � feita quando sobrar tempo).
 *
 *      - os m�todos push() registram os dados a serem enviados no fifo_tx implementado em software;
 *
 *      - o m�todo dumb() envia � porta serial os dados previamente adicionados ao fifo_tx;
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
    SCI(const SCI&) = delete;  // impede construtor de c�pia
};


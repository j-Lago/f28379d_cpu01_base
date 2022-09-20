/*
 * sci.cpp
 *
 *  Created on: 12 de jan de 2022
 *      Author: j-Lago
 */

#include "F28x_Project.h"
#include <stdio.h>
#include "fixed_string.h"
#include "sci.h"
#include "globals.h"


void SCI::setup(void)
{
    EALLOW;

    PieVectTable.SCIC_RX_INT = &sciaRxFifoIsr;
    PieVectTable.SCIC_TX_INT = &sciaTxFifoIsr;


#ifdef CPU1
    GPIO_SetupPinMux(139, GPIO_MUX_CPU1, 6);
    GPIO_SetupPinOptions(139, GPIO_INPUT, GPIO_PUSHPULL);
    GPIO_SetupPinMux(56, GPIO_MUX_CPU1, 6);
    GPIO_SetupPinOptions(56, GPIO_OUTPUT, GPIO_ASYNC);
#endif

    EDIS;

    ScicRegs.SCIFFTX.all = 0xC02F;
    ScicRegs.SCIFFRX.all = 0x0024;
    ScicRegs.SCIFFCT.all = 0x00;



   ScicRegs.SCICCR.all = 0x0007;      // 1 stop bit,  No loopback
                                      // No parity,8 char bits,
                                      // async mode, idle-line protocol
   ScicRegs.SCICTL1.all = 0x0003;     // enable TX, RX, internal SCICLK,
                                      // Disable RX ERR, SLEEP, TXWAKE

   EALLOW;
   ClkCfgRegs.LOSPCP.bit.LSPCLKDIV = 1;   // muda o divisor de clock de 8 para 4
   EDIS;

   ScicRegs.SCICTL2.bit.TXINTENA = 1;
   ScicRegs.SCICTL2.bit.RXBKINTENA = 1;

   uint32_t baud_count = round(CPU_CLOCK / 4.0 / 8.0 / BAUD_RATE - 1);

   ScicRegs.SCIHBAUD.all = (baud_count>>16) & 0xff;
   ScicRegs.SCILBAUD.all = baud_count & 0xff;

   ScicRegs.SCICCR.bit.LOOPBKENA = 0; // disable loop back
   ScicRegs.SCIFFTX.bit.TXFIFORESET = 1;   // reset FIFO tx

   ScicRegs.SCIFFRX.bit.RXFFIL = 1;   // call interrupt at every receive char
   ScicRegs.SCIFFRX.bit.RXFIFORESET = 1;   // reset FIFO rx


}

void SCI::start()
{
    IER |= M_INT8;  // uart
    PieCtrlRegs.PIEIER8.bit.INTx5 = 1;   // PIE Group 8, SCI-C RX
    PieCtrlRegs.PIEIER8.bit.INTx6 = 0;   // PIE Group 8, SCI-C TX

    EALLOW;
    ScicRegs.SCICTL1.all = 0x0023;          // habilita SCI
    ScicRegs.SCIFFRX.bit.RXFFOVRCLR = 1;    // clear Overflow flag
    ScicRegs.SCIFFRX.bit.RXFFINTCLR = 1;    // clear Interrupt flag
    EDIS;
}

void SCI::push(const fixed_string<16>& s16)
{
    for(uint16_t k = 0; k < s16.len & k < 16; k++)
        fifo_tx.push(s16[k]);
}

void SCI::push(const fixed_string<32>& s32)
{
    for(uint16_t k = 0; k < s32.len & k < 32; k++)
        fifo_tx.push(s32[k]);
}


void SCI::push(const char* str)
{
    uint16_t k=0;
    while(str[k] != 0)
        fifo_tx.push(str[k++]);
}

void SCI::push(char c)
{
        fifo_tx.push(c);
}

int SCI::len()
{
    return fifo_tx.len;
}

int SCI::dump()
{
    /*
     while (fifo_tx.len > 0){
        while (ScicRegs.SCIFFTX.bit.TXFFST != 0); // aguarda esvaziar buffer de envio (envia de um por um, poderia nviar de 16 em 16)
        ScicRegs.SCITXBUF.all = fifo_tx.pop();
    }
    */
    int dump_count = fifo_tx.len;
    while(fifo_tx.len > 0){
        while (ScicRegs.SCIFFTX.bit.TXFFST != 0)
            ; // aguarda esvaziar buffer de envio até 16 bytes
        for (int k = 0; k < 16 && fifo_tx.len > 0; k++)
            ScicRegs.SCITXBUF.all = fifo_tx.pop();
    }
    return dump_count;
}

void SCI::unsafe_dump()
{
    while (fifo_tx.len > 0){
        ScicRegs.SCITXBUF.all = fifo_tx.pop();
    }
}

void SCI::send(char c)
{
    ScicRegs.SCITXBUF.all = c;
}



interrupt void sciaRxFifoIsr(void)
{
//logic1.set();
    static fixed_string<16> s16_rx;
    static char last_char = 0xff;

    char c;

    while(ScicRegs.SCIFFRX.bit.RXFFST)
    {
        c = ScicRegs.SCIRXBUF.all;  // Read data

        if (c != 0xff)
        {
            if(last_char == 0xff)
                s16_rx.clear();

            s16_rx.push(c);
        }
        else
            if(last_char != 0xff)
                //page0.decodeMessage(s16_rx);

        last_char = c;
    }

//logic1.clear();

    ScicRegs.SCIFFRX.bit.RXFFOVRCLR=1;   // Clear Overflow flag
    ScicRegs.SCIFFRX.bit.RXFFINTCLR=1;   // Clear Interrupt flag

    PieCtrlRegs.PIEACK.all = PIEACK_GROUP8;
}

interrupt void sciaTxFifoIsr(void)
{

    ScicRegs.SCIFFTX.bit.TXFFINTCLR=1;   // Clear SCI Interrupt flag
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP8;

}




//singleton
SCI SCI::instance;
SCI::SCI(){}
SCI& SCI::getInstance(){ return instance; }


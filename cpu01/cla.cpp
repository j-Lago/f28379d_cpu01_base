/*
 * cla_globals.cpp
 *
 *  Created on: 7 de jan de 2022
 *      Author: j-Lago
 */

#include "F28x_Project.h"
#include "cla_tasks.h"
#include "cla.h"



//--- Variáveis de leitura do CLA escritas pela CPU -----------
//
// variáveis não podem ser inicializadas aqui

#pragma DATA_SECTION("CpuToCla1MsgRAM")
float cla_dq[2];

#pragma DATA_SECTION("CpuToCla1MsgRAM")
float cla_f;

#pragma DATA_SECTION("CpuToCla1MsgRAM")
float cla_dir;
//-------------------------------------------------------------


//--- Variáveis de leitura da CPU escritas pela CLA -----------
//
// variáveis não podem ser inicializadas aqui

#pragma DATA_SECTION("Cla1ToCpuMsgRAM")
float cla_w;

#pragma DATA_SECTION("Cla1ToCpuMsgRAM")
float cla_th;

#pragma DATA_SECTION("Cla1ToCpuMsgRAM")
float cla_abc[3];

//-------------------------------------------------------------





//
// Interrupções chamadas ao final da execução de cada task do CLA.
// As task são implementadas em cla_tasks.cla
//
// para habilitar essa interrupções, incluir "IER |= M_INT11;"  no arquivo principal
//
interrupt void cla1Isr1 ()
{
    //cla_count++;
    PieCtrlRegs.PIEACK.all = M_INT11;
}


interrupt void cla1Isr2 ()
{
    asm(" ESTOP0");
}


interrupt void cla1Isr3 ()
{
    asm(" ESTOP0");
}


interrupt void cla1Isr4 ()
{
    asm(" ESTOP0");
}


interrupt void cla1Isr5 ()
{
    asm(" ESTOP0");
}


interrupt void cla1Isr6 ()
{
    asm(" ESTOP0");
}


interrupt void cla1Isr7 ()
{
    asm(" ESTOP0");
}


interrupt void cla1Isr8 ()
{

}




//
// Configuração de memória compartilhada e da operação do CLA
//
void CLA_setup(void)
{
    EALLOW;
    PieVectTable.CLA1_1_INT = &cla1Isr1;
    PieVectTable.CLA1_2_INT = &cla1Isr2;
    PieVectTable.CLA1_3_INT = &cla1Isr3;
    PieVectTable.CLA1_4_INT = &cla1Isr4;
    PieVectTable.CLA1_5_INT = &cla1Isr5;
    PieVectTable.CLA1_6_INT = &cla1Isr6;
    PieVectTable.CLA1_7_INT = &cla1Isr7;
    PieVectTable.CLA1_8_INT = &cla1Isr8;
    EDIS;


    extern uint32_t Cla1funcsRunStart, Cla1funcsLoadStart, Cla1funcsLoadSize;
    EALLOW;

#ifdef _FLASH
    // Copy over code from FLASH to RAM
    memcpy((uint32_t *)&Cla1funcsRunStart, (uint32_t *)&Cla1funcsLoadStart,
           (uint32_t)&Cla1funcsLoadSize);
#endif //_FLASH

    // Initialize and wait for CLA1ToCPUMsgRAM
    MemCfgRegs.MSGxINIT.bit.INIT_CLA1TOCPU = 1;
    while(MemCfgRegs.MSGxINITDONE.bit.INITDONE_CLA1TOCPU != 1){};

    // Initialize and wait for CPUToCLA1MsgRAM
    MemCfgRegs.MSGxINIT.bit.INIT_CPUTOCLA1 = 1;
    while(MemCfgRegs.MSGxINITDONE.bit.INITDONE_CPUTOCLA1 != 1){};

    // Select LS4RAM and LS5RAM to be the programming space for the CLA
    // First configure the CLA to be the master for LS4 and LS5 and then
    // set the space to be a program block
    MemCfgRegs.LSxMSEL.bit.MSEL_LS4 = 1;
    MemCfgRegs.LSxCLAPGM.bit.CLAPGM_LS4 = 1;
    MemCfgRegs.LSxMSEL.bit.MSEL_LS5 = 1;
    MemCfgRegs.LSxCLAPGM.bit.CLAPGM_LS5 = 1;

    // Configure LS0RAM and LS1RAM as data spaces for the CLA
    // First configure the CLA to be the master for LS0(1) and then
    // set the spaces to be code blocks
    MemCfgRegs.LSxMSEL.bit.MSEL_LS0 = 1;
    MemCfgRegs.LSxCLAPGM.bit.CLAPGM_LS0 = 0;

    MemCfgRegs.LSxMSEL.bit.MSEL_LS1 = 1;
    MemCfgRegs.LSxCLAPGM.bit.CLAPGM_LS1 = 0;

    EDIS;


    // Initialize CLA1 task vectors and end of task interrupts
    EALLOW;
    CpuSysRegs.PCLKCR0.bit.CLA1 = 1;
    CpuSysRegs.PCLKCR0.bit.DMA = 1;

    Cla1Regs.MVECT1 = (uint16_t)(&Cla1Task1);
    Cla1Regs.MVECT2 = (uint16_t)(&Cla1Task2);
    Cla1Regs.MVECT3 = (uint16_t)(&Cla1Task3);
    Cla1Regs.MVECT4 = (uint16_t)(&Cla1Task4);
    Cla1Regs.MVECT5 = (uint16_t)(&Cla1Task5);
    Cla1Regs.MVECT6 = (uint16_t)(&Cla1Task6);
    Cla1Regs.MVECT7 = (uint16_t)(&Cla1Task7);
    Cla1Regs.MVECT8 = (uint16_t)(&Cla1Task8);

    //
    // Enable the IACK instruction to start a task on CLA in software
    // for all  8 CLA tasks. Also, globally enable all 8 tasks (or a
    // subset of tasks) by writing to their respective bits in the
    // MIER register
    //
    Cla1Regs.MCTL.bit.IACKE = 1;
    Cla1Regs.MIER.all = 0x00FF; // habilita todas as 8 tarefas

    //
    // Configure the vectors for the end-of-task interrupt for all
    // 8 tasks
    //


    DmaClaSrcSelRegs.CLA1TASKSRCSEL1.bit.TASK1 = 0; //dispara task 1 por software

    //
    // Enable CLA interrupts at the group and subgroup levels
    //
    //IER |= M_INT11; // cla

}

void CLA_start(void)
{
    //PieCtrlRegs.PIEIER11.all = 0xFFFF; //cla
}

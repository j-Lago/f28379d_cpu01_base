#include "F28x_Project.h"
#include <stdint.h>
#include "globals.h"
#include "pwm.h"
#include "cla.h"
#include "adc.h"
#include "f_controle.h"
#include "control.h"


/*
 * Interrup��o de final de convers�o do ADC
 */
interrupt void main_adc_isr(void)
{
    static uint32_t refresh_count = REFRESH_COMP;
    //static uint16_t comm_downsampling_count = COMM_REFRESH_COMP;

PROBE_SET(0); // probe: 0 - medicao de tempo interrupcao adc


    if(pwm.en) // logica para ligar, desligar e clear pelo debuger
        pwm.enable();
    else if(!pwm.fault)
        prot.clear();



    //enc.update_vel(); // altualiza em frequencia fs/enc.downsample
    adc.read(); // aplica ganhos e offsets nas medi��es

#ifndef DISABLE_PROTECTIONS
    prot.compare(); //trip: software
#endif




PROBE_SET(1);   // probe: 1 - medicao de tempo controle
    //pv_control(); //controle original
    control(); //nova implementa��o reorganizada e com desacoplamento
PROBE_CLEAR(1); // probe: 1 - medicao de tempo controle

    /*
    //
    // aqui vai o c�digo que CPU1 escreve nas vari�veis compartilhadas com CLA (dados enviados para o CLA)
    //
    if(pwm.en)
    {
        Cla1ForceTask1(); // dispara task 1 do CLA
        //
        // aqui vai o c�digo que CPU1 executa que independe do resultado dos c�lculos do CLA task1 (execu��o CPU e CLA em paralelo)
        //

        while(Cla1Regs.MIRUN.bit.INT1 == 1); // aguarda at� CLA task 1 finalizar. Alternativamente, pode-se habilitar em CLA_setup() uma interrup��o para esse evento
        // ... while(Cla1Regs.MIRUN.bit.INT8 == 1); // aguarda at� CLA task 8 finalizar

        //
        // aqui vai o c�digo que CPU1 executa que depende do resultado dos c�lculos do CLA
        //
    }
    //pwm.setComps(cla_abc);
    */
    /*
    if(++comm_downsampling_count >= COMM_REFRESH_COMP){
        comm_downsampling_count = 0;

        switch(teste_count)
        {
                case 0: raspi.unsafe_write_float32(fteste, 3, 45);
        break;  case 1: raspi.unsafe_write_uint16(u16teste, 6, 80);
        break;  case 2: raspi.unsafe_write_int16(i16teste, 6, 105);
        break;  case 3: raspi.unsafe_write_uint32(u32teste, 3, 80);
        break;  case 4: raspi.unsafe_write_int32(i32teste, 3, 85);
        }
        teste_count = (++teste_count) % 5;
    }
    */

    fan.refresh();

    if(refresh_count++ >= REFRESH_COMP){
        led_az.toggle();
        refresh_count = 0;
        page0.periodic_refresh();
    }


PROBE_CLEAR(0);  // probe: 0 - medicao de tempo interrupcao adc

    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; //clear INT1 flag
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

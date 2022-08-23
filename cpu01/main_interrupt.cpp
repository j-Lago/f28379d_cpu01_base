#include "F28x_Project.h"
#include <stdint.h>
#include "globals.h"
#include "pwm.h"
#include "cla.h"
#include "adc.h"
#include "controle.h"

/*
 * Interrupção de final de conversão do ADC
 */
interrupt void main_adc_isr(void)
{
    static uint32_t refresh_count = REFRESH_COMP;
    //static uint16_t comm_downsampling_count = COMM_REFRESH_COMP;

logic0.set(); // todo: temporario para medição de tempo

    // logica para ligar, desligar e clear pelo debuger
    if(pwm.en)
        pwm.enable();
    else if(!pwm.fault)
        prot.clear();

    //enc.update_vel(); // altualiza em frequencia fs/enc.downsample
    adc.read(); // aplica ganhos e offsets nas medições
    prot.test();

logic1.set(); // todo: temporario para medição de tempo
    controle();
logic1.clear(); // todo: temporario para medição de tempo

    /*
    //
    // aqui vai o código que CPU1 escreve nas variáveis compartilhadas com CLA (dados enviados para o CLA)
    //
    if(pwm.en)
    {
        Cla1ForceTask1(); // dispara task 1 do CLA
        //
        // aqui vai o código que CPU1 executa que independe do resultado dos cálculos do CLA task1 (execução CPU e CLA em paralelo)
        //

        while(Cla1Regs.MIRUN.bit.INT1 == 1); // aguarda até CLA task 1 finalizar. Alternativamente, pode-se habilitar em CLA_setup() uma interrupção para esse evento
        // ... while(Cla1Regs.MIRUN.bit.INT8 == 1); // aguarda até CLA task 8 finalizar

        //
        // aqui vai o código que CPU1 executa que depende do resultado dos cálculos do CLA
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

    if(refresh_count++ >= REFRESH_COMP){
        led_az.toggle();
        refresh_count = 0;
        page0.periodic_refresh();
    }


logic0.clear();  // todo: temporario para medição de tempo

    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; //clear INT1 flag
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

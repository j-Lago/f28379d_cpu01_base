#include "F28x_Project.h"
#include <stdint.h>
#include "globals.h"
#include "pwm.h"
#include "cla.h"
#include "adc.h"
#include "control.h"
#include "scope.h"

extern float m_abc[3];
extern pll_s pll;

#define PLOT_POINTS 256

int plot_downsample_count = -1;
int plot_downsample_factor = 4;

bool dump_serial = false;
bool reset_plot_count = false;



int plot_count = 0;
float plot_chA[PLOT_POINTS];
float* chA = &pll.th;


// for debugging
float32 send_f[3] = {0.0f, 3.1415956f , -999999999.9f};


/*
 * Interrupção de final de conversão do ADC
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
    adc.read(); // aplica ganhos e offsets nas medições

#ifndef DISABLE_PROTECTIONS
    prot.compare(); //trip: software
#endif




PROBE_SET(1);   // probe: 1 - medicao de tempo controle
    //pv_control(); //controle original
    control(); //nova implementação reorganizada e com desacoplamento
PROBE_CLEAR(1); // probe: 1 - medicao de tempo controle


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


/*
    if(++comm_downsampling_count >= COMM_REFRESH_COMP)
    {
        comm_downsampling_count = 0;

PROBE_SET(4);   // probe: 4 - medicao de tempo comunicação
        send_f[0] += .01;
        raspi.write_float32(send_f, 3, 0x10, false);
PROBE_CLEAR(4); // probe: 4 - medicao de tempo comunicação
    }
*/

    //plot
    if (++plot_downsample_count >= plot_downsample_factor)
    {
        plot_downsample_count = 0;

        if(plot_count < PLOT_POINTS)
        {
            plot_chA[plot_count] = *chA;
            plot_count++;

            if(scope.state == buffering)
            {
                PROBE_SET(4);   // probe: 4 - medicao de tempo comunicação
                scope.sample();
                PROBE_CLEAR(4); // probe: 4 - medicao de tempo comunicação
            }

        }else
            plot_count = 0;
    }

    fan.refresh();

    if(++refresh_count >= REFRESH_COMP){
        refresh_count = 0;

        led_az.toggle();
        //page0.periodic_refresh();
    }


PROBE_CLEAR(0);  // probe: 0 - medicao de tempo interrupcao adc

    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; //clear INT1 flag
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

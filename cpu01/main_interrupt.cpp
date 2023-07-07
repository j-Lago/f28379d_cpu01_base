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


extern float teste0;
extern float teste1;
extern float teste2;
extern float teste3;
extern float teste4;
extern float teste5;



//int plot_downsample_count = -1;
//int plot_downsample_factor = 0;

bool dump_serial = false;
bool reset_plot_count = false;


//#define PLOT_POINTS 256
//int plot_count = 0;
//float plot_chA[PLOT_POINTS];
//float* chA = &pll.th;



/*
 * Interrupção de final de conversão do ADC
 */
interrupt void main_adc_isr(void)
{
    static uint32_t refresh_count = REFRESH_COMP;

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
    //control(); //nova implementação reorganizada e com desacoplamento
    control_open_loop(); //impões tensoes semoidais (apenas para debug)
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


    // for test
    teste0 += 377.0f/20000.0f * teste1;
    if (teste0 >= 2*3.1415926535897932384626433832795)
        teste0 -= 2*3.1415926535897932384626433832795f;

    teste3 = teste2*cos(teste0);
    teste4 = teste2*sin(teste0);
    teste5 = teste0 * 0.15915494309189533576888376337251f;


    //hmi scope sample
    if (scope.sample())
    {
        PROBE_SET(4);   // probe: 4 - indicação de amostragem scope
        PROBE_CLEAR(4); // probe: 4 - indicação de amostragem scope
    }


    fan.refresh();

    if(++refresh_count >= REFRESH_COMP){
        refresh_count = 0;

        led_az.toggle();
        //page0.periodic_refresh();
        if(scope.state == empty){
            scope.state = buffering;
            if (teste2 < 0.4f)
                teste2=1.0f;
            else
                teste2 *= .95f;
        }
    }


PROBE_CLEAR(0);  // probe: 0 - medicao de tempo interrupcao adc

    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; //clear INT1 flag
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

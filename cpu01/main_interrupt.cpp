#include "F28x_Project.h"
#include <stdint.h>
#include "globals.h"
#include "pwm.h"
#include "cla.h"
#include "adc.h"


// todo: para teste
float fteste[3] = {9584.45f, -45.14f, -0.00254f};
uint16_t u16teste[6] = {0,1,2,3,4,5};
int16_t i16teste[6] = {-6,7,-8,9,-10,11};
uint32_t u32teste[3] = {99,98,12};
int32_t i32teste[3] = {-8,-88,888};
uint16_t teste_count = 0;


// Interrup��o de final de convers�o do ADC
interrupt void main_adc_isr(void)
{
    static uint32_t refresh_count = REFRESH_COMP;
    static uint16_t comm_downsampling_count = COMM_REFRESH_COMP;

logic1.set(); // todo: temporario para medi��o de tempo

    // logica para ligar, desligar e clear pelo debuger
    if(pwm.en)
        pwm.enable();
    else if(!pwm.fault)
        pwm.clear();

    adc.read(); // aplica ganhos e offsets nas medi��es
    enc.update_vel(); // altualiza em frequencia fs/enc.downsample



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

    pwm.setComps(cla_abc);


    if(++comm_downsampling_count >= COMM_REFRESH_COMP){
        comm_downsampling_count = 0;

logic3.set();  // todo: temporario para medi��o de tempo

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
logic3.clear();  // todo: temporario para medi��o de tempo

    if(refresh_count++ >= REFRESH_COMP){
        led_az.toggle();
        refresh_count = 0;
        //page0.periodic_refresh();
    }


logic1.clear();  // todo: temporario para medi��o de tempo

    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; //clear INT1 flag
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

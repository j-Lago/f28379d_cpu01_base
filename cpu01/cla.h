/*
 * cla_globals.h
 *
 *  Created on: 7 de jan de 2022
 *      Author: j-Lago
 */

#pragma once

#define DT  0.00005f // 1/fc
#define PI 3.1415926535897932384626433832795f
#define PI2 6.283185307179586476925286766559f

extern float cla_dq[2];
extern float cla_f;
extern float cla_dir;


extern float cla_w;
extern float cla_th;
extern float cla_abc[3];

void CLA_setup(void);
void CLA_start(void);
interrupt void cla1Isr1();
interrupt void cla1Isr2();
interrupt void cla1Isr3();
interrupt void cla1Isr4();
interrupt void cla1Isr5();
interrupt void cla1Isr6();
interrupt void cla1Isr7();
interrupt void cla1Isr8();


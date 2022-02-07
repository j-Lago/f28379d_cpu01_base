/*
 * parameter_default.h
 *
 *  Created on: Feb 1, 2022
 *      Author: j-Lago
 */

#pragma once
#include "parameters.h"
#include "cla.h"

#define PAR_SIZE 8

#define PAR_INIT {                                                                                                         \
/* ID                 MODE          PAR[5]   UM[5]      MIN         MAX       DEFAULT    DESCRIPTION[32] (max 32 chars)     */ \
/*                                 "*****"  "*****"                                     "********************************"  */ \
/* 00 */    Parameter(read_write , "W000" , "Hz"     ,  0.0f    ,  90.0f   ,  60.0f   , "cla_f: Output frequency"           ), \
/* 01 */    Parameter(read_write , "W001" , "pu"     ,  0.0f    ,  1.2f    ,  1.0f    , "cla_dq[0]: d-Mod. index"           ), \
/* 02 */    Parameter(read_write , "W002" , "pu"     , -0.5f    ,  0.5f    ,  0.0f    , "cla_dq[1]: q-Mod. index"           ), \
/* 03 */    Parameter(read_only  , "R010" , "rad/s"  , -600.0f  ,  600.0f  ,  0.0f    , "cla_w: angular freq."              ), \
/* 04 */    Parameter(read_only  , "R011" , ""       , -1.0f    ,  1.0f    ,  1.0f    , "cla_dir: rotational direction"     ), \
/* 05 */    Parameter(read_only  , "R011" , "rad"    , -PI      ,  PI      ,  0.0f    , "cla_th: modulator angle"           ), \
/* 06 */    Parameter(read_only  , "R012" , "%"      ,  0.0f    ,  100.0f  ,  0.0f    , "adc.pot: Reference"                ), \
/* 07 */    Parameter(error      , "E100" , "A"      ,  0.0f    ,  0.0f    ,  0.0f    , "Overcurrent voltage"               ), \
}


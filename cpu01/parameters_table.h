/*
 * parameters_table.h
 *
 *  Created on: Feb 9, 2022
 *      Author: j-Lago
 */

#pragma once


#define PARAMETERS_W {                                                                                                                      \
        /*             ( &var            ,  name     ,  um       ,  description                      ,   dfault ,     min ,       max )*/   \
        /*  Class      (                 , "*****"   , "*****"   , "********************************",          ,         ,           )*/   \
        fParReadWrite  ( &cla_f          , "W010"    , "Hz"      , "Frequency"                       ,    60.0f ,    0.0f ,     90.0f ),    \
        fParReadWrite  ( &cla_dq[0]      , "W011"    , ""        , "d - modulation index"            ,     0.8f ,    0.0f ,      1.2f ),    \
        fParReadWrite  ( &cla_dq[1]      , "W012"    , ""        , "q - modulation index"            ,   -0.15f ,   -0.5f ,      0.5f ),    \
        }

#define PARAMETERS_R {                                                                                                 \
        /*             ( &var            ,  name     ,  um       ,  description                      ,   dfault )*/    \
        /*  Class      (                 , "*****"   , "*****"   , "********************************",          )*/    \
        fParReadOnly   ( &cla_w          , "R050"    , "rad/s"   , "Angular velocity reference"      ,     0.0f ),     \
        fParReadOnly   ( &adc.pot        , "R060"    , "%"       , "Potentiometer reference"         ,     0.0f ),     \
        fParReadOnly   ( &adc.iu         , "R070"    , "A"       , "U phase inverter current"        ,     0.0f ),     \
        fParReadOnly   ( &adc.iv         , "R071"    , "A"       , "V phase inverter current"        ,     0.0f ),     \
        fParReadOnly   ( &adc.vdc        , "R075"    , "V"       , "dc-link voltage"                 ,     0.0f ),     \
        fParReadOnly   ( &adc.vp0        , "R076"    , "V"       , "dc-link voltage (top: vp0)"      ,     0.0f ),     \
        fParReadOnly   ( &adc.v0n        , "R077"    , "V"       , "dc-link voltage (lower: v0n)"    ,     0.0f ),     \
        }
/*
        fParReadOnly   ( &adc.ib         , "R080"    , "A"       , "B phase line current"            ,     0.0f ),     \
        fParReadOnly   ( &adc.ic         , "R081"    , "A"       , "C phase line current"            ,     0.0f ),     \
        fParReadOnly   ( &adc.ibf        , "R083"    , "A"       , "B phase filter current"          ,     0.0f ),     \
        fParReadOnly   ( &adc.icf        , "R084"    , "A"       , "C phase filter current"          ,     0.0f ),     \
        fParReadOnly   ( &adc.vrs        , "R085"    , "V"       , "RS line voltage"                 ,     0.0f ),     \
        fParReadOnly   ( &adc.vts        , "R086"    , "V"       , "TS line voltage"                 ,     0.0f ),     \
        fParReadOnly   ( &adc.vrsf       , "R085"    , "V"       , "RS filter voltage"               ,     0.0f ),     \
        fParReadOnly   ( &adc.vtsf       , "R086"    , "V"       , "TS filter voltage"               ,     0.0f ),     \
*/

#define PARAMETERS_E {                                                                                                 \
        /*             ( &var            ,  name     ,  um       ,  description                      ,   dfault )*/    \
        /*  Class      (                 , "*****"   , "*****"   , "********************************",          )*/    \
        fParError      ( &adc.iu_over    , "E100"    , "A"       , "U phase overcurrent"             ,     0.0f ),     \
        fParError      ( &adc.iv_over    , "E101"    , "A"       , "V phase overcurrent"             ,     0.0f ),     \
/*      fParError      ( &adc.vdc_over   , "E103"    , "V"       , "dc-link overvoltage"             ,     0.0f ),*/   \
        }

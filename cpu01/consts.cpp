/*
 * consts.cpp
 *
 *  Created on: Aug 23, 2022
 *      Author: j-Lago
 */

#include "F28x_Project.h"
#include <stdint.h>
#include "adc.h"
#include "protecao.h"

const float ADC::offset[]  = {1899.4f, //iu
                              2036.4f, //iv
                              2047.5f, //ic
                              2047.5f, //ib
                              1938.6f, //vrs
                              1945.0f, //vts
                              0.0f,    //vp0
                              0.0f,    //v0n
                              2047.5f, //vrsf
                              2047.5f, //vtsf
                              2047.5f, //icf
                              2047.5f, //ibf
                              0.0f     //pot
                              };

const float ADC::gain[] = {0.006171422365f, //iu
                           0.006171422365f, //iv
                           0.0f,            //ic
                           0.0f,            //ib
                           0.336042171336f, //vrs
                           0.336042171336f, //vts
                           0.087755380000f, //vp0
                           0.087755380000f, //v0n
                           0.0f,            //vrsf
                           0.0f,            //vtsf
                           0.0f,            //icf
                           0.0f,            //ibf
                           0.000244200244f  //pot
                           };

const float Protecao::limits[] = {8.0f,     //iu
                                  8.0f,     //iv
                                  0.0f,     //ic
                                  0.0f,     //ib
                                  150.0f,   //vrs
                                  150.0f,   //vts
                                  80.0f,    //vp0
                                  80.0f,    //v0n
                                  0.0f,     //vrsf
                                  0.0f,     //vtsf
                                  0.0f,     //icf
                                  0.0f,     //ibf
                                  1.0f      //pot
                                  };



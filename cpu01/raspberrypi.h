/*
 * raspberrypi.h
 *
 *  Created on: Jul 25, 2022
 *      Author: j-Lago
 */


#pragma once
#include "F28x_Project.h"
#include "sci.h"
#include "fixed_string.h"
#include <stdint.h>



/*
 * Implementa protocolo de comunica��o simples entre o 28379d (controle) e o rapberry pi zero 2 (hmi)
 *
 * conex�es:
 *          28379D
 *      GPIO139:SCIC_RX---------------------GPIO14:UART_TX
 *      GPIO056:SCIC_TX---------------------GPIO15:UART_RX
 *                  GND---------------------GND
 *
 *  pacote:
 *
 *  modo normal (pacotes curtos de at� 16 bytes*):
 *      * essa limita��o � importante pois � o tamanho do byffer de hardware do m�dulo sci do 28379D. O envio de pacotes maiores que
 *        esse (modo dump) precisam ser gerenciados por software
 *
 *   -------------------------------------------------------------------------------------------------------------------------------------------
 *   escrita no modo normal:
 *
 *   bits   |        8        |    4   :   4    |        8        |        8        |        8        | ... |        8        |    *max 16 bytes
 *
 *          |-----------------+-----------------+-----------------+-----------------+-----------------+     +-----------------|
 *          |       cmd       |   type : len    |      address    |      data00     |      data00     | ... |    checksum     |
 *          |-----------------+-----------------+-----------------+-----------------+-----------------+     +-----------------|
 *                  'w'        2=bytes : 1-12           0-255            0-255             0-255                 (~sum)+1
 *                             5=uint16: 1-6
 *                             6=int16 : 1-6
 *                             7=uint32: 1-3
 *                             8=int32 : 1-3
 *                             11=float: 1-3
 *
 *   solicita��o de leitura no modo normal:
 *
 *   bits   |        8        |    4   :   4    |        8        |        8        |
 *
 *          |-----------------+-----------------+-----------------+-----------------|
 *          |       cmd       |   type : len    |      address    |    checksum     |
 *          |-----------------+-----------------+-----------------+-----------------|
 *                  'r'        2=bytes : 1-12           0-255          (~sum)+1
 *                             5=uint16: 1-6
 *                             6=int16 : 1-6
 *                             7=uint32: 1-3
 *                             8=int32 : 1-3
 *                             11=float: 1-3
 *
 *   -------------------------------------------------------------------------------------------------------------------------------------------
 *   resposta de solicita��o de leitura no modo normal � equivalente ao respondente escrevendo no endere�o solicitado, contudo, o comando utilizado � 'a' e n�o 'w':
 *
 *   bits   |        8        |    4   :   4    |        8        |        8        |        8        | ... |        8        |    *max 16 bytes
 *
 *          |-----------------+-----------------+-----------------+-----------------+-----------------+     +-----------------|
 *          |       cmd       |   type : len    |      address    |      data00     |      data00     | ... |    checksum     |
 *          |-----------------+-----------------+-----------------+-----------------+-----------------+     +-----------------|
 *                  'a'        2=bytes : 1-12           0-255            0-255             0-255                 (~sum)+1
 *                             5=uint16: 1-6
 *                             6=int16 : 1-6
 *                             7=uint32: 1-3
 *                             8=int32 : 1-3
 *                             11=float: 1-3
 *   -------------------------------------------------------------------------------------------------------------------------------------------
 *  modo dump(envio de grandes quantidades de dados por pacote):
 *
 *  envia uma length*ch de dados
 *
 *   bits   |        8        |    4   :   4    |        8        |        8        |        8        |
 *
 *          |-----------------+-----------------+-----------------+-----------------+-----------------+
 *          |       cmd       |   type : ch     |     length      |     address     |    checksum     |
 *          |-----------------+-----------------+-----------------+-----------------+-----------------+
 *                  'd'        2=bytes : x              0-255            0-255          -(~sum)+1
 *                             5=uint16: x
 *                             6=int16 : x
 *                             7=uint32: x
 *                             8=int32 : x
 *                             11=float: x
 *
 */

namespace rPiComm
{
    typedef enum State    // estado do decodificador de pacotes
    {
        r_idle = 0,
        r_fetch_cmd,
        r_fetch_type,
        r_fetch_address,
        r_fetch_data,
        r_fetch_checksum,
        r_error,
        r_valid,
    }commState;

    typedef enum Error    // tipos de erros de pacotes
    {
        no_error = 0,
        bad_cmd,
        bad_lenght,
        bad_type,
        bad_address,
        bad_data,
        bad_checksum,
        timeout
    }commError;

    static const uint_fast8_t TypesBytes[16] = { 0, 1, 1, 1, 1, 2, 2, 4, 4, 8, 8, 4, 8, 0, 0, 0 };
    static const uint_fast8_t TypesMax[16]   = { 0,12,12,12,12, 6, 6, 3, 3, 1, 1, 3, 1, 0, 0, 0 };
    static const char* TypesNames[16] = {
        "invalid_0",    //0
        "bool",      //1
        "byte",      //2
        "uint8",     //3
        "int8",      //4
        "uint16",    //5
        "int16",     //6
        "uint32",    //7
        "int32",     //8
        "uint64",    //9
        "int64",     //10
        "float32",   //11
        "float64",   //12
        "invalid_13",   //13
        "invalid_14",   //14
        "invalid_15"    //15
    };
    typedef enum Types
    {
        np_bool     =  1,
        np_byte     =  2,
        np_uint8    =  3,
        np_int8     =  4,
        np_uint16   =  5,
        np_int16    =  6,
        np_uint32   =  7,
        np_int32    =  8,
        np_uint64   =  9,
        np_int64    = 10,
        np_float32  = 11,
        np_float64  = 12
    }commTypes;

    static const uint_fast8_t MAX_DATA = 12;
    typedef union DataBuff
    {
        //bool        b[12];
        char        c[12];
        //uint8_t     u8[12];
        //int8_t      i8[12];
        uint16_t    u16[6];
        int16_t     i16[6];
        uint32_t    u32[3];
        int32_t     i32[3];
        uint64_t    u64[1];
        int64_t     i64[1];
        float       f32[3];
        double      f64[1];
    }commData;


    class Pkg
    {
    public:
        uint_fast8_t func;
        uint_fast8_t dtype;
        uint_fast8_t address;
        uint_fast8_t lenght;
        DataBuff data;

        Pkg();
        Pkg(const Pkg& cpy);
        void reset();
        void print();
    };


    typedef union{
         uint16_t u16[2];
         float32 f32;
    }UniType;



    class Comm
    {
    public:
        static SCI& ser;   // pointer to SCI (UART) object

    public:
        void write_byte   ( char* c, char length4, char address8, bool buff_dump=true) const;
        void write_float32( float32* f32, char length4, char address8, bool buff_dump=true) const;
        void write_uint16 (uint16_t* u16, char length4, char address8, bool buff_dump=true) const;
        void write_int16  ( int16_t* i16, char length4, char address8, bool buff_dump=true) const;
        void write_uint32 (uint32_t* u32, char length4, char address8, bool buff_dump=true) const;
        void write_int32  ( int32_t* i32, char length4, char address8, bool buff_dump=true) const;
        void write_raw    (char* bytes, int length) const;
        void write_uint16_raw    (uint16_t* uints, int length) const;

        /*
         * os m�todos com prefixo unsafe_ escrevem diretametne no buffer de hardware sem verifica��o se esse
         * est� cheio e podem causar perdas de dados, por�m, s�o r�pidos
         */
        void unsafe_write_float32( float32* f32, char length4, char address8) const;
        void unsafe_write_uint16 (uint16_t* u16, char length4, char address8) const;
        void unsafe_write_int16  ( int16_t* i16, char length4, char address8) const;
        void unsafe_write_uint32 (uint32_t* u32, char length4, char address8) const;
        void unsafe_write_int32  ( int32_t* i32, char length4, char address8) const;
        void unsafe_write_raw    (char* bytes, char length4) const;

        inline void push(char c) const;
        inline void push(const char *str) const;
        inline void send(char c) const;
        inline void dump() const;
    };

};

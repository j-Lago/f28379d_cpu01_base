/*
 * raspberrypi.cpp
 *
 *  Created on: Jul 25, 2022
 *      Author: j-Lago
 */


#include "raspberrypi.h"

SCI& rPiComm::Comm::ser = SCI::getInstance();

inline void rPiComm::Comm::push(char c) const           { ser.push(c);   }
inline void rPiComm::Comm::push(const char *str) const  { ser.push(str); }
inline void rPiComm::Comm::send(char c) const           { ser.send(c);   }
inline void rPiComm::Comm::dump() const                 { ser.dump();    }



void rPiComm::Comm::unsafe_write_float32(float32* f32, char length4, char address8) const
{
    rPiComm::UniType f;

    char cmd = 'w';
    char type_len = ((char)(11<<4)) | (length4 & 0x00ff);
    char checksum;
    char data;

    send(cmd);
    send(type_len);
    send(address8);

    checksum = cmd + type_len + address8;


    for(uint_fast8_t k=0; k<length4; k++)
    {
        f.f32 = f32[k];

        data = f.u16[0] & 0x00ff;
        send(data);
        checksum += data;

        data = f.u16[0] >> 8;
        send(data);
        checksum += data;

        data = f.u16[1] & 0x00ff;
        send(data);
        checksum += data;

        data = f.u16[1] >> 8;
        send(data);
        checksum += data;
    }

    send((~checksum)+1);
}



void rPiComm::Comm::unsafe_write_uint16(uint16_t* u16, char length4, char address8) const
{
    char func = 'w';
    char type_len = ((char)(5<<4)) | (length4 & 0x00ff);
    char checksum;
    char data;

    send(func);
    send(type_len);
    send(address8);

    checksum = func + type_len + address8;

    for(uint_fast8_t k=0; k<length4; k++)
    {
        data = u16[k] & 0x00ff;
        send(data);
        checksum += data;

        data = u16[k] >> 8;
        send(data);
        checksum += data;
    }

    send((~checksum)+1);
}



void rPiComm::Comm::unsafe_write_int16(int16_t* i16, char length4, char address8) const
{
    char func = 'w';
    char type_len = ((char)(6<<4)) | (length4 & 0x00ff);
    char checksum;
    char data;

    send(func);
    send(type_len);
    send(address8);

    checksum = func + type_len + address8;

    for(uint_fast8_t k=0; k<length4; k++)
    {
        data = i16[k] & 0x00ff;
        send(data);
        checksum += data;

        data = i16[k] >> 8;
        send(data);
        checksum += data;
    }

    send((~checksum)+1);
}






void rPiComm::Comm::unsafe_write_uint32(uint32_t* u32, char length4, char address8) const
{
    char func = 'w';
    char type_len = ((char)(7<<4)) | (length4 & 0x00ff);
    char checksum;
    char data;

    send(func);
    send(type_len);
    send(address8);

    checksum = func + type_len + address8;

    for(uint_fast8_t k=0; k<length4; k++)
    {
        data = u32[k] & 0x00ff;
        send(data);
        checksum += data;

        data = u32[k] >> 8;
        send(data);
        checksum += data;

        data = u32[k] >> 16;
        send(data);
        checksum += data;

        data = u32[k] >> 24;
        send(data);
        checksum += data;
    }

    send((~checksum)+1);
}



void rPiComm::Comm::unsafe_write_raw(char* bytes, char length4) const
{
    for(char k=0; k<length4; k++)
        send(bytes[k]);
}

void rPiComm::Comm::unsafe_write_int32(int32_t* i32, char length4, char address8) const
{
    char func = 'w';
    char type_len = ((char)(8<<4)) | (length4 & 0x00ff);
    char checksum;
    char data;

    send(func);
    send(type_len);
    send(address8);

    checksum = func + type_len + address8;

    for(uint_fast8_t k=0; k<length4; k++)
    {
        data = i32[k] & 0x00ff;
        send(data);
        checksum += data;

        data = i32[k] >> 8;
        send(data);
        checksum += data;

        data = i32[k] >> 16;
        send(data);
        checksum += data;

        data = i32[k] >> 24;
        send(data);
        checksum += data;
    }

    send((~checksum)+1);

}




// safe: usa buffer e dump

void rPiComm::Comm::write_raw(char* bytes, int length) const
{
    for(char k=0; k<length; k++)
    {
        if(ser.fifo_tx.len >= ser.FIFO_TX_SIZE)
            dump();
        push(bytes[k]);
    }

    dump();
}

void rPiComm::Comm::write_float32(float32* f32, char length4, char address8, bool dump_buff) const
{
    rPiComm::UniType f;

    char func = 'w';
    char type_len = ((char)(11<<4)) | (length4 & 0x00ff);
    char checksum;
    char data;

    push(func);
    push(type_len);
    push(address8);

    checksum = func + type_len + address8;


    for(uint_fast8_t k=0; k<length4; k++)
    {
        f.f32 = f32[k];

        data = f.u16[0] & 0x00ff;
        push(data);
        checksum += data;

        data = f.u16[0] >> 8;
        push(data);
        checksum += data;

        data = f.u16[1] & 0x00ff;
        push(data);
        checksum += data;

        data = f.u16[1] >> 8;
        push(data);
        checksum += data;
    }

    push((~checksum)+1);

    if (dump_buff)
        dump();
}



void rPiComm::Comm::write_uint16(uint16_t* u16, char length4, char address8, bool dump_buff) const
{
    char func = 'w';
    char type_len = ((char)(5<<4)) | (length4 & 0x00ff);
    char checksum;
    char data;

    push(func);
    push(type_len);
    push(address8);

    checksum = func + type_len + address8;

    for(uint_fast8_t k=0; k<length4; k++)
    {
        data = u16[k] & 0x00ff;
        push(data);
        checksum += data;

        data = u16[k] >> 8;
        push(data);
        checksum += data;
    }

    push((~checksum)+1);

    if (dump_buff)
        dump();
}



void rPiComm::Comm::write_int16(int16_t* i16, char length4, char address8, bool dump_buff ) const
{
    char func = 'w';
    char type_len = ((char)(6<<4)) | (length4 & 0x00ff);
    char checksum;
    char data;

    push(func);
    push(type_len);
    push(address8);

    checksum = func + type_len + address8;

    for(uint_fast8_t k=0; k<length4; k++)
    {
        data = i16[k] & 0x00ff;
        push(data);
        checksum += data;

        data = i16[k] >> 8;
        push(data);
        checksum += data;
    }

    push((~checksum)+1);

    if (dump_buff)
        dump();
}






void rPiComm::Comm::write_uint32(uint32_t* u32, char length4, char address8, bool dump_buff) const
{
    char func = 'w';
    char type_len = ((char)(7<<4)) | (length4 & 0x00ff);
    char checksum;
    char data;

    push(func);
    push(type_len);
    push(address8);

    checksum = func + type_len + address8;

    for(uint_fast8_t k=0; k<length4; k++)
    {
        data = u32[k] & 0x00ff;
        push(data);
        checksum += data;

        data = u32[k] >> 8;
        push(data);
        checksum += data;

        data = u32[k] >> 16;
        push(data);
        checksum += data;

        data = u32[k] >> 24;
        push(data);
        checksum += data;
    }

    push((~checksum)+1);

    if (dump_buff)
        dump();
}



void rPiComm::Comm::write_int32(int32_t* i32, char length4, char address8, bool dump_buff) const
{
    char func = 'w';
    char type_len = ((char)(8<<4)) | (length4 & 0x00ff);
    char checksum;
    char data;

    push(func);
    push(type_len);
    push(address8);

    checksum = func + type_len + address8;

    for(uint_fast8_t k=0; k<length4; k++)
    {
        data = i32[k] & 0x00ff;
        push(data);
        checksum += data;

        data = i32[k] >> 8;
        push(data);
        checksum += data;

        data = i32[k] >> 16;
        push(data);
        checksum += data;

        data = i32[k] >> 24;
        push(data);
        checksum += data;
    }

    push((~checksum)+1);

    if (dump_buff)
        ser.dump();
}

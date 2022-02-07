/*
 * fixed_string.h
 *
 *  Created on: Jan 28, 2022
 *      Author: j-Lago
 *
 *  Classe e funções para manipulação rápida de strings de capacidade fixa definida
 *  em tempo de compilação. Destinada para comunicação eficiente com HMI NEXTION.
 *
 *  funcionalidades:
 *      fixed_string::len //número de caracteres atualmente armazenada pela fixed_string
 *      fixed_string::clear()  // limpa a string (len=0)
 *      fixed_string << int << "string" << 'c' << 0x255   // sintaxe similar à do stringstream da biblioteca padrão cpp
 */


#pragma once

#include "math.h"
#include "F28x_Project.h"

#define NONZERO_DIGITS 6

template<unsigned int Capacity>
class fixed_string
{
public:
    char str[Capacity];
    int len;

    fixed_string();
    fixed_string(const char* str);
    void clear();
    void set(const char* str);
    void set(unsigned int id, const char c);
    void remove(unsigned int id);
    char operator[](const unsigned int id) const;
    bool char_cmp(unsigned int id, const char c);
    bool contains(const char c);
    void backspace();
    void copy(const char* s, char end_char = 0x00);
    void reverse_copy(const fixed_string<Capacity> other);

    void push(char c);
    void push_left(char c);
    void push_middle(unsigned int id, char c);

    void operator+= (const char* s);
    void operator+= (long num);
    void operator+= (float num);
    void operator+= (unsigned int num);

    float to_float();   // ~3x faster than atof()
};
typedef fixed_string<16> string16;
typedef fixed_string<32> string32;





template<unsigned int Capacity>
fixed_string<Capacity>& operator<<(fixed_string<Capacity>& s, const char *c)
{
    s += c;
    return s;
}

template<unsigned int Capacity1, unsigned int Capacity2>
fixed_string<Capacity1>& operator<<(fixed_string<Capacity1>& s1, fixed_string<Capacity2>& s2)
{
    uint16_t len2 = 0;
    while(s1.len<Capacity1 & len2 < s2.len)
        s1.push(s2[len2++]);
    return s1;
}


template<unsigned int Capacity>
fixed_string<Capacity>& operator<<(fixed_string<Capacity>& s, long i)
{
    s += i;
    return s;
}


template<unsigned int Capacity>
fixed_string<Capacity>& operator<<(fixed_string<Capacity>& s, float f)
{
    s += f;
    return s;
}



template<unsigned int Capacity>
fixed_string<Capacity>& operator<<(fixed_string<Capacity>& s, unsigned int i)
{
    s += i;
    return s;
}



template<unsigned int Capacity>
fixed_string<Capacity>& operator<<(fixed_string<Capacity>& s, char c)
{
    s.push(c);
    return s;
}


template<unsigned int Capacity>
bool operator==(const fixed_string<Capacity>& s, const char* c)
{
    for(int k=0; k < s.len; k++)
        if(c[k] != s[k])
            return false;
    if(c[s.len] != 0x00)
        return false;

    return true;
}




template<unsigned int Capacity>
fixed_string<Capacity>::fixed_string()
{
    clear();
}


template<unsigned int Capacity>
fixed_string<Capacity>::fixed_string(const char* str)
{
    set(str);
}

template<unsigned int Capacity>
void fixed_string<Capacity>::clear()
{
    len = 0;
}

template<unsigned int Capacity>
void fixed_string<Capacity>::set(const char* str)
{
    clear();
    while(len<Capacity & str[len] != 0)
        push(str[len]);
}

template<unsigned int Capacity>
void fixed_string<Capacity>::set(unsigned int id, const char c)
{
    if(id < len)
        str[id] = c;
}

template<unsigned int Capacity>
void fixed_string<Capacity>::remove(unsigned int id)
{
    if(id < len)
    {
        for(int k=id; k<len-1; k++)
            str[k] = str[k+1];
        len--;
    }

}

template<unsigned int Capacity>
bool fixed_string<Capacity>::char_cmp(unsigned int id, const char c)
{
    if(id < len)
        if(str[id] == c)
            return true;
    return false;
}

template<unsigned int Capacity>
bool fixed_string<Capacity>::contains(const char c)
{
    for(int k=0; k<len; k++)
        if(str[k]==c)
            return true;
    return false;
}

template<unsigned int Capacity>
void fixed_string<Capacity>::backspace()
{
    len = (len>0) ? len-1 : 0 ;
}

template<unsigned int Capacity>
char fixed_string<Capacity>::operator[](const unsigned int id) const
{
    return str[id];
}

template<unsigned int Capacity>
void fixed_string<Capacity>::copy(const char* s, char end_char)
{
    clear();
    while( (len < Capacity) & (new_str[len] != end_char) )
    {
        str[len] = new_str[len];
        len++;
    }
}


template<unsigned int Capacity>
void fixed_string<Capacity>::reverse_copy(const fixed_string<Capacity> other)
{
    //clear();
    for(len=0; len < other.len; len++)
        str[len] = other[other.len-len-1];
}

template<unsigned int Capacity>
void fixed_string<Capacity>::push(char c)
{
    if(len < Capacity)
        str[len++] = c;
}

template<unsigned int Capacity>
void fixed_string<Capacity>::push_middle(unsigned int id,char c)
{
    if(len < Capacity & id < len)
    {
        for(int k=len; k>id; k--)
            str[k] = str[k-1];
        str[id] = c;
        len++;
    }
}


template<unsigned int Capacity>
void fixed_string<Capacity>::push_left(char c)
{
    if(len < Capacity)
        {
            for(int k=len; k>0; k--)
                str[k] = str[k-1];
            str[0] = c;
            len++;
        }
}

template<unsigned int Capacity>
void fixed_string<Capacity>::operator+= (const char* s)
{
    int count = 0;
    while ( s[count] != 0)
    {
        push((char)s[count++]);
    }
}

template<unsigned int Capacity>
void fixed_string<Capacity>::operator+= (long num)
{
    if (num==0)
        push('0');
    else if(num<0)
    {
        num = -num;
        push('-');
    }

    fixed_string temp;
    long div10;
    while(num>0)
    {
        div10 = num / 10;;
        temp.push( num - div10*10 + '0');
        num = div10;
    }

    for(int k=0; k < temp.len; k++)
        push(temp.str[temp.len-k-1]);
}


template<unsigned int Capacity>
void fixed_string<Capacity>::operator+= (float num)
{
    if (num==0.0f){
        push('0');
        return;
    }

    bool signal = false;
    if(num<0.0f){
        signal = true;
        num = -num;
    }

    uint16_t nonzero = 0;
    fixed_string<Capacity> temp;

    int32_t inteiro = (uint32_t) num; // parte inteira
    float fracionario = num - inteiro; //parte fracionária

    // parte inteira
    int32_t div10;
    int32_t resto;
    while(inteiro > 0   &          \
          temp.len < Capacity-3)
    {
        div10 = inteiro/10;
        resto = inteiro - div10*10;
        inteiro = div10;

        temp.push(resto + '0'); // push in reverse order
        nonzero++;
    }

    // inerte ordem
    if(temp.len)
        for(len=0; len<temp.len; len++)
            str[len] = temp.str[temp.len-1-len];
    else
    {
        push('0');
        temp.len++; // guarda a posição do ponto decimal
    }

    if(fracionario==0)
        goto exit;

    // parte fracionária
    while(fracionario > 0.0f   &          \
          len < Capacity-3     &          \
          nonzero < NONZERO_DIGITS)
    {
        fracionario *= 10;
        inteiro = fracionario;
        fracionario -= inteiro;
        push(inteiro + '0');

        if(inteiro != 0  |  nonzero > 0)
            nonzero++;
    }

    // arredondamento
    if(fracionario >= 0.5f)
    {
        for(uint16_t k = len-1; ; k--)
        {
            inteiro = str[k] - '0';
            if(inteiro < 9)
            {
                str[k] = ++inteiro + '0';
                break;
            }
            else
            {
                str[k] = '0';
                if(k==0)
                {
                    push_left('1');
                    temp.len++; // guarda a posição do ponto decimal
                    break;
                }
            }
        }
    }

    // retira zeros à direita
    while(str[len-1] == '0' &  len > temp.len)
        len--;


    push_middle(temp.len, '.');

    exit:
    if(signal)
        push_left('-');
}


template<unsigned int Capacity>
void fixed_string<Capacity>::operator+= (unsigned int num)
{
    if (num==0)
        push('0');

    fixed_string temp;
    while(num>0)
    {
        temp.push( (num%10) + '0');
        num /= 10;
    }

    for(int k=0; k<temp.len; k++)
        push(temp.str[temp.len-k-1]);
}



template<unsigned int Capacity>
float fixed_string<Capacity>::to_float()
{

    bool decimal_point_already_found = false;
    float val = 0.0f;
    float dig;
    float decimal = 0.1f;


    for(int k=0; k<len; k++)
    {
        if(str[k] == '.')
            decimal_point_already_found = true;
        else if(str[k]>='0' & str[k]<='9')
        {
            dig = str[k]-'0';
            if (!decimal_point_already_found)
            {
                val = val*10.0f + dig;
            }
            else
            {
                val += decimal*dig;
                decimal *= 0.1f;
            }
        }
    }

    return (str[0]=='-') ? -val : val;
}



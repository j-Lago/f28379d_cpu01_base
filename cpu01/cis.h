/*
 * cis.h
 *
 *  Created on: Dez 15, 2021
 *      Author: j-Lago
 */

#pragma once

#include <math.h>

namespace fast_math
{
    const float um_pi = 0.31830988618379067153776752674503;
    const float um_pi2 = 0.1591549430918953357688837633725;
    const float pi = 3.1415926535897932384626433832795;
    const float pi2 = 6.283185307179586476925286766559;
    const float pi_2 = 1.5707963267948966192313216916398;
    const float deg_rad = 0.01745329251994329576923690768489;
    const float rad_deg = 57.29577951308232087679815481410;
};




template<typename T>
class CiS
{
    private:
    union {
        struct { T m_cos, m_sin;};
        T cossin[2];
    };
    T m_rad;

    public:
    CiS()
    {
        m_rad = 0;
        m_sin = 0;
        m_cos = 1;
    }

    CiS(const CiS& cs)
    {
        m_sin = cs.sin();
        m_cos = cs.cos();
        m_rad = cs.rad();
    }

    CiS(const CiS *cs)
    {
        m_sin = cs->sin();
        m_cos = cs->cos();
        m_rad = cs->rad();
    }

    CiS(T rad)
    : m_rad(rad)
    {
        fast_cossin(rad);
    }

    /*
    CosSin(T cos, T sin)
    :m_cos(cos), m_sin(sin)
    {
        cossin_to_angle();
    }
    */

    T rad(void)  const
    {return m_rad;}

    T deg(void)  const
    {
        return m_rad * fast_math::rad_deg;
    }

    T sin(void)  const
    {return m_sin;}

    T cos(void)  const
    {return m_cos;}

    void rad(T rad)
    {
        this->m_rad = rad;
        fast_cossin(m_rad);
    }

    void neg()
    {
        m_sin = -m_sin;
        m_rad = -m_rad;
    }

    void deg(T deg)
    {
        this->m_rad = rad * fast_math::deg_rad;
        fast_cossin(m_rad);
    }

    CiS<T> operator-() const
    {
        CiS<T> temp(this);
        temp.neg();
        return temp;
    }

    CiS<T>& operator=(T rad)
    {
        this->rad(rad);
        return *this;
    }

    CiS<T>& operator=(T cs[2])
    {
        setCosSin(cs[0], cs[1]);
        return *this;
    }

    /*
    void setCosSin(T cos, T sin)
    {
        this->m_sin = sin;
        this->m_cos = cos;
        cossin_to_angle();
    }
    */

    T operator[](uint16_t id) const
    {
        return cossin[id];
    }


    private:
    void fast_cossin(T th)
    {

        using namespace fast_math;

        int ndiv;
        if(fabs(th)>pi_2)
        {
            if(th>=0)
                ndiv = (int)((th+pi) * um_pi2);
            else
                ndiv = (int)((th-pi) * um_pi2);

            m_rad = th - ndiv*pi2;
        }
        else
            m_rad = th;

        /*
        // sin = x - 1/3! * x^3 + 1/5! * x^5 - 1/7! * x^7 +...
        // cos = 1 - 1/2! * x^2 + 1/4! * x^4 - 1/6! * x^6 +...
        T xx = m_rad * m_rad;
        m_cos =  1 + (xx * (coeff_cossin[2] + xx * (coeff_cossin[4] + xx * (coeff_cossin[6] + xx * (coeff_cossin[8] + xx * (coeff_cossin[10] + xx * (coeff_cossin[12])))))));
        m_sin =  m_rad + m_rad * (xx * (coeff_cossin[3] + xx * (coeff_cossin[5] + xx * (coeff_cossin[7] + xx * (coeff_cossin[9] + xx * (coeff_cossin[11] + xx * (coeff_cossin[13])))))));
        */

        m_cos = cos(m_rad);
        m_sin = sin(m_rad);
    }


    /*
    T cossin_to_angle()
    {
        using namespace fast_math;
        bool sign_sin = (this->m_sin<0)? 0 : 1;
        bool sign_cos = (this->m_cos<0)? 0 : 1;
        if (abs(this->m_sin) <= abs(this->m_cos))  // melhor m�todo (arcsin ou arccos) � o de menor valor entre sin ou cos
        {
            if(sign_cos)
                m_rad = fast_asin(this->m_sin);
            else if(sign_sin)
                m_rad = pi-fast_asin(this->m_sin);
            else
                m_rad = -pi-fast_asin(this->m_sin);
        }
        else
        {
            if(sign_sin)
                m_rad = fast_acos(this->m_cos);
            else
                m_rad = -fast_acos(this->m_cos);
        }
        return m_rad;
    }
    */

};

/*
template<typename T>
String sincos2str(const CosSin<T> &sc)
{
    return String("rad=" + String(sc.rad(), 5) + " <-> [cos=" + String(sc.sin(),5) + ", sin=" + String(sc.cos(),5) + "]");
}
*/

/*
 * Copyright (c) 2008-2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * helper class for checking if certain value can be expressed with a specified number of bits
 * the class differs from std::OutOfRange() (bit_limits.h) by passing the number of bits as an
 * argument and not by instantiating different template code. This is useful in situations
 * when the number of used bits is not a constant but a variable.
 * 
 * $Id: bit_var_limits.h 14920 2012-02-06 12:29:35Z damijan.skvarc $
 */

#ifndef BIT_LIMITS_H
#define BIT_LIMITS_H

#include <limits>

namespace istd {

template <typename T, bool isSigned>
struct BitVarLimits;

template <typename T>
    static T Mask(unsigned int a_bits)
    {
        return (static_cast<T>(1) << a_bits)-1;
    }

template <typename T>
    static T MsbMask(unsigned int a_bits)
    {
        return (static_cast<T>(1) << (a_bits-1));
    }


template <typename T>
    struct BitVarLimits<T, true> {
        static bool OutOfRange(unsigned int a_bits, const T& a_value)
        {
            T min = static_cast<T>(-1) << (a_bits-1);
            T max = (static_cast<T>(1) << (a_bits-1)) - 1;
            return (a_value > max) || (a_value < min);
        }

        static T Value(unsigned int a_bits, const T& a_value)
        {
            if (a_value & MsbMask<T>(a_bits)) {
                return (a_value & Mask<T>(a_bits)) |
                       ((static_cast<T>(-1) & ~Mask<T>(a_bits)));
            }
            else {
                return a_value;
            }
        }
    };

template <typename T>
   struct BitVarLimits<T, false> {
       static bool OutOfRange(unsigned int a_bits, const T& a_value)
       {
           return a_value >= (static_cast<T>(1) << a_bits);
       }

       static T Value(unsigned int a_bits, const T& a_value)
       {
           return a_value & Mask<T>(a_bits);
       }
   };


template <typename T>
    bool OutOfRange(unsigned int a_bits, const T& a_value)
    {
        return BitVarLimits<T, std::numeric_limits<T>::is_signed>::OutOfRange(a_bits, a_value);
    }

template <typename T>
    T Value(unsigned int a_bits, const T& a_value)
    {
        return BitVarLimits<T, std::numeric_limits<T>::is_signed>::Value(a_bits, a_value);
    }
}

#endif // BIT_LIMITS_H

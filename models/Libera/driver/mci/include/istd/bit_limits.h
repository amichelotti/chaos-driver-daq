/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: bit_limits.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */


#ifndef ISTD_BIT_LIMITS_H
#define ISTD_BIT_LIMITS_H

#include <type_traits>

namespace istd {
    /**
     * @addtogroup istd_util
     * @{
     */

    // Templates to assist bounds checking function
    template <unsigned int bits, typename T, bool isSigned>
        struct BitLimits;

    template <unsigned int bits, typename T>
        struct BitLimits<bits, T, true> {
            static const T min = -(static_cast<T>(1) << (bits-1));
            static const T max = (static_cast<T>(1) << (bits-1)) - 1;
            static bool OutOfRange(T a_value)
            {
                return a_value > max || a_value < min;
            }
        };

    template <unsigned int bits, typename T>
        struct BitLimits<bits, T, false> {
            static const T min = 0;
            static const T max = (static_cast<T>(1) << bits) - 1;
            static bool OutOfRange(T a_value)
            {
                return a_value > max;
            }
        };

    /**
     * Get min and max value that can be stored into a bitfield. Takes
     * signed (two's complement)/unsigned into account.
     *
     * max4bit = istd::BitFieldLimits<4, int32_t>::max; // 7
     * min4bit = istd::BitFieldLimits<4, int32_t>::min; // -8
     *
     */
    template <unsigned int bits, typename T>
        struct BitFieldLimits {
            static const T min = BitLimits<bits, T, std::is_signed<T>::value>::min;
            static const T max = BitLimits<bits, T, std::is_signed<T>::value>::max;
        };

    /**
      * Tests if the value is out of range of an limited integer (# of bits).
      * For signed types, value must be within (-2^(bits-1), 2^(bits-1))
      * For unsigned types, value must be within [0, 2^(bits))
      */
    template <int bits, typename T>
        bool OutOfRange(const T& a_value)
        {
            return BitLimits<bits, T, std::is_signed<T>::value>::OutOfRange(a_value);
        }

    /**@}*/

} //namespace

#endif  //ISTD_BIT_LIMITS_H


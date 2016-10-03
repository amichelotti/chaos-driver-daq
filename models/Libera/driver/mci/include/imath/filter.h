/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: filter.h 8621 2010-09-14 05:54:19Z matej.kenda $
 */

#ifndef FILTER_H
#define FILTER_H

#include <valarray>

#include <unistd.h>
#include <limits.h>
#include <omnithread.h>

#include "imath/iosystem.h"

namespace imath {

	/**
	 * @addtogroup controls
	 * @{
	 */

    /**
     * Transfer function coefficients of discrete LTI system.
     *
     *        Y(z)    b(0) + b(1)*z^-1 + ... + b(n)*z^-n
     * H(z) = ---- = ------------------------------------
     *        U(z)      1  + a(1)*z^-1 + ... + a(n)*z^-n
     *
     */
    class TransferFunction
    {
    public:
        TransferFunction(size_t a_order);
        virtual ~TransferFunction();

        void SetNumerator(size_t a_idx, double a_val);
        void SetDenominator(size_t a_idx, double a_val);
        void Normalize();
        const std::valarray<double>& GetNumerator() const;
        const std::valarray<double>& GetDenominator() const;

    private:
        std::valarray<double> m_numerator;   //!< Numerator
        std::valarray<double> m_denominator; //!< Denominator
    };
    /**@}*/


	/**
	 * @addtogroup controls
	 * @{
	 */

    /**
     * Discrete LTI system of order @a_order.
     *
     *        Y(z)    b(0) + b(1)*z^-1 + ... + b(n)*z^-n
     * H(z) = ---- = ------------------------------------
     *        U(z)      1  + a(1)*z^-1 + ... + a(n)*z^-n
     *
     */
    class Filter : public IOSystem
    {
    public:

        Filter(size_t a_order);
        virtual ~Filter();

        void Reset();
        void SetTransferFunction(const TransferFunction& a_H);
        const TransferFunction& GetTransferFunction() const;

        double Step(double a_u);

    private:
        size_t m_len;
        size_t m_ptr;

        TransferFunction m_H;
        std::valarray<double> m_w;

        inline size_t k(size_t a_off = 0) {
            size_t off = m_ptr + a_off;
            while (off > (INT_MAX >> 1)) off += m_len;
            while (off >= m_len) off -= m_len;
            return off;
        };

        mutable omni_mutex m_step_x;
    };
    /**@}*/

} // namespace

#endif // FILTER_H

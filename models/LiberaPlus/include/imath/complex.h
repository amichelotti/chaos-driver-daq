/*
* Copyright (c) 2008-2009 Instrumentation Technologies
* All Rights Reserved.
*
* $Id: complex.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
*/

#ifndef IMATH_COMPLEX_H
#define IMATH_COMPLEX_H

#include <complex>
#include <limits>

namespace imath {

	const double epsilon(std::numeric_limits<double>::epsilon());

	/**
	 * @addtogroup math
	 * @{
	 */
    typedef std::complex<double> Complex_t;

    bool equal(const Complex_t &a, const Complex_t &b);

    /** Polar representation of a complex number */
    class Polar_t {
        public:
            explicit Polar_t(double a_amp = 0.0, double a_phase = 0.0)
            : amp(a_amp), phase(a_phase)
            {
            };

            explicit Polar_t(const Complex_t& a_complex)
            : amp(std::abs(a_complex)), phase(std::arg(a_complex))
            {
            };

            Polar_t& operator=(const Complex_t& a_complex)
            {
                amp = std::abs(a_complex);
                phase = std::arg(a_complex);
                return *this;
            };

            operator Complex_t() const
            {
                return std::polar(amp, phase);
            };

            void Normalize()
            {
                Complex_t z(static_cast<Complex_t>(*this));
                this->phase = std::arg(z);
            }

            double  amp;
            double  phase;
    };
    /**@}*/

	/**
	 * @addtogroup math
	 * @{
	 */

    /** 64-bit register in the FPGA can contain complex components (I,Q)
      * packed as low/high bits.
      * This function is used to decompose the 64-bit value.
      */
    inline Complex_t Unpack(uint64_t a_packed)
    {
        const uint64_t c_mask( (1ULL << 32) -1);
        int32_t i(static_cast<int32_t>((a_packed >> 32) & c_mask));
        int32_t q(static_cast<int32_t>( a_packed        & c_mask));
        return Complex_t(static_cast<double>(i), static_cast<double>(q) );
    }
    /**@}*/

} // namespace

/**
 * @addtogroup bmc_streamHelpers
 * @{
 */
std::ostream& operator<< (std::ostream& a_stream, const imath::Polar_t& a_polar);
/**@}*/

#endif // IMATH_COMPLEX_H

// $Id: cordic.h 2192 2008-10-07 09:13:06Z matejk $

//! \file cordic.h
//! Public CORDIC declarations.

#if !defined(_CORDIC_H)
#define _CORDIC_H

/** Private.
 *  Calculates the amplitude from I and Q (sin and cos) value.
 *  Returns amplitude.
 *
 *  @param I I (sin) component of the amplitude.
 *  @param Q Q (cos) component of the amplitude.
 */
int cordic_amp( int I, int Q );

#endif	// _CORDIC_H

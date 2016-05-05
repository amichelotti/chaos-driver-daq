/*
* Copyright (c) 2008-2009 Instrumentation Technologies
* All Rights Reserved.
*
* $Id: poly.h 8621 2010-09-14 05:54:19Z matej.kenda $
*/

#ifndef IMATH_POLY_H
#define IMATH_POLY_H

namespace imath {

/**
 * @addtogroup math Mathematical helper Classes and functions
 * @{
 */
    double poly_eval(const double p[], const int n, const double x);

    void poly_roots(const double p[], const int n, Complex_t r[]);
    void poly_real_roots(const double p[], const int n, double r[], int &m);
/**@}*/

} // namespace

#endif // IMATH_POLY_H

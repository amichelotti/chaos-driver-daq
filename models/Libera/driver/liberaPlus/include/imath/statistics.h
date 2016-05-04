/*
* Copyright (c) 2008-2009 Instrumentation Technologies
* All Rights Reserved.
*
* $Id: poly.h 8621 2010-09-14 05:54:19Z matej.kenda $
*/

#ifndef IMATH_STATISTICS_H
#define IMATH_STATISTICS_H

#include <cmath>

namespace imath {

/**
 * @addtogroup math Mathematical helper Classes and functions
 * @{
 */
    /*-----------------------------------------------------------------------------*/
    /* calculate mean value of the array of elements passed as an argument         */
    /*-----------------------------------------------------------------------------*/
    template <typename T>
    double MeanValue (const T& a_array)
    {
        double sum = 0;

        if (!a_array.empty())
        {
            for (auto it = a_array.begin(); it != a_array.end(); it++) {
                sum += *it;
            }

            return sum / a_array.size();
        }

        return 0;
    }

    /*-----------------------------------------------------------------------------*/
    /* calculate standard deviation of the array of elements passed as an argument */
    /* normalization is calculated on the size N of the input vector               */
    /*-----------------------------------------------------------------------------*/
    template <typename T>
    double StdDeviation (const T& a_array)
    {
        double mean, square_sum = 0;

        if (!a_array.empty())
        {
            for (auto it = a_array.begin(); it != a_array.end(); it++) {
                square_sum += static_cast<double>(*it) * (*it);
            }

            mean = MeanValue(a_array);
            return sqrt ( square_sum / a_array.size() - mean * mean);
        }
    
        return 0;
    }

/**@}*/

} // namespace

#endif // IMATH_STATISTICS_H

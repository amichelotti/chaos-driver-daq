/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: literals.h 8621 2010-09-14 05:54:19Z matej.kenda $
 */


#ifndef ISTD_LITERALS_H
#define ISTD_LITERALS_H

#include <string>

namespace istd
{

    /**
     * @addtogroup istd_util
     * @{
     */

    /** Lookup table to convert literals from text to enum and vice-versa */
    struct Literal_t
    {
        int         numeric;
        const char* textual;
    };

    const char* NumericToTextual(const Literal_t* a_ct, int a_num);
    int TextualToNumeric(const Literal_t* a_ct, const std::string &a_text);

    const char* NumericToTextualAssert(const Literal_t* a_ct, int a_num);
    int TextualToNumericAssert(const Literal_t* a_ct, const std::string &a_text);

    /**@}*/

} //namespace

#endif  //ISTD_LITERALS_H

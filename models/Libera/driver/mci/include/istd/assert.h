/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: assert.h 14606 2011-12-21 12:43:41Z matej.kenda $
 */


#ifndef ISTD_ASSERT_H
#define ISTD_ASSERT_H

/**
 * @addtogroup istd_util
 * @{
 */

#define istd_ASSERT_MSG(_cond, _msg) \
    if (!(_cond)) { \
        istd::LoggedAssert( \
            __FILE__, __LINE__, __PRETTY_FUNCTION__, \
             _msg ": " #_cond); \
    }

#define istd_ASSERT(_condition) \
    if (!(_condition)) { \
        istd::LoggedAssert( \
            __FILE__, __LINE__, __PRETTY_FUNCTION__, \
            "Assertion condition: " #_condition); \
    }

/**@}*/

namespace istd {
    /**
     * @addtogroup istd_util
     * @{
     */

    // Used by macro istd_ASSERT
    void LoggedAssert(const char *a_file, int line, const char *a_func, const char* a_reason);

    /**@}*/

} //namespace

#endif  //ISTD_ASSERT_H


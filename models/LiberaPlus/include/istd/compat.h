/*
 * Copyright (c) 2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: compat.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef COMPAT_H_
#define COMPAT_H_

#include <chrono>

#ifndef __clang__

#if __GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 7)
    #define FINAL
    #define OVERRIDE

    // as standard requires, monotonic_clock was renamed to steady_clock in gcc 4.7
    namespace std {
        namespace chrono {
            typedef monotonic_clock steady_clock;
        }
    }
#elif defined(SUPPRESS_NEW_KEYWORDS)
    #define FINAL
    #define OVERRIDE
#else
    #define FINAL       final
    #define OVERRIDE    override
#endif


#if __GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 5)

// Gcc < 4.5 doesn't have proper implementation of cv_status from
// std::condition_variable::wait_for.
// This code implements simulation using bool.

namespace std {
    struct cv_status {
        enum { timeout = false, no_timeout = true };

        cv_status(const bool a_val) : val(a_val)
        {

        }

        cv_status& operator=(const bool a_val)
        {
            val = a_val;
            return *this;
        }

        operator bool() const
        {
            return val;
        }

    private:
        bool val;
    };
}
#endif

#endif

#endif /* COMPAT_H_ */

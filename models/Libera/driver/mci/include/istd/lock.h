/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: lock.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */


#ifndef ISTD_LOCK_H
#define ISTD_LOCK_H

#include <mutex>
#include <vector>

#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ > 5)

#warning "Don't use istd::Lock on gcc 4.6 or newer: use std::lock instead!"

/*
// make locks but don't yet
// try to acquire the mutexes
std::unique_lock lck1(m1,std::defer_lock);
std::unique_lock lck2(m2,std::defer_lock);
std::unique_lock lck3(m3,std::defer_lock);
// .. Some code and then lock all together
lock(lck1,lck2,lck3);
 */

#endif

namespace istd {

    class Lock
    {
        typedef std::vector<std::mutex*> Handles;

    public:
        Lock() {};

        template <typename First, typename... Rest>
            Lock(First& first, Rest&... rest)
            {
                DoLock(first,rest...);
            }

        virtual ~Lock();

    private:

        inline bool DoTryLock() const
        {
            return true;
        }

        template<typename First, typename... Rest>
            bool DoTryLock(First& first, Rest&... rest)
            {
               std::unique_lock<First> l(first, std::try_to_lock);
               if (l.owns_lock()) {
                   if (DoTryLock(rest...)) {
                      l.release();
                      m_handles.push_back(&first);
                      return true;
                   }
               }
               return false;
            }

        template<typename First, typename... Rest>
            void DoLock(First& first, Rest&... rest)
            {
                while (true) {
                    std::unique_lock<First> firstLock(first);
                    if (DoTryLock(rest...)) {
                        firstLock.release();
                        m_handles.push_back(&first);
                        return;
                    }
                }
            }

        Handles m_handles;
    };
}


#endif  //ISTD_LOCK_H


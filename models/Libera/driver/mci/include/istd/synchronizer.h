/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: synchronizer.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */


#ifndef ISTD_SYNCHRONIZER_H
#define ISTD_SYNCHRONIZER_H


#include <mutex>
#include <condition_variable>
#include <thread>
#include <chrono>

namespace istd {

/**
    Synchronizer is designed to synchronize certain thread to wait until the specified number of events are fired.

    General use case is:
    @verbatim
    istd::Synchronizer sync;
    sync.Reset(5);   // specify 5 events need to be fired before Synchronizer is unblocked
    if (!sync.Wait()) {
        // report timeout
    }
    // continue
    @endverbatim

    Events can be fired from other threads by calling Decrement() functions.
    */
    class Synchronizer
    {
        static const uint32_t MaxTimeout;

    public:
        Synchronizer(uint32_t a_counter = 0,      /* number of notifications to be wait for                     */
                     uint32_t a_retries = 20,     /* number of retries                                          */
                     uint32_t a_timeout = 20);    /* timeout period within each each retry iteration            */
        ~Synchronizer();

        bool     Wait();                          /* wait until counter becomes zero or until timeout or cancel */
        void     Increment();                     /* increment counter                                          */
        void     Decrement();                     /* decrement counter and wake up threads when counter is zero */
        void     Cancel();                        /* cancel waiting threads                                     */
        void     Reset(uint32_t a_counter);       /* set counter to the specified value                         */
        uint32_t Timeout();                       /* retrive timeout period, specified in ms                    */

    private:
        std::mutex              m_mutex;
        std::condition_variable m_condition;
        uint32_t                m_counter;
        uint32_t                m_retries;
        uint32_t                m_timeout;
    };
}


#endif  //ISTD_LOCK_H


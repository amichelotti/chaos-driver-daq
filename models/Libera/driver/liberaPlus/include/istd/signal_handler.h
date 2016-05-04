/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: signal_handler.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef ISTD_SIGNAL_HANDLER_H
#define ISTD_SIGNAL_HANDLER_H

#include <thread>
#include <map>



namespace istd {
    /**
     * @addtogroup istd_system
     * @{
     */

    /**
    * Interface for handling Posix signals (base class).
    * Thread SignalHandlerThread is receiving Posix signals and uses a class
    * derived from SignalHandler to perform application specific actions for
    * a specific signal number.
    */
    class SignalHandler {

        public:
            virtual ~SignalHandler();
            virtual void Handle(int a_signal) = 0;
    };

    class SignalHandlerThread {

        public:
            SignalHandlerThread(SignalHandler& a_handler, const int a_signals[] = NULL);
            void operator()();
            void Stop();

            static SignalHandlerThread *InstallHandler(SignalHandler& a_sigops, const int a_signals[] = NULL);

        private:
            typedef void (*signalHandlerFnc)(int);

            virtual ~SignalHandlerThread(){};

            SignalHandler&                   m_handler;
            std::thread                      m_thread;
            std::map<int, signalHandlerFnc>  m_old_handlers;
    };

    /** @} */
    
} // namespace

#endif /*ISTD_SIGNAL_HANDLER_H*/


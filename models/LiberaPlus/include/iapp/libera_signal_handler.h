/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: libera_signal_handler.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef LIBERA_SIGNAL_HANDLER_H
#define LIBERA_SIGNAL_HANDLER_H

#include <mutex>
#include "istd/signal_handler.h"

namespace iapp {

    class LiberaApplication;

    class LiberaSigHandler : public istd::SignalHandler
    {
    public:
        LiberaSigHandler(LiberaApplication *a_app);
        virtual void Handle(int a_signal);

    private:
        LiberaApplication   *m_app;
        std::mutex           m_shutdownMutex;
        bool                 m_shutdown;
    };
} // namespace


#endif // LIBERA_SIGNAL_HANDLER_H

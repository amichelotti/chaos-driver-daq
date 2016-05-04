/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * There are several cases when someone is requesting data from the future. In this
 * case the request is postponed until the required data is available. During this time
 * a server could shutdown. Before this it must cancel all postponed requests, otherwise
 * the server application could not finish correctly.
 *
 * LiberaWaitObject handles this situation. It is derived from the common LiberaObject
 * class so it must follow the LiberaObject constrains. In the constructor a parent LiberaObject
 * must be specified, together with reference to the mutex, controlling access over required data,
 * and condition variable, which is signaled from outside world, when new data is available. When
 * this happened a virtual OnGetData() function is called, which could be overridden by derived
 * classes to capture required data. Default function is body less, i.e. it does not capture any
 * data, since within class scope the structure of required data is unknown.
 *
 * Since LiberaWaitObject is derived from common LiberaObject class, it receives all events when
 * LiberaApplication state changes. This is used for handling the aforementioned shutdown situation.
 * Within OnShutDown() event all postponed request are canceled, which assures all waiting clients are
 * correctly notified, before the LiberaWaitObject is destroyed.

 * $Id: libera_wait_object.h 19990 2014-01-07 09:35:55Z damijan.skvarc $
 */

#ifndef LIBERA_WAIT_OBJECT_H
#define LIBERA_WAIT_OBJECT_H

#include <mutex>
#include <condition_variable>
#include "libera_feature.h"

namespace iapp {

    class LiberaWaitObject : public LiberaFeature
    {
    public:
        LiberaWaitObject(LiberaObject             &a_parent,
                         std::string               a_name,
                         std::condition_variable  &a_condition,
                         std::mutex               &a_mutex);

        virtual  ~LiberaWaitObject();

        bool     WaitForData(const bool *a_terminate_condition = NULL);
        uint32_t GetWaitingClients() const { return m_waiting_clients; }

    protected:
        virtual void              OnShutdown();
        virtual bool              OnGetData(LiberaObjectPtr a_parent) { return true; }

    private:
        std::condition_variable  &m_data_cond;            /* condition, signaled when required data is available             */
        std::mutex               &m_mutex;                /* mutex, controlling access over required data                    */
        std::condition_variable   m_wait_cond;            /* condition, signaled when wait request is terminated or canceled */
        bool                      m_terminate;            /* flag, indicating the wait request should terminate              */
        uint32_t                  m_waiting_clients;      /* number of waiting clients                                       */
        uint32_t                  m_data_wait_period;     /* sleep period while waiting for data availability                */
        uint32_t                  m_shutdown_wait_period; /* sleep period while waiting for shutdown                         */
    };
}


#endif // LIBERA_WAIT_OBJECT_H

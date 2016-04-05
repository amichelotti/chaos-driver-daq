/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: dispatcher_thread.h 19990 2014-01-07 09:35:55Z damijan.skvarc $
 */


#ifndef MCI_DISPATCHER_THREAD_H_
#define MCI_DISPATCHER_THREAD_H_

#include <set>
#include <list>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <thread>

#include "istd/id.h"
#include "mci/callback.h"
#include "mci/node.h"

//TODO: "Merge into dispatcher.cpp"

namespace mci {

    class DispatcherThread
    {
    public:

        DispatcherThread(const ClientId &a_clientId, const Callback &a_callback);
        ~DispatcherThread();

        /**
         * Main thread loop is processing notifications.
         */
        void operator()();

        /**
         * Registers new node to this client
         */
        bool Register(const Node &a_node);

        /**
         * Unregisters node from this client
         */
        bool Unregister(const Node &a_node);

        bool Unregister(const TreeNodeId &a_tnodeId, const Path &a_relPath);

        void AddToQueue(const NotificationDataPtr &a_data);

        /**
         * Helper methods
         */
        uint32_t NumOfRegisteredNodes();

    private:

        void     StopThread();
        void     AssureThread();

        std::set<Node>                 m_nodes;
        std::list<NotificationDataPtr> m_queue;

        ClientId                       m_clientId;
        Callback                       m_callback;

        bool                           m_terminate;
        std::condition_variable        m_wakeup_c;
        std::mutex                     m_queue_x;
        std::thread                    m_thread;
        bool                           m_thread_active;
        std::mutex                     m_thread_x;
    };

}

#endif /* MCI_DISPATCHER_THREAD_H_ */

/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: dispatcher.h 19990 2014-01-07 09:35:55Z damijan.skvarc $
 */


#ifndef MCI_DISPATCHER_H_
#define MCI_DISPATCHER_H_

#include <memory>
#include <map>
#include <set>
#include <mutex>

#include "istd/exception.h"
#include "istd/any.h"

#include "declarations.h"

namespace mci {

    class DispatchClientLocal;

    /**
     * @addtogroup register Repository interface for signals, configuration parameters, etc...
     * @{
     */

    /**
     * Singleton class
     */

    class Callback;
    class DispatcherThread;
    typedef std::shared_ptr<DispatcherThread> DispatcherThreadPtr;

    class Dispatcher
    {
    public:

        /**
        * Initializes singleton Dispatcher
        */
        static Dispatcher &GetInstance();

        /**
         * Connect new client to the dispatcher
         */
        //ClientId Connect(DispatchClientLocal *a_client);
        ClientId Connect(const Callback &a_callback);

        /**
         * Disconnect connected client from the dispatcher
         */
        void Disconnect(const ClientId &a_clientId);

        /**
         * Registers to node's notifications
         */
        bool Register(const ClientId &a_clientId, const Node &a_node);

        /**
         * Un-registers from node's notifications
         */
        bool Unregister(const ClientId &a_clientId, const Node &a_node);

        /**
         * Unregister node from all clients
         */
        void Unregister(const TreeNodeId &a_tnodeId,
                        const Path &a_relPath,
                        const std::set<ClientId> &a_clients);

        void MarkBroken(const ClientId& a_id);

        /**
         * Emmit the notification data
         */
        bool Emit(const std::set<ClientId> &a_clients,
                  const NotificationData &a_data);

        bool Emit(const std::set<ClientId> &a_clients,
                  const NotificationDataPtr &a_data);

        /**
         * Stop the Dispatcher. This should be run at the end of the application.
         * It stops all notifications and waits current callback methods to finish
         */
        void Stop();

        /**
         * Helper methods...
         */
        uint32_t NumOfRegisteredNodes(const ClientId &a_clientId) const;

    private:

        // Singleton
        Dispatcher() : m_running(true) {};
        ~Dispatcher() {};
        Dispatcher(const Dispatcher&);
        Dispatcher& operator=(const Dispatcher&);

        void Disconnect(const ClientId &a_clientId, bool a_force);
        void CleanBrokenClients();

        mutable std::mutex m_dispatcher_x;

        std::map<ClientId, DispatcherThreadPtr> m_clientMap;
        typedef std::pair<ClientId, DispatcherThreadPtr> ClientMapPair;

        std::set<ClientId>  m_brokenClients;
        std::mutex          m_brokenClients_x;

        bool                m_running;
    };

    /**@}*/

}

#endif /* MCI_DISPATCHER_H_ */

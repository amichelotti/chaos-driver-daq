/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: notification_client.h 19990 2014-01-07 09:35:55Z damijan.skvarc $
 */

#ifndef MCI_NOTIFICATION_CLIENT_H_
#define MCI_NOTIFICATION_CLIENT_H_

#include <mutex>
#include <memory>
#include <map>

#include "node.h"
#include "callback.h"

namespace mci {

    class NotificationClient
    {
    public:

        /**
         * Create notification client with default callback function
         * @param a_callback   default callback function
         */
        NotificationClient(const Callback &a_callback);

        /**
         * Create notification client without default callback function
         */
        NotificationClient();

        ~NotificationClient();

        /**
         * Register to node's notifications. Callback is default callback
         * function is exists.
         * @param a_node   register to this node's notifications
         */
        bool Register(Node &a_node);

        /**
         * Register to node's notifications, and pass callback function.
         * @param a_node      register to this node's notifications
         * @param a_callback  callback method called when notification is emitted.
         */
        bool Register(Node &a_node, const Callback &a_callback);

        /**
         * Unregister from node's notifications.
         * @param a_node      register to this node's notifications
         */
        bool Unregister(Node &a_node);

        /**
         * Reinitialize notifications after server reconnection.
         * @param a_root      root node that was reconnected
         */
        void Reinit(Node &a_root);

        /**
         * Mark the client as broken so that it will stop receiving notifications.
         * Used in notification callbacks.
         */
        void MarkBroken();

        void Shutdown();
        uint32_t NumOfRegisteredNodes();

        const ClientId& GetId() { return m_id; }

    private:
        const ClientId  m_id;
        Callback*       m_callback;

        std::mutex  m_map_x;
        std::map<Node, std::vector<Callback>> m_map;
        bool                                  m_isBroken;

        void Notification(const NotificationData& a_data);
    };

    typedef std::shared_ptr<NotificationClient> NotificationClientPtr;

} // namespace

#endif

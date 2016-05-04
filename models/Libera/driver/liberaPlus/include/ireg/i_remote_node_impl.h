/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: i_remote_node_impl.h 19990 2014-01-07 09:35:55Z damijan.skvarc $
 */

#ifndef IREG_REMOTE_NODE_IMPL_H
#define IREG_REMOTE_NODE_IMPL_H

#include "ireg/node.h"
#include "ireg/notification_client.h"
#include "ireg/notification_data.h"
#include "istd/id.h"

// Replace with "GCC diagnostic push" and "GCC diagnostic pop"
// in gcc 4.5
#pragma GCC diagnostic ignored "-Wold-style-cast"
#include "inet/i_remote_node.h"
#include "inet/i_notification.h"
#pragma GCC diagnostic warning "-Wold-style-cast"

namespace ireg {

    /**
     * Server side implementation to access remote registry.
     * @see RemoteNode
     */
    class i_RemoteNode_impl: public POA_i_NET::RemoteNode {

        struct  IdIRemoteNodeImpl {};
        typedef istd::Id<IdIRemoteNodeImpl, uint64_t>   RemoteNodeImplId;

    private:
        // Make sure all instances are built on the heap by making the
        // destructor non-public
        //virtual ~i_RemoteNode_impl();

    public:
        // standard constructor
        i_RemoteNode_impl();
        i_RemoteNode_impl(i_NET::Notification_ptr a_callback);
        virtual ~i_RemoteNode_impl();

        // methods corresponding to defined IDL attributes and operations
        char* GetName(const i_NET::NodePath& a_relPath);
        char* GetNameByIndex(const i_NET::NodePath& a_relPath, ::CORBA::ULong a_index);
        i_NET::NodeNames* GetNodes(const i_NET::NodePath& a_relPath);
        ::CORBA::ULong GetNodeCount(const i_NET::NodePath& a_relPath);
        ::CORBA::Boolean IsLeaf(const i_NET::NodePath& a_relPath);
        ::CORBA::Long GetValueType(const i_NET::NodePath& a_relPath);

        /**
         * @param a_typeHint Hint to convert the return value
         * @return CORBA any containing value of type as defined by the node or
         *         converted to a_typeHint if possible
         * @retval tk_null_any Value is not available
         */
        ::CORBA::Any* GetValue(const i_NET::NodePath& a_relPath, CORBA::ULong a_pos, CORBA::ULong a_size, CORBA::TypeCode_ptr a_typeHint);
        void SetValue(const i_NET::NodePath& a_relPath, const ::CORBA::Any& a_value, ::CORBA::ULong a_pos);
        ::CORBA::Boolean Execute(const i_NET::NodePath& a_relPath);
        ::CORBA::ULong GetSize(const i_NET::NodePath& a_relPath);
        i_NET::Flags GetFlags(const i_NET::NodePath&);
        ::CORBA::Boolean GetDomainValues(const i_NET::NodePath& a_relPath, i_NET::StringVec_out a_values);
        ::CORBA::Boolean GetValidatorExpression(const i_NET::NodePath& a_relPath, CORBA::String_out a_value);
        ::CORBA::Boolean Subscribe(const i_NET::NodePath& a_relPath);
        ::CORBA::Boolean Unsubscribe(const i_NET::NodePath& a_relPath);
        void StopNotifications();
        i_NET::RemoteSignal_ptr GetRemoteSignal(const i_NET::NodePath&);

        // Non-interface functions
        static void SetRootNode(const Node& a_root);
        bool Equals(i_NET::Notification_ptr a_client);

    private:
        Node    FindNode(const i_NET::NodePath& a_relPath);
        void    NotificationCallback(const NotificationData &a_data);

        static Node             m_root;
        NotificationClientPtr   m_client;
        i_NET::Notification_var m_remoteCallback;
        std::mutex              m_client_x;
        RemoteNodeImplId        m_id;
    };

} // namespace

#endif // IREG_REMOTE_NODE_IMPL_H


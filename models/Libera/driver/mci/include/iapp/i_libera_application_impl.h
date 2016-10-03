/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: i_libera_application_impl.h 19990 2014-01-07 09:35:55Z damijan.skvarc $
 */

#ifndef LIBERA_APPLICATION_IMPL_H
#define LIBERA_APPLICATION_IMPL_H

#include <map>
#include <thread>

#include "ireg/i_remote_node_impl.h"

// Replace with "GCC diagnostic push" and "GCC diagnostic pop"
// in gcc 4.5

#pragma GCC diagnostic ignored "-Wold-style-cast"
#include "inet/i_application.h"
#pragma GCC diagnostic warning "-Wold-style-cast"


class i_LiberaApplication_impl:public POA_i_NET::Application
{
    typedef std::multimap<CORBA::ULong, ireg::i_RemoteNode_impl *> repository;

public:
    i_LiberaApplication_impl();
    virtual ~i_LiberaApplication_impl();

    char*                 GetInterfaceVersion();
    void                  ReleaseRemoteNode(i_NET::Notification_ptr a_client);
    i_NET::RemoteNode_ptr GetRemoteNode(i_NET::Notification_ptr a_client);

private:
    repository::iterator  Find(i_NET::Notification_ptr a_client);

    /* repository of RegNode CORBA proxy interfaces */
    repository            m_registryClients;
    std::mutex            m_registryClients_x;
};


#endif // LIBERA_APPLICATION_IMPL_H

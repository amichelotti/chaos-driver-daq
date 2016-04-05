/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: signal_net_proxy.h 19990 2014-01-07 09:35:55Z damijan.skvarc $
 */


#ifndef ISIG_SIGNAL_NET_PROXY_H
#define ISIG_SIGNAL_NET_PROXY_H

#include <map>
#include <thread>

#include "istd/id.h"

#pragma GCC diagnostic ignored "-Wold-style-cast"
#include "inet/i_signal_receiver.h"
#pragma GCC diagnostic warning "-Wold-style-cast"

#include "isig/signal_client_base.h"

namespace isig {

    /**
     * Library private class used by i_RemoteSignal_impl to dispatch the signal
     * to registered receivers.
     */
    class SignalNetProxy
    {
    public:
        struct IdSignalNetProxy {};
        typedef  istd::Id<IdSignalNetProxy, uint64_t>   SignalNetProxyId;

        /**
         * @param a_client signal client which will be used by the proxy
         * @note proxy takes ownership and will deallocate the client
         */
        SignalNetProxy(
            i_NET::SignalReceiver_ptr   a_rec,
            SignalClientBase*           a_client,
            AccessMode_e                a_accessMode,
            size_t                      a_readSize,
            int64_t                     a_readOffset);

        ~SignalNetProxy();

        bool IsBadState() const;

        SuccessCode_e   Start();
        void            Stop();

        SignalStats GetStatistics() const;
        std::shared_ptr<SignalClientBase>  GetClient() const;

        void operator()();
        SignalNetProxyId GetId() { return m_id; }
        bool SameReceiver(i_NET::SignalReceiver_ptr a_rec);

    private:

        i_NET::SignalReceiver_var           m_rec;
        std::shared_ptr<SignalClientBase>   m_client;
        bool                                m_badState;

        AccessMode_e                        m_accessMode;
        size_t                              m_readSize;
        int64_t                             m_readOffset;

        std::thread                         m_dispatchingThread;
        const SignalNetProxyId              m_id;
    };

    typedef std::multimap<CORBA::ULong, SignalNetProxy*> SignalProxyMap;

} // namespace

#endif /* ISIG_SIGNAL_NET_PROXY_H */

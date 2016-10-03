/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: i_remote_signal_impl.h 19990 2014-01-07 09:35:55Z damijan.skvarc $
 */

#ifndef ISIG_REMOTE_SIGNAL_IMPL_H
#define ISIG_REMOTE_SIGNAL_IMPL_H

#include <thread>

#include "isig/signal_source.h"
#include "isig/signal_net_proxy.h"

#include "istd/id.h"

// Replace with "GCC diagnostic push" and "GCC diagnostic pop"
// in gcc 4.5
#pragma GCC diagnostic ignored "-Wold-style-cast"
#include "inet/i_remote_signal.h"
#pragma GCC diagnostic warning "-Wold-style-cast"

namespace isig {

    /**
     *
     */
    class i_RemoteSignal_impl: public POA_i_NET::RemoteSignal {

        struct  Id_RemoteSignal_impl {};
        typedef istd::Id<Id_RemoteSignal_impl, uint64_t>  RemoteSignal_implId;

    public:
        i_RemoteSignal_impl(SignalSourcePtr a_signal);

        // methods corresponding to defined IDL attributes and operations

        ::CORBA::ULong DodReadI8(i_NET::SignalReceiver_ptr a_rec,
            ::CORBA::ULongLong a_position, ::CORBA::ULong a_len,
            i_NET::Int8Buffer_out a_buf, i_NET::SignalMeta_out a_meta) OVERRIDE;

        ::CORBA::ULong DodReadUI8(i_NET::SignalReceiver_ptr a_rec,
            ::CORBA::ULongLong a_position, ::CORBA::ULong a_len,
             i_NET::UInt8Buffer_out a_buf, i_NET::SignalMeta_out a_meta) OVERRIDE;

        ::CORBA::ULong DodReadI16(i_NET::SignalReceiver_ptr a_rec,
            ::CORBA::ULongLong a_position, ::CORBA::ULong a_len,
             i_NET::Int16Buffer_out a_buf, i_NET::SignalMeta_out a_meta) OVERRIDE;

        ::CORBA::ULong DodReadUI16(i_NET::SignalReceiver_ptr a_rec,
            ::CORBA::ULongLong a_position, ::CORBA::ULong a_len,
             i_NET::UInt16Buffer_out a_buf, i_NET::SignalMeta_out a_meta) OVERRIDE;

        ::CORBA::ULong DodReadI32(i_NET::SignalReceiver_ptr a_rec,
            ::CORBA::ULongLong a_position, ::CORBA::ULong a_len,
             i_NET::Int32Buffer_out a_buf, i_NET::SignalMeta_out a_meta) OVERRIDE;

        ::CORBA::ULong DodReadUI32(i_NET::SignalReceiver_ptr a_rec,
            ::CORBA::ULongLong a_position, ::CORBA::ULong a_len,
             i_NET::UInt32Buffer_out a_buf, i_NET::SignalMeta_out a_meta) OVERRIDE;

        ::CORBA::ULong DodReadI64(i_NET::SignalReceiver_ptr a_rec,
            ::CORBA::ULongLong a_position, ::CORBA::ULong a_len,
             i_NET::Int64Buffer_out a_buf, i_NET::SignalMeta_out a_meta) OVERRIDE;

        ::CORBA::ULong DodReadUI64(i_NET::SignalReceiver_ptr a_rec,
            ::CORBA::ULongLong a_position, ::CORBA::ULong a_len,
             i_NET::UInt64Buffer_out a_buf, i_NET::SignalMeta_out a_meta) OVERRIDE;

        ::CORBA::ULong DodReadFloat(i_NET::SignalReceiver_ptr a_rec,
            ::CORBA::ULongLong a_position, ::CORBA::ULong a_len,
             i_NET::FloatBuffer_out a_buf, i_NET::SignalMeta_out a_meta) OVERRIDE;

        ::CORBA::ULong DodReadDouble(i_NET::SignalReceiver_ptr a_rec,
            ::CORBA::ULongLong a_position, ::CORBA::ULong a_len,
             i_NET::DoubleBuffer_out a_buf, i_NET::SignalMeta_out a_meta) OVERRIDE;

        CORBA::ULong RegisterReceiver(
            i_NET::SignalReceiver_ptr   a_rec,
            CORBA::ULong                a_accessMode,
            CORBA::ULong                a_readSize,
            CORBA::LongLong             a_readOffset,
            CORBA::ULong                a_qcapacity) OVERRIDE;

        void UnregisterReceiver(i_NET::SignalReceiver_ptr a_rec) OVERRIDE;

        // Vector of signal statistics, related to this signal
        i_NET::SignalStats* GetStatistics(i_NET::SignalReceiver_ptr a_rec) OVERRIDE;

        // -- Non-interface method

        void ShutDown();

    private:
        virtual ~i_RemoteSignal_impl();

        SignalProxyMap::iterator  FindProxy(i_NET::SignalReceiver_ptr a_rec);
        void Remove(SignalProxyMap::iterator& a_fi);

        /**
         * Removes all signal proxies that can't access stream receivers
         * any more.
         */
        void CleanupOrphanProxies();

        template <typename CTYPE, typename BASE_TYPE, class CVBUF>
            CORBA::ULong DodRead(
                i_NET::SignalReceiver_ptr   a_rec,
                CORBA::ULongLong            a_position,
                CORBA::ULong                a_len,
                CVBUF&                      a_buf,
                i_NET::SignalMeta_out&      a_meta,
                const CTYPE&,
                const BASE_TYPE&);

        SignalSourcePtr                         m_signal;
        std::mutex                              m_proxies_x;
        SignalProxyMap                          m_proxies;
        std::chrono::system_clock::time_point   m_cleanupTime;
        RemoteSignal_implId                     m_id;
    };

} // namespace

#endif // ISIG_REMOTE_SIGNAL_IMPL_H


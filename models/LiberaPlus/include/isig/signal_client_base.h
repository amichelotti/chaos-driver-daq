/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: signal_client_base.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */


#ifndef ISIG_STREAM_CLIENT_INTF_H
#define ISIG_STREAM_CLIENT_INTF_H

#include "istd/exception.h"
#include "isig/declarations.h"

// Replace with "GCC diagnostic push" and "GCC diagnostic pop"
// in gcc 4.5
#pragma GCC diagnostic ignored "-Wold-style-cast"
#include "inet/i_signal_receiver.h"
#pragma GCC diagnostic warning "-Wold-style-cast"

namespace isig {

    /**
     * Base interface of the stream client.
     * Derived classes implement functions. This interface is used by
     * i_RemoteSignal_impl (SignalNetProxy) to deliver stream data remotely to a
     * signal receiver.
     */
    class SignalClientBase {
    public:

        virtual SuccessCode_e Open()
            __attribute__((warn_unused_result));

        virtual SuccessCode_e Open(
            AccessMode_e a_mode,
            size_t a_readSize = 0,
            int64_t a_offset = 0) __attribute__((warn_unused_result));

        virtual void Close();
        virtual bool IsOpen() const;

        virtual AccessType_e SignalAccessType();
        virtual std::string GetName() const;
        virtual SignalStats GetStatistics() const;

        virtual ~SignalClientBase();

    protected:
        /** An instance of meta data container, which is used in cases, where
         * meta is actually not used.
         */
        SignalMeta   m_dummyMeta;

        // TODO: Remove this friend after removing obsolete DOD interfaces
        friend class SignalNetProxy;
        friend class i_RemoteSignal_impl;

        /**
         * Reads a buffer of atoms and forwards it to the remote signal receiver
         * (CORBA interface).
         */
        virtual SuccessCode_e ForwardTo(i_NET::SignalReceiver_ptr a_rec);

        /**
         * @name Stream dispatching functions
         * One of these functions is called from a derived client's ForwardTo
         * to serialise the signal buffer over the network.
         *  @{
         */
        void DoDispatch(i_NET::SignalReceiver_ptr a_rec, double* a_buf, size_t a_count, const SignalMeta& a_meta);
        void DoDispatch(i_NET::SignalReceiver_ptr a_rec, float* a_buf, size_t a_count, const SignalMeta& a_meta);
        void DoDispatch(i_NET::SignalReceiver_ptr a_rec, uint64_t* a_buf, size_t a_count, const SignalMeta& a_meta);
        void DoDispatch(i_NET::SignalReceiver_ptr a_rec, int64_t* a_buf, size_t a_count, const SignalMeta& a_meta);
        void DoDispatch(i_NET::SignalReceiver_ptr a_rec, uint32_t* a_buf, size_t a_count, const SignalMeta& a_meta);
        void DoDispatch(i_NET::SignalReceiver_ptr a_rec, int32_t* a_buf, size_t a_count, const SignalMeta& a_meta);
        void DoDispatch(i_NET::SignalReceiver_ptr a_rec, uint16_t* a_buf, size_t a_count, const SignalMeta& a_meta);
        void DoDispatch(i_NET::SignalReceiver_ptr a_rec, int16_t* a_buf, size_t a_count, const SignalMeta& a_meta);
        void DoDispatch(i_NET::SignalReceiver_ptr a_rec, uint8_t* a_buf, size_t a_count, const SignalMeta& a_meta);
        void DoDispatch(i_NET::SignalReceiver_ptr a_rec, int8_t* a_buf, size_t a_count, const SignalMeta& a_meta);
        /**
         * @}
         */

        /**
         * @name Data on demand read functions
         * One of these functions is called from i_RemoteSignal_impl to serialise
         * the signal buffer over the network.
         *  @{
         */
        virtual SuccessCode_e Read(double* a_buf, size_t& a_count, uint64_t a_position, SignalMeta& a_meta);
        virtual SuccessCode_e Read(float* a_buf, size_t& a_count, uint64_t a_position, SignalMeta& a_meta);
        virtual SuccessCode_e Read(uint64_t* a_buf, size_t& a_count, uint64_t a_position, SignalMeta& a_meta);
        virtual SuccessCode_e Read(int64_t* a_buf, size_t& a_count, uint64_t a_position, SignalMeta& a_meta);
        virtual SuccessCode_e Read(uint32_t* a_buf, size_t& a_count, uint64_t a_position, SignalMeta& a_meta);
        virtual SuccessCode_e Read(int32_t* a_buf, size_t& a_count, uint64_t a_position, SignalMeta& a_meta);
        virtual SuccessCode_e Read(uint16_t* a_buf, size_t& a_count, uint64_t a_position, SignalMeta& a_meta);
        virtual SuccessCode_e Read(int16_t* a_buf, size_t& a_count, uint64_t a_position, SignalMeta& a_meta);
        virtual SuccessCode_e Read(uint8_t* a_buf, size_t& a_count, uint64_t a_position, SignalMeta& a_meta);
        virtual SuccessCode_e Read(int8_t* a_buf, size_t& a_count, uint64_t a_position, SignalMeta& a_meta);
        /**
         * @}
         */

    };

    /**
     * Internally used constant to properly size queue in remote stream.
     */
    const size_t c_minRemoteQueue = 4;

} // namespace

#endif /* ISIG_STREAM_CLIENT_INTF_H */

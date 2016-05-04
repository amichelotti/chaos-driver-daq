/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: i_signal_receiver_impl.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef ISIG_SIGNAL_RECEIVER_IMPL_H
#define ISIG_SIGNAL_RECEIVER_IMPL_H

#include "isig/signal_dispatch_base.h"

// Replace with "GCC diagnostic push" and "GCC diagnostic pop"
// in gcc 4.5
#pragma GCC diagnostic ignored "-Wold-style-cast"
#include "inet/i_signal_receiver.h"
#pragma GCC diagnostic warning "-Wold-style-cast"

namespace isig {

    class i_SignalReceiver_impl: public POA_i_NET::SignalReceiver {
    private:
        // Make sure all instances are built on the heap by making the
        // destructor non-public
        virtual ~i_SignalReceiver_impl();

    public:

        static i_SignalReceiver_impl* Create(SignalDispatchBase* a_dispatch);
        static void Destroy(i_SignalReceiver_impl* a_receiver);

        // standard constructor
        i_SignalReceiver_impl(SignalDispatchBase* a_dispatch);

        // methods corresponding to defined IDL attributes and operations

        void DispatchInt8(const i_NET::Int8Buffer& a_buf, const i_NET::SignalMeta& a_meta) OVERRIDE;
        void DispatchUInt8(const i_NET::UInt8Buffer& a_buf, const i_NET::SignalMeta& a_meta) OVERRIDE;
        void DispatchInt16(const i_NET::Int16Buffer& a_buf, const i_NET::SignalMeta& a_meta) OVERRIDE;
        void DispatchUInt16(const i_NET::UInt16Buffer& a_buf, const i_NET::SignalMeta& a_meta) OVERRIDE;
        void DispatchInt32(const i_NET::Int32Buffer& a_buf, const i_NET::SignalMeta& a_meta) OVERRIDE;
        void DispatchUInt32(const i_NET::UInt32Buffer& a_buf, const i_NET::SignalMeta& a_meta) OVERRIDE;
        void DispatchInt64(const i_NET::Int64Buffer& a_buf, const i_NET::SignalMeta& a_meta) OVERRIDE;
        void DispatchUInt64(const i_NET::UInt64Buffer& a_buf, const i_NET::SignalMeta& a_meta) OVERRIDE;
        void DispatchFloat(const i_NET::FloatBuffer& a_buf, const i_NET::SignalMeta& a_meta) OVERRIDE;
        void DispatchDouble(const i_NET::DoubleBuffer& a_buf, const i_NET::SignalMeta& a_meta) OVERRIDE;

        void Cancel(const CORBA::Long a_code) OVERRIDE;

    private:
        SignalDispatchBase*     m_dispatch;

    };

} // namespace

#endif // ISIG_SIGNAL_RECEIVER_IMPL_H

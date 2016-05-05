/*
 * Copyright (c) 2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: dod_remote_client.h 19990 2014-01-07 09:35:55Z damijan.skvarc $
 */

#ifndef ISIG_DOD_REMOTE_CLIENT_H
#define ISIG_DOD_REMOTE_CLIENT_H

#include <mutex>

#include "isig/declarations.h"
#include "inet/corba_util.h"
#include "isig/data_on_demand_remote_source.h"
#include "isig/i_signal_receiver_impl.h"
#include "isig/signal_dispatch_base.h"
#include "isig/dod_client_base.h"

namespace isig {

    template <class DOD_TRAITS>
        class DataOnDemandRemoteSource;

    template <class DOD_TRAITS>
        class DodRemoteClient :
            public DodClientBase<DOD_TRAITS>,
            public SignalDispatchBase {

        public:

            typedef DOD_TRAITS                  Traits;
            typedef typename Traits::Atom       Atom;
            typedef typename Traits::BaseType   BaseType;

            typedef DodClientBase<Traits>       ClientBase;

            typedef typename ClientBase::MetaBuffer     MetaBuffer;
            typedef typename ClientBase::Buffer         Buffer;
            typedef typename ClientBase::MetaBufferPtr  MetaBufferPtr;

            typedef DataOnDemandRemoteSource<Traits>    Dod;
            typedef std::shared_ptr<Dod>                DodSharedPtr;
            typedef std::weak_ptr<Dod>                  DodWeakPtr;

            using ClientBase::CreateBuffer;
            using ClientBase::Open;
            using ClientBase::m_traits;

            DodRemoteClient()
            : m_receiver(NULL),
              m_remoteCapacity(m_queue.c_defaultCapacity * c_minRemoteQueue),
              m_open(false)
            {
            }

            DodRemoteClient(
                DodSharedPtr        a_dod,
                const std::string&  a_name,
                const Traits&       a_traits)
            : ClientBase(a_name, a_traits),
              m_remoteCapacity(m_queue.c_defaultCapacity * c_minRemoteQueue),
              m_open(false),
              m_dod(a_dod)
            {
                m_receiver = i_SignalReceiver_impl::Create(this);
                m_receiverRef = m_receiver->_this();
            }

            virtual ~DodRemoteClient()
            {
                Close();
                std::unique_lock<std::mutex> l(m_recMutex);
                m_receiverRef = i_NET::SignalReceiver::_nil();
                i_SignalReceiver_impl::Destroy(m_receiver);
            }


            DodRemoteClient(const DodRemoteClient& a_other)
            : ClientBase(a_other), m_open(false), m_dod(a_other.m_dod)
            {
            }

            DodRemoteClient& operator=(const DodRemoteClient& a_other)
            {
                if (&a_other != this) {
                    Close();
                    m_receiverRef = i_NET::SignalReceiver::_nil();
                    i_SignalReceiver_impl::Destroy(m_receiver);
                    m_receiver = NULL;

                    ClientBase::operator=(a_other);
                    m_remoteCapacity = a_other.m_remoteCapacity;
                    m_dod = a_other.m_dod;
                    if (m_dod.lock()) {
                        m_receiver = i_SignalReceiver_impl::Create(this);
                        m_receiverRef = m_receiver->_this();
                    }
                }
                return *this;
            }

            /**
             * @param a_mode Data on demand acquisition mode
             * @param a_readSize    eModeDodOnEvent: how many atoms to read on each event
             * @param a_offset      eModeDodOnEvent: offset to apply on each read
             */
            virtual SuccessCode_e Open(
                AccessMode_e a_mode,
                size_t a_readSize,
                int64_t a_offset = 0) OVERRIDE __attribute__((warn_unused_result))
            {
                istd_FTRC();

                std::unique_lock<std::recursive_mutex>l (m_mutex);
                if (m_open) {
                    return (a_mode == m_mode) ? eSuccess : eInvalidArg;
                }

                DodSharedPtr d = m_dod.lock();
                if (!d) {
                    Close();
                    return eClosed;
                }

                m_remoteSignal = d->GetRemoteSignal();

                if (CORBA::is_nil(m_remoteSignal)) {
                    istd_TRC(istd::eTrcLow,
                        "isig: Internal error: DOD source doesn't have "
                        "reference to remote signal.");
                    return eInternalError;
                }
                m_queue.Reset();
                try {
                    auto success = m_remoteSignal->RegisterReceiver(
                        m_receiverRef, a_mode, a_readSize, a_offset, m_remoteCapacity
                    );
                    if (success != eSuccess) {
                        m_remoteSignal = i_NET::RemoteSignal::_nil();
                        return static_cast<SuccessCode_e>(success);
                    }
                    m_open = true;
                    m_mode = a_mode;
                    d->Add(this);
                    return eSuccess;
                }
                catch(const CORBA::SystemException &e) {
                    istd_TRC(istd::eTrcHigh, "Can't access remote signal: " << e);
                }
                catch(const CORBA::Exception &e) {
                    istd_TRC(istd::eTrcHigh, "Can't access remote signal: " << e);
                }
                catch(...) {
                    istd_TRC(istd::eTrcHigh, "Can't access remote signal: unknown error.");
                }
                return eIoError;
            }

            virtual bool IsOpen() const OVERRIDE
            {
                return m_open;
            }

            /**
             * @param a_buf output buffer to read data to; buffer size indicates
             *              the required amount of data to be read; it can be
             *              resized if the required amount of data is not available
             * @param a_meta meta data belonging to the buffer
             * @param a_position position to read from (not applicable in all modes)
             */
            SuccessCode_e Read(
                Buffer&         a_buf,
                SignalMeta&     a_meta,
                uint64_t        a_position = 0 )
#ifndef NDEBUG
            __attribute__((warn_unused_result))
#endif
            {
                if (!m_open) {
                    return eClosed;
                }
                DodSharedPtr d = m_dod.lock();
                if (!d) {
                    Close();
                    return eClosed;
                }

                if (m_mode == eModeDodOnEvent) {
                    return m_queue.Pop(a_buf, a_meta);
                }

                try {
                    size_t read(0);
                    const auto components = d->Length();
                    auto success = ReadDodRemote(
                        m_remoteSignal, m_receiverRef,
                        a_position,
                        a_buf.GetBuffer(),
                        a_buf.GetBufferLength(),
                        read, a_meta);
                    if (success == eSuccess) {
                        a_buf.Resize(read/components);
                    }
                    return success;
                }
                catch(const CORBA::SystemException &e) {
                    istd_TRC(istd::eTrcHigh, "Can't access remote signal: " << e);
                }
                catch(const CORBA::Exception &e) {
                    istd_TRC(istd::eTrcHigh, "Can't access remote signal: " << e);
                }
                catch(...) {
                    istd_TRC(istd::eTrcHigh, "Can't access remote signal: unknown error.");
                }
                return eIoError;
            }

            virtual void Close() OVERRIDE
            {
                istd_FTRC();
                {
                    std::unique_lock<std::recursive_mutex>l (m_mutex);
                    if (!m_open) {
                        return;
                    }
                    m_open = false;
                    DodSharedPtr d = m_dod.lock();
                    if (d) {
                        d->Remove(this);
                    }

                    if (CORBA::is_nil(m_remoteSignal)) {
                        istd_TRC(istd::eTrcLow, "isig: Missing remote signal reference.");
                        return;
                    }
                }

                std::unique_lock<std::mutex> l(m_recMutex);
                try {
                    m_remoteSignal->UnregisterReceiver(m_receiverRef);
                }
                catch(const CORBA::SystemException &e) {
                    istd_TRC(istd::eTrcMed, "Can't access remote signal: " << e);
                }
                catch(const CORBA::Exception &e) {
                    istd_TRC(istd::eTrcMed, "Can't access remote signal: " << e);
                }
                catch(...) {
                    istd_TRC(istd::eTrcMed, "Can't access remote signal.");
                }

                m_remoteSignal = i_NET::RemoteSignal::_nil();
            }


            virtual void Cancel(SuccessCode_e a_code) OVERRIDE
            {
                istd_FTRC();
                if (m_mode == eModeDodOnEvent) {
                     m_queue.Cancel(a_code);
                }
                else {
                    Close();
                }
            }

        protected:

            using ClientBase::m_queue;

            /**
             * Function to dispatch the buffer. Used by i_SignalReceiver_impl
             * which receives the buffer of atoms as array of Traits::BaseType.
             */
            virtual void Dispatch(
                const BaseType*     a_buf,
                size_t              a_count,
                const SignalMeta&   a_meta) OVERRIDE
            {
                Buffer b(const_cast<BaseType*>(a_buf), a_count, m_traits);
                Buffer copy(b);
                m_queue.Push(std::move(copy), a_meta);
            }


        private:
            i_NET::RemoteSignal_var     m_remoteSignal;
            i_SignalReceiver_impl*      m_receiver;
            i_NET::SignalReceiver_var   m_receiverRef;
            size_t                      m_remoteCapacity;

            bool                 m_open;
            DodWeakPtr           m_dod;
            AccessMode_e         m_mode;
            std::recursive_mutex m_mutex;
            std::mutex           m_recMutex;
        };

    // DodRemoteClient for runtime traits are instantiated in the library
    extern template class DodRemoteClient<SignalTraitsVarUint8>;
    extern template class DodRemoteClient<SignalTraitsVarInt16>;
    extern template class DodRemoteClient<SignalTraitsVarUint16>;
    extern template class DodRemoteClient<SignalTraitsVarInt32>;
    extern template class DodRemoteClient<SignalTraitsVarUint32>;
    extern template class DodRemoteClient<SignalTraitsVarInt64>;
    extern template class DodRemoteClient<SignalTraitsVarUint64>;
    extern template class DodRemoteClient<SignalTraitsVarFloat>;
    extern template class DodRemoteClient<SignalTraitsVarDouble>;

}

#endif /* ISIG_DOD_REMOTE_CLIENT_H */

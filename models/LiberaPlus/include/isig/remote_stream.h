/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: remote_stream.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef ISIG_REMOTE_STREAM_H
#define ISIG_REMOTE_STREAM_H

#include <string>

#pragma GCC diagnostic ignored "-Wold-style-cast"
#include "inet/i_remote_signal.h"
#pragma GCC diagnostic warning "-Wold-style-cast"

#include "inet/corba_util.h"
#include "isig/stream.h"
#include "isig/i_signal_receiver_impl.h"

namespace isig {

	/**
	 * @addtogroup signal
	 * @{
	 */

    /**
     * Input stream that receives signal data from remote
     */
    template <class STREAM_TRAITS> 
        class RemoteStream : public Stream <STREAM_TRAITS> {
        public:

            typedef Stream<STREAM_TRAITS>       BaseStream;
            typedef typename BaseStream::Traits Traits;
            typedef typename BaseStream::Buffer Buffer;
            typedef typename BaseStream::Client Client;
            using BaseStream::AtomSize;
            using BaseStream::c_defaultCapacity;

            /**
             * Remote stream is driven by the remote calls through
             * isig::i_SignalDispatcher_impl and doesn't need a thread to
             * dispatch stream data.
             *
             * @param a_traits Runtime defined signal traits
             * @param a_remoteSignal remote signal interface to register to
             */
            explicit RemoteStream(
                const std::string& a_name,
                i_NET::RemoteSignal_ptr a_remoteSignal,
                const Traits& a_traits,
                const size_t a_remoteCapacity = BaseStream::c_defaultCapacity * c_minRemoteQueue)
            : BaseStream(a_name, a_traits, StreamMode_e::Passive),
              m_remoteSignal(i_NET::RemoteSignal::_duplicate(a_remoteSignal)),
              m_receiver(NULL),
              m_remoteCapacity(a_remoteCapacity),
              m_opened(false)
            {
                istd_FTRC();
                m_remoteCapacity = std::max(m_remoteCapacity, c_minRemoteQueue);
                istd_TRC(istd::eTrcHigh, "Group size: " << a_traits.GetGroupSize());
                m_receiver = i_SignalReceiver_impl::Create(this);
            };

            virtual ~RemoteStream()
            {
                istd_FTRC();
                this->ShutDown();
                i_SignalReceiver_impl::Destroy(m_receiver);
            };

            /**
             * Retrieves server-side statistics related to this signal chain.
             * Failure to retrieve statistics is ignored.
             */
            virtual SignalStats GetStatistics() const
            {
                SignalStats st;
                try {
                    i_NET::SignalStats_var ist (
                        m_remoteSignal->GetStatistics(m_receiver->_this())
                    );
                    st.reserve(ist->length());

                    for (size_t i(0); i < ist->length(); ++i) {
                        const auto& in(ist[i]);
                        st.emplace_back( SignalStatItem (
                            {std::string(in.name), in.total, in.rejected, in.asize} )
                        );
                    }
                }
                catch(const CORBA::SystemException &e) {
                    istd_TRC(istd::eTrcHigh,
                        "Can't get remote statistics (ignoring): " << e);
                }
                catch(const CORBA::Exception &e) {
                    istd_TRC(istd::eTrcHigh,
                        "Can't get remote statistics (ignoring): " << e);
                }
                catch(...) {
                    istd_TRC(istd::eTrcHigh,
                        "Can't get remote statistics (ignoring).");
                }
                return st;
            }

        private:
            i_NET::RemoteSignal_var m_remoteSignal;
            i_SignalReceiver_impl*  m_receiver;
            size_t                  m_remoteCapacity;
            bool                    m_opened;

            virtual void OpenInput()
            {
                istd_FTRC();

                if (m_opened) {
                    return;
                }
                if (CORBA::is_nil(m_remoteSignal)) {
                    throw istd::Exception("OpenInput: Missing remote signal reference.");
                }
                try {
                    i_NET::SignalReceiver_var sr(m_receiver->_this());
                    // Remote signal will starting dispatching data to
                    // the queue as soon as the receiver is registered.
                    auto success = m_remoteSignal->RegisterReceiver(
                        sr, eModeStream, this->GroupSize(), 0, m_remoteCapacity
                    );
                    if (success == eSuccess) {
                        m_opened = true;
                    }
                }
                catch(const CORBA::SystemException &e) {
                    istd_TRC(istd::eTrcHigh, "Can't access remote signal: " << e);
                    throw istd::Exception("isig: can't register signal receiver.");
                }
                catch(const CORBA::Exception &e) {
                    istd_TRC(istd::eTrcHigh, "Can't access remote signal: " << e);
                    throw istd::Exception("isig: can't register signal receiver.");
                }
                catch(...) {
                    throw istd::Exception("isig: can't register signal receiver.");
                }
            };

            virtual void CloseInput()
            {
                istd_FTRC();

                if (!m_opened) {
                    return;
                }
                m_opened = false;
                if (CORBA::is_nil(m_remoteSignal)) {
                    istd_TRC(istd::eTrcHigh, "Close: Missing remote signal reference.");
                    return;
                }
                try {
                    i_NET::SignalReceiver_var sr(m_receiver->_this());
                    m_remoteSignal->UnregisterReceiver(sr);
                }
                catch(const CORBA::SystemException &e) {
                    istd_TRC(istd::eTrcHigh, "Can't access remote signal: " << e);
                }
                catch(const CORBA::Exception &e) {
                    istd_TRC(istd::eTrcHigh, "Can't access remote signal: " << e);
                    throw istd::Exception("isig: can't register signal receiver.");
                }
                catch(...) {
                    istd_TRC(istd::eTrcHigh, "Can't access remote signal.");
                }
            };

            virtual SuccessCode_e ReadInput(Buffer &a_buf)
            {
                throw istd::Exception("RemoteStream::ReadInput should not be called.");
            }

        };
        /**@}*/
}

#endif /* ISIG_REMOTE_STREAM_H */

/*
 * Copyright (c) 2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: dod_client.h 21233 2014-11-12 08:02:36Z damijan.skvarc $
 */

#ifndef ISIG_DOD_CLIENT_H
#define ISIG_DOD_CLIENT_H

#include <thread>
#include <mutex>


#include "isig/declarations.h"
#include "isig/dod_controller_interface.h"
#include "isig/dod_position_controller.h"
#include "isig/dod_client_base.h"

namespace isig {

    template <class DOD_TRAITS>
        class DodClient : public DodClientBase<DOD_TRAITS> {
        public:

            typedef DOD_TRAITS                  Traits;
            typedef typename Traits::Atom       Atom;
            typedef typename Traits::BaseType   BaseType;

            typedef DodClientBase<Traits>       ClientBase;

            typedef typename ClientBase::MetaBuffer     MetaBuffer;
            typedef typename ClientBase::Buffer         Buffer;
            typedef typename ClientBase::MetaBufferPtr  MetaBufferPtr;

            typedef DodControllerInterface<Traits>      Dod;
            typedef typename Dod::SharedPtr             DodSharedPtr;
            typedef typename Dod::WeakPtr               DodWeakPtr;

            using ClientBase::CreateBuffer;
            using ClientBase::Open;

            DodClient() : m_open(false)
            {
            }

            DodClient(
                DodSharedPtr        a_dod,
                const std::string&  a_name,
                const Traits&       a_traits)
            : ClientBase(a_name, a_traits),
              m_open(false),
              m_dod(a_dod)
            {
            }

            virtual ~DodClient()
            {
                Close();
            }

            DodClient(const DodClient& a_other)
            : ClientBase(a_other), m_open(false), m_dod(a_other.m_dod)
            {
            }

            DodClient& operator=(const DodClient& a_other)
            {
                if (&a_other != this) {
                    Close();
                    ClientBase::operator=(a_other);
                    m_dod = a_other.m_dod;
                }
                return *this;
            }

            /**
             * @param a_mode Data on demand acquisition mode
             * @param a_readSize    eModeDodOnEvent: how many atoms to read on each event
             * @param a_offset      offset to apply on each read
             */
            virtual SuccessCode_e Open(
                AccessMode_e a_mode,
                size_t a_readSize,
                int64_t a_offset = 0) OVERRIDE __attribute__((warn_unused_result))
            {
                std::unique_lock<std::mutex> openLock(m_open_x);

                if (m_open) {
                    return (a_mode == m_mode) ? eSuccess : eInvalidArg;
                }

                DodSharedPtr d = m_dod.lock();
                if (!d) {
                    openLock.unlock();
                    Close();
                    return eClosed;
                }
                m_mode = a_mode;
                PosControllerPtr ctrl;

                // A copy of the position controller is used for OnEvent
                // acquisition, otherwise the DOD position controller is shared
                // by all DOD clients
                if (m_mode == eModeDodOnEvent || m_mode == eModeDodSingleEvent) {
                    ctrl = d->GetPositionControllerCopy(m_mode);
                }
                else {
                    ctrl = d->GetPositionController(m_mode);
                }
                if (!ctrl) {
                    return eInvalidArg;
                }
                m_posController = ctrl;

                if (!m_posController) {
                    istd_TRC(istd::eTrcLow,"DodClient::Open Internal error: position controller not defined");
                    return eInvalidArg;
                }
                m_queue.Reset();
                m_offset = a_offset;
                m_readSize = std::min(
                    a_readSize,
                    m_posController->GetMaxReadSize(m_offset, m_mode)
                );

                m_open = true;
                openLock.unlock();
                if (a_mode == eModeDodOnEvent) {
                    std::unique_lock < std::mutex > l(m_mutex);
                    m_streamThread = std::thread(std::ref(*this));
                    m_started.wait(l);
                    if (!m_running) {
                        // Unable to start the thread: join it and return error
                        if (m_streamThread.joinable()) {
                            m_streamThread.join();
                        }
                        openLock.lock();
                        m_open = false;
                        istd_TRC(istd::eTrcLow,"DodClient::Open failed to open");
                        return eNoData;
                    }
                }
                return eSuccess;
            }

            virtual bool IsOpen() const OVERRIDE
            {
                std::unique_lock<std::mutex> l(m_open_x);
                return m_open;
            }

            /**
             * @param a_buf output buffer to read data to; buffer size indicates
             *              the required amount of data to be read; it can be
             *              resized if the required amount of data is not available
             * @param a_meta meta data belonging to the buffer
             * @param a_position position to read from (not applicable in all modes and controllers)
             */
            SuccessCode_e Read(
                Buffer&         a_buf,
                SignalMeta&     a_meta,
                uint64_t        a_position = 0 )
#ifndef NDEBUG
            __attribute__((warn_unused_result))
#endif
            {
                std::unique_lock<std::mutex> l(m_open_x);
                if (!m_open) {
                    return eClosed;
                }
                DodSharedPtr d = m_dod.lock();
                if (!d) {
                    istd_TRC(istd::eTrcLow,"DodClient::Read -> Signal source is not available");
                    l.unlock();
                    Close();
                    return eClosed;
                }

                if (m_mode == eModeDodOnEvent) {
                    l.unlock();
                    return m_queue.Pop(a_buf, a_meta);
                }

                if (!m_posController) {
                    istd_TRC(istd::eTrcLow,"DodClient::Read -> Position controller is not available");
                    return eClosed;
                }

                const size_t rs = std::min(
                    a_buf.GetLength(),
                    m_posController->GetMaxReadSize(m_offset, m_mode)
                );
                size_t absPos;
                LMT lmt;
                auto ret = m_posController->GetPosition(
                    m_mode, a_position, m_offset, rs,
                    lmt, absPos, a_meta);

                if (ret != eSuccess) {
                    return ret;
                }

                a_buf.Resize(rs);

                try {
                    l.unlock();
                    return d->Read(a_buf, absPos, m_posController);
                }
                catch (const std::exception& e) {
                    istd_TRC(istd::eTrcMed,
                        "dod: Reading from input buffer failed: " << e.what());
                }
                return eIoError;
            }

            virtual void Close() OVERRIDE
            {
                std::unique_lock<std::mutex> l(m_open_x);
                if (!m_open) {
                    return;
                }
                m_open = false;
                l.unlock();
                if (m_mode == eModeDodOnEvent) {
                    // The controller shall stop waiting for events
                    istd_TRC(istd::eTrcLow,"DodClient::Close m_posController->Stop");
                    m_posController->Stop();
                    if (m_streamThread.joinable()) {
                        m_streamThread.join();
                    }
                    m_queue.Cancel(eClosed);
                }
                if (m_mode == eModeDodOnEvent || m_mode == eModeDodSingleEvent) {
                    DodSharedPtr d = m_dod.lock();
                    if (d) {
                        d->ReleasePositionControllerCopy(m_posController);
                    }
                }
                m_posController.reset();
            }

        protected:

            using ClientBase::m_queue;

            /**
             * Reads one buffer from the queue.
             * @note Used internally only for mode eModeDodOnEvent.
             */
            inline SuccessCode_e Read(MetaBufferPtr& a_buf)
            {
                if (m_mode != eModeDodOnEvent) {
                    return eInternalError;
                }
                return m_queue.Pop(a_buf);
            }

            /**
             * @see SignalClientBase::ForwardTo
             */
            virtual SuccessCode_e ForwardTo(i_NET::SignalReceiver_ptr a_rec) OVERRIDE
            {
                if (m_mode != eModeDodOnEvent) {
                    return eNoData;
                }
                // TODO: The same piece of code is also in the stream client
                // Try to merge into a single function

                // Dispatch 1 to c_minRemoteQueue buffers in one run
                const size_t elements =
                    std::max(
                        static_cast<size_t>(1),
                        std::min(m_queue.Size(), c_minRemoteQueue)
                    );

                istd_TRC(istd::eTrcDetail, "Dispatching elements: " << elements);

                SuccessCode_e ret(eSuccess);
                for (size_t i(0); i < elements; ++i) {
                    MetaBufferPtr b;
                    SuccessCode_e ret = Read(b);
                    if (ret != eSuccess) {
                        break;
                    }
                    SignalClientBase::DoDispatch(
                        a_rec,
                        b->data.GetBuffer(),
                        b->data.GetBufferLength(),
                        b->meta);
                }
                return ret;
            }

            /**
             * @see SignalClientBase::Read
             */
            virtual SuccessCode_e Read(
                BaseType*   a_buf,
                size_t&     a_count,
                uint64_t    a_position,
                SignalMeta& a_meta) OVERRIDE
            {
                Buffer b(a_buf, a_count, ClientBase::GetTraits());
                auto ret = Read(b, a_meta, a_position);
                a_count = b.GetBufferLength();
                return ret;
            }

        private:
            bool                    m_open;
            mutable std::mutex      m_open_x;
            DodWeakPtr              m_dod;
            AccessMode_e            m_mode;
            PosControllerPtr        m_posController;
            size_t                  m_readSize;
            int64_t                 m_offset;

            std::thread             m_streamThread;
            std::condition_variable m_started;
            bool                    m_running;
            mutable std::mutex      m_mutex;

        public:
            /**
             * Thread for mode eModeDodOnEvent to dispatch the buffers as a
             * stream.
             */
            void operator ()()
            {
                istd_FTRC();

                auto dod(m_dod.lock());
                {
                    std::unique_lock < std::mutex > l(m_mutex);
                    m_running = dod ? true : false;
                    m_started.notify_one();
                    if (!m_running) {
                        return;
                    }
                }

                try {
                    while (IsOpen()) {
                        Buffer b(m_readSize, ClientBase::GetTraits());
                        SignalMeta meta;
                        size_t absPos;
                        LMT lmt;
                        auto ret = m_posController->GetPosition(
                            m_mode, 0, m_offset, m_readSize,
                            lmt, absPos, meta
                        );

                        if (ret == eNoData) {
                            istd_TRC(istd::eTrcLow, "Get position again.");
                            continue;
                        }
                        if (ret == eSuccess) {
                            ret = dod->Read(b, absPos, m_posController);
                            if (ret == eSuccess) {
                                m_queue.Push(std::move(b), meta);
                            }
                        }
                        if (ret != eSuccess) {
                            m_queue.Cancel(ret);
                            if ((ret == eIncomplete) || (ret == eTimeout)) {
                                continue;
                            }
                            break;
                        }
                    }
                    // Normal completion
                    m_queue.Cancel(eClosed);
                    goto exit;
                }
                catch (const std::system_error& e) {
                    istd_TRC(istd::eTrcLow,
                        "dod client: Dispatching thread error: "
                        << e.code().message());
                }
                catch (const std::exception& e) {
                    istd_TRC(istd::eTrcLow,
                        "dod client: Dispatching thread error: " << e.what());
                }
                m_queue.Cancel(eIoError);

            exit:
                m_running = false;
            }

        };

    // DodClient for runtime traits are instantiated in the library
    extern template class DodClient<SignalTraitsVarUint8>;
    extern template class DodClient<SignalTraitsVarInt16>;
    extern template class DodClient<SignalTraitsVarUint16>;
    extern template class DodClient<SignalTraitsVarInt32>;
    extern template class DodClient<SignalTraitsVarUint32>;
    extern template class DodClient<SignalTraitsVarInt64>;
    extern template class DodClient<SignalTraitsVarUint64>;
    extern template class DodClient<SignalTraitsVarFloat>;
    extern template class DodClient<SignalTraitsVarDouble>;

}

#endif /* ISIG_DOD_CLIENT_H */

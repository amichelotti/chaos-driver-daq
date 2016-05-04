/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: stream_client.h 19990 2014-01-07 09:35:55Z damijan.skvarc $
 */

#ifndef ISIG_STREAM_CLIENT_H
#define ISIG_STREAM_CLIENT_H

#include <system_error>
#include <thread>
#include <memory>
#include <functional>
#include "istd/trace.h"
#include "istd/system.h"
#include "istd/string.h"
#include "istd/exception.h"
#include "isig/stream.h"
#include "isig/signal_client_base.h"
#include "isig/isig_util.h"
#include "isig/stream_queue.h"

namespace isig {

    /**
	 * @addtogroup signal
	 * @{
	 */

    template <class STREAM_TRAITS>
        class Stream;

    /**
     * Template for the stream-type data source with type (atom).
     *
     * @param STREAM_TRAITS   type of one record (atom)
     */
    template <class STREAM_TRAITS>
        class StreamClient : public SignalClientBase {
        public:
            typedef STREAM_TRAITS                       Traits;
            typedef typename Traits::Atom               Atom;

            typedef MetaBufferPair<Traits>              MetaBuffer;
            typedef typename MetaBuffer::Buffer         Buffer;
            typedef typename MetaBuffer::MetaBufferPtr  MetaBufferPtr;

            // TODO: Replace with shared_ptr!!
            typedef Stream<Traits>*                     StreamPtr;

        private:
            typedef StreamQueue<Traits>                 ThisStreamQueue;
            typedef typename ThisStreamQueue::FilterPtr FilterPtr;
            typedef typename ThisStreamQueue::Filter    Filter;

        public:

            StreamClient()
            : m_stream(NULL),
              m_traits(Traits()),
              m_queue(m_traits, "Useless default client", 1),
              m_open(false)
            {
            };

            /**
              * @param a_stream     Stream to which this client should connect
              * @param a_capacity   The capacity of this client's queue in atom groups.
              */
            StreamClient(
                StreamPtr a_stream,
                const std::string &a_name,
                unsigned int a_capacity = ThisStreamQueue::c_defaultCapacity)
            : m_stream(a_stream),
              m_traits(m_stream->GetTraits()),
              m_queue(m_traits, a_name, a_capacity),
              m_open(false)
            {
            };

            // Copy ctor
            StreamClient(const StreamClient& a_other)
            : m_stream(a_other.m_stream),
              m_traits(a_other.m_traits),
              m_queue(a_other.m_queue),
              m_open(false)
            {
            };

            // Assignment operator
            StreamClient& operator=(const StreamClient& a_other)
            {
                if (&a_other == this) {
                    return *this;
                }
                m_stream = a_other.m_stream;
                m_traits = a_other.m_traits;
                m_queue = a_other.m_queue;
                m_open = false;
                return *this;
            }

            // Move assignment
            StreamClient& operator=(StreamClient&& a_other)
            {
                if (&a_other == this) {
                    return *this;
                }
                m_stream = a_other.m_stream;
                std::swap(m_traits, a_other.m_traits);
                std::swap(m_queue, a_other.m_queue);
                m_open = a_other.m_open;
                return *this;
            }

            // Move ctor
            StreamClient(const StreamClient&& a_other)
            : m_stream(a_other.m_stream),
              m_traits( std::move(a_other.m_traits) ),
              m_queue( std::move(a_other.m_queue) ),
              m_open(a_other.m_open)
            {
            };

            virtual ~StreamClient()
            {
                Close();
            }

            virtual AccessType_e SignalAccessType() OVERRIDE
            {
                return eAccessStream;
            }

            virtual std::string GetName() const OVERRIDE
            {
                return m_queue.GetName();
            }

            /**
             * @param a_length number in atom units as defined in the
             *                 signal traits.
             */
            Buffer CreateBuffer(size_t a_length)
            {
                if (m_stream == NULL) {
                    istd_EXCEPTION(
                        "isig: This client doesn't have a stream associated.");
                }
                return Buffer(a_length, m_traits);
            }

            /**
             * Operator to access the stream functions.
             * @return
             */
            StreamPtr operator->()
            {
                return m_stream;
            }

            /** @name Client interface
             *  These functions are intended to be used by stream clients
             *  @{
             */

            virtual SuccessCode_e Open() OVERRIDE __attribute__((warn_unused_result))
            {
                std::unique_lock<std::mutex> l(m_open_x);

                if (m_open) {
                    return eSuccess;
                }

                if (m_stream == NULL) {
                    // Client doesn't have an associated stream
                    return eTerminated;
                }

                if (m_queue.GetStatus() == eTerminated) {
                    // Don't attempt to access terminated (deallocated)
                    // stream.
                    return eTerminated;
                }
                SuccessCode_e ret = m_stream->AddQueue(m_queue);
                m_open = (ret == eSuccess);
                return ret;
            };

            virtual void Close() OVERRIDE
            {
                std::unique_lock<std::mutex> l(m_open_x);

                if (!m_open) {
                    return;
                }
                m_open = false;
                if (m_queue.GetStatus() == eTerminated) {
                    // Don't attempt to access terminated (deallocated)
                    // stream.
                    return;
                }
                // Only access m_stream if the queue is not canceled
                m_stream->RemoveQueue(m_queue);
            };

            virtual bool IsOpen() const OVERRIDE
            {
                std::unique_lock<std::mutex> l(m_open_x);
                return m_open;
            }

            /**
             * Returns the number of rejected atoms
             * (skipped on dispatch on full queue) since the last open.
             */
            size_t RejectedCount() const {
                return m_queue.RejectedCount();
            }

            /**
             * Returns the number of total atoms, delivered to the queue.
             * including rejected.
             */
            size_t TotalCount() const {
                return m_queue.TotalCount();
            }

            virtual SignalStats GetStatistics() const OVERRIDE
            {
                SignalStats st(m_stream->GetStatistics());
                auto qst(m_queue.GetStatistics());
                st.insert(st.end(), qst.begin(), qst.end());
                return st;
            }

            /**
             * Set read timeout in seconds
             */
            void SetReadTimeout(const std::chrono::seconds& a_timeout) {
                std::chrono::milliseconds t(a_timeout);
                m_queue.SetReadTimeout(t);
            }

            /**
             * Set read timeout in seconds
             */
            void SetReadTimeout(const std::chrono::milliseconds& a_timeout) {
                m_queue.SetReadTimeout(a_timeout);
            }

            /**
             * Read returns the shared pointer to the buffer.
             *
             * @warning Do not modify the content of the buffer, because a
             *  pointer to the same buffer is shared with other clients as well.
             */
            SuccessCode_e Read(MetaBufferPtr& a_buffer) __attribute__((warn_unused_result))
            {
                if (!IsOpen()) {
                    return eClosed;
                }
                return m_queue.Pop(a_buffer);
            };

            /**
             * Reading buffer from data stream.
             * Buffer is isig::Array as defined by signal traits.
             * It must be a multiple of GroupSize().
             *
             * @param a_atoms Buffer to read data into
             * @return Success code of the operation (
             * @see SuccessCode_e
             */
            SuccessCode_e Read(Buffer &a_atoms) __attribute__((warn_unused_result))
            {
                if (!IsOpen()) {
                    return eClosed;
                }
                SuccessCode_e qs = m_queue.GetStatus();
                if (qs != eSuccess) {
                    return qs;
                }
                if ( (a_atoms.GetLength() % m_traits.GetGroupSize()) != 0) {
                    istd_TRC(istd::eTrcLow,
                        "stream: Read size must be a multiple of group: "
                        << m_traits.GetGroupSize());
                    return eInvalidArg;
                }
                const size_t total(a_atoms.GetLength());
                Buffer out(m_traits);
                out.Reserve(total);
                while (out.GetLength() < total) {
                    MetaBufferPtr b;
                    auto status = m_queue.Pop(b);
                    if (status != eSuccess) {
                        return status;
                    }
                    out.Append(b->data);
                }
                out.Resize(total);
                std::swap(a_atoms, out);
                return eSuccess;
            };

            SuccessCode_e Read(Atom &a_atom) __attribute__((warn_unused_result))
            {
                if (!IsOpen()) {
                    return eClosed;
                }
                SuccessCode_e qs = m_queue.GetStatus();
                if (qs != eSuccess) {
                    return qs;
                }
                if (m_traits.GetGroupSize() > 1) {
                    istd_TRC(istd::eTrcLow,
                        "stream: Can't use this function for group size: "
                        << m_traits.GetGroupSize());
                    return eInvalidArg;
                }
                MetaBufferPtr b;
                auto ret = m_queue.Pop(b);
                if (ret == eSuccess) {
                    istd_ASSERT(b->data.GetLength() == 1);
                    a_atom = b->data[0];
                }
                return ret;
            };


            /**
             * Copies latest atom from the queue if available without removing
             * it from the queue.
             */
            SuccessCode_e PeekLast(Atom &a_atom) const __attribute__((warn_unused_result))
            {
                if (!IsOpen()) {
                    return eClosed;
                }
                SuccessCode_e qs = m_queue.GetStatus();
                if (qs != eSuccess) {
                    return qs;
                }
                return m_queue.PeekLast(a_atom);
            };

            /**
             * Set filter method
             */
            void SetFilter(const FilterPtr &a_filter) {
                m_queue.SetFilter(a_filter);
            }

            void ClearFilter() {
                m_queue.ClearFilter();
            }

            template<typename OBJ_TYPE, typename ...ARGS>
                static FilterPtr CreateFilter(ARGS&& ...a_args)
                {
                    FilterPtr sp(new OBJ_TYPE(std::forward<ARGS>(a_args)...));
                    return sp;
                }

            /**
             * @note Policy can be only changed when the client is closed.
             * @see isig::QueuePolicy
             */
            void SetPolicy(QueuePolicy a_policy)
            {
                std::unique_lock<std::mutex> l(m_open_x);
                if (m_open) {
                    return;
                }
                m_queue.SetPolicy(a_policy);
            }

            inline QueuePolicy GetPolicy() const
            {
                return m_queue.GetPolicy();
            }

            void ResizeQueue(size_t a_size)
            {
                std::unique_lock<std::mutex> l(m_open_x);
                if (m_open) {
                    return;
                }
                m_queue.Resize(a_size);
            }

            void ResetQueue()
            {
                std::unique_lock<std::mutex> l(m_open_x);
                m_queue.Reset();
            }


            ///@}

        protected:
            StreamPtr           m_stream;

        private:
            Traits              m_traits;
            ThisStreamQueue     m_queue;
            bool                m_open;
            mutable std::mutex  m_open_x;

            /**
             * @see SignalClientBase::ForwardTo.
             */
            virtual SuccessCode_e ForwardTo(i_NET::SignalReceiver_ptr a_rec) OVERRIDE
            {
                if (m_stream == NULL) {
                    istd_EXCEPTION(
                        "isig: Stream client does't have a stream associated.");
                }

                // TODO: The same piece of code is also in the DOD client
                // Try to merge into a single function

                // Dispatch 1 to c_minRemoteQueue buffers in one run
                const size_t elements =
                    std::max(
                        static_cast<size_t>(1),
                        std::min(m_queue.Size(), c_minRemoteQueue)
                    );

                istd_TRC(istd::eTrcDetail, "Dispatching elements: " << elements);

// TODO: Possible performance optimisation in the network interface is to
// collect multiple buffers and send them all at once over the network
// instead of one by one.

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

        };
        /**@}*/

        // StreamClient for runtime traits are instantiated in the library
        extern template class StreamClient<SignalTraitsVarInt8>;
        extern template class StreamClient<SignalTraitsVarUint8>;
        extern template class StreamClient<SignalTraitsVarInt16>;
        extern template class StreamClient<SignalTraitsVarUint16>;
        extern template class StreamClient<SignalTraitsVarInt32>;
        extern template class StreamClient<SignalTraitsVarUint32>;
        extern template class StreamClient<SignalTraitsVarInt64>;
        extern template class StreamClient<SignalTraitsVarUint64>;
        extern template class StreamClient<SignalTraitsVarFloat>;
        extern template class StreamClient<SignalTraitsVarDouble>;

}

#endif /* ISIG_STREAM_CLIENT_H */


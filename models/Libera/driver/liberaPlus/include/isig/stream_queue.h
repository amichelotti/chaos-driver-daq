/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: stream_queue.h 20395 2014-04-28 10:52:25Z damijan.skvarc $
 */


#ifndef ISIG_STREAM_QUEUE_H_
#define ISIG_STREAM_QUEUE_H_

#include <mutex>
#include <condition_variable>
#include <vector>
#include <memory>
#include "istd/assert.h"
#include "istd/trace.h"
#include "istd/compat.h"
#include "isig/declarations.h"
#include "isig/buffer_queue.h"
#include "isig/stream_filter.h"

namespace isig {

    template <class TRAITS>
        class StreamQueue {
        public:
            typedef TRAITS                              Traits;
            typedef typename Traits::Atom               Atom;
            typedef MetaBufferPair<Traits>              MetaBuffer;
            typedef typename MetaBuffer::Buffer         Buffer;
            typedef typename MetaBuffer::MetaBufferPtr  MetaBufferPtr;

            typedef typename isig::StreamFilter<Traits> Filter;
            typedef typename std::shared_ptr<Filter>    FilterPtr;

        private:
            typedef BufferQueue<Traits>                 DataQueue;

            const static double c_readWait;

        public:

            const static size_t c_defaultCapacity = 64;

            StreamQueue(
                const Traits& a_traits,
                const std::string &a_name,
                unsigned int a_capacity = c_defaultCapacity)
            : m_traits(a_traits),
              m_name(a_name),
              m_waiting(false),
              m_status(eSuccess),
              m_eofReported(false),
              m_total(0),
              m_rejected(0),
              m_filtTotal(0),
              m_filtRejected(0),
              m_readTimeout(0),
              m_queue(a_capacity, a_traits),
              m_policy(QueuePolicy::eOnFullKeepOld)
            {
                istd_TRC(istd::eTrcHigh,
                    m_name << ": New stream queue. Length: "
                    << m_queue.Capacity());
            };

            // Copy ctor
            StreamQueue(const StreamQueue &a_other)
            : m_traits(a_other.m_traits),
              m_name(a_other.m_name + "_copy"),
              m_waiting(false), // No client is waiting for the data from this queue
              m_status(eSuccess), // Initial status
              m_eofReported(false), // EOF not yet reported on this queue
              m_total(0),  // No atoms were processed yet
              m_rejected(0), // No atoms were rejected yet
              m_filtTotal(0),
              m_filtRejected(0),
              // We don't copy other client's data queue, but create
              // new, empty buffer queue instead
              m_queue(a_other.m_queue.Capacity(), m_traits)
            {
                // copying members that can be accessed from other threads
                std::lock_guard<std::mutex> l(a_other.m_queue_x);
                m_readTimeout = a_other.m_readTimeout;
                m_filter = a_other.m_filter;
                m_policy = a_other.m_policy;

                istd_TRC(istd::eTrcHigh,
                    m_name << ": Copied stream queue. Length: " << m_queue.Capacity());
            };

            // Assignment operator
            StreamQueue& operator=(const StreamQueue &a_other)
            {
                if (&a_other == this) {
                    return *this;
                }
                m_name = a_other.m_name + "_copy";
                m_waiting = false;
                m_status = eSuccess;
                m_eofReported = false;
                m_rejected = 0;
                // copying members that can be accessed from other threads
                std::lock_guard<std::mutex> l(a_other.m_queue_x);

                // We don't copy other client's data queue, but create
                // new, empty buffer queue instead
                m_queue = DataQueue(a_other.m_queue.Capacity(), m_traits);
                m_readTimeout = a_other.m_readTimeout;
                m_filter = a_other.m_filter;
                m_policy = a_other.m_policy;

                istd_TRC(istd::eTrcHigh,
                    m_name << ": Assigned stream queue. Length: " << m_queue.Capacity());

                return *this;
            };

            // Move ctor
            StreamQueue(StreamQueue &&a_other)
            : m_traits( std::move(a_other.m_traits) ),
              m_name( std::move(a_other.m_name) ),
              m_waiting(a_other.m_waiting),
              m_status(a_other.m_status),
              m_eofReported(a_other.m_eofReported),
              m_rejected(a_other.m_rejected),
              m_readTimeout(a_other.m_readTimeout),
              m_queue( std::move(a_other.m_queue) ),
              m_filter( std::move(a_other.m_filter) ),
              m_policy(a_other.m_policy)
            {
                istd_TRC(istd::eTrcHigh,
                    m_name << ": Moved stream queue. Length: " << m_queue.Capacity());
            };

            // Move assignment
            StreamQueue& operator=(StreamQueue &&a_other)
            {
                if (&a_other == this) {
                    return *this;
                }
                m_waiting = a_other.m_waiting;
                std::swap(m_name, a_other.m_name);
                m_status = a_other.m_status;
                m_eofReported = a_other.m_eofReported;
                m_rejected = a_other.m_rejected;
                m_readTimeout = a_other.m_readTimeout;
                std::swap(m_queue, a_other.m_queue);
                std::swap(m_filter, a_other.m_filter);
                m_policy = a_other.m_policy;

                istd_TRC(istd::eTrcHigh,
                    m_name << ": Moved stream queue. Length: " << m_queue.Capacity());

                return *this;
            };

            ~StreamQueue()
            {
            };

            void Reset()
            {
                std::lock_guard<std::mutex> l(m_queue_x);
                if (m_status == eTerminated) {
                    throw istd::Exception(
                        "isig: Can't connect to terminated stream.");
                }
                m_status = eSuccess;
                m_eofReported = false;
                m_total = 0;
                m_rejected = 0;
                m_filtTotal = 0;
                m_filtRejected = 0;
                m_queue.Reset();
            }

            void Resize(size_t a_size)
            {
                std::lock_guard<std::mutex> l(m_queue_x);
                istd_TRC(istd::eTrcHigh,
                    m_name << ": resizing from "
                    << m_queue.Capacity() << " to " << a_size);

                m_queue.Resize(a_size);
            }

            void Cancel(SuccessCode_e a_code)
            {
                istd_FTRC();
                std::lock_guard<std::mutex> l(m_queue_x);
                if (a_code == eSuccess) {
                    a_code = eTerminated;
                }
                m_status = a_code;
                m_ready_c.notify_one();
            }

            SuccessCode_e GetStatus() const
            {
                std::lock_guard<std::mutex> l(m_queue_x);
                return m_status;
            }

            const std::string& GetName() const
            {
                return m_name;
            }

            /**
             * Number of elements in the queue
             */
            size_t Size() const
            {
                std::lock_guard<std::mutex> l(m_queue_x);
                return m_queue.Size();
            }

            /**
             * Capacity of the queue
             */
            size_t Capacity() const
            {
                return m_queue.Capacity();
            }

            /** Rejected atoms because of overflows. */
            size_t RejectedCount() const
            {
                std::lock_guard<std::mutex> l(m_queue_x);
                return m_rejected;
            }

            /** Total atoms before adding to the queue. */
            size_t TotalCount() const
            {
                std::lock_guard<std::mutex> l(m_queue_x);
                return m_total;
            }

            /** Rejected atoms by the filter. */
            size_t FilterRejectedCount() const
            {
                std::lock_guard<std::mutex> l(m_queue_x);
                return m_filtRejected;
            }

            /** Total atoms before adding to the queue. */
            size_t FilterTotalCount() const
            {
                std::lock_guard<std::mutex> l(m_queue_x);
                return m_filtTotal;
            }


            SignalStats GetStatistics() const
            {
                std::lock_guard<std::mutex> l(m_queue_x);
                SignalStats st;
                if (m_filter) {
                    // Filter statistics
                    st.emplace_back( SignalStatItem(
                        { m_name+"_filt",
                          m_filtTotal,
                          m_filtRejected,
                          m_traits.GetAtomSize() })
                    );
                }
                // Queue statistics
                st.emplace_back( SignalStatItem(
                    { m_name, m_total, m_rejected, m_traits.GetAtomSize() })
                );
                return st;
            }

            /**
             * Set read timeout in seconds
             */
            void SetReadTimeout(const std::chrono::milliseconds& a_timeout)
            {
                m_readTimeout = a_timeout;
            }

            void SetPolicy(QueuePolicy a_policy)
            {
                m_policy = a_policy;
            }

            QueuePolicy GetPolicy() const
            {
                return m_policy;
            }

            bool Push(Buffer&& a_buffer, const SignalMeta& a_meta = c_noMeta)
            {
                MetaBufferPtr b(new MetaBuffer( {std::move(a_buffer), a_meta} ));
                return Push(b);
            }

            bool Push(const Buffer& a_buffer, const SignalMeta& a_meta = c_noMeta)
            {
                MetaBufferPtr b(new MetaBuffer( {a_buffer, a_meta} ));
                return Push(b);
            }

            /**
             * @note a_meta is ignored in this implementation of the queue
             */
            bool Push(const MetaBufferPtr& a_buffer)
            {
                std::lock_guard<std::mutex> l(m_queue_x);

                if (m_traits.GetComponents() == 0) {
                    return eInternalError;
                }

                if (a_buffer->data.GetLength() == 0) {
                    istd_TRC(istd::eTrcHigh, "Pushing empty buffer.");
                    return eInternalError;
                }

                MetaBufferPtr buffer(a_buffer);
                auto len = buffer->data.GetLength();

                // filter buffer if callback is defined
                if (m_filter && len > 0) {
                    m_filtTotal += len;
                    buffer = m_filter->Filter(a_buffer);
                    if (!buffer) {
                        m_filtRejected += len;
                        return false;
                    }
                    auto prevLen = len;
                    len = buffer->data.GetLength();
                    if (len == 0) {
                        m_filtRejected += len;
                        return false;
                    }
                    m_filtRejected += prevLen - len;
                }
                m_total += len;

                if (m_queue.IsFull()) {
                    if (m_policy == QueuePolicy::eOnFullKeepOld) {
                        istd_TRC(istd::eTrcHigh, "Stream queue " << m_name << " is full.");
                        m_rejected += len;
                        return false;
                    }
                    else if (m_policy == QueuePolicy::eOnFullKeepNew) {
                        // Make space for new buffer
                        MetaBufferPtr old;
                        m_queue.Pop(old);
                        m_rejected += old->data.GetLength();
                        istd_TRC(istd::eTrcHigh,
                            "Stream queue " << m_name << " is full: " <<
                            "discarded old buffer.");
                    }
                    else {
                        istd_EXCEPTION("INTERNAL ERROR: Unknown queue policy.");
                    }
                }

                m_queue.Push(buffer);
                if (m_waiting) {
                    // Reader is waiting for data:
                    // thread must be notified of the queue state change
                    m_ready_c.notify_one();
                }
                return true;
            }

            /**
             * @param a_atoms   MetaBuffer to receive atoms from the queue
             * @param a_meta    Meta data, belonging to the buffer
             * @return Status code
             */
            SuccessCode_e Pop(Buffer& a_atoms, SignalMeta& a_meta)
                __attribute__((warn_unused_result))
            {
                MetaBufferPtr b;
                auto ret = Pop(b);
                if (b) {
                    if (b.use_count() == 1) {
                        a_atoms = std::move(b->data);
                        a_meta = std::move(b->meta);
                    }
                    else {
                        // Copy the data to output
                        a_atoms = b->data;
                        a_meta = b->meta;
                    }
                }
                return ret;
            }

            SuccessCode_e Pop(MetaBufferPtr& a_buffer) __attribute__((warn_unused_result))
            {
                std::unique_lock<std::mutex> l(m_queue_x);
                a_buffer.reset();

                if (m_traits.GetComponents() == 0) {
                    return eInternalError;
                }

                if (m_status != eSuccess) {
                    if (!m_eofReported) {
                        istd_TRC(istd::eTrcLow, m_name << " canceled, returning " << m_status);
                        m_eofReported = true;
                        return m_status;
                    }
                    // EOF situation already reported, throw exception
                    istd_EXCEPTION("Stream queue " << m_name << " is closed.");
                }

                std::cv_status ret(std::cv_status::no_timeout);
                while (m_queue.IsEmpty() && m_status == eSuccess) {
                    m_waiting = true;

                    if (m_readTimeout == std::chrono::milliseconds::zero()) {
                        m_ready_c.wait(l);
                    }
                    else {
                        ret = m_ready_c.wait_for(l, m_readTimeout);
                        if (ret == std::cv_status::timeout) {
                            // Timeout occurred.

                            // TODO: Can wait_for return before the timeout
                            // even if it was not signaled?
                            // http://www.stdthread.co.uk/doc/headers/condition_variable/condition_variable/wait_for_pred.html
                            m_status = eTimeout;
                            istd_TRC(istd::eTrcDetail, "Queue " << m_name << " timeout reached");
                            break;
                        }
                    }
                }
                m_waiting = false;

                /* timeout & incomplete status are temporary statuses, connected to read */
                /* request and should not be stored permanently in m_status variable.    */
                if (m_status == eTimeout) {
                    istd_TRC(istd::eTrcHigh, m_name << ": Timeout reached, returning " << eTimeout);
                    m_status = eSuccess;
                    return eTimeout;
                }

                if (m_status == eIncomplete) {
                    istd_TRC(istd::eTrcHigh, m_name << ": Incomplete data returned");
                    m_status = eSuccess;
                    return eIncomplete;
                }

                /* other statuses (except timeout & incomplete) are permanent and should persist in m_status */
                if (m_status != eSuccess) {
                    istd_TRC(istd::eTrcHigh, m_name << " canceled, returning " << m_status);
                    m_eofReported = true;
                    return m_status;
                }

                m_queue.Pop(a_buffer);
                return eSuccess;
            }

            /**
             * @param a_atom Atom to be copied from the head of the queue
             * @return status of the operation
             */
            SuccessCode_e PeekLast(Atom& a_atom) const __attribute__((warn_unused_result))
            {
                std::unique_lock<std::mutex> l(m_queue_x);
                if (m_status != eSuccess) {
                    return m_status;
                }
                if (m_queue.IsEmpty()) {
                    return eNoData;
                }
                m_queue.PeekLast(a_atom);
                return eSuccess;
            }

            /**
             * Set filter method
             */
            void SetFilter(const FilterPtr &a_filter) {
                m_filter = a_filter;
            }

            void ClearFilter() {
                m_filter.reset();
            }

        private:
            const Traits                m_traits;
            std::string                 m_name;
            bool                        m_waiting;
            SuccessCode_e               m_status;
            bool                        m_eofReported;
            size_t                      m_total;
            size_t                      m_rejected;
            size_t                      m_filtTotal;
            size_t                      m_filtRejected;
            std::chrono::milliseconds   m_readTimeout;
            DataQueue                   m_queue;
            std::condition_variable     m_ready_c;
            mutable std::mutex          m_queue_x;
            FilterPtr                   m_filter;
            QueuePolicy                 m_policy;
        };

    template <class TRAITS>
        const double StreamQueue<TRAITS>::c_readWait = 0.1;

    // StreamQueue for runtime traits are instantiated in the library
    extern template class StreamQueue<SignalTraitsVarInt8>;
    extern template class StreamQueue<SignalTraitsVarUint8>;
    extern template class StreamQueue<SignalTraitsVarInt16>;
    extern template class StreamQueue<SignalTraitsVarUint16>;
    extern template class StreamQueue<SignalTraitsVarInt32>;
    extern template class StreamQueue<SignalTraitsVarUint32>;
    extern template class StreamQueue<SignalTraitsVarInt64>;
    extern template class StreamQueue<SignalTraitsVarUint64>;
    extern template class StreamQueue<SignalTraitsVarFloat>;
    extern template class StreamQueue<SignalTraitsVarDouble>;

} // namespace

#endif /* ISIG_STREAM_QUEUE_H_ */

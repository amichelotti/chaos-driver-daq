/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: buffer_queue.h 19990 2014-01-07 09:35:55Z damijan.skvarc $
 */


#ifndef BUFFER_QUEUE_H_
#define BUFFER_QUEUE_H_

#include <queue>
#include <memory>

#include "istd/assert.h"
#include "isig/declarations.h"
#include "isig/array.h"

namespace isig {

/**
 * @addtogroup signal Signals and Streams handling Classes and functions
 * @{
 */

    /**
      * Template for the queue that contains pairs Buffer/SignalMeta.
      *
      * This queue is used for storing buffers of Atoms. Buffers do not have to
      * be of the same size. This queue is optimised for cases when larger
      * amount of data needs to be streamed.
      */

    template <class TRAITS>
    class BufferQueue
    {
    public:
        typedef TRAITS                              Traits;
        typedef typename Traits::Atom               Atom;
        typedef MetaBufferPair<Traits>              MetaBuffer;
        typedef typename MetaBuffer::Buffer         Buffer;
        typedef typename MetaBuffer::MetaBufferPtr  MetaBufferPtr;

        /**
         * @param a_capacity  Capacity of circular buffer in atoms. Maximum number
         *                    of atoms which can be held by the queue.
         */
        BufferQueue(
            size_t a_capacity,
            const Traits& a_traits = Traits())
         : m_capacity(a_capacity)
        {
        }

        // Copy ctor
        BufferQueue(const BufferQueue& a_other)
         : m_container(a_other.m_container),
           m_capacity(a_other.m_capacity)
        {
        }

        BufferQueue& operator=(const BufferQueue& a_other)
        {
            if (&a_other == this) {
                return *this;
            }
            m_container = a_other.m_container;
            m_capacity = a_other.m_capacity;
            return *this;
        }

        // Move ctor
        BufferQueue(BufferQueue&& a_other)
         : m_container( std::move(a_other.m_container) ),
           m_capacity(a_other.m_capacity)
        {
            a_other.m_capacity = 0;
        }

        BufferQueue& operator=(BufferQueue&& a_other)
        {
            if (&a_other == this) {
                return *this;
            }
            m_container = std::move(a_other.m_container);
            m_capacity = std::move(a_other.m_capacity);
            return *this;
        }

        /**
         * @return Number of items waiting in the queue
         */
        size_t Size() const
        {
            return m_container.size();
        }

        /**
         * @return Resizes the queue to a_size
         */
        void Resize(size_t a_capacity)
        {
            Reset();
            m_capacity = a_capacity;
        }

        /**
         * @return Available space in atoms
         */
        size_t Available() const
        {
            return m_capacity - Size();
        }

        /**
         * @return Total capacity in atoms
         */
        inline size_t Capacity() const
        {
            return m_capacity;
        }

        /**
         * @warning Copies the input buffer. Use variant with MetaBufferPtr
         *          whenever possible.
         */
        void Push(const Buffer& a_buffer, const SignalMeta& a_meta = c_noMeta)
        {
            // Create a copy of arguments and store it
            auto bp = std::make_shared<MetaBuffer>( MetaBuffer( {a_buffer, a_meta} ) );
            Push(bp);
        }

        /**
         * Adds vector of atoms into the queue.
         *
         * @param  a_buffer buffer of atoms and meta data to be inserted into the queue
         */
        void Push(const MetaBufferPtr& a_buffer)
        {
            if (IsFull()) {
                istd_EXCEPTION("isig: The queue is full.");
            }
            istd_ASSERT(m_container.size() <= m_capacity);
            m_container.emplace( a_buffer );
        }

        /**
         * @warning Copies to the output buffer. Use variant with MetaBufferPtr
         *          whenever possible.
         */
        void Pop(Buffer& a_buffer, SignalMeta& a_meta)
        {
            MetaBufferPtr b;
            Pop(b);
            const bool poppedValidBuffer(b);
            istd_ASSERT(poppedValidBuffer == true);
            if (b.use_count() == 1) {
                // We own the only reference to the buffer:
                // contents can be moved
                a_buffer = std::move(b->data);
                a_meta = std::move(b->meta);
            }
            else {
                // Copy the buffer to target
                a_buffer = b->data;
                a_meta = b->meta;
            }
        }

        /**
         * Pops the oldest buffer from the queue. Optimised version, which uses
         * shared pointers to avoid copying.
         * Output number of atoms is defined by the buffer popped from the
         * queue.
         *
         * @param  a_buffer  output buffer and meta
         */
        void Pop(MetaBufferPtr& a_buffer)
        {
            if (IsEmpty()) {
                istd_EXCEPTION("isig: The queue is empty.");
            }
            a_buffer = m_container.front();
            m_container.pop();
        }

        /**
         * Copy last atom from the queue.
         * @throw istd::Exception if the queue is empty
         */
        void PeekLast(Atom& a_atom) const
        {
            if (IsEmpty()) {
                istd_EXCEPTION("isig: Can't get last atom from empty queue.");
            }
            const auto& bp = m_container.back();
            const auto& b = bp->data;
            a_atom = b[b.GetLength()-1];
        }

        /**
         * Empties the queue.
         */
        void Reset()
        {
            m_container = std::move( Queue() );
        }

        inline bool IsEmpty() const
        {
            return m_container.empty();
        }

        inline bool IsFull() const
        {
            return (m_container.size() == m_capacity);
        }

    private:
        typedef std::queue<MetaBufferPtr>   Queue;

        Queue   m_container;
        size_t  m_capacity; // the capacity of the queue
    };

/**@}*/

    // CircularQueueBase for runtime traits are instantiated in the library
    extern template class BufferQueue<SignalTraitsVarUint8>;
    extern template class BufferQueue<SignalTraitsVarInt16>;
    extern template class BufferQueue<SignalTraitsVarUint16>;
    extern template class BufferQueue<SignalTraitsVarInt32>;
    extern template class BufferQueue<SignalTraitsVarUint32>;
    extern template class BufferQueue<SignalTraitsVarInt64>;
    extern template class BufferQueue<SignalTraitsVarUint64>;
    extern template class BufferQueue<SignalTraitsVarFloat>;
    extern template class BufferQueue<SignalTraitsVarDouble>;

}

#endif /* BUFFER_QUEUE_H_ */

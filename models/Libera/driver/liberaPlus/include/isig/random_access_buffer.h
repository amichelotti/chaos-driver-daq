/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: random_access_buffer.h 19990 2014-01-07 09:35:55Z damijan.skvarc $
 */

#ifndef ISIG_RANDOM_ACCESS_BUFFER_H_
#define ISIG_RANDOM_ACCESS_BUFFER_H_

#include <cstddef>
#include <thread>
#include <memory>

#include "istd/id.h"
#include "random_access_request.h"

namespace isig {


    struct  Id_RandomAccessBuffer {};
    typedef istd::Id<Id_RandomAccessBuffer, uint64_t>  RandomAccessBufferId;

    /**
     * Class, designed to queue input requests to the buffer and process them
     * from a priority queue.
     * Derived classes implement actual reading from the input medium, whatever
     * it may be: Libera device file, file on the disk, ...
     */
    class RandomAccessBuffer {
    public:
        /**
         * @param a_size Total size of the random access buffer in bytes.
         */
        RandomAccessBuffer(size_t a_size = 0, const std::string &a_owner = "");
        virtual ~RandomAccessBuffer();

        /** Returns size in bytes. */
        size_t GetByteSize();

        /**
         * Pause will block all new request, until Resume is called. Pause call
         * will wait and return when all requests already in the queue are processed.
         */
        void Pause();
        /**
         * Resume will unblock pending requests, blocked by previous Pause call.
         */
        void Resume();

        void Open();
        void Close();

        int GetOpenCount() const;

        std::size_t Read(
            void*                   a_buf,
            size_t                  a_count,
            size_t                  a_position,
            RequestPriorityLevel_e  a_priority);

        /** Function executed by the thread, executing the requests. */
        void operator()();

        virtual bool IsCircular() { return false; }

        const RandomAccessBufferId GetId() const { return m_id; }

        void                 SetOwner(const std::string &a_owner) { m_owner = a_owner; }
        const std::string&   GetOwner() const                     { return m_owner; }

    protected:
        void    Cleanup();

    private:
        bool    PushRequest(const ReqPtr& a_req);
        ReqPtr  PopRequest();

        void    ClearQueues();
        bool    AreQueuesEmpty();
        ReqPtr  GetNextRequest();
        void    ResetStats();

        virtual size_t QueryByteSize() = 0;
        virtual void OpenInput() = 0;
        virtual void CloseInput() = 0;

        /**
         * Read the data from the source (FPGA via device file, from remote)
         * @param a_buf Where to read data to
         * @param a_bytes Number of bytes to read
         * @param a_position Offset from the start of the buffer
         * @return number of bytes read
         */
        virtual std::size_t  ReadInput(
            void* a_buf,
            size_t a_bytes,
            size_t a_position) = 0;

        size_t                    m_size;
        mutable std::mutex        m_queue_x;
        std::condition_variable   m_queueNotEmpty; // true, when m_queue becomes not empty
        std::condition_variable   m_queueEmpty;    // true, when m_queue becomes empty
        std::vector<RequestQueue> m_queues;
        volatile int              m_openCount;

        /**
         * Sets the ratio between two consecutive priorities when more than one
         * priority queues have requests to process.
         * Ex. when set to 10 this means, that after 10 high priority requests
         * one medium priority request is processes. The same relation is true
         * between medium and low priority requests.
         */
        static const size_t       m_prioLevelRatio;
        std::vector<size_t>       m_stat;  // statistics for priority queues

        std::thread               m_thread;

        bool                      m_paused;
        std::mutex                m_pause_x;
        std::condition_variable   m_pauseStatus;   // true, when m_paused is changed
        RandomAccessBufferId      m_id;            // instance id
        std::string               m_owner;         // owner id, usually filled with board name
    };

    typedef std::shared_ptr<RandomAccessBuffer> RandomAccessBufferPtr;
}

#endif /* ISIG_RANDOM_ACCESS_BUFFER_H_ */

/*
 * Copyright (c) 2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: worker_thread.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef ISTD_WORKER_THREAD_H
#define ISTD_WORKER_THREAD_H

#include <thread>
#include <condition_variable>
#include <string>

namespace istd {

    class WorkerPool;
    struct TerminateWorker;

	/**
	 * @addtogroup istd_threads
	 * @{
	 */
    class WorkerThread {
        public:

            WorkerThread(const std::string& a_id);

            const std::string& GetId() const
            {
                return m_id;
            };

            void Start();
            void Stop();
            void Join()
            {
                m_thread.join();
            };

            bool Joinable() const
            {
                return m_thread.joinable();
            }

            /**
             * Main thread loop
             */
            void operator()();

        protected:
            const std::string   m_id;

            // Make sure all instances are built on the heap by making the
            // destructor non-public
            virtual ~WorkerThread();

            bool StopRequested() const;

            /// Timed wait bound on the condition to stop the execution
            bool TimedWait(unsigned long a_sec, unsigned long a_nsec = 0) const;

            // TODO: Review the interface of main function.
            /** Main function of the worker thread. Derived classes override
                this function.
            */
            virtual void Main()
            {
                // Base worker class has empty main function.
                return;
            };

        private:
            bool                            m_stop;
            mutable std::condition_variable m_stop_cond;
            mutable std::mutex		        m_stop_x;
            std::thread                     m_thread;
            WorkerPool*                     m_pool;

            void SetPool(WorkerPool* const a_pool)
            {
                m_pool = a_pool;
            };

            friend class WorkerPool;
            friend struct TerminateWorker;
            friend struct DeleteMapElement;
    };
    /**@}*/

} // namespace

#endif // WORKER_THREAD_H

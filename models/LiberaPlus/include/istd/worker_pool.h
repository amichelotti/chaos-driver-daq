/*
 * Copyright (c) 2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: worker_pool.h 8621 2010-09-14 05:54:19Z matej.kenda $
 */

#ifndef ISTD_WORKER_POOL_H
#define ISTD_WORKER_POOL_H

#include <map>
#include <string>
#include <thread>

namespace istd {

    class WorkerThread;

	/**
	 * @addtogroup istd_threads
	 * @{
	 */

    typedef std::map< std::string, istd::WorkerThread* >  WorkerMap;

    /**
    Container for worker threads. Hides resource management from the user.
    Acts as abstract factory as well.

    Factory method Create in implemented by derived class, knowing details
    about how to create appropriate type of worker.
    */
    class WorkerPool {
        public:
            WorkerPool();
            virtual ~WorkerPool();

            WorkerThread* GetWorker(const std::string& a_id);
            void Terminate(const std::string& a_id);
            void TerminateAll();

        private:
            WorkerMap   m_wm;
            std::mutex  m_workers_x;

            virtual WorkerThread* Create(const std::string& a_id) = 0;
            void Remove(WorkerThread* const a_wrk);

            // copying not allowed
            WorkerPool(const WorkerPool& other);
            WorkerPool& operator=(const WorkerPool& other);

        friend class WorkerThread;
    };
    /**@}*/

} // namespace

#endif // ISTD_WORKER_POOL_H

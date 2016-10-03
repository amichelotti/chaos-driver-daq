/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: random_access_request.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef ISIG_RANDOM_ACCESS_REQUEST_H_
#define ISIG_RANDOM_ACCESS_REQUEST_H_

#include <mutex>
#include <condition_variable>
#include <memory>
#include <queue>

#include "isig/declarations.h"

namespace isig {

    class RandomAccessRequest;
    typedef std::shared_ptr<RandomAccessRequest> ReqPtr;

    /**
     * Each read request to RandomAccessBuffer is transformed into a request,
     * put into priority queue, which is and processed by a thread.
     */
    class RandomAccessRequest {
    public:
        RandomAccessRequest(
            void*                   a_buf,
            size_t                  a_count,
            size_t                  a_position,
            RequestPriorityLevel_e  a_prioLevel
        );
        ~RandomAccessRequest();

        size_t WaitCompletion() const;
        void Notify(
                size_t a_retval,
                int a_retcode,
                const std::string& a_retmsg
        );

        void*                   buf;
        size_t                  count;
        size_t                  position;
        RequestPriorityLevel_e  priority;

    private:

        /** On successful completion will hold number of bytes read. On failure
         * its value is 0. m_retmsg will contain error code message.
         */
        size_t                          m_retval;
        /** In case of failed read, it will contain error code message. */
        int                             m_retcode;
        std::string                     m_retmsg;
        bool                            m_complete;
        mutable std::mutex              m_retval_x;
        mutable std::condition_variable m_completed_c;
    };

    typedef std::queue<ReqPtr> RequestQueue;

}

#endif /* ISIG_RANDOM_ACCESS_REQUEST_H_ */

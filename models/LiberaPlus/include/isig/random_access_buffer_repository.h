/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: random_access_buffer_repository.h 19025 2013-04-24 08:48:33Z damijan.skvarc $
 */

#ifndef ISIG_RANDOM_ACCESS_BUFFER_REPOSITORY_H
#define ISIG_RANDOM_ACCESS_BUFFER_REPOSITORY_H

#include <typeinfo>
#include <string>

#include "mci/declarations.h"
#include "isig/declarations.h"
#include "isig/random_access_buffer.h"

namespace isig {

    /**
     * @addtogroup signal
     * @
     */
    typedef std::vector<RandomAccessBufferPtr>                          RandomAccessBufferPtrs;
    typedef std::map<isig::RandomAccessBufferId, RandomAccessBuffer*>   RandomAccessBufferMap;
    /**
     * Signal Container class. This class is intended to manage all instantiated signals
     */
    class RandomAccessBufferRepository : public RandomAccessBufferMap
    {

    public:
        static void                           Resume(const std::string &a_owner);
        static void                           Pause(const std::string &a_owner);
        static RandomAccessBufferRepository&  Instance();

    private:
        /* Attach, Detach functions should be called only in RandomAccessBuffer ctor & dtor */
        friend RandomAccessBuffer::RandomAccessBuffer(size_t a_size, const std::string &a_owner);
        friend RandomAccessBuffer::~RandomAccessBuffer();

        static void Attach(RandomAccessBuffer *a_buffer);
        static void Dettach(RandomAccessBuffer *a_buffer);

    private:
        std::mutex m_mutex;
    };

    /**@}*/
}

#endif /* ISIG_RANDOM_ACCESS_BUFFER_REPOSITORY_H */

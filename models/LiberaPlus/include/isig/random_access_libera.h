/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: random_access_libera.h 19990 2014-01-07 09:35:55Z damijan.skvarc $
 */

#ifndef RANDOM_ACCESS_LIBERA_H_
#define RANDOM_ACCESS_LIBERA_H_

#include <string>
#include "random_access_buffer.h"

namespace isig {

    /**
     * Specific RandomAccessBuffer, which reads from a device file,
     * created by the Libera Linux kernel module.
     */
    class RandomAccessLibera: public isig::RandomAccessBuffer {
    public:
        RandomAccessLibera(const std::string& a_devName, const std::string &a_owner = "");
        virtual ~RandomAccessLibera();

        virtual bool IsCircular() { return true; }

    protected:
        virtual size_t QueryByteSize();
        virtual void OpenInput();
        virtual void CloseInput();
        virtual std::size_t  ReadInput(
            void* a_buf,
            size_t a_count,
            size_t a_position);

    private:
        std::string     m_devName;
        int             m_fd;
    };

}

#endif /* RANDOM_ACCESS_LIBERA_H_ */

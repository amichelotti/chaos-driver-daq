/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: random_access_file.h 19990 2014-01-07 09:35:55Z damijan.skvarc $
 */

#ifndef RANDOM_ACCESS_FILE_H_
#define RANDOM_ACCESS_FILE_H_

#include <string>
#include "random_access_buffer.h"

namespace isig {

    /**
     * Specific RandomAccessBuffer, which reads from a file.
     */
    class RandomAccessFile: public isig::RandomAccessBuffer {
    public:
        RandomAccessFile(const std::string& a_fname, size_t a_size = 0, const std::string &a_owner = "");
        virtual ~RandomAccessFile();

    protected:
        virtual size_t QueryByteSize();
        virtual void OpenInput();
        virtual void CloseInput();
        virtual std::size_t  ReadInput(
            void* a_buf,
            size_t a_count,
            size_t a_position);

    private:
        std::string     m_fname;
        int             m_fd;
    };

}

#endif /* RANDOM_ACCESS_FILE_H_ */

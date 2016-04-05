/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: file_stream.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef ISIG_FILE_STREAM_H
#define ISIG_FILE_STREAM_H

#include <string>
#include <system_error>
#include <cerrno>
#include <cstring>
#include <unistd.h>
#include "istd/system.h"
#include "isig/stream.h"

namespace isig {

	/**
	 * @addtogroup signal
	 * @{
	 */

    template <class STREAM_ATOM>
        class FileStream : public Stream <STREAM_ATOM> {
        public:

            using Stream<STREAM_ATOM>::AtomSize;
            typedef typename Stream<STREAM_ATOM>::Buffer   Buffer;

            FileStream(const std::string& a_fname)
                : m_fname(a_fname), m_fd(istd::c_invalidFd)
            {

            };

            virtual ~FileStream()
            {
                // Shutdown must be called in each dtor to disconnect any
                // stream client that is currently connected.
                this->ShutDown();
            };

        private:
            std::string     m_fname;
            int             m_fd;

            virtual void OpenInput()
            {
                if (m_fd != istd::c_invalidFd) {
                    istd_TRC(istd::eTrcDetail, "File: " << m_fname << " already open.");
                    return;
                }
                m_fd = OpenDevice(m_fname, false);
            };

            virtual void CloseInput()
            {
                CloseDevice(m_fd);
            };

            virtual SuccessCode_e ReadInput(Buffer &a_buf)
            {
                size_t s = a_buf.GetByteSize();
                if (s == 0) {
                    return eInvalidArg;
                }
                ssize_t ret = read(m_fd, a_buf.GetBuffer(), s);
                if (ret == -1) {
                    int err = errno;
                    istd_LOG(istd::eTrcMed,
                        "FileStream: Read error " << std::strerror(err));
                    return eIoError;
                }
                return eSuccess;
            }

        };
        /**@}*/
}

#endif /* ISIG_FILE_STREAM_H */

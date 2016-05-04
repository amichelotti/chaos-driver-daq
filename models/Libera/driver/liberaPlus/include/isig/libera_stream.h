/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: libera_stream.h 19990 2014-01-07 09:35:55Z damijan.skvarc $
 */

#ifndef ISIG_LIBERA_STREAM_H
#define ISIG_LIBERA_STREAM_H

#include <string>

#include "istd/system.h"
#include "isig/stream.h"

namespace isig {

    /**
     * Open the device and performs checks of atom and group sizes.
     */
    int OpenLiberaStreamDevice(
        const std::string& a_fname, size_t a_atomSize, uint32_t a_groupSize);

    SuccessCode_e LiberaStreamRead(
        int a_fd, void* a_buf, size_t a_count, size_t a_atomSize, size_t& a_atoms);

    /**
     * This function will try to unblock a_fd pending stream read in
     * Libera device driver and close the device.
     * It sends LIBERA_CANCEL_IO IOCTL code to a_fd device handle.
     */
    void CloseLiberaStreamDevice(int a_fd);

    uint32_t GetLiberaStreamFlushTimeout(const std::string& a_fname);
    void SetLiberaStreamFlushTimeout(const std::string& a_fname, uint32_t a_clockCycles);
    void LiberaStreamFlush(const std::string& a_fname);

    /**
     * Retrieves FPGA and LKM statistics.
     */
    void GetLiberaDeviceStatistics(int a_fd, size_t a_asize, SignalStats& a_stats);

    /**
	 * @addtogroup signal
	 * @{
	 */

    /**
     * Template class, which reads FPGA generated atoms from a device file
     * exported by Libera Linux kernel module.
     */
    template <class STREAM_TRAITS>
        class LiberaStream : public Stream <STREAM_TRAITS> {
        public:

            typedef STREAM_TRAITS               Traits;
            typedef Stream<Traits>              BaseStream;
            typedef typename BaseStream::Buffer Buffer;
            using BaseStream::AtomSize;
            using BaseStream::GroupSize;

            /**
             * @param a_fname   device name
             * @param a_traits  The traits (if default doesn't fit)
             */
            explicit LiberaStream(
                    const std::string&  a_name,
                    const std::string&  a_fname,
                    const Traits&       a_traits = Traits(),
                    const std::string&  a_owner = "") :
                BaseStream(a_name, a_traits, StreamMode_e::Active, a_owner),
                m_fname(a_fname),
                m_fd(istd::c_invalidFd)
            {

            };

            virtual ~LiberaStream()
            {
                // Shutdown must be called in each dtor to disconnect any
                // stream client that is currently connected.
                this->ShutDown();
            };

            virtual SignalStats GetStatistics() const
            {
                istd_FTRC();
                SignalStats stats;
                if (m_fd != istd::c_invalidFd) {
                    GetLiberaDeviceStatistics(m_fd, AtomSize(), stats);
                }
                return stats;
            }

            void SetFlushTimeout(uint32_t a_clockCycles)
            {
                istd_FTRC();
                SetLiberaStreamFlushTimeout(m_fname, a_clockCycles);
            }

            uint32_t GetFlushTimeout() const
            {
                istd_FTRC();
                return GetLiberaStreamFlushTimeout(m_fname);
            }

            void Flush() const
            {
                istd_FTRC();
                LiberaStreamFlush(m_fname);
            }

        protected:
            const std::string   m_fname;
            int                 m_fd;

        private:
            virtual void OpenInput()
            {
                istd_FTRC();
                if (m_fd != istd::c_invalidFd) {
                    istd_TRC(istd::eTrcDetail, "File: " << m_fname << " already open.");
                    return;
                }
                m_fd = OpenLiberaStreamDevice(m_fname, AtomSize(), GroupSize());
            };

            virtual void CloseInput()
            {
                istd_FTRC();
                if (m_fd != istd::c_invalidFd) {
                    istd_TRC(istd::eTrcMed, "Closing file: " << m_fname);
                    CloseLiberaStreamDevice(m_fd);
                    m_fd = istd::c_invalidFd;
                }
            };

            virtual SuccessCode_e ReadInput(Buffer &a_buf)
            {
                size_t s = a_buf.GetByteSize();
                if (s == 0) {
                    return eInvalidArg;
                }
                size_t atoms = 0;
                auto ret = LiberaStreamRead(m_fd, a_buf.GetBuffer(), s, AtomSize(), atoms);
                if (ret == eSuccess) {
                    // Actual read might return less data than requested
                    a_buf.Resize(atoms);
                }
                return ret;
            }

        };
        /**@}*/
}

#endif /* ISIG_LIBERA_STREAM_H */

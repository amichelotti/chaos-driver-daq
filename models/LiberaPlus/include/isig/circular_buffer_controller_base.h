/*
 * Copyright (c) 2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: circular_buffer_controller_base.h 19990 2014-01-07 09:35:55Z damijan.skvarc $
 */

#ifndef CIRCULAR_BUFFER_CONTROLLER_BASE_H_
#define CIRCULAR_BUFFER_CONTROLLER_BASE_H_

#include <chrono>
#include "dod_position_controller.h"

namespace isig {

    using std::chrono::milliseconds;
    using std::chrono::seconds;

    class CircularBufferControllerBase;
    typedef std::shared_ptr<CircularBufferControllerBase>  CircularControllerPtr;

    /**
     * Class implements logic to provide proper positions for circular buffer
     * implementations of data on demand data.
     *
     * Derived classes implement functions to provide LMT of next event
     * (GetEventLmt) and current LMT (GetCurrentLmt).
     */
    class CircularBufferControllerBase: public virtual DodPositionController {

    public:

        /**
         * @param a_size        Total size of the circular buffer in atoms
         * @param a_frequency   Frequency at which LMT is incremented
         */
        explicit CircularBufferControllerBase(
            size_t a_size, const uint64_t& a_frequency);

        virtual ~CircularBufferControllerBase();

        uint64_t GetFrequency() const;

        /**
         * Time when the circular buffer was reset. Writing to absolute position
         * 0 starts then and time before start time is considered invalid.
         */
        void SetStartLmt(const LMT& a_lmt);
        LMT GetStartLmt() const;

        /**
         * An atom is put in circular buffer every a_decimation LMT units.
         */
        void SetDecimation(unsigned int a_decimation);
        unsigned int GetDecimation() const;

        /**
         * Timeout to wait for the data if it is not available at this moment:
         * for example requested LMT is in the future, waiting for an event.
         */
        void SetTimeout(const seconds& a_secs);
        seconds GetTimeout() const;

        /**
         * Area in the circular buffer which is just going to be overwritten.
         * Reading from that area is prohibited. Calculated from frequency and
         * decimation.
         */
        size_t GetDeadZoneSize() const;

        virtual size_t GetMaxReadSize(int64_t, AccessMode_e) const OVERRIDE;

        /**
         * a_posIn defines absolute position for mode eModeDodPosition and LMT for
         * eModeDodLMT.
         *
         * a_posIn is ignored for modes eModeDodOnEvent and eModeDodNow and
         * is acquired from an event.
         *
         * a_readSize is ignored.
         *
         * a_offset is applied only if mode is not eModeDodPosition.
         */
        virtual SuccessCode_e GetPosition(
            const AccessMode_e  a_mode,
            const uint64_t      a_posIn,
            const int64_t       a_offset,
            const size_t        a_readSize,
            LMT&                a_lmt,
            size_t&             a_absPos,
            SignalMeta&         a_meta) OVERRIDE __attribute__((warn_unused_result));

        virtual bool IsPositionValid(const uint64_t& a_pos) const OVERRIDE;

    private:
        /// Size of the history buffer
        const size_t    m_size;
        /// LMT frequency
        const uint64_t  m_frequency;

        /// LMT when the circular buffer was started
        LMT             m_startLmt;
        bool            m_startLmtOk;
        /// decimation used to convert position in LMT to absolute position
        unsigned int    m_decimation;

        size_t          m_deadZone;
        milliseconds    m_timeout;

        virtual SuccessCode_e GetEventLmt(LMT& a_lmt, uint64_t &a_count) const = 0;
        virtual SuccessCode_e GetCurrentLmt(LMT& a_lmt) const = 0;

        /**
         * Converts LMT counter to the number of atoms. Number of atoms
         * can be smaller than LMT, usually because of decimation.
         * @param a_lmt Timestamp in Libera Machine Time
         * @return Corresponding counter in atoms.
         */
        size_t  LmtToMt(uint64_t a_lmt) const;
        milliseconds LmtToMilliseconds(uint64_t a_lmt) const;
    };

}

#endif /* CIRCULAR_BUFFER_CONTROLLER_BASE_H_ */

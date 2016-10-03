/*
 * Copyright (c) 2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: segmented_buffer_controller_base.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef SEGMENTED_BUFFER_CONTROLLER_BASE_H_
#define SEGMENTED_BUFFER_CONTROLLER_BASE_H_

#include <chrono>
#include "dod_position_controller.h"

namespace isig {

    using std::chrono::milliseconds;
    using std::chrono::seconds;

    class SegmentedBufferControllerBase;
    typedef std::shared_ptr<SegmentedBufferControllerBase>  SegmentedControllerPtr;

    /**
     * Class implements logic to provide proper positions for segmented buffer
     * implementations of data on demand data.
     *
     * Derived classes implement functions to provide LMT of next event
     * (GetEventLmt) and current LMT (GetCurrentLmt).
     */
    class SegmentedBufferControllerBase: public virtual DodPositionController {
    public:

        /**
         * @param a_size        Total size of the circular buffer in atoms
         * @param a_segments    Number of segments in the buffer
         */
        explicit SegmentedBufferControllerBase(size_t a_size, size_t a_segments);
        virtual ~SegmentedBufferControllerBase();

        /**
         * Timeout to wait for the data if it is not available at this moment:
         * for example requested LMT is in the future, waiting for an event.
         */
        void SetTimeout(const seconds& a_secs);
        seconds GetTimeout() const;

        /**
         * Offset that is always applied to the acquisition.
         */
        void SetAcquisitionOffset(size_t a_offset);
        size_t GetAcquisitionOffset() const;

        /**
         * @note The controller must be reset after changing segment size
         * @param a_segmentSize in atom units
         */
        void SetSegmentSize(size_t a_segmentSize);
        size_t GetSegmentSize() const;

        size_t GetSegments() const;
        size_t GetValidSegments() const;

        virtual size_t GetMaxReadSize(int64_t a_offset, AccessMode_e a_mode) const OVERRIDE;

        /**
         * a_posIn defines absolute position for mode eDodPosition.
         *
         * a_posIn is ignored for modes eModeDodOnEvent and eModeDodNow;
         * it is acquired from an event.
         *
         * a_posIn is used as minimum event LMT for mode eModeDodSingleEvent.
         *
         * a_offset must be positive and is applied only if mode is
         * not eModeDodPosition.
         *
         * a_readSize is ignored for this controller.
         *
         * @note eModeDodLMT is not supported for segmented buffers
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
        /// Number of segments in the buffer
        size_t          m_segments;
        size_t          m_segmentSize;
        size_t          m_acqOffset;

        milliseconds    m_timeout;

        virtual SuccessCode_e GetEvent(
            uint64_t  a_minLmt,
            uint64_t& a_evtCount,
            uint64_t& a_lastEvtCount,
            LMT& a_lmt) const = 0;

        virtual SuccessCode_e GetLastEvent(uint64_t& a_evtCount, LMT& a_lmt) const = 0;
    };

}

#endif /* SEGMENTED_BUFFER_CONTROLLER_BASE_H_ */

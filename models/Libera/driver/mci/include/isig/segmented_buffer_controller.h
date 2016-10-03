/*
 * Copyright (c) 2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: segmented_buffer_controller.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef ISIG_SEGMENTED_BUFFER_CONTROLLER_H_
#define ISIG_SEGMENTED_BUFFER_CONTROLLER_H_

#include "isig/segmented_buffer_controller_base.h"
#include "isig/libera_event_stream.h"

namespace isig {

    class SegmentedBufferController:
        public SegmentedBufferControllerBase,
        public Clonable<SegmentedBufferController> {

    public:
        SegmentedBufferController(
            size_t      a_size,
            size_t      a_segments,
            uint64_t    a_eventId);

        virtual ~SegmentedBufferController();

        void SetEventClient(const LiberaEventClient& a_evtClient);
        virtual void Reset(const LMT& a_startLmt = 0ULL) OVERRIDE;
        virtual void Start() OVERRIDE;
        virtual void Stop() OVERRIDE;

    private:
        uint64_t                    m_eventId;
        mutable LMT                 m_lastEventLmt;
        mutable uint64_t            m_lastEventCount;
        mutable LiberaEventClient   m_evtClient;

        virtual SuccessCode_e GetEvent(
            uint64_t    a_minLmt,
            uint64_t&   a_evtCount,
            uint64_t&   a_lastEvtCount,
            LMT&        a_lmt) const OVERRIDE;

        virtual SuccessCode_e GetLastEvent(uint64_t& a_evtCount, LMT& a_lmt) const OVERRIDE;

    };

} // namespace

#endif /* ISIG_SEGMENTED_BUFFER_CONTROLLER_H_ */

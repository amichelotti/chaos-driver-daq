/*
 * Copyright (c) 2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: circular_buffer_controller.h 19990 2014-01-07 09:35:55Z damijan.skvarc $
 */

#ifndef CIRCULAR_BUFFER_CONTROLLER_H_
#define CIRCULAR_BUFFER_CONTROLLER_H_

#include "isig/circular_buffer_controller_base.h"
#include "isig/libera_event_stream.h"

namespace isig {

    class CircularBufferController:
        public CircularBufferControllerBase,
        public Clonable<CircularBufferController> {

    public:
        /**
         * Position controller for data on demand acquisitions, which gets LMT
         * from LiberaEventStream.
         * @see CircularBufferControllerBase
         * @param a_eventId The ID on which the GetEventLmt reacts (for example trigger)
         */
        CircularBufferController(
            size_t          a_size,
            const uint64_t& a_frequency,
            uint64_t        a_eventId);

        virtual ~CircularBufferController();

        typedef LiberaEventStream::Client   EvtClient;

        void SetEventClient(const EvtClient& a_evtClient);
        virtual void Reset(const LMT& a_startLmt) OVERRIDE;
        virtual void Start() OVERRIDE;
        virtual void Stop() OVERRIDE;

    private:
        uint64_t            m_eventId;
        mutable EvtClient   m_evtClient;

        virtual SuccessCode_e GetEventLmt(LMT& a_lmt, uint64_t &a_count) const OVERRIDE;
        virtual SuccessCode_e GetCurrentLmt(LMT& a_lmt) const OVERRIDE;

    };

}

#endif /* CIRCULAR_BUFFER_CONTROLLER_H_ */

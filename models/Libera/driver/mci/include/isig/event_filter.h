/*
 * Copyright (c) 2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: event_filter.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef EVENT_FILTER_H_
#define EVENT_FILTER_H_

#include <set>

#include "isig/libera_event_stream.h"
#include "isig/stream_filter.h"

namespace isig {

    class EventFilter : public StreamFilter<EventTraits> {
    public:

        /**
         * Set which event IDs will be read. All others will be filtered out
         * and won't be returned through Read method
         * @param op variadic number of EventIdType_e ids
         */
        template <typename ...OP>
            EventFilter(OP&& ...op) : m_filterEventIds({ op... })
            {
            }

    protected:
        /**
         * Filter method for event stream
         */
        const MetaBufferPtr Filter(const MetaBufferPtr& a_buffer);

        std::set<EventIdType> m_filterEventIds;
    };
}

#endif /* EVENTS_FILTER_H_ */

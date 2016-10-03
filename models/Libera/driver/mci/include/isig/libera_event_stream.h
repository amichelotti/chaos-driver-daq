/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: libera_event_stream.h 19990 2014-01-07 09:35:55Z damijan.skvarc $
 */

#ifndef ISIG_LIBERA_EVENT_STREAM_H
#define ISIG_LIBERA_EVENT_STREAM_H

#include <string>
#include <memory>

#include "isig/libera_stream.h"
#include "isig/libera_machine_time.h"

namespace isig {

    /**
     * @addtogroup signal Signals and Streams handling Classes and functions
     * @{
     */

    /**
     * Common event id definitions. NOTE: They are defined by HW.
     */
    enum EventIdType_e {
        eEventIdSync = 0, ///< Event timestamp synchronization event.
        eEventIdPostMortem = 1, ///< Post mortem event.
        eEventIdTrigger = 2, ///< Acquisition (or main) trigger event.
        eEventIdInterlock = 3, ///< Interlock event.
        eEventIdCurrentTime = 4 ///< Current time event.
    };

    /**
     * Max event ID, generated from Libera Boards.
     */
    extern const uint64_t c_maxEventId;

    /**
     * Mask to enable all events.
     */
    extern const uint64_t c_allEventMask;

    /**
    * Class to provide access to event data source provided by Libera
    * Linux kernel module.
    *
    * Type of the raw event atom is defined by FPGA and imported via device driver
    * header file.
    *
    * Exposed event atom structure is defined as EVT_ATOM.
    */
    typedef uint64_t    EventIdType;

    /** Type of event atom.  */
    struct EventStruct {
        EventIdType id; ///< Unique event identification.
        uint64_t    count; ///< Count of emitted events.
        uint64_t    timestamp; ///< Time stamp of emitted event.
        uint64_t    data; ///< Contains 4-bit serial code for triggers 0, 1, 2
    };

    typedef SignalTraits<uint64_t, EventStruct>     EventTraits;
    typedef LiberaStream<EventTraits>               BaseEventStream;

    // Using extern to shorten compile time
    extern template class SignalTraits<uint64_t, EventStruct>;
    extern template class LiberaStream<EventTraits>;
    extern template class StreamClient<EventTraits>;
    extern template class StreamQueue<EventTraits>;

    /**
     * Specialised stream class to read events (triggers, ...).
     */
    class LiberaEventStream : public BaseEventStream {
    public:

        explicit LiberaEventStream(const std::string&  a_stmName,
                                   const std::string&  a_devName,
                                   bool                a_write = false,
                                   const std::string&  a_owner = "");

        virtual ~LiberaEventStream();

        /**
         * Generate events identified by bits set in event a_buf.id at time
         * set in a_buf.timestamp.
         */
        virtual void Write(Atom &a_buf);

        /**
         * Reset event count value to 0 for event identified
         * by a_eventId (0..63).
         */
        virtual void ResetCounter(const EventIdType& a_eventId);

        /** Adjust event time stamp by a_timeAdjust value. */
        virtual void AdjustTimestamp(const int64_t& a_timeAdjust);

        /** Enables the specified event by setting proper event mask */
        void EnableEvent(const EventIdType& a_event);

        /** Disables the specified event by setting proper event mask */
        void DisableEvent(const EventIdType& a_event);

        /** Set event capture mask register to a_eventsMask. */
        virtual void SetMask(const uint64_t& a_eventsMask);

        /** Get current event capture mask register. */
        virtual uint64_t GetMask();

        /**
         * Custom stream client for reading event stream, which is aware
         * of additional functionality of LiberaEventStream (writing).
         *
         * Default queue size is 96 elements to cover bursts of events from
         * the Linux kernel module. Up to 48 bits set in 64-bit register from
         * FPGA generate the same amount of events.
         */
        class Client : public BaseEventStream::Client {
        public:

            Client();

            Client(
                LiberaEventStream*  a_stream,
                std::string         a_name,
                unsigned int        a_capacity = (c_maxEventId+1)*2);

            inline LiberaEventStream* operator->()
            {
                return GetEventStream();
            }

            /** Returns current libera machine time */
            LiberaMachineTime GetCurrentMachineTime();

        private:

            LiberaEventStream* GetEventStream() const
            {
                LiberaEventStream* evt = dynamic_cast<LiberaEventStream*>(m_stream);
                if (evt == NULL) {
                    throw std::bad_cast();
                }
                return evt;
            }
        };

    private:
        bool                    m_write;
        std::recursive_mutex    m_fdMutex;
    };

    typedef LiberaEventStream::Client LiberaEventClient;

    /**@}*/
} // namespace

#endif /* ISIG_LIBERA_EVENT_STREAM_H */

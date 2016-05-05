/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: declarations.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */


#ifndef ISIG_DECLARATIONS_H_
#define ISIG_DECLARATIONS_H_

#include <cstddef>
#include <string>
#include <vector>
#include <map>
#include <iostream>

#include "istd/compat.h"

namespace isig {

    const size_t c_defaultTimeoutDOD = 30; //seconds

    /**
     * Level is used to tune the priority in the data on demand request queue.
     */
    enum RequestPriorityLevel_e {
        ePrioHighest,
        ePrioHigh,
        ePrioNormal,
        ePrioLow
    };
    // NOTE: Please make sure that count is updated accordingly to
    // the number of enum values in priority level
    const size_t c_RequestPriorityLevelCount = 4;

    enum AccessType_e {
        eAccessUnknown      = -1,
        eAccessStream       = 1,
        eAccessDataOnDemand = 2
    };

    enum AtomType_e {
        eTypeUnknown    = -1,
        eTypeDouble     = 1,
        eTypeFloat      = 2,
        eTypeUInt64     = 3,
        eTypeInt64      = 4,
        eTypeUInt32     = 5,
        eTypeInt32      = 6,
        eTypeUInt16     = 7,
        eTypeInt16      = 8,
        eTypeUInt8      = 9,
        eTypeInt8       = 10
    };

    /**
     * Mode to access the signal.
     */
    enum AccessMode_e {
        eModeStream         = 0,    ///< Stream has just one access mode
        eModeDodPosition    = 10,   ///< Data on demand: Absolute position in the input buffer in atom units
        eModeDodNow         = 11,   ///< Data on demand: Best approximation of now
        eModeDodLMT         = 12,   ///< Data on demand: Position of the data is defined by the specified LMT
        eModeDodOnEvent     = 13,   ///< Data on demand: Position is defined by a series of events (for example triggers)
        eModeDodSingleEvent = 14    ///< Data on demand: Position is defined by single next event (for example trigger)
    };

    enum SuccessCode_e {
        eSuccess        =  0,   ///< Operation successful
        eInvalidArg     = -1,   ///< Invalid arguments
        eIncomplete     = -2,   ///< Operation partially completed (e.g. read part of requested data)
        eTimeout        = -3,   ///< Operation completed with timeout
        eClosed         = -4,   ///< Attempting to read before opening stream or after closing it
        eTerminated     = -5,   ///< Source signal terminated and can't be accessed
        eIoError        = -6,   ///< Input/output error
        eNoData         = -7,   ///< No data available
        eInternalError  = -99   ///< Internal error
    };

    /**
     * Control the behaviour of the queue.
     * @see StreamQueueBase::SetPolicy
     */
    enum class QueuePolicy {
        /**
         * When the queue is full, keep old elements and don't add new.
         * Default behaviour.
         */
        eOnFullKeepOld,

        /**
         * When the queue is full, drop old elements and add new.
         * Default behaviour.
         */
        eOnFullKeepNew
    };

    typedef uint64_t LMT;

    enum SignalMetaId_e {
        eMetaLMT        = 0,
        eMetaMT         = 1,
        eMetaAbsPos     = 2,
        eMetaCounter    = 3,
        eMetaFlags      = 4
    };

    typedef std::map<SignalMetaId_e, int64_t>   SignalMeta;

    extern const SignalMeta c_noMeta;

    struct SignalStatItem {
        std::string name;
        uint64_t    total;
        uint64_t    rejected;
        size_t      asize;
    };

    typedef std::vector<SignalStatItem> SignalStats;

    std::ostream& operator<<(std::ostream&, const isig::SuccessCode_e&);

// To be removed soon

// #define ENABLE_DEPRECATED_DOD_WARNINGS

#ifdef ENABLE_DEPRECATED_DOD_WARNINGS
    #define DEPRECATED_DOD __attribute__((deprecated))
#else
    #define DEPRECATED_DOD
#endif


} // namespace



#endif

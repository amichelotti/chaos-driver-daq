/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: libera_health_stream.h 10002 2010-12-03 08:21:50Z tomaz.juretic $
 */

#ifndef ISIG_LIBERA_HEALTH_STREAM_H
#define ISIG_LIBERA_HEALTH_STREAM_H

#include <string>

#include "isig/libera_stream.h"

namespace isig {

    /**
     * @addtogroup signal Signals and Streams handling Classes and functions
     * @{
     */

    /**
    * Class to provide access to health data source provided by Libera
    * Linux kernel module.
    * 
    * Exposed event atom structure is defined as HealthAtom.
    */

    /** Health device atom */
    struct HealthStruct {
        uint64_t  count;
        uint64_t  status;
    };

    typedef SignalTraits<uint64_t, HealthStruct>    HealthTraits;
    typedef LiberaStream<HealthTraits>              LiberaHealthStream;

    // Using extern to shorten compile time
    extern template class SignalTraits<uint64_t, HealthStruct>;
    extern template class LiberaStream<HealthTraits>;
    extern template class StreamClient<HealthTraits>;
    extern template class StreamQueue<HealthTraits>;

    /**@}*/
}

#endif /* ISIG_LIBERA_HEALTH_STREAM_H */

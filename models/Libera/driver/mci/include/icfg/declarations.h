/*
 * Copyright (c) 2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: declarations.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef ICFG_DECLARATIONS_H_
#define ICFG_DECLARATIONS_H_

#include <string>

namespace icfg {

    // Namespace abbreviations
    const std::string c_ns = "xs";
    const std::string c_nsItech = "itech";

    // Namespaces
    const std::string c_ns_http = "http://www.w3.org/2001/XMLSchema";
    const std::string c_nsItech_http = "http://www.i-tech.si/types";

    /**
     * All available data types
     */
    enum DataType_e {
        eCtUndefined,
        eCtNone,
        eCtBoolean,
        eCtString,
        eCtLong,
        eCtInt,
        eCtDouble,
        eCtFloat,
        eCtUnsignedLong,
        eCtUnsignedInt,
        eCtHexInt,
        eCtHexLong,
        eCtHexUnsignedInt,
        eCtHexUnsignedLong
    };

}

#endif /* ICFG_DECLARATIONS_H_ */

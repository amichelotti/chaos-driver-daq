/*
 * Copyright (c) 2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: interface_version.h 20696 2014-06-20 09:22:35Z damijan.skvarc $
 */

#ifndef INTERFACE_VERSION_H_
#define INTERFACE_VERSION_H_

namespace inet {

    /* supported interface versions. */
    static const char* c_interfaceVersion3_0 = "3.0";
    static const char* c_interfaceVersion2_6 = "2.6";

    /* Current interface version. */
    static const char* c_interfaceVersion = c_interfaceVersion2_6;

}

#endif /* INTERFACE_VERSION_H_ */

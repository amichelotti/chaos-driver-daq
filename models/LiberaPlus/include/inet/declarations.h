/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: declarations.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */


#ifndef INET_DECLARATIONS_H
#define INET_DECLARATIONS_H

#include <omniORB4/CORBA.h>


namespace inet {

    typedef CORBA::Char      Char;
    typedef CORBA::Octet     Octet;
    typedef CORBA::Short     Short;
    typedef CORBA::UShort    UShort;
    typedef CORBA::Long      Long;
    typedef CORBA::ULong     ULong;
    typedef CORBA::LongLong  LongLong;
    typedef CORBA::ULongLong ULongLong;
    typedef CORBA::Double    Double;
    typedef CORBA::Float     Float;
    typedef CORBA::Boolean   Boolean;


    inet::ULong  GetMaxPacketSize() {
        return omniORB::giopMaxMsgSize();
    }

}

#endif /* INET_DECLARATIONS_H */

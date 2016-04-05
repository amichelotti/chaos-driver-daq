/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: iosystem.h 6569 2010-04-21 14:56:02Z matej.kenda $
 */

#ifndef IOSYSTEM_H
#define IOSYSTEM_H


namespace imath {

    /**
     * Base abstract IO System.
     *
     * Every single-input single-output (SISO) system of type double.
     */
    class IOSystem
    {
    public:
        IOSystem() {};
        virtual ~IOSystem() {};

        virtual double Step(double a_u) = 0;
    };

} // namespace

#endif // IOSYSTEM_H

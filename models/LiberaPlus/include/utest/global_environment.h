/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: global_environment.h 8621 2010-09-14 05:54:19Z matej.kenda $
 */

#ifndef GLOBAL_ENVIRONMENT_H_
#define GLOBAL_ENVIRONMENT_H_

class GlobalEnvironment {
public:
    GlobalEnvironment();
    virtual ~GlobalEnvironment();

    virtual void Setup(int argc, char** argv) = 0;
    virtual void Cleanup() = 0;
};

#endif /* GLOBAL_ENVIRONMENT_H_ */

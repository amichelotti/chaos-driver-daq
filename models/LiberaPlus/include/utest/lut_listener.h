/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: lut_listener.h 8621 2010-09-14 05:54:19Z matej.kenda $
 */

#ifndef LIBERA_UNIT_TEST_LISTENER_H_
#define LIBERA_UNIT_TEST_LISTENER_H_

#include <cstddef>
#include <cppunit/BriefTestProgressListener.h>

class GlobalEnvironment;

class LUTListener : public CPPUNIT_NS::BriefTestProgressListener {
public:
    LUTListener(int argc, char** argv, GlobalEnvironment* a_env = NULL);
    virtual ~LUTListener();

    // Needed to perform setup of global resources needed for all tests
    virtual void startTestRun(
        CPPUNIT_NS::Test* a_test, CPPUNIT_NS::TestResult* a_result);

    // Needed to perform cleanup of global resources needed for all tests
    virtual void endTestRun(
        CPPUNIT_NS::Test* a_test, CPPUNIT_NS::TestResult* a_result);

private:
    int                 m_argc;
    char**              m_argv;
    GlobalEnvironment*  m_env;
};

#endif /* LIBERA_UNIT_TEST_LISTENER_H_ */

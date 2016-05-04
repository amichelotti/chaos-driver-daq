/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: utest.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef UTEST_H
#define UTEST_H

#include <cstddef>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestSuite.h>
#include <cppunit/TestPath.h>
#include "utest/global_environment.h"

int UnitTestMain(int argc, char** argv, GlobalEnvironment *a_env = NULL);

#endif  // UTEST_H

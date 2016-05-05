/*
 * Copyright (c) 2008-2015 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: mci_init.cpp 22034 2015-06-19 08:32:00Z damijan.skvarc $
 */

#include "mci/mci.h"

int main(int a_argc, char* a_argv[])
{
    mci::Init(a_argc, a_argv);
    mci::Shutdown();
}

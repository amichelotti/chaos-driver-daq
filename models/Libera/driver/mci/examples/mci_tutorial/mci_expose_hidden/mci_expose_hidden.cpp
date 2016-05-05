/*
 * Copyright (c) 2008-2015 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: mci_expose_hidden.cpp 22034 2015-06-19 08:32:00Z damijan.skvarc $
 */

#include "mci/mci.h"
#include "mci/local_node_structure.h"

int main(int a_argc, char* a_argv[])
{
    mci::Init(a_argc, a_argv);
    mci::ExposeHiddenRegistry();    /* speed up connection by not using hidden registy interface */

    mci::Shutdown();
}

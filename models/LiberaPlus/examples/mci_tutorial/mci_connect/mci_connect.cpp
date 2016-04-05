/*
 * Copyright (c) 2008-2015 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: mci_connect.cpp 22034 2015-06-19 08:32:00Z damijan.skvarc $
 */

#include "istd/trace.h"
#include "mci/mci.h"
#include "../common.h"


int main(int a_argc, char* a_argv[])
{
    istd::TraceInit("-");
    istd::TraceStart(istd::eTrcLow);

    mci::Init(a_argc, a_argv);

    /* connect to LiberaBase application running on local host */
    mci::Node rootLocalApplication    = mci::Connect();

    /* connect to LiberaPlatform application running on local host */
    mci::Node rootLocalPlatform       = mci::Connect(mci::Root::Platform);

    /* connect to LiberaPlatform application running on remote instrument  */
    mci::Node rootRemoteApplication   = mci::Connect(cHost);

    /* connect to LiberaPlatform application running on remote instrument  */
    mci::Node rootRemotePlatform      = mci::Connect(cHost, mci::Root::Platform);

    /* connect to LiberaBase application running on remote system and listening on non-default port */
    mci::Node rootRemoteAppCustomPort = mci::Connect(cHost, mci::Root::Application, cCustomPort);

    mci::Shutdown();
}

/*
 * Copyright (c) 2008-2015 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: mci_set_agc.cpp 22034 2015-06-19 08:32:00Z damijan.skvarc $
 */

#include "istd/trace.h"
#include "mci/mci.h"
#include "../common.h"

int main(int a_argc, char* a_argv[])
{
    istd::TraceInit("-");
    istd::TraceStart(istd::eTrcOff);

    mci::Init(a_argc, a_argv);

    /* connect to LiberaBase application running on remote instrument  */
    mci::Node root = mci::Connect(cHost);

    /* get reference to appropriate registry node  */
    mci::Node node = root.GetNode(mci::Tokenize(cAGCNode));

    /* ..and modify its value */
    istd::TraceStart(istd::eTrcLow);
    bool ok = node.SetValue(false);
    if (ok) {
        istd_TRC(istd::eTrcLow, cAGCNode << " on remote application was successfully set to False value.");
    }

    mci::Shutdown();
}


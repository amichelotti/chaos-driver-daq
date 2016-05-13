/*
 * Copyright (c) 2008-2015 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: mci_get_scalar.cpp 22034 2015-06-19 08:32:00Z damijan.skvarc $
 */

#include "mci/mci.h"
#include "istd/trace.h"
#include "../common.h"

int main(int a_argc, char* a_argv[])
{
    istd::TraceInit("-");
    istd::TraceStart(istd::eTrcOff);

    mci::Init(a_argc, a_argv);

    /* connect to LiberaBase application running on remote instrument  */
    mci::Node root = mci::Connect(cHost);

    /* get reference to registry node which defines trigger source  */
    mci::Node node = root.GetNode(mci::Tokenize(cTriggerSourceNode));

    /* ..and retrieve its value */
    std::string strValue;
    node.GetValue(strValue);

    istd::TraceStart(istd::eTrcLow);
    istd_TRC(istd::eTrcLow, cTriggerSourceNode << " is set to " << strValue);

    mci::Shutdown();
}
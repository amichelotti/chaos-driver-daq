/*
 * Copyright (c) 2008-2015 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: mci_get_array.cpp 22034 2015-06-19 08:32:00Z damijan.skvarc $
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

    /* get reference to array registry node, containing names of event signal atom */
    mci::Node node = root.GetNode(mci::Tokenize(cStringArrayNode));

    /* get the number of items in the array */
    size_t nSize = node.GetSize();

    /* ...and retrieve all its values */
    std::vector<std::string> names;
    node.Get(names, 0, nSize);

    /* report name to trace output  */
    istd::TraceStart(istd::eTrcLow);
    istd_TRC(istd::eTrcLow, cStringArrayNode << " registry node contains the following values: ");
    for (auto it=names.begin(); it!=names.end(); it++) {
        istd_TRC(istd::eTrcLow, "   " << *it);
    }

    if (nSize > 1) {
        /* retrieve now only the last item */
        std::vector<std::string> names;
        node.Get(names, nSize-1, 1);
        istd_TRC(istd::eTrcLow, "Last item of " << cStringArrayNode << " array registry node: " << names[0]);
    }

    mci::Shutdown();
}

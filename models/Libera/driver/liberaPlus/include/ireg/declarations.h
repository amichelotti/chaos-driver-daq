/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: declarations.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */


#ifndef IREG_DECLARATIONS_H
#define IREG_DECLARATIONS_H

#include "mci/declarations.h"

namespace ireg {

    using mci::Flags;

    using namespace mci::enums;

    using mci::TreeNodeWeakPtr;
    using mci::TreeNodePtr;

    using mci::Path;
    using mci::NodeNames;

    using mci::c_noRelPath;

    using mci::TreeNodeAddr;
    using mci::TreeNodeAddresses;

    using mci::ClientId;
    using mci::TreeNodeId;
}

#endif /* IREG_DECLARATIONS_H */

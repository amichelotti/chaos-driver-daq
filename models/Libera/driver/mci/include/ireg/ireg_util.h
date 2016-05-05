/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: ireg_util.h 14606 2011-12-21 12:43:41Z matej.kenda $
 */


#ifndef IREG_UTIL_H_
#define IREG_UTIL_H_

#include "mci/mci_util.h"
#include "ireg/declarations.h"

namespace i_NET {
    class NodeNames;
}

namespace ireg {

    using mci::c_delimiter;

    using mci::PathToRemoteNodeNames;
    using mci::RemoteNodeNamesToPath;

    using mci::ToString;

} // namespace

#endif /* IREG_UTIL_H_ */

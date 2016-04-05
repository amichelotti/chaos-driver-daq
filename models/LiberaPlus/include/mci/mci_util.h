/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: mci_util.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */


#ifndef MCI_UTIL_H_
#define MCI_UTIL_H_

#include "mci/declarations.h"
#include "mci/node.h"

#include "isig/declarations.h"

namespace i_NET {
    class NodeNames;
}

namespace mci {

    extern const std::string c_accessType;
    extern const std::string c_atomSize;
    extern const std::string c_groupSize;
    extern const std::string c_compNames;
    extern const std::string c_compNum;
    extern const std::string c_compType;

    extern const char*       c_delimiter;

    i_NET::NodeNames* PathToRemoteNodeNames(const Path& a_relPath);
    Path RemoteNodeNamesToPath(const i_NET::NodeNames& a_nn);

    const std::string ToString(const Path &a_path);
    const std::string StringFlags(const mci::Flags &a_flags);

    /**
     * Helper method which connects to remote signal node, retrieves
     * all signal's properties and instantiates the signal.
     */
    isig::SignalSourceSharedPtr CreateRemoteSignal(const Node& a_node);

} // namespace

#endif /* MCI_UTIL_H_ */

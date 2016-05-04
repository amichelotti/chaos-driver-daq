/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: declarations.h 19990 2014-01-07 09:35:55Z damijan.skvarc $
 */


#ifndef MCI_DECLARATIONS_H
#define MCI_DECLARATIONS_H

#include <memory>
#include <string>
#include <vector>

#include "istd/id.h"
#include "istd/exception.h"

namespace mci {

    /**
     * @addtogroup mci
     * @{
     */

    using istd::Exception;
    using istd::Id;

    typedef uint64_t    Flags;

    namespace enums {
        // Node flags
        enum Flags_e {
            eNfReadable     = (1ULL << 1),
            eNfWritable     = (1ULL << 2),
            eNfPersistent   = (1ULL << 3),
            eNfUnlockedRead = (1ULL << 4),
            eNfConstant     = (1ULL << 5),
            eNfHidden       = (1ULL << 6),
            eNfExecutable   = (1ULL << 7),
            eNfAttribute    = (1ULL << 8),
            eNfArray        = (1ULL << 9),
            eNfSignal       = (1ULL << 10),
            eNfDefault      = (Flags_e::eNfReadable | Flags_e::eNfWritable)
        };

        // Node value types
        enum NodeValType_e {
            eNvUndefined,
            eNvBool,
            eNvLong,
            eNvULong,
            eNvLongLong,
            eNvULongLong,
            eNvDouble,
            eNvFloat,
            eNvString,
            eNvEnumeration
        };

        // Notification types
        enum NotificationType_e {
            eNtValueChanged,
            eNtOutOfRange
        };

        enum NodeCommand_e {
            eExecute = 0xEE
        };
    } // namespace enums

    using namespace enums;

    class TreeNode;
    typedef std::weak_ptr<TreeNode>             TreeNodeWeakPtr;
    typedef std::shared_ptr<TreeNode>           TreeNodePtr;

    // TODO: Create a class for Path
    typedef std::vector<std::string> Path;
    typedef std::vector<std::string> NodeNames;

    extern const Path c_noRelPath;

    struct TreeNodeAddr {
        TreeNodePtr node;
        Path        relPath;
    };
    typedef std::vector<TreeNodeAddr>  TreeNodeAddresses;

    class NotificationData;
    typedef std::shared_ptr<NotificationData> NotificationDataPtr;

    struct IdIregClient {};
    struct IdIregCallback {};
    struct IdIregTreeNode {};

    typedef  Id<IdIregClient, uint64_t>   ClientId;
    typedef  Id<IdIregCallback, uint64_t> CallbackId;
    typedef  Id<IdIregTreeNode, uint64_t> TreeNodeId;

    class Node;

    //@}

    std::ostream& operator<< (std::ostream &a_os, const Path &a_path);

}

namespace istd {
    extern template class Id<mci::IdIregClient, uint64_t>;
    extern template class Id<mci::IdIregCallback, uint64_t>;
    extern template class Id<mci::IdIregTreeNode, uint64_t>;
}

// Temporary
namespace isig {
    // Declarations of signal types

    class SignalSource;
    typedef SignalSource*                     SignalSourcePtr;
    typedef std::shared_ptr<SignalSource>     SignalSourceSharedPtr;
    typedef std::weak_ptr<SignalSource>       SignalSourceWeakPtr;
}

#endif /* MCI_DECLARATIONS_H */

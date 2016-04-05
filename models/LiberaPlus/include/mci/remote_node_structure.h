/*
 * Copyright (c) 2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: remote_node_structure.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef MCI_REMOTE_NODE_STRUCTURE_H
#define MCI_REMOTE_NODE_STRUCTURE_H

#pragma GCC diagnostic ignored "-Wold-style-cast"
#include "inet/i_remote_node.h"
#pragma GCC diagnostic warning "-Wold-style-cast"

#include "mci/node_structure.h"

namespace mci {

    class RemoteNodeStructure : public NodeStructure {
    public:

        void SetRemoteNodeInterface(i_NET::RemoteNode_ptr a_remote);

        /** Subnode */
        virtual TreeNodeAddr GetNode(const std::string& a_name, const Path& a_rp);

        /** Subnode by index */
        virtual TreeNodeAddr GetNode(const size_t a_idx, const Path& a_rp);

        /** A node by path */
        virtual TreeNodeAddr GetNode(const Path& a_path, const Path& a_rp);

        /** Get all non-hidden direct descendants */
        virtual TreeNodeAddresses GetNodes(const Path& a_rp);

        /** Checks if node does not have any children */
        virtual bool IsLeaf(const Path& a_rp);

        /**
         *  @return Number of non-hidden children nodes
         */
        virtual size_t GetNodeCount(const Path& a_rp) const;

        virtual ~RemoteNodeStructure();

    protected:

        explicit RemoteNodeStructure(TreeNode* a_node);

        mci_CREATE_NODE_FRIEND;

    private:
        i_NET::RemoteNode_var       m_remote;
    };
}

#endif /* MCI_REMOTE_NODE_STRUCTURE_H */

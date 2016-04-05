/*
 * Copyright (c) 2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: node_structure.h 19990 2014-01-07 09:35:55Z damijan.skvarc $
 */

#ifndef MCI_NODE_STRUCTURE_H
#define MCI_NODE_STRUCTURE_H

#include <string>
#include <memory>

#include "mci/declarations.h"
#include "mci/node_factory_decl.h"

namespace mci {

    typedef std::vector<TreeNodePtr> TreeNodeVec;

    class NodeStructure {
    public:

        explicit NodeStructure(TreeNode* a_node);

        /** Subnode */
        virtual TreeNodeAddr GetNode(const std::string& a_name, const Path& a_rp = c_noRelPath);

        /** Subnode by index */
        virtual TreeNodeAddr GetNode(const size_t a_idx, const Path& a_rp = c_noRelPath);

        /** A node by path */
        virtual TreeNodeAddr GetNode(const Path& a_path, const Path& a_rp = c_noRelPath);

        /** Get all non-hidden direct descendants */
        virtual TreeNodeAddresses GetNodes(const Path& a_rp = c_noRelPath);

        void SetParent(const TreeNodePtr& a_parent);
        TreeNodeAddr GetParent(const Path& a_rp = c_noRelPath) const;

        /** Checks if this node does not have a parent */
        bool IsRoot(const Path& a_rp = c_noRelPath) const;

        /** Checks if node does not have any children */
        virtual bool IsLeaf(const Path& a_rp = c_noRelPath);

        /**
         *  @return Number of non-hidden children nodes
         */
        virtual size_t GetNodeCount(const Path& a_rp = c_noRelPath) const;

        /**
         * @param a_child Child node to be inserted into the tree structure
         */
        virtual void Add(const TreeNodePtr& a_child);

        /**
         * @param a_subtree Child node to be merged into the tree structure
         */
        virtual void Merge(const TreeNodePtr& a_subtree);

        /**
         * @param a_subtree registry node whose children will be adopted
         */
        virtual void Adopt(const TreeNodePtr& a_subtree);

        virtual TreeNodePtr Detach(const std::string& a_child,
                           const Path& a_rp = c_noRelPath);

        virtual TreeNodePtr Detach(const TreeNodePtr& a_child,
                           const Path& a_rp = c_noRelPath);

        /**
         * Query all children (also hidden) for internal purposes
         * TODO: CALLED ONLY FROM RegNode::OnMerge
         */
        virtual TreeNodeVec GetAllNodes() const;

        virtual ~NodeStructure();

    protected:

        mci_CREATE_NODE_FRIEND;

        NodeStructure(const NodeStructure& a_other) = delete;
        NodeStructure& operator = (const TreeNode& a_other) = delete;

        // There is no need to use shared pointer, because
        // the referenced TreeNode owns NodeStructure.
        TreeNode*   m_node;

    private:

        TreeNodeWeakPtr     m_parent;
        mutable std::mutex  m_parent_x;
    };
}

#endif /* MCI_NODE_STRUCTURE_H */

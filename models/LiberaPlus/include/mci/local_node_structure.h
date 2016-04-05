/*
 * Copyright (c) 2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: local_node_structure.h 19990 2014-01-07 09:35:55Z damijan.skvarc $
 */

#ifndef MCI_LOCAL_NODE_DESCENDANTS_H
#define MCI_LOCAL_NODE_DESCENDANTS_H

#include <map>
#include "mci/node_structure.h"

namespace mci {

    typedef std::map<std::string, TreeNodePtr>  TreeNodeMap;

    class LocalNodeStructure : public NodeStructure {
    public:

        /** Subnode */
        virtual TreeNodeAddr GetNode(const std::string& a_name, const Path& a_rp = c_noRelPath);

        /** Subnode by index */
        virtual TreeNodeAddr GetNode(const size_t a_idx, const Path& a_rp = c_noRelPath);

        /** A node by path */
        virtual TreeNodeAddr GetNode(const Path& a_path, const Path& a_rp = c_noRelPath);

        /** Get all non-hidden direct descendants */
        virtual TreeNodeAddresses GetNodes(const Path& a_rp = c_noRelPath);

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
         * @param a_child Child node to be merged into the tree structure
         */
        virtual void Merge(const TreeNodePtr& a_child);

        /**
         * @param a_child registry node whose children node will be adopted
         */
        virtual void Adopt(const TreeNodePtr& a_subtree);

        virtual TreeNodePtr Detach(const std::string& a_child,
                           const Path& a_rp = c_noRelPath);

        virtual TreeNodePtr Detach(const TreeNodePtr& a_child,
                           const Path& a_rp = c_noRelPath);

        /**
         * Query all children (also hidden) for internal purposes
         * TODO: CALLED ONLY FROM Merge
         */
        virtual TreeNodeVec GetAllNodes() const;

        virtual ~LocalNodeStructure();

    protected:

        explicit LocalNodeStructure(TreeNode* a_node);

        mci_CREATE_NODE_FRIEND;

        LocalNodeStructure(const LocalNodeStructure& a_other) = delete;
        LocalNodeStructure& operator = (const TreeNode& a_other) = delete;

    private:
        void AdoptTo(LocalNodeStructure& a_parent);

        /**
         * A map containing all child nodes for access by name
         */
        TreeNodeMap         m_children;

        /**
         * A map containing only non-hidden children
         */
        TreeNodeVec                     m_childrenVec;
        mutable std::recursive_mutex    m_children_x;

    };

    void ExposeHiddenRegistry();
}

#endif /* MCI_LOCAL_NODE_DESCENDANTS_H */

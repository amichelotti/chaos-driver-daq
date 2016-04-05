/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: tree_node.h 19990 2014-01-07 09:35:55Z damijan.skvarc $
 */

#ifndef MCI_TREE_NODE_H
#define MCI_TREE_NODE_H

#include <string>
#include <mutex>
#include <memory>

#include "istd/any.h"

#include "mci/declarations.h"
#include "mci/node_factory_decl.h"
#include "mci/node_structure.h"
#include "mci/local_node_structure.h"
#include "mci/remote_node_structure.h"
#include "mci/notification_data.h"

namespace mci {

	/**
	 * @addtogroup register
	 * @{
	 */

    /**
     * Abstract interface for implementation of nodes in the tree.
     * The tree is intended to be used as a registry to store
     * different types of information. Each node can carry different
     * type of payload, defined by the implementation of the derived
     * node class.
     *
     * Implementations of nodes can refer to local or remote resources,
     * provide read-only runtime information, persistent
     *
     * Derived classes implement specific nodes in the tree,
     * for example RegNode, RegValueNode, RemoteNode...
     *
     * Nodes can operate in two different modes:
     * -# one TreeNode per node
     * -# TreeNode with relative path
     *
     * The former has one TreeNode per each node, the latter has one proxy
     * TreeNode that is able to serve a subtree using relative paths.
     * This mode is convenient for implementation of a TreeNode that
     * accesses a remote tree.
     */
    class TreeNode : public std::enable_shared_from_this<TreeNode> {
    public:

        virtual bool operator==(const TreeNode& a_other) const;
        bool operator!=(const TreeNode& a_other) const
        {
            return !(*this == a_other);
        }

        /**
         * @name Registry node properties
         * @{
         */

        /**
         * Change the name of the node.
         * @note Only nodes, which are not yet added into the tree can be renamed.
         */
        void SetName(const std::string& a_name) throw (istd::Exception);

        const std::string& GetName() const;

        NodeValType_e GetValueType(const Path& a_rp = c_noRelPath);

        Flags GetFlags(const Path& a_rp = c_noRelPath) const;
        Flags SetFlags(const Flags a_flags, const Path& a_rp = c_noRelPath);

        /** This node uses persistent storage */
        bool IsPersistent(const Path& a_rp = c_noRelPath) const;

        /** This node is writeable */
        bool IsWritable(const Path& a_rp = c_noRelPath) const;

        /** This node is readable */
        bool IsReadable(const Path& a_rp = c_noRelPath) const;

        /** This node contains constant value */
        bool IsConstant(const Path& a_rp = c_noRelPath) const;

        /** This node is executable */
        bool IsExecutable(const Path& a_rp = c_noRelPath) const;

        /** This node is array */
        bool IsArray(const Path& a_rp = c_noRelPath) const;

        /** This node holds a signal */
        bool IsSignal(const Path& a_rp = c_noRelPath) const;

        /** This node is hidden node */
        bool IsHidden(const Path& a_rp = c_noRelPath) const;

        istd::StrTokens GetDomainValues(const Path& a_rp = c_noRelPath) const;

        /// @}

        /**
         * @name Registry tree structure.
         * @{
         */

        void SetParent(const TreeNodePtr& a_parent);
        TreeNodeAddr GetParent(const Path& a_rp = c_noRelPath) const;

        /** Subnode */
        TreeNodeAddr GetNode(const std::string& a_name,
                             const Path& a_rp = c_noRelPath);

        /** Subnode by index */
        TreeNodeAddr GetNode(const size_t a_idx,
                             const Path& a_rp = c_noRelPath);

        /** A node by path */
        TreeNodeAddr GetNode(const Path& a_path,
                             const Path& a_rp = c_noRelPath);

        /** Get all non-hidden direct descendants */
        TreeNodeAddresses GetNodes(const Path& a_rp = c_noRelPath);

        /** Checks if this node does not have a parent */
        bool IsRoot() const;

        /** Checks if node does not have any children */
        bool IsLeaf(const Path& a_rp = c_noRelPath);

        /**
         *  @return Number of non-hidden children nodes
         */
        size_t GetNodeCount(const Path& a_rp = c_noRelPath) const;

        /**
         *
         * @param a_child Child node to be inserted into the tree structure
         * @return Shared pointer to the inserted child node
         */
        TreeNodePtr Add(const TreeNodePtr& a_child);

        /**
         *
         * @param a_child Child node to be inserted into the tree structure
         * @return Shared pointer to self
         */
        TreeNodePtr Attach(const TreeNodePtr& a_child);

        /**
         *
         * @param a_child Child node to be merged into the tree structure
         * @return Shared pointer to the inserted child node
         */
        TreeNodePtr Merge(const TreeNodePtr& a_child);


        /**
         * @param a_parent registry node whose children will be adopted
         */
        void Adopt(const TreeNodePtr& a_parent);


        TreeNodePtr Detach(const std::string& a_child,
                           const Path& a_rp = c_noRelPath);

        TreeNodePtr Detach(const TreeNodePtr& a_child,
                           const Path& a_rp = c_noRelPath);

        void Remove(const std::string& a_child,
                    const Path& a_rp = c_noRelPath);

        void Remove(const TreeNodePtr& a_child,
                    const Path& a_rp = c_noRelPath);

        /// @}

        /**
         * @name Node value accessors.
         * @{
         */

        // --- Getters

        virtual void Get(std::string &v, size_t a_pos = 0, const Path& a_rp = c_noRelPath);
        virtual void Get(int64_t &v, size_t a_pos = 0, const Path& a_rp = c_noRelPath);
        virtual void Get(uint64_t &v, size_t a_pos = 0, const Path& a_rp = c_noRelPath);
        virtual void Get(int32_t &v, size_t a_pos = 0, const Path& a_rp = c_noRelPath);
        virtual void Get(uint32_t &v, size_t a_pos = 0, const Path& a_rp = c_noRelPath);
        virtual void Get(double &v, size_t a_pos = 0, const Path& a_rp = c_noRelPath);
        virtual void Get(float &v, size_t a_pos = 0, const Path& a_rp = c_noRelPath);
        virtual void Get(bool &v, size_t a_pos = 0, const Path& a_rp = c_noRelPath);

        virtual void Get(std::vector<std::string> &v,
                size_t a_pos = 0, size_t a_size = 0, const Path& a_rp = c_noRelPath);

        virtual void Get(std::vector<int64_t> &v,
            size_t a_pos = 0, size_t a_size = 0, const Path& a_rp = c_noRelPath);

        virtual void Get(std::vector<uint64_t> &v,
            size_t a_pos = 0, size_t a_size = 0, const Path& a_rp = c_noRelPath);

        virtual void Get(std::vector<int32_t> &v,
            size_t a_pos = 0, size_t a_size = 0, const Path& a_rp = c_noRelPath);

        virtual void Get(std::vector<uint32_t> &v,
            size_t a_pos = 0, size_t a_size = 0, const Path& a_rp = c_noRelPath);

        virtual void Get(std::vector<double> &v,
            size_t a_pos = 0, size_t a_size = 0, const Path& a_rp = c_noRelPath);

        virtual void Get(std::vector<float> &v,
            size_t a_pos = 0, size_t a_size = 0, const Path& a_rp = c_noRelPath);

        virtual void Get(std::vector<bool> &v,
            size_t a_pos = 0, size_t a_size = 0, const Path& a_rp = c_noRelPath);

        // --- Setters

        virtual void Set(const std::string &v, size_t a_pos = 0, const Path& a_rp = c_noRelPath);
        virtual void Set(const int64_t &v, size_t a_pos = 0, const Path& a_rp = c_noRelPath);
        virtual void Set(const uint64_t &v, size_t a_pos = 0, const Path& a_rp = c_noRelPath);
        virtual void Set(const int32_t &v, size_t a_pos = 0, const Path& a_rp = c_noRelPath);
        virtual void Set(const uint32_t &v, size_t a_pos = 0, const Path& a_rp = c_noRelPath);
        virtual void Set(const double &v, size_t a_pos = 0, const Path& a_rp = c_noRelPath);
        virtual void Set(const float &v, size_t a_pos = 0, const Path& a_rp = c_noRelPath);
        virtual void Set(const bool &v, size_t a_pos = 0, const Path& a_rp = c_noRelPath);

        virtual void Set(const std::vector<std::string> &v,
            size_t a_pos = 0, const Path& a_rp = c_noRelPath);

        virtual void Set(const std::vector<int64_t> &v,
            size_t a_pos = 0, const Path& a_rp = c_noRelPath);

        virtual void Set(const std::vector<uint64_t> &v,
            size_t a_pos = 0, const Path& a_rp = c_noRelPath);

        virtual void Set(const std::vector<int32_t> &v,
            size_t a_pos = 0, const Path& a_rp = c_noRelPath);

        virtual void Set(const std::vector<uint32_t> &v,
            size_t a_pos = 0, const Path& a_rp = c_noRelPath);

        virtual void Set(const std::vector<double> &v,
            size_t a_pos = 0, const Path& a_rp = c_noRelPath);

        virtual void Set(const std::vector<float> &v,
            size_t a_pos = 0, const Path& a_rp = c_noRelPath);

        virtual void Set(const std::vector<bool> &v,
            size_t a_pos = 0, const Path& a_rp = c_noRelPath);

        bool Resize(size_t a_size, const Path& a_rp = c_noRelPath);
        size_t GetSize(const Path& a_rp = c_noRelPath) const;

        bool Execute(const Path& a_rp = c_noRelPath);

        /// @}

        virtual bool Emit();

        bool GetValidatorExpression(std::string &a_value, const Path& a_rp = c_noRelPath) const;

        bool Cleanup(const Path& a_rp);

        inline TreeNodePtr SharedPtr()
        {
            return this->shared_from_this();
        }

// TODO: Implement different solution, because RegNode is not in libmci!
//        RegNode* operator->() throw(std::bad_cast);

        virtual ~TreeNode();

    protected:
        mci_CREATE_NODE_FRIEND;

        typedef std::unique_lock<std::recursive_mutex>  UniqueLock;

        explicit TreeNode(const std::string& a_name, const Flags& a_flags = 0);
        TreeNode(const Flags& a_flags = 0);

        /**
         * Function to initialise the node after it is constructed.
         *
         * Any initialisation using virtual functions (directly or indirectly)
         * should be implemented in Init, not in ctor.
         *
         * @see mci::Create.
         */
        virtual bool Init();

        /**
         * Attach a helper class, which defines how the tree structure
         * is defined and accessed.
         * @param a_ns Pointer to a heap allocated NodeStructure
         * @note The node takes ownership of the object.
         */
        void SetNodeStructure(NodeStructure* a_ns);

        std::unique_ptr<NodeStructure>  m_structure;

        TreeNode(const TreeNode& a_other) = delete;
        TreeNode& operator = (const TreeNode& a_other) = delete;

        inline const TreeNodeId& GetId() const {
            return m_id;
        };

        /**
         * Query all children (also hidden) for internal purposes
         */
        TreeNodeVec GetAllNodes() const;

        /**
         * Locks internal mutex depending on flags for reading purposes.
         * @return handle to lock
         */
        UniqueLock LockRead() const __attribute__ ((warn_unused_result));
        UniqueLock Lock() const __attribute__ ((warn_unused_result));

        ///// Notifications

        /**
         * Emit notification from this node
         * @param a_data  notification data, which is sent to clients
         * @return true if notification is sent
         */
        bool Emit(const NotificationData &a_data);

        bool Subscribe(const ClientId &a_id,
                       const Path& a_rp = c_noRelPath);

        bool Unsubscribe(const ClientId &a_id,
                         const Path& a_rp = c_noRelPath,
                         bool a_updateDispatcher = true);

        // virtual methods

        virtual bool OnCleanup(const Path& a_rp);


        virtual bool OnEmit(const NotificationData &a_data);
        virtual bool OnEmitValue(const istd::Any &v, const size_t &a_pos);

        virtual bool OnSubscribe(const ClientId &a_id,
                                 const Path& a_rp = c_noRelPath);

        virtual bool OnUnsubscribe(const ClientId &a_id,
                                   const Path& a_rp = c_noRelPath,
                                   bool a_updateDispatcher = true);

        inline virtual Flags OnGetFlags(const Path& a_rp = c_noRelPath) const
        {
            return m_flags;
        }

        inline virtual Flags OnSetFlags(const Flags a_flags, const Path& a_rp = c_noRelPath)
        {
            m_flags |= a_flags;
            return m_flags;
        }

        virtual bool OnResize(size_t a_size, const Path& a_rp = c_noRelPath);

        virtual size_t OnGetSize(const Path& a_rp = c_noRelPath) const;

        virtual NodeValType_e OnGetValueType(const Path& a_rp = c_noRelPath) const;

        virtual bool OnGetDomainValues(istd::StrTokens &vs, const Path& a_rp = c_noRelPath) const;

        virtual bool OnGetValidatorExpression(std::string &a_value, const Path& a_relPath) const;

    private:

        bool HasFlagSet(Flags a_flag, const Path& a_rp) const;

        std::string     m_name;
        TreeNodeId      m_id;   // local node id (unique only in the local tree)

        Flags           m_flags;

        mutable std::recursive_mutex m_treeNode_x;

        friend class Node;
        friend class LocalNodeStructure;
        friend class DispatcherThread;
    };

    /**@}*/

    namespace factory {
        // Definition of templates

        template<class NODE_S, class NODE_TYPE, typename ...ARGS>
            TreeNodePtr Create(ARGS&& ...args)
            {
                static_assert(
                    std::is_base_of<NodeStructure, NODE_S>::value,
                    "Invalid type of NODE_S");

                static_assert(
                    std::is_base_of<TreeNode, NODE_TYPE>::value,
                    "Invalid type of NODE_TYPE");

                TreeNodePtr node(new NODE_TYPE(std::forward<ARGS>(args)...));
                std::unique_ptr<NODE_S> ns(new NODE_S(node.get()));
                node->SetNodeStructure(ns.get());
                ns.release();
                if (!node->Init()) {
                    // Init function failed --> node is not valid.
                    node.reset();
                }
                return node;
            }

        template<class NT>
            struct Factory {
                template<typename ...ARGS>
                    static inline TreeNodePtr New(ARGS&& ...args)
                    {
                        return Create<LocalNodeStructure, NT>(std::forward<ARGS>(args)...);
                    }
            };

        template<typename NODE_TYPE, typename ...ARGS>
            TreeNodePtr Create(ARGS&& ...args)
            {
                return Factory<NODE_TYPE>::New(std::forward<ARGS>(args)...);
            }

        /**
         * Factory function to create any type of node when the first
         * parameter is a C-string literal.
         */
        template<typename NODE_TYPE, typename ...ARGS>
            TreeNodePtr Create(const char* a_name, ARGS&& ...args)
            {
                return Factory<NODE_TYPE>::New(
                    std::string(a_name),
                    std::forward<ARGS>(args)...);
            }

    } // namespace

//    std::ostream& operator<< (std::ostream &a_os, const TreeNode &a_node);

} // namespace

#endif /* MCI_TREE_NODE_H */

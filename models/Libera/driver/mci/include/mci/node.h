/*
 * Copyright (c) 2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: node.h 19990 2014-01-07 09:35:55Z damijan.skvarc $
 */

#ifndef MCI_NODE_H
#define MCI_NODE_H
 
#include <string>

#include "istd/string.h"
#include "istd/any.h"

#include "mci/declarations.h"

namespace mci {

    class NotificationData;

    /**
     * @addtogroup register
     * @{
     */

    /**
     * Node is a client proxy class which provides access to the
     * parameter tree structure.
     * Main purpose of this class is to hide differences
     * implementation of TreeNode instances from the user that
     * accesses the tree.
     *
     * @see TreeNode
     */
    class Node {
    public:

        static const size_t c_defMaxValueLen; // unlimited
        enum DumpFlags {
            eFullPath   = (1 << 0),
            eShowFlags  = (1 << 1),
            eCompact    = (1 << 2)
        };

        typedef std::vector<Node>   Nodes;

        Node();

        /**
         * @param a_relPath relative path from the parent of a_tnode
         * @param a_tnode pointer to the node in the parameter registry tree
         */
        Node(const TreeNodePtr& a_tnode, const Path& a_relPath = c_noRelPath);
        Node(const TreeNodeAddr& a_addr);
        ~Node();

        Node(const Node& a_other);
        Node& operator=(const Node& a_other);

        Node  operator[](const char* a_name) const;
        Node  operator[](const std::string& a_name) const;

        bool operator==(const Node &rhs ) const;
        bool operator<(const Node &rhs ) const;

        /** The name of this node */
        std::string GetName() const;

        /** Full path from the root */
        Path GetFullPath(bool a_skip_root = false) const;

        /** Optional relative path of this node to the wrapped TreeNode */
        Path GetRelPath() const;

        /**
         * @name Navigation in the tree.
         * @{
         */
        Node GetParent() const;
        Node GetRoot() const;
        size_t GetNodeCount() const;
        Node GetNode(const std::string& a_name) const;
        Node GetNode(size_t a_idx) const;
        Node GetNode(const Path& a_path) const;
        void DeleteNode(const std::string& a_path, const std::string& a_name);
        Nodes GetNodes() const;
        bool IsValid() const;
        bool IsRoot() const;
        bool IsLeaf() const;
        /// @}

        /** Value manipulation routines */
        NodeValType_e GetValueType() const;

        operator std::string() const;
        operator int64_t() const;
        operator uint64_t() const;
        operator int32_t() const;
        operator uint32_t() const;
        operator double() const;
        operator float() const;
        operator bool() const;

        operator std::vector<std::string>() const;
        operator std::vector<int64_t>() const;
        operator std::vector<uint64_t>() const;
        operator std::vector<int32_t>() const;
        operator std::vector<uint32_t>() const;
        operator std::vector<double>() const;
        operator std::vector<float>() const;
        operator std::vector<bool>() const;

        /**
         * Get returns one value from node
         * @param a_value  Value returned from node
         * @param a_pos    Position of element in node's array (default = first)
         * @retval Exception is thrown if value cannot be returned
         */
        template <class T>
            void Get(T &a_value, const size_t a_pos = 0) const;

        /**
         * Get returns vector of values from node
         * @param a_value  Values returned from node
         * @param a_pos    Starting position of element in node's array
         * @param a_size   Number of elements to return (all by default)
         * @retval Exception is thrown if values cannot be returned
         */
        template <class T>
        void Get(std::vector<T> &a_value,
                 const size_t a_pos = 0,
                 const size_t a_size = 0) const;

        /**
         * GetValue returns one value from node
         * @param a_value  Value returned from node
         * @param a_pos    Position of element in node's array (default = first)
         * @retval true if valid value is returned
         */
        template <class T>
            bool GetValue(T &a_value, const size_t a_pos = 0) const throw()
            __attribute__((warn_unused_result));

        /**
         * GetValue returns vector of values from node
         * @param a_value  Vector of values returned from node
         * @param a_pos    Starting position of element in node's array
         * @param a_size   Number of elements to return (all by default)
         * @retval true if at least one valid value is returned
         */
        template <class T>
            bool GetValue(std::vector<T> &a_value,
                          const size_t a_pos = 0,
                          const size_t a_size = 0) const throw()
            __attribute__((warn_unused_result));

        /**
         * Set sets one value or vector of values into the node
         * @param a_value New value/values for the node
         * @param a_pos   Position of element in node's array, where new value
         *                will be set (default = first)
         * @return Exception is thrown if value cannot be set
         */
        template <class T>
            typename std::enable_if<!std::is_enum<T>::value, void>::type
            Set(const T &a_value, const size_t a_pos = 0) const;

        void Set(const char* a_value, const size_t a_pos = 0) const;

        /**
         *  Partial specialization for enums (enum is converted to int64_t)
         */
        template<class T>
            typename std::enable_if<std::is_enum<T>::value, void>::type
            Set(const T &a_value, const size_t a_pos = 0) const;

        /**
         * SetValue sets one value or vector of values into the node
         * @param a_value New value/values for the node
         * @param a_pos   Position of element in node's array, where new value
         *                will be set (default = first)
         * @return true if value was set
         */
        template <class T>
            typename std::enable_if<!std::is_enum<T>::value, bool>::type
            SetValue(const T &a_value, const size_t a_pos = 0) const throw()
            __attribute__((warn_unused_result));

        bool SetValue(const char* a_value, const size_t a_pos = 0) const throw()
        __attribute__((warn_unused_result));

        /**
         *  Partial specialization for enums (enum is converted to int64_t)
         */
        template<class T>
            typename std::enable_if<std::is_enum<T>::value, bool>::type
            SetValue(const T &a_value, const size_t a_pos = 0) const throw()
            __attribute__((warn_unused_result));

        // Explicit specialisation for vector<bool>::reference below

        bool Execute() const;

        /** Notifications */
        bool Emit(NotificationType_e a_type, istd::Any a_data);

        size_t GetSize() const;

        Flags GetFlags() const;

        /** This parameter uses persistent storage */
        bool IsPersistent() const;
        bool IsWritable() const;
        bool IsReadable() const;
        bool IsExecutable() const;
        bool IsArray() const;
        bool IsConstant() const;
        bool IsSignal() const;

        bool GetDomainValues(istd::StrTokens &a_values) const;
        bool GetValidatorExpression(std::string &a_value) const;
        void SetUnmanagedData(void* a_unmanaged);
        void* GetUnmanagedData() const;

        /* Helper function that copies the values from source pointer to node. */
        template <typename TYPE>
        void SetArray(void *a_src);

        /* Helper function calls SetArray template for node type. */
        void SetArrayByType(void *a_src);

        /* Helper function that converts strings to array values and copies to node. */
        template <typename TYPE>
        void SetStringArray(void *a_src, uint32_t a_segment);

        /* Helper function calls SetStringArray template for node type. */
        void SetStringArrayByType(void *a_src, uint32_t a_segment);

        /* Helper function that copies the array node values to destination pointer. */
        template <typename TYPE>
        void GetArray(void *a_dst, uint32_t a_size) const;

        /* Helper function calls GetArray template for node type. */
        void GetArrayByType(void *a_dst, uint32_t a_size) const;

        /* Helper function that converts the array node values to strings. */
        template <typename TYPE>
        void GetStringArray(void *a_dst, uint32_t a_size,
                  uint32_t a_segment) const;

        /* Helper function calls GetStringArray template for node type. */
        void GetStringArrayByType(void *a_dst, uint32_t a_size,
                  uint32_t a_segment) const;

        /**
         * Remove this node from the tree and destroys internal allocated structures.
         * Root node cannot be destroyed by this method.
         */
        bool Destroy();

        /**
         * Print out the tree from *this node for a_level depth
         * @param a_level is tree level: -1 prints all tree,
         *                0 to tree size to print till the required level
         * @param a_flags see DumpFlags
         * @return the tree in string format
         */
        std::string DumpSubTree(
            int a_level = -1,
            unsigned long a_flags = 0,
            const size_t a_maxValueLen = c_defMaxValueLen) const;

        std::string ToString(const size_t a_maxValueLen = c_defMaxValueLen) const;

        TreeNodePtr GetTreeNode() const;

        // Notifications
        bool Subscribe(const ClientId &a_id) const;
        bool Unsubscribe(const ClientId &a_id);

    private:

        Node(const TreeNodeId &a_tnodeId, const Path &a_relPath);

        template <class T>
            std::string PrintValue() const;

        TreeNodeId      m_tnodeId;
        TreeNodeWeakPtr m_tnode;
        Path            m_relPath;
        void*           m_unmanaged;

        friend class DispatcherThread;
    };

    typedef Node::Nodes   Nodes;

    /**
     * Get a list of all sub-nodes of a specified node (direct and indirect
     * descendants down to the leaves).
     * @param a_node The node to be used as a root for extracting sub-nodes
     */
    Nodes SubTree(const Node& a_node);
    Nodes SubTree(const Node& a_node, unsigned int a_depth);

    /**@}*/

    // Clean-up and destroy TreeNodePtr
    void DestroyNode(TreeNodePtr& a_node);

    // Specialisations for vector<bool>
    template <>
        void Node::Set(const std::vector<bool>::reference &a_value, const size_t a_pos) const;

    template <>
        bool Node::SetValue(const std::vector<bool>::reference &a_value, const size_t a_pos) const throw();

    // Specialisations for enums
    template<class T>
        typename std::enable_if<std::is_enum<T>::value, void>::type
        Node::Set(const T &a_value, const size_t a_pos) const
        {
            return Set(static_cast<int64_t>(a_value), a_pos);
        }

    template<class T>
        typename std::enable_if<std::is_enum<T>::value, bool>::type
        Node::SetValue(const T &a_value, const size_t a_pos) const throw()
        {
            return SetValue(static_cast<int64_t>(a_value), a_pos);
        }

    // Declaration of externals for supported types
    extern template void Node::Get(std::string &a_value, const size_t) const;
    extern template void Node::Get(int64_t &a_value, const size_t) const;
    extern template void Node::Get(uint64_t &a_value, const size_t) const;
    extern template void Node::Get(int32_t &a_value, const size_t) const;
    extern template void Node::Get(uint32_t &a_value, const size_t) const;
    extern template void Node::Get(double &a_value, const size_t) const;
    extern template void Node::Get(float &a_value, const size_t) const;
    extern template void Node::Get(bool &a_value, const size_t) const;

    extern template void Node::Get(std::vector<std::string>&, const size_t, const size_t) const;
    extern template void Node::Get(std::vector<int64_t>&, const size_t, const size_t) const;
    extern template void Node::Get(std::vector<uint64_t>&, const size_t, const size_t) const;
    extern template void Node::Get(std::vector<int32_t>&, const size_t, const size_t) const;
    extern template void Node::Get(std::vector<uint32_t>&, const size_t, const size_t) const;
    extern template void Node::Get(std::vector<double>&, const size_t, const size_t) const;
    extern template void Node::Get(std::vector<float>&, const size_t, const size_t) const;
    extern template void Node::Get(std::vector<bool>&, const size_t, const size_t) const;

    extern template bool Node::GetValue(std::string&, const size_t) const throw();
    extern template bool Node::GetValue(int64_t &, const size_t) const throw();
    extern template bool Node::GetValue(uint64_t &, const size_t) const throw();
    extern template bool Node::GetValue(int32_t &, const size_t) const throw();
    extern template bool Node::GetValue(uint32_t &, const size_t) const throw();
    extern template bool Node::GetValue(double &, const size_t) const throw();
    extern template bool Node::GetValue(float &, const size_t) const throw();
    extern template bool Node::GetValue(bool &, const size_t) const throw();

    extern template bool Node::GetValue(std::vector<std::string> &, const size_t, const size_t) const throw();
    extern template bool Node::GetValue(std::vector<int64_t>&, const size_t, const size_t) const throw();
    extern template bool Node::GetValue(std::vector<uint64_t>&, const size_t, const size_t) const throw();
    extern template bool Node::GetValue(std::vector<int32_t>&, const size_t, const size_t) const throw();
    extern template bool Node::GetValue(std::vector<uint32_t>&, const size_t, const size_t) const throw();
    extern template bool Node::GetValue(std::vector<double>&, const size_t, const size_t) const throw();
    extern template bool Node::GetValue(std::vector<float>&, const size_t, const size_t) const throw();
    extern template bool Node::GetValue(std::vector<bool>&, const size_t, const size_t) const throw();

    extern template void Node::Set(const std::string&, const size_t) const;
    extern template void Node::Set(const int64_t&, const size_t) const;
    extern template void Node::Set(const uint64_t&, const size_t) const;
    extern template void Node::Set(const int32_t&, const size_t) const;
    extern template void Node::Set(const uint32_t&, const size_t) const;
    extern template void Node::Set(const double&, const size_t) const;
    extern template void Node::Set(const float&, const size_t) const;
    extern template void Node::Set(const bool&, const size_t) const;
    extern template void Node::Set(const std::vector<bool>::reference&, const size_t) const;

    extern template void Node::Set(const std::vector<std::string>&, const size_t) const;
    extern template void Node::Set(const std::vector<int64_t>&, const size_t) const;
    extern template void Node::Set(const std::vector<uint64_t>&, const size_t) const;
    extern template void Node::Set(const std::vector<int32_t>&, const size_t) const;
    extern template void Node::Set(const std::vector<uint32_t>&, const size_t) const;
    extern template void Node::Set(const std::vector<double>&, const size_t) const;
    extern template void Node::Set(const std::vector<float>&, const size_t) const;
    extern template void Node::Set(const std::vector<bool>&, const size_t) const;

    extern template bool Node::SetValue(const std::string&, const size_t) const throw();
    extern template bool Node::SetValue(const int64_t&, const size_t) const throw();
    extern template bool Node::SetValue(const uint64_t&, const size_t) const throw();
    extern template bool Node::SetValue(const int32_t&, const size_t) const throw();
    extern template bool Node::SetValue(const uint32_t&, const size_t) const throw();
    extern template bool Node::SetValue(const double&, const size_t) const throw();
    extern template bool Node::SetValue(const float&, const size_t) const throw();
    extern template bool Node::SetValue(const bool&, const size_t) const throw();
    extern template bool Node::SetValue(const std::vector<bool>::reference&, const size_t) const throw();

    extern template bool Node::SetValue(const std::vector<std::string>&, const size_t) const throw();
    extern template bool Node::SetValue(const std::vector<int64_t>&, const size_t) const throw();
    extern template bool Node::SetValue(const std::vector<uint32_t>&, const size_t) const throw();
    extern template bool Node::SetValue(const std::vector<int32_t>&, const size_t) const throw();
    extern template bool Node::SetValue(const std::vector<uint32_t>&, const size_t) const throw();
    extern template bool Node::SetValue(const std::vector<double>&, const size_t) const throw();
    extern template bool Node::SetValue(const std::vector<float>&, const size_t) const throw();
    extern template bool Node::SetValue(const std::vector<bool>&, const size_t) const throw();

    extern template void Node::SetArray<int32_t>(void*);
    extern template void Node::SetArray<uint32_t>(void*);
    extern template void Node::SetArray<float>(void*);
    extern template void Node::SetArray<double>(void*);

    extern template void Node::SetStringArray<int64_t>(void*, unsigned int);
    extern template void Node::SetStringArray<uint64_t>(void*, unsigned int);

    extern template void Node::GetArray<int32_t>(void*, unsigned int) const;
    extern template void Node::GetArray<uint32_t>(void*, unsigned int) const;
    extern template void Node::GetArray<float>(void*, unsigned int) const;
    extern template void Node::GetArray<double>(void*, unsigned int) const;

    extern template void Node::GetStringArray<int64_t>(void*, unsigned int, unsigned int) const;
    extern template void Node::GetStringArray<uint64_t>(void*, unsigned int, unsigned int) const;

    std::ostream& operator<< (std::ostream &a_os, const Node &a_node);

}

#endif /* MCI_NODE_H */

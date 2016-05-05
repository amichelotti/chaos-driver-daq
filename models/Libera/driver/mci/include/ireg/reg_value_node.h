/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: reg_value_node.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef REG_VALUE_NODE_H_
#define REG_VALUE_NODE_H_

#include "reg_type_node.h"

namespace ireg {

    /**
     * @addtogroup register
     * @{
     */
    /**
     * @brief Template implementation of a RegNode which holds a value.
     *
     * When writing and reading values from this type of nodes, internal value
     * is accessed. Value type can be one of pre-defined Node value types.
     *
     *  @param T  Node value type
     *
     * Example:
     * @dontinclude ut_node.cpp
     *  @skip ValueNodeExample()
     *  @until }
     */

    template <class T>
        class RegValueNode: public RegTypeNode<T> {
        public:

            virtual ~RegValueNode()
            {

            }

        protected:

            /**
             * @param a_name  Node name
             * @param a_val   value of template type RT
             * @param a_flags Various flags can be set to the node
             */
            RegValueNode(
                const std::string& a_name,
                const T& a_val,
                Flags a_flags = ireg::eNfDefault)
            : RegTypeNode<T>(a_name, a_flags)
            {
                m_values.resize(1, a_val);
            }

            /**
             * @param a_name  Node name
             * @param a_val   array value of template type T
             * @param a_flags Various flags can be set to the node
             */
            RegValueNode(
                const std::string& a_name,
                const std::vector<T>& a_val,
                Flags a_flags)
            : RegTypeNode<T>(a_name, a_flags | eNfArray ), m_values(a_val)
            {
            }


            /**
             * @param a_name  Node name
             * @param a_val   value of template type RT
             * @param a_size  size of array
             * @param a_flags Various flags can be set to the node
             */
            RegValueNode(
                const std::string& a_name,
                size_t a_size,
                const T& a_val,
                Flags a_flags)
            : RegTypeNode<T>(a_name, a_flags | eNfArray )
            {
                m_values.resize(a_size, a_val);
            }

            mci_CREATE_NODE_FRIEND;

            virtual size_t OnGet(T* a_out, size_t a_pos, size_t a_size) const
            {
                std::copy (m_values.begin() + a_pos,
                           m_values.begin() + a_pos + a_size,
                           a_out);

                return a_size;
            }

            virtual void OnSet(const T* a_in, size_t a_pos, size_t a_size)
            {
                std::copy (a_in, a_in + a_size, m_values.begin() + a_pos);
            }

            virtual bool OnResize(size_t a_size, const Path& a_relPath = c_noRelPath)
            {
                istd_FTRC();
                m_values.resize(a_size);
                return true;
            }

            virtual size_t OnGetSize(const Path& a_relPath) const
            {
                return m_values.size();
            }

        private:
            std::vector<T> m_values;
        };

    typedef RegValueNode<double>         RegValueDoubleNode;
    typedef RegValueNode<float>          RegValueFloatNode;
    typedef RegValueNode<std::string>    RegValueStringNode;
    typedef RegValueNode<bool>           RegValueBoolNode;
    typedef RegValueNode<int32_t>        RegValueInt32Node;
    typedef RegValueNode<int64_t>        RegValueInt64Node;
    typedef RegValueNode<uint32_t>       RegValueUInt32Node;
    typedef RegValueNode<uint64_t>       RegValueUInt64Node;
}

#endif /* REG_VALUE_NODE_H_ */

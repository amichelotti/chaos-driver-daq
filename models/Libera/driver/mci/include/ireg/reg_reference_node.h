/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: reg_reference_node.h 14606 2011-12-21 12:43:41Z matej.kenda $
 */

#ifndef REG_REFERENCE_NODE_H_
#define REG_REFERENCE_NODE_H_

#include "reg_type_node.h"

namespace ireg {

    /**
     * @addtogroup register
     * @{
     */
    /**
     * @brief Template implementation of a RegNode which holds reference to external
     * memory location (variable).
     *
     * When writing and reading values from this type of nodes, referenced memory
     * location is directly accessed.
     *
     *  @param AT  Node value type
     *  @param RT  Reference type (type of referenced variable)
     *
     * Example:
     * @dontinclude ut_node.cpp
     *  @skip ReferenceNodeExample()
     *  @until }
     *
     * In most cases AT and RT are the same types. But there are some cases,
     * where RT is volatile type which is than converted into AT without
     * volatile. See typedefs below.
     */

    template <class AT, typename RT>
        class RegReferenceNode: public RegTypeNode<AT> {
        public:

            virtual ~RegReferenceNode()
            {

            }

            virtual bool operator==(const TreeNode& a_other) const
            {
                const RegReferenceNode<AT,RT>*pOther = dynamic_cast<const RegReferenceNode<AT, RT>*>(&a_other);
                return ((pOther != NULL) &&
                        (RegTypeNode<AT>::operator==(a_other)) &&
                        (m_reference == (pOther->m_reference))
                       );
            }

        protected:
            RegReferenceNode(
                const std::string& a_name,
                RT& a_ref,
                Flags a_flags = eNfDefault)
            : RegTypeNode<AT>(a_name, a_flags), m_reference(&a_ref), m_size(1)
            {
            }

            /**
             * @param a_name  Node name
             * @param a_size  Size of array
             * @param a_val   Reference to a variable of template type RT
             * @param a_flags Various flags can be set to the node
             */
            RegReferenceNode(
                const std::string& a_name,
                size_t a_size,
                RT* a_ref,
                Flags a_flags)
            : RegTypeNode<AT>(a_name, a_flags | eNfArray),
              m_reference(a_ref),
              m_size(a_size)
            {
            }

            mci_CREATE_NODE_FRIEND;

            virtual size_t OnGet( AT* a_out, size_t a_pos, size_t a_size) const
            {
                 std::copy (m_reference + a_pos,
                            m_reference + a_pos + a_size,
                            a_out);
                 return a_size;
            }

            virtual void OnSet(const AT* a_in, size_t a_pos, size_t a_size)
            {
                std::copy (a_in, a_in + a_size, m_reference + a_pos);
            }

            virtual bool OnResize(size_t a_size, const Path& a_relPath = c_noRelPath)
            {
                m_size = a_size;
                return true;
            }

            virtual size_t OnGetSize(const Path& a_relPath) const
            {
                return m_size;
            }

        private:
            RT     *m_reference;
            size_t  m_size;
        };

    typedef RegReferenceNode<double,      double>             RegReferenceDoubleNode;
    typedef RegReferenceNode<float,       float>              RegReferenceFloatNode;
    typedef RegReferenceNode<std::string, std::string>        RegReferenceStringNode;
    typedef RegReferenceNode<bool,        bool>               RegReferenceBoolNode;
    typedef RegReferenceNode<int32_t,     int32_t>            RegReferenceInt32Node;
    typedef RegReferenceNode<int64_t,     int64_t>            RegReferenceInt64Node;
    typedef RegReferenceNode<uint32_t,    uint32_t>           RegReferenceUInt32Node;
    typedef RegReferenceNode<uint64_t,    uint64_t>           RegReferenceUInt64Node;

    typedef RegReferenceNode<int32_t,     volatile int32_t>   RegReferenceVolatileInt32Node;
    typedef RegReferenceNode<int64_t,     volatile int64_t>   RegReferenceVolatileInt64Node;
    typedef RegReferenceNode<uint32_t,    volatile uint32_t>  RegReferenceVolatileUInt32Node;
    typedef RegReferenceNode<uint64_t,    volatile uint64_t>  RegReferenceVolatileUInt64Node;

    /**@}*/
}

#endif /* REG_REFERENCE_NODE_H_ */

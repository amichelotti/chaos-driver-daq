/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: reg_reference_bit_node.h 14606 2011-12-21 12:43:41Z matej.kenda $
 */


#ifndef REG_REFERENCE_BIT_NODE_H_
#define REG_REFERENCE_BIT_NODE_H_

#include <limits>

#include "reg_reference_node.h"
#include "reg_type_node.h"

namespace ireg {

    /**
     * @addtogroup register
     * @{
     */
    /**
     * @brief Template implementation of a RegNode which holds reference to
     * specific part of external memory location (variable).
     *
     * When writing and reading values from this type
     * of nodes, referenced memory location is directly accessed. This is specific
     * version of RegReferenceNode, where only part of the referenced memory is read
     * or written. User has a possibility to exactly define which part (bits) to use.
     *
     *  \param AT  Node value type
     *  \param RT  Reference type (type of referenced variable)
     *
     * Example:
     * @dontinclude ut_node.cpp
     *  \skip ReferenceBitNodeExample()
     *  \until }
     *
     */

    template <class AT, class RT>
        class RegReferenceBitNode : public RegTypeNode<AT> {
        public:


            virtual ~RegReferenceBitNode()
            {

            }

            virtual bool operator==(const TreeNode& a_other) const
            {
                const RegReferenceBitNode<AT,RT>*pOther = dynamic_cast<const RegReferenceBitNode<AT,RT>*>(&a_other);
                return ((pOther != NULL) &&
                        (RegNode::operator==(a_other)) &&
                        (m_reference == (pOther->m_reference)) &&
                        (m_bitOffset == pOther->m_bitOffset) &&
                        (m_bitSize == pOther->m_bitSize)
                       );
            }

        protected:

            /**
             * @param a_name       Node name
             * @param a_size       Node array size
             * @param a_val        Reference to a variable of template type RT
             * @param a_bitOffset  Start bit position inside reference value
             * @param a_bitSize    How many bits inside reference value are used by this node
             * @param a_flags      Various flags can be set to the node
             */
            RegReferenceBitNode(
                const std::string& a_name,
                const size_t  a_size, // node size
                RT* a_ref,
                const uint8_t a_bitOffset,
                const uint8_t a_bitSize,
                Flags a_flags)
            : RegTypeNode<AT>(a_name, a_flags | eNfArray),
              m_reference(a_ref),
              m_bitOffset(a_bitOffset),
              m_bitSize(a_bitSize),
              m_size(a_size)
            {
                SanityCheck();
            }

            RegReferenceBitNode(
                const std::string& a_name,
                RT& a_ref,
                const uint8_t a_bitOffset,
                const uint8_t a_bitSize,
                Flags a_flags)
            : RegTypeNode<AT>(a_name, a_flags),
              m_reference(&a_ref),
              m_bitOffset(a_bitOffset),
              m_bitSize(a_bitSize),
              m_size(1)
            {
                SanityCheck();
            }

            RegReferenceBitNode(
                const std::string& a_name,
                RT& a_ref,
                const uint8_t a_bitOffset,
                const uint8_t a_bitSize)
            : RegTypeNode<AT>(a_name, eNfDefault),
              m_reference(&a_ref),
              m_bitOffset(a_bitOffset),
              m_bitSize(a_bitSize),
              m_size(1)
            {
                SanityCheck();
            }

            mci_CREATE_NODE_FRIEND;

            virtual size_t OnGet(AT* a_out, size_t a_pos, size_t a_size) const
            {
                return GetValueBits(a_out, a_pos, a_size);
            }

            virtual void OnSet(const AT* a_in, size_t a_pos, size_t a_size)
            {
                SetValueBits(a_in, a_pos, a_size);
            }


            virtual bool OnResize(size_t a_size, const Path& a_relPath = c_noRelPath)
            {
                istd_FTRC();
                m_size = a_size;
                return true;
            }

            virtual size_t OnGetSize(const Path& a_relPath) const
            {
                return m_size;
            }

        private:
            void SanityCheck() const
            {
                if (m_bitOffset + m_bitSize > sizeof(RT) * 8) {
                    throw istd::Exception("size and offset are out of range");
                }
                if (m_bitSize == 0) {
                    throw istd::Exception("size could not be zero");
                }
                if (m_bitSize > sizeof(AT) * 8) {
                    throw istd::Exception("size is of range");
                }
            }

//TODO: "Replace with compile-time bitfields."

            size_t GetValueBits(
                AT* a_out, const size_t a_pos, const size_t a_size) const
            {
                if (std::numeric_limits<AT>::is_signed) {
                    for (size_t i = 0; i < a_size; ++i) {
                        /* get a value from referenced bits */
                        AT val = static_cast<AT>(((m_reference[i + a_pos]) >> (m_bitOffset)) & GetBitMask());

                        /* check if MSB bit is set, which indicates negative value */
                        if (val & GetMSBMask()) {
                            /* handle negative value */
                            int64_t tmp = val;
                            tmp |= ~GetBitMask();
                            a_out[i] = static_cast<AT>(tmp);
                        }
                        else {
                            a_out[i] = val;
                        }
                    }
                }
                else {
                    for (size_t i = 0; i < a_size; ++i) {
                        a_out[i] = static_cast<AT>(((m_reference[i + a_pos]) >>
                                               (m_bitOffset)) & GetBitMask());
                    }
                }

                return a_size;
            }

            void SetValueBits(
                const AT* a_in, const size_t a_pos, const size_t a_size)
            {
                if (std::numeric_limits<AT>::is_signed) {
                    // Check if any value is too large
                    for (size_t i = 0; i < a_size; ++i) {
                        RT val = static_cast<RT>(a_in[i]);
                        /* define mask for unused bits */
                        RT unused_bits = ~(GetBitMask()>>1);

                        /* filter out data bits */
                        val &= unused_bits;

                        /* check if none if unused bits is set */
                        if ((val != 0) && (val != unused_bits)) {
                            istd_EXCEPTION("Value " << a_in[i] << " is out of range for node " << RegTypeNode<AT>::GetName());
                        }
                    }

                    // apply values
                    for (size_t i = 0; i < a_size; ++i) {
                        RT new_val;
                        if (a_in[i]>0) {
                            /* handle positive numbers */
                            new_val = static_cast<RT>(a_in[i]);
                        }
                        else {
                            new_val = static_cast<RT>(a_in[i]);
                        }
                        // Read & Clear masked bits
                        RT val = m_reference[i + a_pos] & (~(GetBitMask() << m_bitOffset));

                        // Set new value
                        m_reference[i + a_pos] = val | static_cast<RT>(((new_val & GetBitMask())) << m_bitOffset);
                    }
                }
                else {
                    // Check if any value is too large
                    for (size_t i = 0; i < a_size; ++i) {
                        RT val = static_cast<RT>(a_in[i]);
                        if ((val & GetBitMask()) != val) {
                            istd_EXCEPTION("Too large value." << a_in[i] << " for node " << RegTypeNode<AT>::GetName());
                        }
                    }

                    // apply values
                    for (size_t i = 0; i < a_size; ++i) {
                        RT new_val = static_cast<RT>(a_in[i]);

                        // Read & Clear masked bits
                        RT val =  m_reference[i + a_pos] & (~(GetBitMask() << m_bitOffset));

                        // Set new value
                        m_reference[i + a_pos] = val | static_cast<RT>(((new_val & GetBitMask())) << m_bitOffset);
                    }
                }
            }

            inline uint64_t GetBitMask() const
            {
                return (-1ULL >> (sizeof(uint64_t)*8 - m_bitSize));
            }

            inline uint64_t GetMSBMask() const
            {
                return (0x1ULL << (m_bitSize-1));
            }

            RT              *m_reference;
            const uint8_t    m_bitOffset;
            const uint8_t    m_bitSize;
            size_t           m_size; // node array size
        };


    typedef RegReferenceBitNode<uint32_t, volatile uint64_t> RegRefUint64VUInt32Node;
    typedef RegReferenceBitNode<uint32_t, volatile uint32_t> RegRefUint32VUInt32Node;
    typedef RegReferenceBitNode<int32_t,  volatile uint64_t> RegRefUint64VInt32Node;
    typedef RegReferenceBitNode<int32_t,  volatile uint32_t> RegRefUint32VInt32Node;
    typedef RegReferenceBitNode<bool,     volatile uint64_t> RegRefUint64VBoolNode;
    typedef RegReferenceBitNode<bool,     volatile uint32_t> RegRefUint32VBoolNode;
}

#endif /* REG_REFERENCE_BIT_NODE_H_ */

/*
 * Copyright (c) 2012 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: reg_ref_node.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */


#ifndef REG_REF_NODE_H_
#define REG_REF_NODE_H_

#include <limits>

#include "istd/bit_limits.h"
#include "ireg/reg_enum_mixin.h"
#include "reg_type_node.h"

namespace ireg {

    template <typename T, size_t BITSIZE, size_t OFF>   struct ValueAccessor;
    template <typename T, size_t BITSIZE>               struct ValueAccessor<T, BITSIZE, 0>;
    template <typename T>                               struct ValueAccessor<T, 0, 0>;

    template <typename NT, size_t BITSIZE = 0, size_t OFF = 0, typename RT = NT>
        class RegRefNode :
            public RegTypeNode<typename std::remove_cv<NT>::type>  {

        public:
            typedef typename std::remove_cv<NT>::type   NodeType;
            typedef RT                                  RefType;
            typedef RegTypeNode<NodeType>               BaseNode;

            virtual ~RegRefNode()
            {
            }

        protected:
            typedef ValueAccessor<RefType, BITSIZE, OFF> Accessor;

            explicit RegRefNode(
                const std::string&  a_name,
                RefType&            a_ref,
                Flags               a_flags = eNfDefault)
            : BaseNode(a_name, a_flags), m_ref(&a_ref), m_size(1)
            {
                // Set limits automatically?
            };

            explicit RegRefNode(
                const std::string&  a_name,
                size_t              a_size,
                RefType*            a_ref,
                Flags               a_flags = eNfDefault)
            : BaseNode(a_name, a_flags | eNfArray), m_ref(a_ref), m_size(a_size)
            {
                // Set limits automatically?
            };

            mci_CREATE_NODE_FRIEND;

            virtual void OnSet(const NodeType* a_in, size_t a_pos, size_t a_size)
            {
                for (size_t i(0); i < a_size; ++i) {
                    const NodeType& in(a_in[i]);
                    RefType& bf(m_ref[a_pos+i]);
                    Accessor tmp(bf);
                    tmp = in;
                }
            }

            virtual size_t OnGet(NodeType* a_out, size_t a_pos, size_t a_size) const
            {
                for (size_t i(0); i < a_size; ++i) {
                    RefType& ref(m_ref[a_pos+i]);
                    Accessor tmp(ref);
                    const RefType nt = tmp;
                    a_out[i] = static_cast<const NodeType>(nt);
                }
                return a_size;
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
            RefType*    m_ref;
            size_t      m_size; // node array size
        };

//---

    template <typename T, size_t BITSIZE, size_t OFF>
        struct ValueAccessor {

            union Bitmask {
                T   val;
                // TODO: Revert fields for different endianness
                struct {
                    T   pad:OFF; // padded lower bits
                    T   val:BITSIZE;
                } s;
            };

            static_assert(
                std::is_integral<T>::value && !std::is_same<T, bool>::value,
                "Bitfields available only for integer types."
            );
            static_assert( (BITSIZE + OFF) <= sizeof(T)*8, "Bitfield overflow");
            static_assert( sizeof(Bitmask) == sizeof(T), "Internal error: sizes don't match");

            ValueAccessor(T& a_val) : val(a_val) { };
            ~ValueAccessor() { };

            inline const ValueAccessor& operator=(const T& v)
            {
                Bitmask b;
                b.s.val = v;
                val = b.val;
                return *this;
            };

            inline operator T()
            {
                Bitmask b;
                b.val = val;
                return b.s.val;
            };

            T&  val;
        };

    template <typename T, size_t BITSIZE>
        struct ValueAccessor<T, BITSIZE, 0> {

            union Bitmask {
                T   val;
                struct {
                    T   val:BITSIZE;
                } s;
            };

            static_assert(
                std::is_integral<T>::value && !std::is_same<T, bool>::value,
                "Bitfields available only for integer types."
            );
            static_assert(BITSIZE <= sizeof(T)*8, "Bitfield overflow");
            static_assert( sizeof(Bitmask) == sizeof(T), "Internal error: sizes don't match");

            ValueAccessor(T& a_val) : val(a_val) { };
            ~ValueAccessor() { };

            inline const ValueAccessor& operator=(const T& v)
            {
                Bitmask b;
                b.s.val = v;
                val = b.val;
                return *this;
            };

            inline operator T()
            {
                Bitmask b;
                b.val = val;
                return b.s.val;
            };

            T&  val;
        };

    // Not accessing a bitfield, but a regular type
    template <typename T>
        struct ValueAccessor<T, 0, 0> {
            ValueAccessor(T& a_val) : val(a_val) { };
            ~ValueAccessor() { };

            inline const ValueAccessor& operator=(const T& v)
            {
                val = v;
                return *this;
            };

            inline operator T()
            {
                return val;
            };

            T&  val;
        };

}

#endif /* REG_REF_NODE_H_ */

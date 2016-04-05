/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: runtime_atom.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef ISIG_TUNTIME_ATOM_H_
#define ISIG_TUNTIME_ATOM_H_

#include <cstddef>
#include "istd/exception.h"

namespace isig {

    /**
     * RuntimeAtom is used in situation where the atom properties can be
     * determined only in runtime (for example, remote).
     * It is used to access individual atoms inside an Array.
     * @param BASETYPE The base type of the underlying atom (float, int16_t, ...)
     */
    template <class BASETYPE>
        class RuntimeAtom {
        public:
            typedef BASETYPE    BaseType;

            explicit RuntimeAtom(BaseType* a_atom, size_t a_components)
            : m_components(a_components),
              m_readOnly(false),
              m_atom_p(a_atom)
            {
            }

            explicit RuntimeAtom(const BaseType* a_atom, size_t a_components)
            : m_components(a_components),
              m_readOnly(true),
              m_atom_p(const_cast<BaseType*>(a_atom))
            {
            }

            ~RuntimeAtom()
            {
            }

            RuntimeAtom& operator=(const RuntimeAtom& a_other)
            {
                if (&a_other == this) {
                    return *this;
                }
                if (a_other.m_atom_p == m_atom_p) {
                    return *this;
                }

                if (m_readOnly) {
                    throw istd::Exception("runtime atom: read-only");
                }
                if (m_components != a_other.m_components) {
                    throw istd::Exception("runtime atom: incompatible runtime atoms");
                }
                std::copy(
                    a_other.m_atom_p,
                    a_other.m_atom_p + m_components,
                    m_atom_p);

                return *this;
            }

            RuntimeAtom(const RuntimeAtom& a_other)
            : m_components(a_other.m_components),
              m_readOnly(a_other.m_readOnly),
              m_atom_p(a_other.m_atom_p)
            {

            }

            /** Access to a specific member of the atom */
            BaseType& operator[](size_t a_index)
            {
                if (a_index >= m_components) {
                    throw istd::Exception("runtime atom: index out of range");
                }
                return m_atom_p[a_index];
            }

            const BaseType& operator[](size_t a_index) const
            {
                if (a_index >= m_components) {
                    throw istd::Exception("array: index out of range");
                }
                return m_atom_p[a_index];
            }

            /** Number of components of this atom */
            inline size_t GetComponents() const
            {
                return m_components;
            }

        private:
            const size_t    m_components;
            const bool      m_readOnly;
            BaseType        *m_atom_p;
        };

    extern template class RuntimeAtom<int8_t>;
    extern template class RuntimeAtom<uint8_t>;
    extern template class RuntimeAtom<int16_t>;
    extern template class RuntimeAtom<uint16_t>;
    extern template class RuntimeAtom<int32_t>;
    extern template class RuntimeAtom<uint32_t>;
    extern template class RuntimeAtom<int64_t>;
    extern template class RuntimeAtom<uint64_t>;
    extern template class RuntimeAtom<float>;
    extern template class RuntimeAtom<double>;

}

#endif /* ISIG_TUNTIME_ATOM_H_ */

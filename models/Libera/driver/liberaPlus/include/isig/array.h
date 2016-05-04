/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: array.h 19990 2014-01-07 09:35:55Z damijan.skvarc $
 */

#ifndef ISIG_ARRAY_H_
#define ISIG_ARRAY_H_

#include <memory>
#include <algorithm>
#include <initializer_list>
#include "isig/signal_traits.h"
#include "isig/declarations.h"

namespace isig {

    /**
     * Array is acting as a proxy class to the underlying memory buffer of
     * Traits::BaseType. Linear memory layout provides easy reading and
     * marshaling of the data.
     * On the other hand, the memory buffer is presented as an array of
     * BaseType::Atom to allow easy manipulation of the data atoms.
     * @param TRAITS Signal traits defining the content
     */
    template <class TRAITS>
        class Array {
        public:
            typedef TRAITS                          Traits;
            typedef typename Traits::BaseType       BaseType;
            typedef typename Traits::Atom           Atom;
            typedef typename Traits::Reference      Reference;
            typedef typename Traits::ConstReference ConstReference;

            Array(
                size_t          a_length = 0,
                const Traits&   a_traits = Traits())
            : m_traits(a_traits),
              m_length(a_length),
              m_capacity(m_length),
              m_bufferOwner(true),
              m_buffer_p(NULL)
            {
                if (m_length > 0) {
                    m_buffer_p = new BaseType[m_length * m_traits.GetAtomSize()];
                }
            }

            Array(const Traits& a_traits)
            : m_traits(a_traits),
              m_length(0),
              m_capacity(m_length),
              m_bufferOwner(true),
              m_buffer_p(NULL)
            {
            }

            Array(
                std::initializer_list<BaseType> a_values,
                const Traits& a_traits = Traits())
            : m_traits(a_traits),
              m_length(a_values.size() / m_traits.GetComponents()),
              m_capacity(m_length),
              m_bufferOwner(true),
              m_buffer_p(NULL)
            {
                m_buffer_p = new BaseType[m_length * m_traits.GetAtomSize()];
                std::copy(a_values.begin(), a_values.end(), m_buffer_p);
            }

            /**
             * Wrapper around an existing atom. Will not deallocate wrapped
             * buffer.
             */
            explicit Array(
                Atom* a_atom,
                const Traits& a_traits = Traits())
            : m_traits(a_traits),
              m_length(1),
              m_capacity(m_length),
              m_bufferOwner(false),
              m_buffer_p(Traits::BaseTypeCast(a_atom))
            {
            }

            /**
             * Wrapper around existing buffer. Will not deallocate wrapped
             * memory.
             */
            explicit Array(BaseType* a_buf, size_t a_size, Traits a_traits = Traits())
            : m_traits(a_traits),
              m_length(a_size / m_traits.GetComponents()),
              m_capacity(m_length),
              m_bufferOwner(false),
              m_buffer_p(a_buf)
            {
            }

            ~Array()
            {
                DeleteBuffer();
            }

            Array& operator=(const Array& a_other)
            {
                if (&a_other == this) {
                    return *this;
                }
                m_traits = a_other.m_traits;
                Resize(a_other.m_length);
                CopyFrom(a_other);
                return *this;
            }

            /**
             * Copy ctor creates a deep copy of the managed memory. If a_other
             * is a wrapper around existing memory, then the its content
             * will be copied to the target (this).
             */
            Array(const Array& a_other)
            : m_traits(a_other.m_traits),
              m_length(a_other.m_length),
              m_capacity(m_length),
              m_bufferOwner(true),
              m_buffer_p(NULL)
            {
                m_buffer_p = new BaseType[m_length * m_traits.GetAtomSize()];
                Copy(m_buffer_p, a_other.m_buffer_p, m_length);
            }

            /**
             * Move ctor.
             */
            Array(Array&& a_other)
            : m_traits( std::move(a_other.m_traits) ),
              m_length(a_other.m_length),
              m_capacity(a_other.m_capacity),
              m_bufferOwner(a_other.m_bufferOwner),
              m_buffer_p(a_other.m_buffer_p)
            {
                // Ownership is taken by this instance.
                // Prevent double release.
                a_other.m_buffer_p = NULL;
                a_other.m_capacity = a_other.m_length = 0;
            }

            // Move assignment
            Array& operator=(Array&& a_other)
            {
                if (&a_other == this) {
                    return *this;
                }
                std::swap(m_traits, a_other.m_traits);

                DeleteBuffer();

                m_length = a_other.m_length;
                m_capacity = a_other.m_capacity;
                m_bufferOwner = a_other.m_bufferOwner;
                m_buffer_p = a_other.m_buffer_p;

                a_other.m_buffer_p = NULL;
                a_other.m_capacity = a_other.m_length = 0;

                return *this;
            }

            /** Length in atom units */
            inline size_t GetLength() const
            {
                return m_length;
            }

            /** Length in base type units */
            inline size_t GetBufferLength() const
            {
                return m_length * m_traits.GetComponents();
            }

            /** Total size in bytes (useful for direct buffer manipulation) */
            inline size_t GetByteSize() const
            {
                return m_length * m_traits.GetAtomSize();
            }

            /**
             * Capacity (allocated memory) in atom units.
             * Actual length can be smaller.
             */
            inline size_t GetCapacity() const
            {
                return m_capacity;
            }

            /**
             * Signal traits used when constructing the array.
             */
            inline const Traits& GetTraits() const
            {
                return m_traits;
            }

            void Append(const Array& a_other)
            {
                const auto prevLen(GetBufferLength());
                Resize(m_length + a_other.GetLength());
                BaseType* start( m_buffer_p + prevLen );
                Copy(start, a_other.m_buffer_p, a_other.GetLength());
            }

            void CopyFrom(const Array& a_other)
            {
                Copy(m_buffer_p, a_other.m_buffer_p, std::min(m_length, a_other.m_length));
            }

            /**
             * Resizes the array. Memory is not reallocated if the new array is
             * shorter than the allocated memory.
             * When new length exceeds the capacity then new memory is allocated
             * and existing content is copied to newly allocated buffer.
             *
             * @note Attempt to extend the array size beyond the capacity of the
             * non-owner array (wrapped) throws an exception.
             */
            void Resize(size_t a_atoms)
            {
                if (a_atoms <= m_capacity) {
                    m_length = a_atoms;
                    return;
                }
                if (!m_bufferOwner) {
                    throw istd::Exception("array: Cannot extend non-owned array.");
                }
                Reallocate(a_atoms);
                m_length = a_atoms;
            }

            /**
             * Extend the container without affecting the size.
             *
             * @note The container will not be shrunk below the current size
             *       of array.
             */
            void Reserve(size_t a_capacity)
            {
                if (a_capacity <= m_length) {
                    return;
                }
                if (!m_bufferOwner) {
                    return;
                }
                Reallocate(a_capacity);
            }

            /**
             * Extend the size to the full capacity of the array.
             */
            void Expand()
            {
                m_length = m_capacity;
            }

            /**
             * Access to a specific member of an array
             * @note Implementation for compile-time defined atoms
             * @return Atom&
             */
            Reference operator[](size_t a_index)
            {
                if (a_index >= m_length) {
                    throw istd::Exception("array: index out of range");
                }
                return OperatorAt(a_index, m_traits);
            }

            /**
             * Access to a specific member of an array
             * @note Implementation for compile-time defined atoms
             * @return const Atom&
             */
            ConstReference operator[](size_t a_index) const
            {
                if (a_index >= m_length) {
                    throw istd::Exception("array: index out of range");
                }
                return OperatorAt(a_index, m_traits);
            }

            /**
             * Copy multiple atoms into the a_array starting from a_pos. The
             * amount of data to be copied is determined from the size of
             * a_array. a_array might be shortened to the available data size.
             *
             * @param a_array Destination array
             * @param a_pos starting position in atoms
             */
            void SubArray(Array& a_array, size_t a_pos) const
            {
                if (a_pos >= m_length) {
                    throw istd::Exception("array: index out of range");
                }
                const size_t size( std::min(a_array.GetLength(), m_length - a_pos) );
                a_array.Resize(size);
                Copy(a_array.AtomAt(0), this->AtomAt(a_pos), size);
            }

            Array SubArray(size_t a_pos, size_t a_length) const
            {
                Array out(a_length);
                SubArray(out, a_pos);
                return out;
            }

            /** Insert an array to position a_pos */
            void Insert(const Array& a_array, size_t a_pos)
            {
                if (a_pos >= m_length) {
                    throw istd::Exception("array: index out of range");
                }
                if (a_array.GetLength() > m_length - a_pos) {
                    throw istd::Exception("array: argument too large");
                }

                Copy(this->AtomAt(a_pos), a_array.AtomAt(0), a_array.GetLength());
            }

            inline BaseType* GetBuffer()
            {
                return m_buffer_p;
            }

            inline const BaseType* GetBuffer() const
            {
                return m_buffer_p;
            }

            inline bool IsMemoryOwner() const
            {
                return m_bufferOwner;
            }

        private:
            Traits      m_traits;
            size_t      m_length;
            size_t      m_capacity;
            bool        m_bufferOwner;
            BaseType    *m_buffer_p;

            inline BaseType* AtomAt(size_t a_pos)
            {
                return m_buffer_p + a_pos * m_traits.GetComponents();
            }

            inline const BaseType* AtomAt(size_t a_pos) const
            {
                return m_buffer_p + a_pos * m_traits.GetComponents();
            }

            inline void Copy(BaseType* a_dest, const BaseType* a_src, size_t a_len)
            {
                std::copy(a_src, a_src + a_len * m_traits.GetComponents(), a_dest);
            }

            inline void Copy(BaseType* a_dest, const BaseType* a_src, size_t a_len) const
            {
                std::copy(a_src, a_src + a_len * m_traits.GetComponents(), a_dest);
            }

            inline void Reallocate(size_t a_capacity)
            {
                // This array owns the memory from this point on
                // and the array will get extended
                std::unique_ptr<BaseType[]>
                    nb(new BaseType[a_capacity * m_traits.GetAtomSize()]);

                Copy(nb.get(), this->AtomAt(0), this->GetLength());
                delete [] m_buffer_p;
                m_buffer_p = nb.release();
                m_capacity = a_capacity;
            }

            inline void DeleteBuffer()
            {
                if (m_bufferOwner) {
                    delete [] m_buffer_p;
                    m_buffer_p = NULL;
                }
            }

            /**
             * Access to a specific member of an array
             * @note Implementation for compile-time defined atoms
             * @return Atom&
             */
            template <typename N, typename A, size_t GS>
                inline Reference OperatorAt(size_t a_index, const SignalTraits<N, A, GS>&)
                {
                    BaseType* ap = AtomAt(a_index);
                    return (*Traits::AtomCast(ap));
                }

            /**
             * Access to a specific member of an array
             * @note Implementation for compile-time defined atoms
             * @return const Atom&
             */
            template <typename N, typename A, size_t GS>
                inline ConstReference OperatorAt(size_t a_index, const SignalTraits<N, A, GS>&) const
                {
                    const BaseType* ap = AtomAt(a_index);
                    return (*Traits::AtomCast(ap));
                }

            template <typename N>
                inline Reference OperatorAt(size_t a_index, const SignalTraits<N, RuntimeDefined, 1>&)
                {
                    return Atom(AtomAt(a_index), this->m_traits.GetComponents());
                }

            template <typename N>
                inline ConstReference OperatorAt(size_t a_index, const SignalTraits<N, RuntimeDefined, 1>&) const
                {
                    return Atom(AtomAt(a_index), this->m_traits.GetComponents());
                }

        };


    template <class TRAITS>
        struct MetaBufferPair {
            typedef TRAITS                          Traits;
            typedef Array<Traits>                   Buffer;
            typedef std::shared_ptr<MetaBufferPair> MetaBufferPtr;

            MetaBufferPair(Buffer &&a_data, const SignalMeta &a_meta)       : data(std::move(a_data)), meta(a_meta) {}
            MetaBufferPair(const  Buffer &a_data, const SignalMeta &a_meta) : data(a_data), meta(a_meta) {}

            MetaBufferPair(MetaBufferPair &&a_buffer) : data(std::move(a_buffer.data)), meta(a_buffer.meta) {}

            Buffer      data;
            SignalMeta  meta;
        };


    // Array for runtime traits are instantiated in the library
    extern template class Array<SignalTraitsVarUint8>;
    extern template class Array<SignalTraitsVarInt16>;
    extern template class Array<SignalTraitsVarUint16>;
    extern template class Array<SignalTraitsVarInt32>;
    extern template class Array<SignalTraitsVarUint32>;
    extern template class Array<SignalTraitsVarInt64>;
    extern template class Array<SignalTraitsVarUint64>;
    extern template class Array<SignalTraitsVarFloat>;
    extern template class Array<SignalTraitsVarDouble>;

}

#endif /* ISIG_ARRAY_H_ */

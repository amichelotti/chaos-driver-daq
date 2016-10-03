/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: signal_traits.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */


#ifndef ISIG_SIGNAL_TRAITS_H_
#define ISIG_SIGNAL_TRAITS_H_

#include <iostream>
#include <array>
#include <vector>
#include <string>
#include <type_traits>
#include <algorithm>
#include "istd/exception.h"
#include "isig/runtime_atom.h"

namespace isig {

    template <class CONT>
        int FindElementPosition(const CONT& a_names, const std::string& a_elName)
        {
            if (a_elName.empty()) {
                return -1;
            }
            auto it = std::find(a_names.begin(), a_names.end(), a_elName);
            if (it == a_names.end()) {
                return -1;
            }
            // The distance from the iterator to the beginning is
            // exactly the index
            return std::distance(a_names.begin(), it);
        }

    /**
     * Type traits to describe a signal atom. The template creates a union of
     * a composite type (a struct) and an array of the same size.
     *
     * A static array SignalTraits::names is created to hold descriptions of the
     * individual columns.
     *
     * SignalTraits::Array and SignalTraits::names are used to simplify (enable)
     * access to signal components for display and transfer over the network.
     *
     * Precondition is that the composite type is composed of components of
     * a single type (parameter NUMERIC). However, it can hold SIMD vectors
     * of the same type, used for fast computation.
     *
     * @note NUMERIC may be only integer or floating type
     * @param NUMERIC the base type
     * @param COMPOSITE struct or some other type
     */
    template<typename NUMERIC, typename COMPOSITE, size_t GROUP_SIZE = 1>
        class SignalTraits {
        public:
            typedef NUMERIC     BaseType;
            typedef COMPOSITE   Atom;
            typedef Atom&       Reference;
            typedef const Atom& ConstReference;

        public:
            SignalTraits(size_t a_groupSize = GROUP_SIZE)
            : m_groupSize(a_groupSize)
            {
                if (m_groupSize < 1) {
                    throw istd::Exception("signal traits: Wrong group size.");
                }
            }

            // Copy ctor
            SignalTraits(const SignalTraits& a_other)
            : m_groupSize(a_other.m_groupSize)
            {
            }

            // Assignment operator
            SignalTraits& operator=(const SignalTraits& a_other)
            {
                if (&a_other == this) {
                    return *this;
                }
                m_groupSize = a_other.m_groupSize;
                return *this;
            }

            // Move ctor
            SignalTraits(SignalTraits&& a_other)
            : m_groupSize(a_other.m_groupSize)
            {
            }

            // Move assignment
            SignalTraits& operator=(SignalTraits&& a_other)
            {
                m_groupSize = a_other.m_groupSize;
                return *this;
            }

            static const size_t m_components = sizeof(Atom)/sizeof(BaseType);

            /**
             * The number of (vertical) components of the signal
             */
            inline static size_t GetComponents()
            {
                return m_components;
            }

            /**
             * The number of atoms in the group (horizontal) when multiple of
             * them must be treated together.
             */
            inline size_t GetGroupSize() const
            {
                return m_groupSize;
            }

            typedef std::array<std::string, m_components>   Names;
            /**
             * Human readable names of the individual components
             * @note Must be instantiated separately for each instance
             * of the template
             */
            static const Names names;

            inline static const Names& GetNames()
            {
                return names;
            }

            /**
             * Size of the signal atom in bytes.
             */
            inline static size_t GetAtomSize()
            {
                return sizeof(Atom);
            }

            /**
             * Finds the position of the element name.
             * @return Column number corresponding to the element name
             * @retval -1 not found
             */
            inline int PositionOf(const std::string& a_elName) const
            {
                return FindElementPosition(names, a_elName);
            }

            inline static Atom* AtomCast(BaseType* a_base)
            {
                    return reinterpret_cast<Atom*>(a_base);
            }

            inline static const Atom* AtomCast(const BaseType* a_base)
            {
                    return reinterpret_cast<const Atom*>(a_base);
            }

            inline static BaseType* BaseTypeCast(Atom* a_atom)
            {
                    return reinterpret_cast<BaseType*>(a_atom);
            }

            inline static const BaseType* BaseTypeCast(const Atom* a_atom)
            {
                    return reinterpret_cast<const BaseType*>(a_atom);
            }

            inline static void Dump(std::ostream& a_os, const Atom& a)
            {
                const BaseTypeArray& bta = reinterpret_cast<const BaseTypeArray&>(a);
                for (auto i = bta.begin(); i != bta.end(); ++i) {
                    a_os << (*i) << " ";
                }
            }

        private:
            size_t              m_groupSize;

            /**
             * Array of base type of the same size as the composite type.
             * Used to verify size at compile time.
             */
            typedef std::array<BaseType, m_components>      BaseTypeArray;

            /**
             * Overlapping composite and array of the same size
             */
            struct Union {
                union {
                    BaseTypeArray   simple;
                    Atom            atom;
                };
            };

            static_assert(
                sizeof(BaseTypeArray) == sizeof(Atom),
                "Size of Array and Composite differ");

            static_assert(
                sizeof(Atom) == sizeof(Union),
                "Size of Atom and Composite differ");

            static_assert(
                std::is_arithmetic<BaseType>::value,
                "NUMERIC is not numerical type");

            static_assert(GROUP_SIZE > 0, "GROUP_SIZE is too small");
        };

    /**
     * Partial specialisation if the "composite" is actually simple numeric
     * type.
     *
     * Atom is the numeric simple type in that case.
     * @param NUMERIC simple numeric type (integer or double)
     */
    template<typename NUMERIC, size_t GROUP_SIZE>
        class SignalTraits<NUMERIC, NUMERIC, GROUP_SIZE> {
        public:
            typedef NUMERIC     BaseType;
            typedef BaseType    Atom;
            typedef Atom&       Reference;
            typedef const Atom& ConstReference;

        public:

            SignalTraits(size_t a_groupSize = GROUP_SIZE)
            : m_groupSize(a_groupSize)
            {
                if (m_groupSize < 1) {
                    throw istd::Exception("signal traits: Wrong group size.");
                }
            }

            // Copy ctor
            SignalTraits(const SignalTraits& a_other)
            : m_groupSize(a_other.m_groupSize)
            {
            }

            // Assignment operator
            SignalTraits& operator=(const SignalTraits& a_other)
            {
                if (&a_other == this) {
                    return *this;
                }
                m_groupSize = a_other.m_groupSize;
                return *this;
            }

            // Move ctor
            SignalTraits(SignalTraits&& a_other)
            : m_groupSize(a_other.m_groupSize)
            {
            }

            // Move assignment
            SignalTraits& operator=(SignalTraits&& a_other)
            {
                m_groupSize = a_other.m_groupSize;
                return *this;
            }

            static const size_t m_components = 1;

            /**
             * The number of (vertical) components of the signal
             */
            inline static size_t GetComponents()
            {
                return m_components;
            }

            /**
             * The number of atoms in the group (horizontal) when multiple of
             * them must be treated together.
             */
            inline size_t GetGroupSize() const
            {
                return m_groupSize;
            }

            typedef std::array<std::string, m_components>   Names;
            /**
             * Human readable names of the individual components
             * @note Must be instantiated separately for each instance
             * of the template
             */
            static const Names names;

            inline static const Names& GetNames()
            {
                return names;
            }

            /**
             * Size of the signal atom in bytes.
             */
            inline static size_t GetAtomSize()
            {
                return sizeof(Atom);
            }

            /**
             * Finds the position of the element name.
             * @return Column number corresponding to the element name
             * @retval -1 not found
             */
            inline int PositionOf(const std::string& a_elName)
            {
                if (names[0] == a_elName) {
                    return 0;
                }
                else {
                    return -1;
                }
            }

            // No casting, because both types are the same
            inline static Atom* AtomCast(BaseType* a_base)
            {
                return a_base;
            }

            // No casting, because both types are the same
            inline static const Atom* AtomCast(const BaseType* a_base)
            {
                return a_base;
            }

            inline static BaseType* BaseTypeCast(Atom* a_atom)
            {
                return a_atom;
            }

            inline static const BaseType* BaseTypeCast(const Atom* a_atom)
            {
                return a_atom;
            }

            inline static void Dump(std::ostream& a_os, const Atom& a)
            {
                a_os << a ;
            }

        private:
            size_t              m_groupSize;

            static_assert(
                std::is_arithmetic<BaseType>::value,
                "NUMERIC is not numerical type");

            static_assert(GROUP_SIZE > 0, "GROUP_SIZE is too small");
        };

    class RuntimeDefined {};

    /**
     * Specialisation for atom, whose size and column names is specified in
     * runtime, not at compile time.
     * @param NUMERIC simple numeric type (integer or double)
     */
    template<typename NUMERIC>
        class SignalTraits<NUMERIC, RuntimeDefined, 1> {
        public:
            typedef NUMERIC                     BaseType;
            typedef std::vector<std::string>    Names;

            /**
             * The vector can contain one or more atoms in a serialised
             * form. This means that the
             */
            typedef RuntimeAtom<BaseType>       Atom;
            typedef Atom                        Reference;
            typedef const Atom                  ConstReference;

            SignalTraits()
            : m_valid(false),
              m_components(0),
              m_groupSize(1),
              m_atomSize(0)
            {
            }

            /**
             * Runtime defined traits of the signal.
             * @note There is no default ctor for runtime traits because they
             *  always have to be constructed with parameters. Using default
             *  ctor will cause compile time error.
             * @param a_components Number of components (columns) in the signal
             * @param a_names Description of those components
             */
            SignalTraits(
                size_t a_components,
                Names a_names,
                size_t a_groupSize = 1)
            : m_valid(true),
              m_components(a_components),
              m_groupSize(a_groupSize),
              m_names(a_names),
              m_atomSize(sizeof(BaseType) * m_components)
            {
                if (m_groupSize < 1) {
                    throw istd::Exception("signal traits: Wrong group size.");
                }
                if (m_components < 1) {
                    throw istd::Exception("signal traits: Wrong number of components.");
                }
            }

            // Copy ctor
            SignalTraits(const SignalTraits& a_other)
            : m_valid(a_other.m_valid),
              m_components(a_other.m_components),
              m_groupSize(a_other.m_groupSize),
              m_names( std::move(a_other.m_names) ),
              m_atomSize( a_other.m_atomSize )
            {
            }

            // Assignment operator
            SignalTraits& operator=(const SignalTraits& a_other)
            {
                if (&a_other == this) {
                    return *this;
                }
                m_valid = a_other.m_valid;
                m_components = a_other.m_components;
                m_groupSize = a_other.m_groupSize;
                m_names = a_other.m_names;
                m_atomSize = a_other.m_atomSize;
                return *this;
            }

            // Move ctor
            SignalTraits(SignalTraits&& a_other)
            : m_valid(a_other.m_valid),
              m_components(a_other.m_components),
              m_groupSize(a_other.m_groupSize),
              m_names( std::move(a_other.m_names) ),
              m_atomSize( a_other.m_atomSize )
            {
            }

            // Move assignment
            SignalTraits& operator=(SignalTraits&& a_other)
            {
                if (&a_other == this) {
                    return *this;
                }
                m_valid = a_other.m_valid;
                m_components = a_other.m_components;
                m_groupSize = a_other.m_groupSize;
                std::swap( m_names, a_other.m_names );
                m_atomSize = a_other.m_atomSize;
                return *this;
            }

            /**
             * The number of (vertical) components of the signal
             */
            inline size_t GetComponents() const
            {
                return m_components;
            }

            /**
             * The number of atoms in the group (horizontal) when multiple of
             * them must be treated together.
             */
            inline size_t GetGroupSize() const
            {
                if (!m_valid) {
                    istd_EXCEPTION("isig: Runtime atom is not valid.");
                }
                return m_groupSize;
            }

            /**
             * Human readable names of the individual components
             */
            inline const Names& GetNames() const
            {
                if (!m_valid) {
                    istd_EXCEPTION("isig: Runtime atom is not valid.");
                }
                return m_names;
            }

            /**
             * Size of the signal atom in bytes.
             */
            inline size_t GetAtomSize() const
            {
                if (!m_valid) {
                    istd_EXCEPTION("isig: Runtime atom is not valid.");
                }
                return m_atomSize;
            }

            /**
             * Finds the position of the element name.
             * @return Column number corresponding to the element name
             * @retval -1 not found
             */
            inline int PositionOf(const std::string& a_elName) const
            {
                return FindElementPosition(m_names, a_elName);
            }

            inline static BaseType* BaseTypeCast(Atom* a_atom)
            {
                BaseType& bt((*a_atom)[0]);
                return &bt;
            }

            inline static const BaseType* BaseTypeCast(const Atom* a_atom)
            {
                const BaseType& bt((*a_atom)[0]);
                return &bt;
            }

            static void Dump(std::ostream& a_os, const Atom& a)
            {
                for (size_t i = 0; i < a.GetComponents(); ++i) {
                    a_os << a[i] << " ";
                }
            }

        private:
            bool    m_valid;
            size_t  m_components;
            size_t  m_groupSize;
            Names   m_names;
            size_t  m_atomSize;

            static_assert(
                std::is_arithmetic<BaseType>::value,
                "NUMERIC is not numerical type");
        };


    // Typedefs for supported base numeric types
    typedef SignalTraits<int8_t, RuntimeDefined>    SignalTraitsVarInt8;
    typedef SignalTraits<uint8_t, RuntimeDefined>   SignalTraitsVarUint8;
    typedef SignalTraits<int16_t, RuntimeDefined>   SignalTraitsVarInt16;
    typedef SignalTraits<uint16_t, RuntimeDefined>  SignalTraitsVarUint16;
    typedef SignalTraits<int32_t, RuntimeDefined>   SignalTraitsVarInt32;
    typedef SignalTraits<uint32_t, RuntimeDefined>  SignalTraitsVarUint32;
    typedef SignalTraits<int64_t, RuntimeDefined>   SignalTraitsVarInt64;
    typedef SignalTraits<uint64_t, RuntimeDefined>  SignalTraitsVarUint64;
    typedef SignalTraits<float, RuntimeDefined>     SignalTraitsVarFloat;
    typedef SignalTraits<double, RuntimeDefined>    SignalTraitsVarDouble;

    extern template class SignalTraits<int8_t, RuntimeDefined>;
    extern template class SignalTraits<uint8_t, RuntimeDefined>;
    extern template class SignalTraits<int16_t, RuntimeDefined>;
    extern template class SignalTraits<uint16_t, RuntimeDefined>;
    extern template class SignalTraits<int32_t, RuntimeDefined>;
    extern template class SignalTraits<uint32_t, RuntimeDefined>;
    extern template class SignalTraits<int64_t, RuntimeDefined>;
    extern template class SignalTraits<uint64_t, RuntimeDefined>;
    extern template class SignalTraits<float, RuntimeDefined>;
    extern template class SignalTraits<double, RuntimeDefined>;

#if 0
    /** (Maybe)
     * Specialisation for void, which treats composite type as a buffer of
     * uninterpreted bytes (octets).
     */
    template<void, typename COMPOSITE>
        class SignalTraits<void, COMPOSITE> {
        public:

        };
#endif

    extern template class SignalTraits<char, char>;
    typedef SignalTraits<char, char> SimpleCharTraits;

    extern template class SignalTraits<uint32_t, uint32_t>;
    typedef SignalTraits<uint32_t, uint32_t> SimpleUint32Traits;
    extern template class SignalTraits<uint64_t, uint64_t>;
    typedef SignalTraits<uint64_t, uint64_t> SimpleUint64Traits;

    extern template class SignalTraits<float, float>;
    typedef SignalTraits<float, float> SimpleFloatTraits;

} // namespace



#endif /* ISIG_SIGNAL_TRAITS_H_ */

/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: any_reference.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef ISTD_ANY_REFERENCE_H
#define ISTD_ANY_REFERENCE_H

#include <typeinfo>
#include "istd/exception.h"
#include <string>
#include <functional>

namespace istd {

	/**
	 * @addtogroup istd_anyref Reference to any type
	 * Type-safe reference-like class that is able to point to any type of variable.
	 * @note Similar to std::ref which is available in C++0x.
	 * @{
	 */

    /**
    * Reference to any type, defined at construction time.
    * Loosely modelled by CORBA Any and Boost::any.
    *
    * The class acts as an assignment proxy for any type of data and implements
    * operator= to assign value of identical type as the target variable.
    *
    * @code
    * int var1 = 30;
    * std::string var2 = "some text";
    *
    * // Store references to a map
    * map<std::string, istd::AnyReference> variables:
    * variables["INT"] = istd::AnyReference(var1);
    * variables["STR"] = istd::AnyReference(var2);
    *
    * // Modify variables through AnyReference.
    * variables["INT"] = 45;
    * variables["STR"] = std::string("Text assigned through AnyReference.");
    *
    * // var1 and var2 now have values, assigned through the AnyReference.
    *
    * // Copy the wrapped value (does runtime type checking)
    * int var3 = variables["INT"];
    *
    * // Get the reference to wrapped value (does runtime type checking)
    * // &var1c == &var1
    * int& var1c = variables["INT"];
    *
    * @endcode
    * AnyReference must point to a variable that has a life span during the whole
    * life span of AnyReference. The following will cause runtime exception:
    * @code
    * AnyReference ref;
    * {
    *   int var = 20;
    *   AnyReference ref2(var);
    *   ref = ref2;
    * }
    * ref = 30; // Crash: var was already deallocated.
    * @endcode
    */
    class AnyReference
    {
    public:
        inline AnyReference()
        : m_ptrHolder(NULL)
        {
        };

        virtual ~AnyReference() throw()
        {
            delete m_ptrHolder;
        };

        /** Copy constructor */
        inline AnyReference(const AnyReference &a_other)
        : m_ptrHolder(a_other.m_ptrHolder == NULL ? NULL : a_other.m_ptrHolder->Clone())
        {
        };

        /** Copy constructor */
        inline AnyReference(AnyReference &a_other)
        : m_ptrHolder(a_other.m_ptrHolder == NULL ? NULL : a_other.m_ptrHolder->Clone())
        {
        };

        /** Assignment operator for AnyReference */
        inline AnyReference& operator=(const AnyReference &a_other)
        {
            if (&a_other == this) {
                return *this;
            }
            delete m_ptrHolder;
            m_ptrHolder = NULL;
            if (a_other.m_ptrHolder != NULL) {
                m_ptrHolder = a_other.m_ptrHolder->Clone();
            }
            return *this;
        };

        /** Comparison operator for AnyReference */
        inline bool operator==(const AnyReference &a_other) const
        {
            return (&a_other == this);
        };

        /** Comparison operator for AnyReference */
        inline bool operator!=(const AnyReference &a_other) const
        {
            return (&a_other != this);
        };

        inline bool Empty() const
        {
            return m_ptrHolder == NULL;
        };

        inline const std::type_info& Type() const
        {
            return m_ptrHolder != NULL ? m_ptrHolder->Type() : typeid(void);
        };

        /** Templated constructor for a target type. */
        template<typename RT>
            explicit inline AnyReference(RT &a_var)
            : m_ptrHolder(new SpecificPtrHolder<RT>(a_var))
            {
            }

        /** Templated assignment operator for target type assigns
          * to referenced variable.
          * Type of T is verified with the wrapped reference type.
          * @throw istd::Exception when types don't match.
          */
        template<typename T>
            inline AnyReference& operator=(const T &a_val) throw (istd::Exception)
            {
                SpecificPtrHolder<T> *ptr;
                ptr = StrictCast<T>(__PRETTY_FUNCTION__);
                ptr->operator=(a_val);
                return *this;
            }

        /** Templated cast operator (T&) for reading values from AnyReference
          * Type of T is verified with the wrapped reference type.
          * @throw istd::Exception when types don't match.
          */
        template<typename T>
            inline operator T&() const
            {
                SpecificPtrHolder<T> *ptr;
                ptr = StrictCast<T>(__PRETTY_FUNCTION__);
                return *(ptr->m_pointer);
            }

        /** Comparison operator for target type.
          * Type of T is verified with the wrapped reference type.
          * @note Comparison compares memory addresses, not values.
          * @throw istd::Exception when types don't match.
          */
        template<typename T>
            inline bool operator==(const T &a_val) const
            {
                SpecificPtrHolder<T> *ptr;
                ptr = StrictCast<T>(__PRETTY_FUNCTION__);
                return ptr->m_pointer == &a_val;
            }

        /** Comparison operator for target type.
          * Type of T is verified with the wrapped reference type.
          * @throw istd::Exception when types don't match.
          */
        template<typename T>
            inline bool operator!=(const T &a_other) const
            {
                return !(*this == a_other);
            }

        /**
          * Private class of AnyReference.
          * Reference is converted to a pointer, which is controlled
          * by a templated class (SpecificPtrHolder), inherited from PtrHolder.
          * PtrHolder does type verification.
          */
        class PtrHolder
        {
            public:
                virtual ~PtrHolder() { };

                virtual const std::type_info& Type() const = 0;
                virtual PtrHolder* Clone() const = 0;
        };

        /** Private class of AnyReference */
        template<typename RT>
            class SpecificPtrHolder : public PtrHolder
            {
                friend class AnyReference;
                public:
                    inline SpecificPtrHolder(RT &a_var) : m_pointer(&a_var) { };

                    inline SpecificPtrHolder& operator=(const RT &a_var)
                        { *m_pointer = a_var; return *this; };

                    inline virtual const std::type_info& Type() const
                        { return typeid(RT); };

                    inline virtual PtrHolder* Clone() const
                        { return new SpecificPtrHolder(*m_pointer); };

                private:
                    /** Pointer to the variable, referenced to by AnyReference */
                    RT *m_pointer;
            };

    private:
        PtrHolder *m_ptrHolder;

        static void ThrowTypeException(
            const std::string& a_where,
            const std::type_info& a_expected,
            const std::type_info& a_actual);

        /** Private function to cast SpecificPtrHolder of referenced type
         * to SpecificPtrHolder of target type.
         * Verifies whether anything is wrapped and checks types.
         * @throw istd::Exception when types don't match or nothing is wrapped.
         */
        template <class T>
            SpecificPtrHolder<T>* StrictCast(const char* a_fcn) const
            {
                if (m_ptrHolder == NULL) {
                    throw istd::Exception("AnyReference: nothing wrapped.");
                }
                if (typeid(T) != m_ptrHolder->Type()) {
                    ThrowTypeException(a_fcn, m_ptrHolder->Type(), typeid(T));
                }
                SpecificPtrHolder<T> *ptr;
                ptr = dynamic_cast< SpecificPtrHolder<T> * >(m_ptrHolder);
                if (ptr == NULL) {
                    // Types differ in const or volatile only

                    ThrowTypeException(a_fcn, m_ptrHolder->Type(), typeid(T));
                }
                return ptr;
            }
    };
    /**@}*/

} // namespace


#endif  //ISTD_ANY_REFERENCE_H


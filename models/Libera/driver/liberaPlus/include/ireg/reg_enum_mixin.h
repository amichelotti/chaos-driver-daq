/*
 * Copyright (c) 2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: reg_enum_mixin.h 19990 2014-01-07 09:35:55Z damijan.skvarc $
 */

#ifndef REG_ENUM_MIXIN_H_
#define REG_ENUM_MIXIN_H_

#include "istd/trace.h"
#include "istd/log.h"
#include "istd/enum_cast.h"
#include "ireg/declarations.h"

#include <type_traits>

namespace ireg {

    template <typename ENUM, class BASE>
        /**
         * Mixin template, used to create registry classes that support enums.
         *
         * @see istd::EnumCast
         */
        class RegEnumMixin: public virtual BASE {
        public:
            typedef BASE    BaseNode;
            typedef ENUM    EnumType;

            static_assert(
                std::is_enum<EnumType>::value,
                "ENUM is expected to be an enum");

            virtual ~RegEnumMixin()
            {

            }

            virtual void Set(
                const EnumType& a_value,
                const size_t a_pos = 0,
                const Path& rp = c_noRelPath)
            {
                int64_t val = static_cast<int64_t>(a_value);
                BaseNode::Set(val, a_pos, rp);
            }

            virtual void Set(
                const std::vector<std::string> &a_val,
                size_t p = 0,
                const Path& rp = c_noRelPath)
            {
                std::vector<int64_t> v(a_val.size());
                for (size_t i(0); i < a_val.size(); ++i) {
                    v[i] = istd::EnumCast<EnumType>(a_val[i]);
                }
                BaseNode::Set(v, p, rp);
            }

            void Get(
                EnumType& a_value,
                const size_t a_pos = 0,
                const Path& rp = c_noRelPath) const
            {
                int64_t val;
                BaseNode::GetValue(val, a_pos);
                a_value = static_cast<EnumType>(val);
            }

            virtual NodeValType_e OnGetValueType(const Path& a_relPath = c_noRelPath) const
            {
                return eNvEnumeration;
            }

            bool OnGetDomainValues(std::vector<std::string> &a_values,
                                   const Path& a_relPath) const
            {
                for (size_t idx = 0; idx < istd::EnumMap<EnumType>::Size(); ++idx) {
                    a_values.push_back(istd::EnumMap<EnumType>::At(idx));
                }
                return true;
            }

        protected:
            /**
             * Constructor passes arguments to base class, which can be for
             * example RegValueNode, RegReferenceNode, RegFcnNode.
             *
             * Ctors of each of base classes can have different arguments and
             * therefore we have to use variadic template arguments for the
             * ctor of mixin.
             */
            template <class... ARGS>
                RegEnumMixin(ARGS&& ...a_args) : BaseNode(a_args...)
                {
                }

            /**
             * Get the enum value as int64.
             * @note Calls _OnGet(...)
             */
            virtual size_t OnGet(int64_t *a_out, size_t a_pos, size_t a_size) const
            {
                return _OnGet(a_out, a_pos, a_size);
            }

            /**
             * Get the enum value as int32.
             * @note Calls _OnGet(...)
             */
            virtual size_t OnGet(int32_t *a_out, size_t a_pos, size_t a_size) const
            {
                size_t len = _OnGet(a_out, a_pos, a_size);

                if (len > 0) {
                    for (size_t i = 0; i < len; ++i) {
                        // TODO: Useless check, can't be true because of
                        // value domain of int32_t.
                        if (a_out[i] > INT_MAX || a_out[i] < INT_MIN) {
                            istd_EXCEPTION("Value " << a_out[i] << " is out of int32 range.");
                        }
                    }
                }
                return len;
            }

            /**
             * Get the enum value as string.
             * @see istd::EnumCast
             * @note Calls _OnGet(...)
             */
            virtual size_t OnGet(std::string* a_out, size_t a_pos, size_t a_size) const
            {
                size_t s = _OnGet(a_out, a_pos, a_size);
                return s;
            }

            /**
             * Set the enum value as int64.
             */
            virtual void OnSet(const int64_t* a_in, size_t a_pos, size_t a_size)
            {
                _OnSet(a_in, a_pos, a_size);
            }

            /**
             * Set the enum value as int32.
             */
            virtual void OnSet(const int32_t* a_in, size_t a_pos, size_t a_size)
            {
               _OnSet(a_in, a_pos, a_size);
            }

            /**
             * Set the enum value as string.
             * @see istd::EnumCast
             */
            virtual void OnSet(const std::string* a_in, size_t a_pos, size_t a_size)
            {
                _OnSet(a_in, a_pos, a_size);
            }

        private:

            /**
             * Get the enum value for general type.
             */
            template <class T>
                size_t _OnGet(T *a_out, size_t a_pos, size_t a_size) const
                {
                    std::unique_ptr<EnumType[]> pArray(new EnumType[a_size]);
                    size_t len = BaseNode::OnGet(pArray.get(), a_pos, a_size);
                    for (size_t i = 0; i < len; ++i) {
                        int64_t v = pArray[i];
                        ConvertEnum(v, a_out[i]);
                    }
                    return len;
                }

            /**
             * Sets the enum value for general type.
             */
            template <class T>
                void _OnSet(const T* a_in, size_t a_pos, size_t a_size)
                {
                    std::unique_ptr<EnumType[]> pArray(new EnumType[a_size]);
                    for (size_t i = 0; i < a_size; ++i) {
                        pArray[i] = istd::EnumCast<EnumType>(a_in[i]);
                    }
                    BaseNode::OnSet(pArray.get(), a_pos, a_size);
                }

            /**
             * Converts and verifies the enum value
             */
            inline void ConvertEnum(const int64_t &a_in, int64_t& a_out) const
            {
                a_out = istd::EnumCast<EnumType>(a_in);
            }
            inline void ConvertEnum(const int64_t &a_in, int32_t& a_out) const
            {
                a_out = istd::EnumCast<EnumType>(a_in);
            }
            inline void ConvertEnum(const int64_t &a_in, std::string& a_out) const
            {
                EnumType ev = static_cast<EnumType>(a_in);
                a_out = istd::EnumCast<EnumType>(ev);
            }

        };
}

#endif /* REG_ENUM_MIXIN_H_ */

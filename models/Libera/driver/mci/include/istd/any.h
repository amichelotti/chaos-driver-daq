/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: any.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef ISTD_ANY_H
#define ISTD_ANY_H

#include <cstdint>
#include <string>
#include <vector>
#include <typeinfo>
#include <boost/any.hpp>

#include <istd/exception.h>

namespace istd {

    /**
     * @addtogroup ireg_any any type variable
     * Type-safe class that is able to hold different value type, which are supported by ireg registry
     * @note Encapsulates boost::any - only adds friendlier cast operators.
     * @{
     */

    class Any {
    public:

        Any(){}

        Any(bool a_val)         { m_value = a_val; }
        Any(int32_t a_val)      { m_value = a_val; }
        Any(uint32_t a_val)     { m_value = a_val; }
        Any(int64_t a_val)      { m_value = a_val; }
        Any(uint64_t a_val)     { m_value = a_val; }
        Any(double a_val)       { m_value = a_val; }
        Any(float a_val)        { m_value = a_val; }
        Any(std::string a_val)  { m_value = a_val; }

        Any(const std::vector<bool> &a_val)         { m_value = a_val; }
        Any(const std::vector<int32_t> &a_val)      { m_value = a_val; }
        Any(const std::vector<uint32_t> &a_val)     { m_value = a_val; }
        Any(const std::vector<int64_t> &a_val)      { m_value = a_val; }
        Any(const std::vector<uint64_t> &a_val)     { m_value = a_val; }
        Any(const std::vector<double> &a_val)       { m_value = a_val; }
        Any(const std::vector<float> &a_val)        { m_value = a_val; }
        Any(const std::vector<std::string> &a_val)  { m_value = a_val; }

        operator bool() const                      { return Get<bool>();        }
        operator int32_t() const                   { return Get<int32_t>();     }
        operator uint32_t() const                  { return Get<uint32_t>();    }
        operator int64_t() const                   { return Get<int64_t>();     }
        operator uint64_t() const                  { return Get<uint64_t>();    }
        operator double() const                    { return Get<double>();      }
        operator float() const                     { return Get<float>();       }
        operator std::string() const               { return Get<std::string>(); }

        operator std::vector<bool>() const         { return Get<std::vector<bool>>();     }
        operator std::vector<int32_t>() const      { return Get<std::vector<int32_t>>();  }
        operator std::vector<uint32_t>() const     { return Get<std::vector<uint32_t>>(); }
        operator std::vector<int64_t>() const      { return Get<std::vector<int64_t>>();  }
        operator std::vector<uint64_t>() const     { return Get<std::vector<uint64_t>>(); }
        operator std::vector<double>() const       { return Get<std::vector<double>>();   }
        operator std::vector<float>() const        { return Get<std::vector<float>>();    }
        operator std::vector<std::string>() const  { return Get<std::vector<std::string>>(); }

        Any& operator = (const bool& a_val)        { Set<bool>(a_val);     return *this; }
        Any& operator = (const int32_t& a_val)     { Set<int32_t>(a_val);  return *this; }
        Any& operator = (const uint32_t& a_val)    { Set<uint32_t>(a_val); return *this; }
        Any& operator = (const int64_t& a_val)     { Set<int64_t>(a_val);  return *this; }
        Any& operator = (const uint64_t& a_val)    { Set<uint64_t>(a_val); return *this; }
        Any& operator = (const double& a_val)      { Set<double>(a_val);   return *this; }
        Any& operator = (const float& a_val)       { Set<float>(a_val);    return *this; }
        Any& operator = (const std::string& a_val) { Set<std::string>(a_val); return *this; }

        const std::type_info& Type() const         { return m_value.type(); }

        /**
         * Return number of elements inside any. When any contains vector this method
         * returns number of elements inside vector otherwise returns 1
         */
        size_t Size() const;

        /**
         * Returns true, when Any contains vector
         */
        bool IsArray() const;

        void Print(std::ostream &a_os) const;

    private:

        template <typename T>
            T Get() const {
                try {
                    return boost::any_cast<T>(m_value);
                }
                catch (...) {
                    throw istd::Exception("Invalid value type");
                }
            }

        template <typename T>
            Any& Set(const T& a_val) {
                if (Type() != typeid(a_val) && !m_value.empty()) {
                    throw istd::Exception("Invalid value type");
                }
                try {
                    m_value = a_val;
                    return *this;
                }
                catch (...) {
                    throw istd::Exception("Invalid assign operation");
                }
            }

        template <class T>
            void PrintArray (std::ostream &a_os) const;

        template <class T>
            size_t GetSize() const;

        boost::any m_value;
    };


    /**@}*/

    std::ostream& operator << (std::ostream &a_os, const istd::Any &a_data);

}  // namespace

#endif /* ISTD_ANY_H */

/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: enum_cast.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */


#ifndef ENUM_CAST_H_
#define ENUM_CAST_H_

#include <map>
#include <cstring>
#include <boost/lexical_cast.hpp>

#include "istd/exception.h"

namespace istd {

    struct BimapStrCmp {
        bool operator() (const char* ls, const char* rs) const {
            return strcmp(ls, rs) < 0;
        }
    };

    typedef std::map<const int, const char*> EnumItems;

    /**
     * BiMap template class handles bidirectional indexing into an array
     * of values, holding enumeration values and its string representative.
     *
     * this array should be expressed in a form like for an example:
     *
     * @verbatim
     *  template <>
     *  const istd::EnumMap<istd::TraceLevel>::Items
     *      istd::EnumMap<istd::TraceLevel>::items =
     *          {{istd::eTrcOff,     "Off"},
     *           {istd::eTrcLow,     "Low"},
     *           {istd::eTrcMed,     "Medium"},
     *           {istd::eTrcHigh,    "High"},
     *           {istd::eTrcDetail,  "Detail"}};
     * @endverbatim
     *
     * BiMap class assures that both values within this structure
     * are indexed, so that accompanied value can be efficiently found with
     * no performance payload.
     */
    template <class T>
        class BiMap {
        public:
            typedef std::pair<const T, const char*> ValueType;

            BiMap(std::initializer_list<ValueType> items) : m_ids(items)
            {
                for (auto it = items.begin(); it != items.end(); ++it) {
                    m_names.insert (std::pair<const char*,T>(it->second, it->first) );
                    m_items.insert (std::pair<int, const char*>(static_cast<int>(it->first), it->second));
                }

                if (m_names.size() != m_ids.size()) {
                    throw istd::Exception("invalid enum description table");
                }
            }

            /**
             * @return number of different enums
             */
            inline std::size_t Size() const
            {
                return m_ids.size();
            }

            /**
             * @param idx
             * @return string representation of the enum at the given index
             */
            inline const char* At(const std::size_t idx) const
            {
                size_t id = idx;
                for (auto iter = m_ids.begin(); iter != m_ids.end(); ++iter) {
                    if (id == 0) {
                        return iter->second;
                    }
                    --id;
                }
                throw istd::Exception("index out of range");
            }

            /**
             * it is not necessary the string representatives depict all
             * possible values of supported enum declarations. Operator () is
             * used to find if the specified id has the accompanied string
             * description. If not the exception is thrown.
             * @param id
             * @return
             * @throw
             */
            inline T operator () (T id) const
            {
                auto v = m_ids.find(id);
                if (v == m_ids.end()) {
                    throw istd::Exception("invalid value");
                }
                return v->first;
            }

            inline bool IsValid(const char* name) const
            {
                auto v = m_names.find(name);
                return (v != m_names.end());
            }

            inline bool IsValid(T id) const
            {
                auto v = m_ids.find(id);
                return (v != m_ids.end());
            }

            /**
             * the following [] operator is used to get the string description
             * of the specified enum value. Exception is thrown, if the
             * specified enum value is not lexically described.
             * @param id
             * @return
             */
            inline const char* operator [] (T id) const
            {
                auto v = m_ids.find(id);
                if (v == m_ids.end()) {
                    throw istd::Exception("invalid value");
                }
                return v->second;
            }

            /**
             * the following [] operator is used to get the enum value, based on
             * registered string description
             * @param a_name
             * @return
             */
            inline const T operator [] (const char* a_name) const
            {
                auto v = m_names.find(a_name);
                if (v == m_names.end()) {
                    /* the enum string description is not found, but hey, hey,    */
                    /* maybe a string does not hold the enum string description,  */
                    /* but it holds the string representative of enum value       */
                    /* itself. try to convert string into enum value and find it  */
                    /* among registered descriptions                              */
                    try {
                        int64_t i = boost::lexical_cast<int64_t>(a_name);
                        T e = static_cast<T>(i);
                        auto vId = m_ids.find(e);
                        if (vId == m_ids.end()) {
                            throw istd::Exception("Invalid enum value");
                        }
                        return vId->first;
                    }
                    catch (const boost::bad_lexical_cast &e) {
                        throw istd::Exception("Invalid integer value");
                    }
                }
                return v->second;
            }

            const EnumItems *Map() const
            {
                return &m_items;
            }

        private:
            std::map<const T, const char*>          m_ids;
            std::map<const char*, T, BimapStrCmp>   m_names;
            EnumItems                               m_items;
        };

    /**
     * EnumMap template class wraps EnumCast<T> functions to data
     * containers, which are used for vice versa mapping between enum values
     * and their string representatives.
     */
    template <class T>
        class EnumMap {
        public:
           typedef BiMap<T> Items;

           // Disabled functions
           EnumMap() = delete;
           ~EnumMap() = delete;

           static const char* ToString(const T a_val)
           {
               return items[a_val];
           }

           static inline T ToEnum(const std::string &a_val)
           {
               return items[a_val.c_str()];
           }

           static inline T ToEnum(int32_t a_val)
           {
               T val = static_cast<T>(a_val);
               return items(val);
           }

           static inline T ToEnum(int64_t a_val)
           {
               T val = static_cast<T>(a_val);
               return items(val);
           }

           static inline bool IsValid(const std::string& a_val)
           {
               return items.IsValid(a_val.c_str());
           }

           static inline bool IsValid(int64_t a_val)
           {
               T val = static_cast<T>(a_val);
               return items.IsValid(val);
           }

           static inline bool IsValid(int32_t a_val)
           {
               T val = static_cast<T>(a_val);
               return items.IsValid(val);
           }

           static inline std::size_t Size()
           {
               return items.Size();
           }

           static inline const char* At(const std::size_t a_idx)
           {
               return items.At(a_idx);
           }

           static const EnumItems *Map()
           {
               return items.Map();
           }

        private:
           static const Items items;
        };

    /**
     * hey guys, finally we come to EnumCast functions, which are intended
     * to be used from the outside world.
     *
     * @note In order to make this functions to work, each T typename must
     *       be equipped with appropriate data instance (istd::EnumMap)
     *       which is usually located in EnumCast.cpp file
     *
     * @see istd::EnumMap
     *
     * @param a_val
     * @return
     */
    template<typename T>
        inline const char* EnumCast(T a_val)
        {
            return istd::EnumMap<T>::ToString(a_val);
        }

    template<typename T>
        inline T EnumCast(const std::string &a_val)
        {
            return istd::EnumMap<T>::ToEnum(a_val);
        }

    template<typename T>
        inline T EnumCast(int32_t a_val)
        {
            T val = static_cast<T>(a_val);
            return istd::EnumMap<T>::ToEnum(val);
        }

    template<typename T>
        inline T EnumCast(int64_t a_val)
        {
            T val = static_cast<T>(a_val);
            return istd::EnumMap<T>::ToEnum(val);
        }

    template<typename T>
        inline T EnumCast(uint32_t a_val)
        {
            T val = static_cast<T>(a_val);
            return istd::EnumMap<T>::ToEnum(val);
        }

    template<typename T>
        inline T EnumCast(uint64_t a_val)
        {
            T val = static_cast<T>(a_val);
            return istd::EnumMap<T>::ToEnum(val);
        }

    template<typename T>
        inline bool ValidEnumCast(const std::string &a_val)
        {
            return istd::EnumMap<T>::IsValid(a_val);
        }

    template<typename T>
        inline bool ValidEnumCast(int64_t a_val)
        {
            return istd::EnumMap<T>::IsValid(a_val);
        }

    template<typename T>
        inline bool ValidEnumCast(int32_t a_val)
        {
            return istd::EnumMap<T>::IsValid(a_val);
        }

    template<typename T>
        inline bool ValidEnumCast(uint64_t a_val)
        {
            return istd::EnumMap<T>::IsValid(a_val);
        }

    template<typename T>
        inline bool ValidEnumCast(uint32_t a_val)
        {
            return istd::EnumMap<T>::IsValid(a_val);
        }
}

#endif /* ENUM_CAST_H_ */

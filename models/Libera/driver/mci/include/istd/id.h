/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: id.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef ISTD_ID_H
#define ISTD_ID_H

#include <mutex>

namespace istd {

    template <typename V, class T>
        class Id {
        public:
            static Id<V,T> Generate()
            {
                Id id;
                return id;
            }

            inline operator T() const
            {
                return m_value;
            }

            T Value() const
            {
                return m_value;
            }

            Id<V,T> & operator = (const Id<V,T> & a_other)
            {
                m_value = a_other.m_value;
                return *this;
            }

            Id<V,T>(const Id<V,T> & a_other) : m_value (a_other.m_value)
            {

            }

          /*  Id<V,T>(const T& a_other) : m_value (a_other)
            {

            }*/

        private:

            Id()
            {
                std::lock_guard<std::mutex> l(m_lastValue_x);
                m_value = ++m_lastValue;
            }

            T                 m_value;
            static T          m_lastValue;
            static std::mutex m_lastValue_x;
        };

    template<typename V, class T> T Id<V,T>::m_lastValue = 0;
    template<typename V, class T> std::mutex Id<V,T>::m_lastValue_x;

    template <typename V, class T>
        bool operator< (const Id<V,T>& lhs, const Id<V,T>& rhs)
        {
            return lhs.Value() < rhs.Value();
        }

} // namespace

#endif /* ISTD_ID_H */

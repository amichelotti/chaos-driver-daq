/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: libera_machine_time.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef ISIG_LIBERA_MACHINE_TIME_H
#define ISIG_LIBERA_MACHINE_TIME_H

#include <cstdint>
#include <iostream>

#include "isig/declarations.h"

namespace isig {

    class LiberaMachineTime {
    public:

        LiberaMachineTime(const uint64_t &a_time = 0);

        LiberaMachineTime& operator =(const LiberaMachineTime& a_other);
        LiberaMachineTime& operator =(const uint64_t& a_other);

        inline LiberaMachineTime operator +(
                const LiberaMachineTime& a_other) const
        {
            return m_time + a_other.m_time;
        }

        inline LiberaMachineTime operator -(
                const LiberaMachineTime& a_other) const
        {
            return m_time - a_other.m_time;
        }

        LiberaMachineTime operator +(const int64_t& a_other) const;
        LiberaMachineTime operator -(const int64_t& a_other) const;
        LiberaMachineTime operator -(const uint64_t& a_other) const;

        inline LiberaMachineTime operator +(const uint64_t& a_other) const
        {
            return m_time + a_other;
        }

        inline LiberaMachineTime operator +(const int32_t& a_other) const
        {
            return *this + static_cast<int64_t>(a_other);
        }

        inline LiberaMachineTime operator -(const int32_t& a_other) const
        {
            return *this - static_cast<int64_t>(a_other);
        }

        inline LiberaMachineTime operator +(const uint32_t& a_other) const
        {
            return *this + static_cast<uint64_t>(a_other);
        }

        inline LiberaMachineTime operator -(const uint32_t& a_other) const
        {
            return *this - static_cast<uint64_t>(a_other);
        }

        inline bool operator >(const LiberaMachineTime& a_other) const
        {
            return m_time > a_other.m_time;
        }

        inline bool operator !=(const LiberaMachineTime& a_other) const
        {
            return m_time != a_other.m_time;
        }

        inline bool operator >=(const LiberaMachineTime& a_other) const
        {
            return m_time >= a_other.m_time;
        }

        inline bool operator <(const LiberaMachineTime& a_other) const
        {
            return m_time < a_other.m_time;
        }

        inline bool operator <=(const LiberaMachineTime& a_other) const
        {
            return m_time <= a_other.m_time;
        }

        bool operator==(const LiberaMachineTime& a_other) const;

        const uint64_t& Value() const;

    private:
        uint64_t m_time;
    } DEPRECATED_DOD;

    std::ostream& operator<<(std::ostream &a_out, const LiberaMachineTime &a_time);

} // namespace

#endif /* ISIG_LIBERA_MACHINE_TIME_H */

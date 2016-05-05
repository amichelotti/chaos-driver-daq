/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: time.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */


#ifndef ISTD_TIME_H
#define ISTD_TIME_H

#include <chrono>
#include <string>

namespace istd {

	/**
	 * @addtogroup istd_time Date and time
	 * Classes and functions to acquire time and date to nanosecond
	 * precision and perform basic operations.
	 * Note: Don't throw from time class since its used to log exceptions.
	 * @{
	 */

    const long c_nsecPerSec   (1000000000L);
    const long c_msecPerSec   (1000L);
    const long c_nsecPerMsec  (c_nsecPerSec/c_msecPerSec);

    class Time
    {
    public:
        typedef std::chrono::duration<double> DurationDouble;

        // Formatting flags
        enum Flags_e {
            eNsec   = 1 << 0,
            eTime   = 1 << 1,
            eDate   = 1 << 2,
            eUTC    = 1 << 3
        };

        static const int c_dateTime    = eDate | eTime;
        static const int c_timeNsec    = eTime | eNsec;
        static const int c_all         = eDate | eTime | eNsec;

        static const int c_defaultFlags = c_timeNsec;
        static const int c_dtimeFlags = c_timeNsec | eUTC;

        /** Current time in specified format */
        explicit Time(int a_flags = c_defaultFlags);

        /** Nanosecond precision time in default format */
        explicit Time(unsigned long a_sec, unsigned long a_nsec = 0);

        /** Second (time_t) time in default format */
        explicit Time(uint32_t a_timet);

        Time(const Time& a_other);
        Time& operator=(const Time& a_other);

        ~Time() {};

        static Time Now(int a_flags = c_defaultFlags);

        bool IsEpoch() const;

        unsigned long Sec() const
            { return m_time.tv_sec; };

        unsigned long NSec() const
            { return m_time.tv_nsec; };

        /**
         * Time in seconds from Epoch, represented in double.
         */
        operator DurationDouble() const;

        void UpdateToCurrent() const;

        Time& SetDisplayFlags(int a_flags)
        {
            m_flags = a_flags; m_dirty = true;
            return *this;
        };

        Time& operator+=( const Time & a_rhs );
        Time& operator-=( const Time & a_rhs );
        Time& operator/=( unsigned long a_rhs );

        const char* GetTextTime() const
            {
                if (m_dirty) {
                    ToText();
                }
                return m_textTime;
            };

    private:
        static const int c_textTimeLen = 30;

        void ToText() const;

        int                     m_flags;
        mutable timespec        m_time;

        // Textual representation with lazy evaluation
        mutable bool            m_dirty;
        mutable char            m_textTime[c_textTimeLen];

    };
    /** Validates input string and create a new Time object.
     * @throw Exception
    */
    Time CreateTime(const std::string& a_textual);


    const Time operator+( Time lhs, const Time & rhs );
    const Time operator-( Time lhs, const Time & rhs );
    const Time operator/( Time lhs, unsigned long rhs );
    bool operator==( const Time & lhs, const Time & rhs );
    inline bool operator!=( const Time & lhs, const Time & rhs )
    {
        return !(lhs == rhs);
    }

    // For comparison of equivalence
    bool operator<( const Time & lhs, const Time & rhs );

    inline bool operator>( const Time & lhs, const Time & rhs )
    {
        return (!(lhs == rhs) && !(lhs < rhs) );
    }

    inline bool operator <= ( const Time & lhs, const Time & rhs )
    {
        return ((lhs < rhs) || (lhs == rhs));
    }

    inline bool operator >= ( const Time & lhs, const Time & rhs )
    {
        return ((lhs > rhs) || (lhs == rhs));
    }

    std::ostream& operator<< (std::ostream& a_stream, const Time& a_time);

} // namespace
/**@}*/

#endif // ISTD_TIME_H


/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: string.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef ISTD_STRING_H
#define ISTD_STRING_H

#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <boost/lexical_cast.hpp>

#include "istd/system.h"
#include "istd/exception.h"

namespace istd {

    /**
     * @addtogroup istd_util String and STL utilities
     * Manipulation of strings, conversion from/to numbers, useful functors
     * for std::for_each.
     * @see istd_system
     * @{
     */

    typedef std::vector<std::string> StrTokens;

    /** Splits the string into multiple strings as defined with a_delim.  */
    StrTokens TokenizeString(const char *a_str, const char* a_delim = " \t\n");
    inline StrTokens TokenizeString(const std::string &a_str, const char *a_delim = " \t\n")
    {
        return istd::TokenizeString(a_str.c_str(), a_delim);
    }

    /** Trims leading and trailing non-alphanumeric characters*/
    std::string TrimString(const char *a_str);
    inline std::string TrimString(const std::string& a_str)
    {
        return TrimString(a_str.c_str());
    }

    /** Converts vector of strings into one string, delimited by a_demiter */
    std::string StringVecIntoString(std::vector<std::string> a_sVec,
                                    const char *a_delimiter);

    /** Converts any type/class that has operator<< to std::string */
    template <class T>
        inline std::string ToString (const T& t)
        {
            std::stringstream ss;
            ss << t;
            return ss.str();
        }

    /** Converts any type/class that has operator<< to std::string */
    template <>
        inline std::string ToString (const double& t)
        {
            std::stringstream ss;
            ss << std::setprecision(15) << t;
            return ss.str();
        }

    /** Converts any type/class that has operator<< to std::string */
    template <>
        inline std::string ToString (const float& t)
        {
            std::stringstream ss;
            ss << std::setprecision(6) << t;
            return ss.str();
        }

    /** Converts any type/class that has operator<< to std::string */
    template <>
        inline std::string ToString (const bool& b)
        {
            return ((b) ? "true" : "false");
        }


    bool ToBool(const std::string &a_string);

    /**
        Hexadecimal dump of memory. By default, 24 bytes per line without text.
        Optional arguments:
        @param a_width Number of bytes per line - max 24 (18 if a_txt is true)
        @param a_txt Do text dump beside hex dump
     */
    std::string HexDump(const void *a_buf, unsigned int a_size, unsigned int a_width = 24, bool a_txt = false);

    /**
        Convert std::dec, std::hex, std::oct, std::fixes, std::scientific string to numerical class T.
        Arguments:
        @param a_t Resulting variable with converted string value.
        @param a_s Input string.
        @parama a_f Conversion function (std::dec, std::hex, std::oct, std::fixes, std::scientific).
     */
    template <class T>
        inline bool FromString(T& a_t, const std::string& a_s, std::ios_base& (*a_f)(std::ios_base&))
        {
            static_assert(
                std::is_arithmetic<T>::value, "T is not numerical type");

            std::istringstream iss(a_s);
            return !(iss >> a_f >> a_t).fail();
        }


    /**
     * Convert string in hex format into unsigned integral type
     * (uchar, ushort, uint, ulong, ulong long)
     * @param a_str String to convert (0xFFA0)
     * @return converted value
     */
    template <class T>
    typename std::enable_if<!std::is_signed<T>::value, T>::type HexTo(const std::string &a_str)
    {
       T val;
        std::istringstream sstr(a_str);
        if ((sstr >> std::hex >> val).fail()) {
            istd_EXCEPTION("Bad hex cast from " <<  a_str << " to " <<
                            istd::CppName(typeid(val)));
        }
        return val;
    }


    /**
     * Convert string in hex format into signed integral type
     * (char, short, int, long, long long)
     * @param a_str String to convert (0xFFA0)
     * @return converted value
     */
    template <class T>
    typename std::enable_if<std::is_signed<T>::value, T>::type HexTo(const std::string &a_str)
    {
        T val;
        val = HexTo<typename std::make_unsigned<T>::type>(a_str);
        return val;
    }

    /**
     * Convert string to numerical type T.
     * implementation for non integral types
     * @param a_s String to convert
     * @return converted value
     */
    template <class T>
    typename std::enable_if<!std::is_integral<T>::value, T>::type
        inline FromString(const std::string& a_s)
    {
        try {
            return boost::lexical_cast<T>(a_s);
        }
        catch(const boost::bad_lexical_cast &) {
            T val;
            istd_EXCEPTION(
                "Bad cast from " <<  a_s << " to " << istd::CppName(typeid(val)));
        }
    }


    /**
     * Convert string to numerical type T. Supports (decimal and hex notation)
     * implementation for integral types (all sort of integers and char)
     * @param a_s String to convert
     * @return converted value
     */
    template <class T>
    typename std::enable_if<std::is_integral<T>::value, T>::type
        inline  FromString(const std::string& a_s)
        {
            T val;

            // TODO: Use strtol/strtoul/ instead
            // if hex prefix found, try to convert through HexTo function
            if (a_s.size() > 2 && a_s.compare(0,2,"0x") == 0) {
                return HexTo<T>(a_s);
            }

            if (typeid(bool) == typeid(val)) {
                return ToBool(a_s);
            }

            if (a_s.size() > 0 && std::is_unsigned<T>::value) {
                if (a_s[0] == '-') {
                    istd_EXCEPTION("Bad cast from " <<  a_s << " to " <<
                              istd::CppName(typeid(val)));
                }
            }

            try {
                return boost::lexical_cast<T>(a_s.c_str());
            }
            catch(const boost::bad_lexical_cast &) {
                istd_EXCEPTION("Bad cast from " <<  a_s << " to " <<
                          istd::CppName(typeid(val)));
            }
        }

    /**@}*/

} // namespace



#endif // ISTD_STRING_H

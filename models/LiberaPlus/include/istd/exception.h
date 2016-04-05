/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: exception.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */


#ifndef ISTD_EXCEPTION_H
#define ISTD_EXCEPTION_H

#include <exception>
#include <string>
#include <sstream>

namespace istd {

	/**
	 * @addtogroup istd_trace
	 * @{
	 */
    class Exception : public std::exception
    {
    public:
        Exception(const char* a_what, bool a_tracedump = true) throw();
        Exception(const std::string &a_what, bool a_tracedump = true) throw();
        Exception(std::ostringstream &a_what, bool a_tracedump = true) throw();
        virtual ~Exception() throw() {};

        char const *what() const throw();

    private:
        std::string m_what;
    };

    class ParameterNotFoundException : public istd::Exception
    {
       public:
           ParameterNotFoundException(const char* a_what, bool a_tracedump = true)
           : Exception(a_what, a_tracedump) {};
           ParameterNotFoundException(const std::string &a_what, bool a_tracedump = true)
           : Exception(a_what, a_tracedump) {};
           virtual ~ParameterNotFoundException() throw() {};
    };
    /**@}*/

    std::ostream& operator<< (std::ostream& a_stream, const istd::Exception& a_ie);

} // namespace

/**
 * A helper macro which throws an istd::Exception with the _msg message
 * which can be defined by using stringstream operator <<, for example
 * "Result of " << dividend << "/" << divisor << " is " << divident/divisor << "."
 * @param _msg The trace message in the form for printing to std::ostream
 */
#define istd_EXCEPTION(_msg) \
    { \
        std::ostringstream sstr; \
        sstr << _msg; \
        throw istd::Exception(sstr.str()); \
    }


#endif  //ISTD_EXCEPTION_H


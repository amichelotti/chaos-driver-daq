/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: ipmi_exception.h 8621 2010-09-14 05:54:19Z matej.kenda $
 */

#ifndef BMC_IPMI_EXCEPTION_H_
#define BMC_IPMI_EXCEPTION_H_

#include "istd/exception.h"
#include "istd/literals.h"

namespace bmc {

	/**
	 * @addtogroup bmc_connection
	 * @{
	 */
    class IpmiException : public istd::Exception {

    public:
        IpmiException(int a_code, const char* a_message, bool a_tracedump = true)
        : istd::Exception(IpmiException::CreateMessage(a_message, a_code), a_tracedump), m_code(a_code)
        {

        };

        virtual ~IpmiException() throw() {};

        int GetCode();

    private:
        static std::string CreateMessage(const char* a_message, int a_code);

        int m_code;
    };
    /**@}*/
}


#endif /* BMC_IPMI_EXCEPTION_H_ */

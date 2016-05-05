/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: system.h 14606 2011-12-21 12:43:41Z matej.kenda $
 */


#ifndef ISTD_SYSTEM_H
#define ISTD_SYSTEM_H

#include <string>
#include <typeinfo>
#include <vector>

/**
 * @addtogroup istd_system System and compiler helpers
 * Getting program ID, thread ID, program name, stack trace, etc..
 * @see istd_util
 * @{
 */

//@{
/** Macros from Linux kernel sources to instruct the compiler
 * which of the if branches if more probable.
 */
#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)
//@}

/**@}*/


namespace istd {

    /**
     * @addtogroup istd_system
     * @{
     */

    static const int c_invalidFd(-1);

    typedef std::vector<std::string> ExecutionStack;

    unsigned long   GetTid();       ///< This thread ID
    unsigned long   GetPid();       ///< This process ID
    const char*     GetProgName();  ///< Name of the running process

    /** Convert cryptic C++ symbol name into human readable type name. */
    std::string     CppName(const char* a_symbolName);
    std::string     CppName(const std::type_info &a_typeInfo);

    /** Complete call stack at the execution point */
    ExecutionStack  GetExecutionStack(int a_maxDepth = 64);

    /** Name of current function, read from stack */
    std::string     ThisFncName();

    /**@}*/

}

#endif // ISTD_SYSTEM_H

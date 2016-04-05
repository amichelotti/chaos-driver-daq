/*
 * Copyright (c) 2008-2015 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: mci_trace.cpp 22034 2015-06-19 08:32:00Z damijan.skvarc $
 */


#include "istd/trace.h"                          /* tracing mechanism is separated from common mci header file  */
#include "mci/mci.h"


int main(int a_argc, char* a_argv[])
{
    istd::TraceInit("-");                         /* initialize tracing to standard output                          */
    istd::TraceStart(istd::eTrcDetail);           /* start tracing at certain trace level                           */
    istd::TraceSetStackDump(true);                /* in case of Exception the call stack is reported automatically  */
    istd::TraceDumpStack();                       /* call stack can be reported even manually                       */

    istd::TraceSetLevel(istd::eTrcLow);           /* tracing level can be modified any time                         */
    istd::TraceDumpStack();                       /* dumping call stack is automatically disabled when lowering     */
                                                  /* the trace level, thus nothing will be dumped here              */
    istd::TracePause();                           /* tracing can temporary be paused                                */
    istd::TraceContinue();                        /* ...and re-enabled afterwards                                   */

    istd_TRC(istd::eTrcLow, "That's all folks");  /* additional messages can be reported to trace output            */
}

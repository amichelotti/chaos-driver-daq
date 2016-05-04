/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: trace.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */


#ifndef ISTD_TRACE_H
#define ISTD_TRACE_H

#include <sstream>

#include "istd/tracefile.h"

namespace istd {

    /**
     * @addtogroup istd_trace
     * @{
     */
    void TraceInit(const std::string& a_fname = "", const std::string& a_dirname = "");
    void TraceStart(TraceLevel_e a_level);
    void TraceSetLevel(TraceLevel_e a_level);
    void TracePause();
    void TraceContinue();
    void TraceStop();
    void TraceDumpStack();
    bool IsTraceActive();
    TraceLevel_e TraceActiveLevel();
    /**@}*/

    /**
     * Execution tracing helper function used by macro istd_TRC.
     * Not intended to be used directly.
     */
    void TraceWrite(
        TraceLevel_e a_lvl,
        const char* const a_fnc,
        int a_line,
        const std::string& a_msg);

} // namespace

#include "tracefnc.h"

/**
 * @addtogroup istd_trace
 * @{
 */

/**
 * Macro to insert execution traces into the source code.
 * @param _lvl On which level should the message be displayed (istd::TraceLevel_e)
 * @param _msg The trace message in the form for printing to std::ostream
 */
#define istd_TRC(_lvl, _msg) \
    if ( istd::IsTraceActive() && _lvl <= istd::TraceActiveLevel() ) { \
        std::ostringstream _trc_sstr; \
        _trc_sstr << _msg; \
        istd::TraceWrite(_lvl, __PRETTY_FUNCTION__, __FILE__, __LINE__, _trc_sstr.str()); \
    }\

/**
 * Macro to instantiate function execution tracer using
 * full function signature __PRETTY_FUNCTION__ (can be long for templates).
 * Function tracer prints a log message when entering and exiting
 * a function (or scope) if the tracing is on level istd::eTrcHigh or above.
 */
#define istd_FTRC() \
    istd::TraceFnc _this_fnc(__PRETTY_FUNCTION__, __FILE__, __LINE__)

/**
 * Macro to instantiate function execution tracer using
 * function name __func__ without the name of the class.
 */
#define istd_FT() \
    istd::TraceFnc _this_fnc(__func__,__FILE__, __LINE__)

/**@}*/

namespace istd {

    /**
     * @param a_fname The name of the file where the traces are written.
     * @param a_dirname The directory where to put log files in
     *
     * Special file names:
     * - "-" or "": Standard output (screen)
     * - "+": Trace file name is generated in the form progname_pid.log
     */
    inline void TraceInit(const std::string& a_fname, const std::string& a_dirname)
    {
        TraceFile::GetInstance().Init(a_fname, a_dirname);
    }

    inline void TraceStart(TraceLevel_e a_level)
    {
        TraceFile::GetInstance().Start(a_level);
    }

    inline void TraceSetLevel(TraceLevel_e a_level)
    {
        TraceFile::GetInstance().SetLevel(a_level);
    }

    inline void TracePause()
    {
        TraceFile::GetInstance().Pause();
    }

    inline void TraceContinue()
    {
        TraceFile::GetInstance().Continue();
    }

    inline void TraceStop()
    {
        TraceFile::GetInstance().Stop();
    }

    inline void TraceDumpStack()
    {
        TraceFile::GetInstance().DumpStack();
    }

    inline bool IsTraceStackDumpEnabled()
    {
        bool enabled = false;
        (void)TraceFile::GetInstance().GetDumpStack(enabled);
        return enabled;
    }

    inline void TraceSetStackDump(bool a_enabled)
    {
        (void)TraceFile::GetInstance().SetDumpStack(a_enabled);
    }

    inline bool IsTraceActive()
    {
        return TraceFile::GetInstance().IsActive();
    }

    inline TraceLevel_e TraceActiveLevel()
    {
        TraceLevel_e l;

        TraceFile::GetInstance().GetLevel(l);
        return l;
    }

    inline void TraceWrite(
        TraceLevel_e a_lvl,
        const char* const a_fnc,
        const char *a_file, int a_line,
        const std::string& a_msg)
    {
        TraceFile::GetInstance().Trace(a_lvl, a_fnc, a_file, a_line, a_msg);
    }

} // namespace

#endif  //ISTD_TRACE_H


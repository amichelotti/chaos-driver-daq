/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: tracefile.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */


#ifndef ISTD_TRACEFILE_H
#define ISTD_TRACEFILE_H

#include <string>
#include <mutex>

#include "istd/logfile.h"
#include "istd/time.h"

namespace istd {

    /**
     * @addtogroup istd_trace
     * @{
     */
    enum TraceLevel_e {
        eTrcOff     = 0,
        eTrcLow     = 1,
        eTrcMed     = 2,
        eTrcHigh    = 3,
        eTrcDetail  = 4
    };
    /**@}*/

    class TraceFile : public LogFile {
    public:

        static TraceFile& GetInstance();
        static void Init(const std::string& a_fname, const std::string& a_dirname = LogFile::c_emptyStr);

        void Start(TraceLevel_e a_level);

        /* function declarations, compatible with ireg::RegEnumFncNode function declarations */
        bool SetLevel(const TraceLevel_e& level);
        bool GetLevel(TraceLevel_e& a_level) const;

        bool SetPrintFile(const bool& a_val);
        bool GetPrintFile(bool& a_val) const;

        bool SetPrintLine(const bool& a_val);
        bool GetPrintLine(bool& a_val) const;

        bool SetDumpStack(const bool& a_val);
        bool GetDumpStack(bool& a_val) const;

        void Pause();
        void Continue();
        void Stop();

        void DumpStack();

        bool IsActive() const
            { return m_active; };

        void Trace(TraceLevel_e a_lvl, const char* const a_fnc,  const char *a_file, int a_line, const std::string& a_text);

    protected:
        TraceFile();
        virtual ~TraceFile();
        TraceFile(const TraceFile&);
        TraceFile& operator=(const TraceFile&);

        std::string GeneratedTraceFileName();
        virtual std::string FileHeader() const;
        virtual std::string AppendBanner() const;
        virtual std::string FileTrailer() const;
        virtual std::string LineHeader() const;

    private:
        TraceLevel_e    m_level;
        bool            m_active;
        Time            m_time;
        std::string     m_fname;
        std::string     m_dirname;
        bool            m_printFile;
        bool            m_printLine;
        bool            m_dumpStack;
        std::mutex      m_mutex;
    };

} // namespace

#endif  //ISTD_TRACEFILE_H


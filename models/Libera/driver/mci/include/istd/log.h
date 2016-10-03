/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: log.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */


#ifndef ISTD_LOG_H_
#define ISTD_LOG_H_

#include <string>
#include <mutex>
#include <sstream>

#include "istd/trace.h"
#include "istd/logfile.h"
#include "istd/time.h"

namespace istd {

    class Log : protected LogFile {

    public:
        static Log& GetInstance();
        static bool Init(const std::string &a_fname);

        void Info(const std::string &a_message);
        void Warning(const std::string &a_message);
        void Error(const std::string &a_message);
        void Critical(const std::string &a_message);

        static bool m_enabled;

    protected:
        Log();
        virtual ~Log();

        std::string LineHeader() const;
        std::string FileHeader() const;
        void WriteMessage(const std::string &a_prefix, const std::string &a_message);

        // prefixes
        std::string m_info;
        std::string m_warning;
        std::string m_error;
        std::string m_critical;

        Time        m_time;
        std::mutex  m_write_x;
    };

    inline void LogCritical(const std::string &a_message) {
        istd_TRC(eTrcLow, a_message);
        if (Log::m_enabled) {
            Log::GetInstance().Critical(a_message);
        }
    }

    inline void LogError(const std::string &a_message) {
        istd_TRC(eTrcLow, a_message);
        if (Log::m_enabled) {
            Log::GetInstance().Error(a_message);
        }
    }

    inline void LogWarning(const std::string &a_message) {
        istd_TRC(eTrcMed, a_message);
        if (Log::m_enabled) {
            Log::GetInstance().Warning(a_message);
        }
    }

    inline void LogInfo(const std::string &a_message) {
        istd_TRC(eTrcHigh, a_message);
        if (Log::m_enabled) {
            Log::GetInstance().Info(a_message);
        }
    }
}

/**
 * @addtogroup istd_trace
 * @{
 */
#define istd_LOGc(_msg) \
    { \
        std::ostringstream sstr; \
        sstr << _msg; \
        istd::LogCritical(sstr.str()); \
    }\

#define istd_LOGe(_msg) \
    { \
        std::ostringstream sstr; \
        sstr << _msg; \
        istd::LogError(sstr.str()); \
    }\

#define istd_LOGw(_msg) \
    { \
        std::ostringstream sstr; \
        sstr << _msg; \
        istd::LogWarning(sstr.str()); \
    }\

#define istd_LOGi(_msg) \
    { \
        std::ostringstream sstr; \
        sstr << _msg; \
        istd::LogInfo(sstr.str()); \
    }\
/**@}*/

#endif /* ISTD_LOG_H_ */

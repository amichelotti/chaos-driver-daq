/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: logfile.h 10002 2010-12-03 08:21:50Z tomaz.juretic $
 */


#ifndef ISTD_LOGFILE_H
#define ISTD_LOGFILE_H

#include <string>

namespace istd {

    static const int c_invalidHandle(-1);

	/**
	 * @addtogroup istd_trace
	 * @{
	 */
    class LogFile {
    public:
        LogFile();
        virtual ~LogFile();

        static const std::string c_emptyStr;
        static const std::string c_emptyLine;

        bool Open();
        bool Open(const std::string &a_fileName, bool a_flushWrite = false);
        void Close();

        bool IsOpen() const
            { return m_handle != c_invalidHandle; };

        const std::string& GetFileName() const
            { return m_fileName; };

        void Write(const char* a_buf, size_t a_size);

        void Write(const std::string &a_os)
            { Write(a_os.data(), a_os.length() ); };

        bool WriteHeader(std::string &a_text);

        virtual std::string FileHeader() const
            { return c_emptyStr; };

        virtual std::string AppendBanner() const
            { return c_emptyStr; };

        virtual std::string FileTrailer() const
            { return c_emptyStr; };

        virtual std::string LineHeader() const
            { return c_emptyStr; };

        virtual std::string LineTrailer() const
            { return c_emptyStr; };

        virtual void   Error(int /* a_errorCode */)
            { Close(); };

        /* function declarations, compatible with ireg::RegEnumFncNode function declarations */
        bool SetPrintLineHeader(const bool& a_val);
        bool GetPrintLineHeader(bool& a_val) const;

        bool SetPrintLineTrailer(const bool& a_val);
        bool GetPrintLineTrailer(bool& a_val) const;

    protected:
        std::string m_fileName;

    private:
        bool    m_flushWrite;
        int     m_handle;
        bool    m_inErrorHandling;
        bool    m_printLineHeader;
        bool    m_printLineTrailer;

        void HandleError(int a_error);
    };
    /**@}*/

} // namespace

#endif  //ISTD_LOGFILE_H


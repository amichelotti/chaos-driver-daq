/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: verinfo.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */


#ifndef ISTD_VERINFO_H
#define ISTD_VERINFO_H

#include <string>

namespace istd {

	/**
	 * @addtogroup istd_trace
	 * @{
	 */
    class VerInfo {
    public:

        static VerInfo& GetInstance();
        static void Init(
            int a_maj, int a_min, int a_build, int a_revision,
            const char* a_label,
            const char* a_buildcompiler,
            const char* a_buildtime,
            const char* a_buildmachine,
            const char* a_builduser);

        static int Major();
        static int Minor();
        static int Build();
        static int Revision();
        static const std::string& Label();

        static const std::string& VersionString();

        static const std::string& BuildCompiler();
        static const std::string& BuildTime();
        static const std::string& BuildMachine();
        static const std::string& BuildUser();
        static void  Dump();

    protected:
        VerInfo();
        virtual ~VerInfo();
        VerInfo(const VerInfo&);
        VerInfo& operator=(const VerInfo&);

    private:
        int m_maj, m_min, m_build, m_revision;
        std::string m_label;

        std::string m_versionString;

        std::string m_buildcompiler;
        std::string m_buildtime;
        std::string m_buildmachine;
        std::string m_builduser;
    };

    inline const istd::VerInfo& VI()
    {
        return istd::VerInfo::GetInstance();
    }

    /**@}*/


    /**
     * @addtogroup bmc_streamHelpers
     * @{
     */
    std::ostream& operator<<(std::ostream& os, const VerInfo& vi);

} // namespace
/**@}*/

#endif  //ISTD_VERINFO_H


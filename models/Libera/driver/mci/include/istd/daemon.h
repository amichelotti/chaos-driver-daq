/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: daemon.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */


#ifndef ISTD_DAEMON_H
#define ISTD_DAEMON_H

#include <string>

namespace istd {
    /**
     * @addtogroup istd_system
     * @{
     */

    class Daemon {
        public:
            static Daemon& Instance();

            int Start(const std::string& a_pidfile);
            int Stop();

        private:
            Daemon();
            ~Daemon();

            std::string     m_pidfile;
            bool            m_started;
    };

    /**@}*/

}


#endif // ISTD_DAEMON_H

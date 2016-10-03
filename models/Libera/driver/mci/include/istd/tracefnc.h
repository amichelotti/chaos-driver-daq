/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: tracefnc.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */


#ifndef ISTD_TRACEFNC_H
#define ISTD_TRACEFNC_H


namespace istd {

    class TraceFnc {
    public:
        TraceFnc(const char* a_fnc, const char* a_file, int a_line);
        virtual ~TraceFnc();

    private:
        const char* m_fnc;
        const char* m_file;
        int         m_line;
    };


} // namespace

#endif  //ISTD_TRACEFNC_H

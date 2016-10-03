/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * LiberaSignalRepository is intended expose through the registry interface
 * repository of all data signals which are instantiated in the final application.
 * 
 * $Id: libera_signal_repository.h 18727 2013-03-05 12:55:25Z damijan.skvarc $
 */

#ifndef LIBERA_SIGNAL_REPOSITORY_H
#define LIBERA_SIGNAL_REPOSITORY_H

#include "istd/trace.h"
#include "libera_feature.h"
#include "ireg/declarations.h"

namespace iapp {


    /* class declaration */
    class LiberaSignalRepository : public LiberaFeature
    {
    public:
        LiberaSignalRepository(LiberaObject& a_parent, const std::string &a_name);

    protected:
        virtual  ~LiberaSignalRepository() {};

        /* prevent copy constructor and assignment, since we are dealing with singleton repository */
        LiberaSignalRepository(const LiberaSignalRepository& a_other) = delete;
        LiberaSignalRepository& operator = (const LiberaSignalRepository& a_other) = delete;

    protected:
        void  OnStartRunning();

    private:
    };
}


#endif // LIBERA_SIGNAL_REPOSITORY_H

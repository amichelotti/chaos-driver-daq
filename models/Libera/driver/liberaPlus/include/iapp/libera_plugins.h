/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * LiberaPlugins  singleton class instance is intended to be served as a container
 * class for all loaded plugin modules
 * 
 * $Id: libera_plugins.h 18727 2013-03-05 12:55:25Z damijan.skvarc $
 */

#ifndef LIBERA_PLUGINS_H
#define LIBERA_PLUGINS_H

#include <iostream>
#include <sys/resource.h>

#include "libera_feature.h"

namespace iapp {

    /*----------------------------------------------------------------------------*/
    class LiberaApplication;
    class LiberaPlugins : public LiberaFeature
    {
    public:
        LiberaPlugins(LiberaApplication &a_parent, const std::string &a_name);
        virtual  ~LiberaPlugins();

        void     Load();

    protected:
        virtual bool OnQueryChild(const LiberaObject &a_child);

    protected:
        /* prevent copy constructor and assignment */
        LiberaPlugins(const LiberaPlugins& a_other) = delete;
        LiberaPlugins& operator = (const LiberaPlugins& a_other) = delete;
    };
}


#endif // LIBERA_PLUGINS_H

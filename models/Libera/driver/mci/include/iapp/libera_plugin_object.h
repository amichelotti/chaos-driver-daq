/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * LiberaPluginObject instance is passed to plugin entry point function, what allows plugin developers
 * to introduced additional information about the plugin or to instantiate new LiberaObjects directly
 * under the specified instance.
 * 
 * $Id: libera_plugin_object.h 19990 2014-01-07 09:35:55Z damijan.skvarc $
 */

#ifndef LIBERA_PLUGIN_OBJECT_H
#define LIBERA_PLUGIN_OBJECT_H

#include "libera_object.h"

namespace iapp {

    /* class declaration */
    class LiberaPlugins;
    class LiberaPluginObject : public LiberaObject
    {
       public:
           typedef std::pair<std::string, std::string> InfoPair;
           typedef std::vector<InfoPair>               Info;

           void SetVersion(const std::string &a_version);
           void SetName(const std::string &a_name);
           void SetInfo(const Info& a_info);

       protected:
           LiberaPluginObject(LiberaPlugins& a_parent);
           ~LiberaPluginObject() {};
           bool  OnQueryChild(const LiberaObject &a_child);

       protected:
           std::string      m_version;
           std::string      m_name;
           Info             m_info;
    };

    /* declaration of entry point function  in shared library */
    typedef void (*fnc_inst_t)(iapp::LiberaPluginObject&);
}

#endif // LIBERA_PLUGIN_INFO_H

/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * LiberaPluginHandler is intended to be used as a manager for shared library file
 * It is able to load/unload library file and to keep track of libera objects, which
 * are created within a library. This information is required to be able to unload
 * plugin object in runtime
 *
 * Class is split into two layers, LiberaPluginObject class is used to publish
 * component to the plugin, so that the code in the plugin is able to attach additional
 * information, which can be exposed to the user through the registry, or to attach
 * some additional LiberaObject instance directly under LiberaPluginHandler instance.
 * 
 * $Id: libera_plugin_handler.h 18727 2013-03-05 12:55:25Z manuel.cargnelutti $
 */

#ifndef LIBERA_PLUGIN_HANDLER_H
#define LIBERA_PLUGIN_HANDLER_H

#include "istd/trace.h"
#include "libera_plugin_object.h"

namespace iapp {


    /* class declaration */
    class LiberaPluginHandler : public LiberaPluginObject
    {
    public:
        LiberaPluginHandler(LiberaPlugins &a_parent, const std::string &a_file_name);

    protected:
        virtual  ~LiberaPluginHandler();

        /* prevent copy constructor and assignment */
        LiberaPluginHandler(const LiberaPluginHandler& a_other) = delete;
        LiberaPluginHandler& operator = (const LiberaPluginHandler& a_other) = delete;

    protected:
        const std::string& OnGetName() const;
        void  OnRegistryAdd(ireg::TreeNodePtr &parent);

    private:
        void Load();

    private:
        std::string      m_file_name;
        void            *m_handle;
        LiberaObjects    m_objects;
    };
}


#endif // LIBERA_PLUGIN_HANDLER_H

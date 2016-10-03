/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: libera_config.h 11980 2011-06-15 10:36:13Z matej.kenda $
 */


#ifndef LIBERA_CONFIG_H_
#define LIBERA_CONFIG_H_

#include "icfg/configuration.h"

namespace iapp {
    class LiberaConfig : public icfg::Configuration {

    public:
        LiberaConfig();

    protected:
        void ErrorCallback(const std::string &a_message);
    };
} // namespace


#endif /* LIBERA_CONFIG_H_ */

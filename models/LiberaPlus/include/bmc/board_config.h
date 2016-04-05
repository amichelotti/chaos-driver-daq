/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: board_config.h 14606 2011-12-21 12:43:41Z matej.kenda $
 */


#ifndef BOARD_CONFIG_H_
#define BOARD_CONFIG_H_

#include <string>
#include <thread>

#include "icfg/configuration.h"

/**
    * @class BoardConfig
    *
    * @brief  Singleton configuration class
    */
class BoardConfig : public icfg::Configuration {

public:

    /**
     * Initialize singleton instance
     */
    static void Init(const std::string &a_file);

    /**
     * Get singleton instance
     */
    static BoardConfig &GetInstance();

protected:

    BoardConfig();
    virtual ~BoardConfig();
    BoardConfig(const BoardConfig&);
    BoardConfig& operator=(const BoardConfig&);

    void ErrorCallback(const std::string &a_message);
};

#endif /* BOARD_CONFIG_H_ */

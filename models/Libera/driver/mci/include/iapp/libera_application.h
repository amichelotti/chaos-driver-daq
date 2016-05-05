/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: libera_application.h 19990 2014-01-07 09:35:55Z damijan.skvarc $
 */

#ifndef LIBERA_APPLICATION_H
#define LIBERA_APPLICATION_H

#include <string>
#include <iostream>

#include <thread>



#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/tokenizer.hpp>
#include <boost/token_functions.hpp>

#include "ireg/reg_node.h"
#include "ireg/i_remote_node_impl.h"
#include "istd/signal_handler.h"

#include "bmc/types.h"

#include "libera_config.h"
#include "libera_board.h"
#include "libera_signal_handler.h"
#include "libera_plugins.h"
#include "libera_signal_repository.h"
#include "features/local_timing_sync.h"

using namespace ireg;

namespace iapp {

    class LiberaApplication : public LiberaObject {
    public:

        LiberaApplication(const std::string &a_name,
                          bool a_useRegistryConfig = true,
                          bool a_exitNoConfig = true);
        virtual ~LiberaApplication();
        static LiberaApplication     *Instance();

        /* prevent copy constructor and assignment */
        LiberaApplication(const LiberaApplication& a_other) = delete;
        LiberaApplication& operator = (const LiberaApplication& a_other) = delete;

        /**
         * Calls Init, Setup and starts the application.
         */
        bool Run(int argc, char *argv[]);

        bool Init(int argc, char *argv[]);
        bool Setup();

        Node GetRegistry();
        LiberaConfig &GetConfig();

        /**
         * Unique application name, used while forming default values for logging,
         * tracing, configuration, remote access....
         *
         * @return
         */
        virtual const std::string &ApplicationName() const = 0;

        /**
         *  Return value is used to daemonize the process, for example
         *  "/var/run/libera-appl.pid"
         */
        virtual const std::string PidFile() const;

        /**
         * Default name to be used to create persistent CORBA reference.
         *
         * The name is fixed for all applications to "libera-app".
         *
         * @note Do not change the name from default unless there is a
         *       good reason to do so.
         */
        virtual const std::string &NetID() const;

        /**
         * The port on which the application listens for incoming connections.
         * In practice, platform daemon uses one port and the application is
         * using another.
         *
         * The port from DefaultPort is used unless overridden by environment
         * variable LIBERA_APP_PORT (e.g. from /etc/defaults/libera*) or by
         * command line parameter --port or -p
         */
        unsigned int               Port() const;

        /**
         * The maximal size of data packet for communication with outside world
         *
         * The maximal size from MaxPacketSize is used unless overridden by environment
         * variable LIBERA_APP_MAX_PACKET_SIZE (e.g. from /etc/defaults/libera*) or by
         * command line parameter --max-packet-size or -s
         */
        unsigned int               MaxPacketSize() const;

        /**
         * Default port for applications. It can be overridden by the specific
         * application daemon.
         */
        virtual unsigned int       DefaultPort() const;

        /**
         * Default maximal size of data packet thransmit over the network
         */
        virtual unsigned int       DefaultMaxPacketSize() const;

        /**
         * Return value is used to define file folder for configuration files,
         *  for example "/var/opt/libera/cfg"
         */
        virtual const std::string &ConfDir() const;

        /**
         * @return Path and filename for the configuration file
         */
        virtual const std::string &ConfFile() const;

        /**
         * Return flag indicating how to create XSD file when storing
         * configuration into xml file (default is always).
         */
        virtual icfg::Configuration::CreateXsd_e CreateXsd() const;

        /**
         * @return Path and filename for the log file.
         */
        const std::string  LogFile() const;

        /**
         * @return Path for the log file .
         */
        const std::string &LogFileDir() const;

        /**
          * @return log file name (without path).
          */
        const std::string &LogFileName() const;

        /**
         * @return Path for the plugin files from where plugin dynamic libraries are loaded.
         */
        const std::string &PluginDir() const;

        void   DefineRegistry();

        void   AddRegNode(const TreeNodePtr& a_node);

        /**
         * @create an instance of LiberaBoard class, which is automatically exposed into registry
         */
        virtual LiberaBoard *CreateBoard(const bmc::Board* a_board) = 0;

        ApplicationState_e   GetState() const;

        void                 Shutdown();

        /**
         * @return all boards of a_boardType
         */
        void               GetBoards(LiberaBoards& a_boards, bmc::BoardType_e a_boardType) const;
        void               GetBoards(LiberaBoards& a_boards) const;

        /**
         * @return the first found board of the specified type
         */
        LiberaBoard*       GetBoard(bmc::BoardType_e a_boardType) const;

        virtual bool       AddBoard(LiberaBoard *a_board);

        LiberaPlugins     *Plugins() { return m_plugins; }

        virtual void       OnSyncAnnounced(ifeature::LocalTimingSync &a_ltim) {};
        virtual void       OnSynchronized(ifeature::LocalTimingSync &a_ltim) {};

    protected:
        virtual void       OnInitLogFileName(std::string &a_file_name) const {};
        virtual void       OnInitLogFileDir(std::string &a_path) const {};
        virtual void       OnProgramOptionsAdd(boost::program_options::options_description &a_visibleOptions,
                                               boost::program_options::options_description &a_hiddenOptions);
        virtual bool       OnProgramOptionsProcess(const boost::program_options::variables_map &vm);
        virtual bool       OnQueryChild(const LiberaObject &a_child);

    private:
        friend class       LiberaSigHandler;

        bool               ProcessProgramOptions(int argc, char *argv[]);
        bool               InitConfiguration();
        void               CreateBoards();
        bool               TrainLVDS();
        void               Destroy();

        void               DefineBoardsDomain(const std::vector<std::string> &a_boards);
        void               DefinePort(const std::string &a_port);
        void               DefineMaxPacketSize(const std::string &a_max_packet_size);
        bool               TraceMessage(const std::string &a_val);

        bool               _GetMaxPacketSize(uint32_t &a_val) const;

        ireg::TreeNodePtr           m_registry;
        ireg::i_RemoteNode_impl    *m_registryIntf;
        LiberaPlugins              *m_plugins;
        LiberaSignalRepository     *m_signalRepository;

        LiberaSigHandler            m_sigHandler;
        istd::SignalHandlerThread  *m_sigThread;

        std::string                 m_logFileDir;    /* file folder holding log files             */
        std::string                 m_logFileName;   /* log file name, relative to m_logFileDir   */
        std::string                 m_confDir;
        std::string                 m_confFile;
        LiberaConfig                m_config;
        std::string                 m_pluginDir;     /* file folder from where plugins are loaded */

        bool                        m_useRegistryConfig;
        bool                        m_exitNoConfig;
        LiberaBoards                m_boards;
        ApplicationState_e          m_state;

        bmc::UriVector              m_domain;        /* domain of used boards */
        unsigned int                m_port;          /* port for communication with outside world */
        unsigned int                m_maxPacketSize; /* max size of data packet for communication */
    };

} // namespace

#endif // LIBERA_BASE_APPLICATION_H

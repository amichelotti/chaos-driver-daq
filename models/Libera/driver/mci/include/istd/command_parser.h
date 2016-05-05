/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: command_parser.h 14606 2011-12-21 12:43:41Z matej.kenda $
 */

#ifndef COMMAND_PARSER_H_
#define COMMAND_PARSER_H_

#include <vector>
#include <string>
#include <iostream>
#include <iomanip>

#include <boost/program_options/options_description.hpp>

#include "enum_cast.h"

// Namespaces
using namespace boost;
using namespace boost::program_options;
namespace po = boost::program_options;

namespace istd {

    template<class T>
        class CommandParser {
        public:

            struct Command {
                T command;
                std::string description;
            };

            CommandParser(std::vector<Command> &a_commands, std::string &a_cli) :
                m_commands(a_commands), m_cli(a_cli)
            {
            }

            virtual ~CommandParser()
            {
            }

            void PrintHelp(Command &a_command, bool a_generalHelp = false)
            {
                std::cout << std::endl;

                if (!a_generalHelp && a_command.description != "help") {

                    std::cout << "Help for command: " << istd::EnumCast<T>(
                            a_command.command) << std::endl << std::endl;

                    po::options_description validOptions;
                    if (GetValidOptions(a_command.command, validOptions)) {
                        std::cout << "Valid Options: " << std::endl;
                        std::cout << validOptions << std::endl;
                    }

                    std::cout << "Global Options: " << std::endl;
                    po::options_description globalOptions;
                    GetGlobalOptions(globalOptions);
                    std::cout << globalOptions << std::endl;

                    std::string ex;
                    GetUsageExamples(a_command.command, ex);
                    std::cout << "Show Examples: " << std::endl;
                    std::cout << ex << std::endl;
                }
                else {
                    PrintCommands();
                    std::cout << std::endl;

                    std::string genHelp;
                    GetGeneralHelp(genHelp);
                    std::cout << genHelp << std::endl;
                }
                std::cout << std::endl;
            }

            void SetDefault(T a_defaultCmd)
            {
                m_defaultCmd = a_defaultCmd;
            }

            virtual void GetGlobalOptions(po::options_description &a_options) = 0;

            virtual bool GetValidOptions(T a_command,
                    po::options_description &a_options) = 0;

            virtual void GetUsageExamples(T a_command, std::string &a_ex) = 0;

            virtual void GetGeneralHelp(std::string &a_ex) = 0;

            void PrintCommands()
            {
                std::cout << std::left << std::endl << "Available commands: "
                        << std::endl;

                for (auto iter = m_commands.begin(); iter != m_commands.end(); ++iter) {
                    std::cout
                        << "  " << std::setw(17) << istd::EnumCast<T>(
                        iter->command) << "   [" << iter->description << "]"
                        << std::endl;
                }
            }

            bool ParseCommand(int a_argc, char* a_argv[], Command &a_command,
                    po::options_description &a_validOptions,
                    po::options_description &a_globalOptions,
                    bool &a_defaultCommand)
            {
                if (a_argc < 2) {
                    std::cout
                        << "Type '" << m_cli << " help' for usage."
                        << std::endl;
                    return false;
                }

                T command;
                bool help = false;
                std::string cmdStr(a_argv[1]);
                try {
                    if (cmdStr == "help") {

                        help = true;
                        Command cmd;
                        if (a_argc > 2) {

                            cmdStr = a_argv[2];
                            if (cmdStr == "help") {
                                cmd.description = "help";
                            }
                            else {
                                command = istd::EnumCast<T>(cmdStr);
                                cmd.command = command;
                                cmd.description = GetDescription(a_command.command);
                            }
                            PrintHelp(cmd);
                        }
                        else {
                            PrintHelp(cmd, true);
                        }

                        return false;
                    }
                    else {
                        a_command.command = istd::EnumCast<T>(cmdStr);
                        a_command.description = GetDescription(a_command.command);
                        GetValidOptions(a_command.command, a_validOptions);
                        GetGlobalOptions(a_globalOptions);
                        a_defaultCommand = false;
                        return true;
                    }
                }
                catch (const istd::Exception&) {
                    if (IsValidCommand(m_defaultCmd) && !help) {
                        a_command.command = m_defaultCmd;
                        a_command.description = GetDescription(m_defaultCmd);
                        GetValidOptions(a_command.command, a_validOptions);
                        GetGlobalOptions(a_globalOptions);
                        a_defaultCommand = true;
                        return true;
                    }
                    else {
                        std::cout << "ERROR: Unknown command " << cmdStr
                                << std::endl;
                        return false;
                    }
                }
            }

        private:
            std::vector<Command> m_commands;
            std::string m_cli;
            T m_defaultCmd;

            std::string GetDescription(T a_command)
            {
                for (auto iter = m_commands.begin(); iter != m_commands.end(); ++iter) {
                    if (iter->command == a_command) {
                        return iter->description;
                    }
                }
                return "";
            }

            bool IsValidCommand(T a_command)
            {
                for (auto iter = m_commands.begin(); iter != m_commands.end(); ++iter) {
                    if (iter->command == a_command) {
                        return true;
                    }
                }
                return false;
            }

        };

}

#endif

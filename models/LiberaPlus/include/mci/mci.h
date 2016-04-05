/*
 * Copyright (c) 2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: mci.h 19990 2014-01-07 09:35:55Z damijan.skvarc $
 */


#ifndef MCI_H_
#define MCI_H_

#include "mci/declarations.h"
#include "mci/node.h"

namespace mci {

    void Init() throw (Exception);
    void Init(int argc, char* argv[]) throw (Exception);
    void Shutdown() throw ();

    enum ConnectionState {
        Disconnected,  // Before calling Connect and after calling Disconnect.
        Connected,     // After calling Connect and before calling Disconnect.
        NotAccessible, // Connected but can't access remote server.
        Reconnected    // Restored connection from NotAccessible.
    };

    enum class Root {
        Platform,
        Application
    };

    const int c_defaultPort = 0;

    Node Connect(Root a_rtype = Root::Application, int a_port = c_defaultPort);

    Node Connect(
        const std::string& a_host,
        Root a_rtype = Root::Application,
        int a_port = c_defaultPort,
        bool a_managed = false);

    void Disconnect(Root a_rtype = Root::Application, int a_port = c_defaultPort);

    void Disconnect(
        const std::string& a_host,
        Root a_rtype = Root::Application,
        int a_port = c_defaultPort);

    /**
     * Test connection and return true if the connected server responds.
     * If it doesn't, the attempt for node reconnection will be made.
     */
    ConnectionState Ping(Node &a_node);

    Node GetNode(Root a_rtype = Root::Application, int a_port = c_defaultPort);

    Node GetNode(
        const std::string& a_host,
        Root a_rtype = Root::Application,
        int a_port = c_defaultPort);

    void DeleteNode(
        const std::string& a_host,
        Root a_rtype = Root::Application,
        int a_port = c_defaultPort);

    /**
     * Convert stringified path ("node.nodeA.nodeb") to mci::Path
     */
    Path Tokenize(const std::string& a_parameterPath);
    Path Tokenize(const char* a_parameterPath);

    std::ostream& operator<< (std::ostream &a_os, Root a_rtype);

} // namespace

#endif /* MCI_H_ */

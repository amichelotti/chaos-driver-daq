/*
 * Copyright (c) 2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: orb_manager.h 19990 2014-01-07 09:35:55Z damijan.skvarc $
 */

#ifndef ORB_MANAGER_H
#define ORB_MANAGER_H

#include <string>
#include <omniORB4/CORBA.h>

// Replace with "GCC diagnostic push" and "GCC diagnostic pop"
// in gcc 4.5

#pragma GCC diagnostic ignored "-Wold-style-cast"
#include "i_pingable.h"
#pragma GCC diagnostic warning "-Wold-style-cast"

namespace inet {

    const unsigned int          c_defaultPlatformPort(5678);
    const unsigned int          c_defaultApplicationPort(5679);
    const unsigned int          c_maxGiopSize(512*1024*1024 + 20);
    extern const std::string    c_defaultNetId;

    class i_Pingable_impl;

    /**
     * @addtogroup corba
     * @{
     */
    class OrbManager {
    public:
        static const unsigned int c_noServerPort = 0;

        static OrbManager* Instance();

        /**
         *
         * @param argc Number of arguments (from command line)
         * @param argv List of arguments (from command line)
         * @param a_serverPort The port on which the process will listen for
         *                     incoming connections
         */
        static void Init(
            int     argc,
            char*   argv[],
            unsigned int a_serverPort = c_noServerPort,
            unsigned int a_giopMaxSize = c_maxGiopSize);

        //  Servants with persistent IDs
        void ActivatePersistent(
            PortableServer::ServantBase* a_servant,
            const std::string &oid);

        void DeactivatePersistent(PortableServer::ServantBase* a_servant);

        // Servants in the bi-dir POA
        void Activate(PortableServer::ServantBase* a_servant);
        void Deactivate(PortableServer::ServantBase* a_servant);

        CORBA::Object_ptr StringToObject(const std::string &a_string);
        const std::string ObjectToString(CORBA::Object_ptr a_obj);
        void ORBRun();
        void ORBShutdown(bool a_wait = true);
        void ORBDestroy();
        void HoldRequests();

        // Helper pingable servant to test communication
        i_NET::Pingable_ptr    GetPingable();

    private:
        // singleton private constructor
        OrbManager();
        ~OrbManager();

        // copying not allowed
        OrbManager(const OrbManager& other);
        OrbManager& operator=(const OrbManager& other);

        void Setup();

        CORBA::ORB_var          m_orb;
        PortableServer::POA_var m_insPOA;
        PortableServer::POA_var m_bidirPOA;
        i_Pingable_impl        *m_pingable;
        bool                    m_active;
    };
    /**@}*/

    /**
     * @addtogroup corba CORBA utilities
     * @{
     */
    /** Helper function for OrbManager global singleton access. */
    OrbManager* OM();
    /**@}*/

}

#endif // ORB_MANAGER_H


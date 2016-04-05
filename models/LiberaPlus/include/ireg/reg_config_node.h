/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: reg_config_node.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */


#ifndef REG_CONFIG_NODE_H_
#define REG_CONFIG_NODE_H_

#include <string>

#include "icfg/serializable.h"
#include "icfg/configuration.h"

#include "ireg/reg_node.h"

namespace ireg {

    /**
     * @addtogroup register Repository interface for signals, configuration parameters, etc...
     * @{
     */

    /**
     * Specific implementation of a TreeNode with Configuration specifics.
     */
    class RegConfigNode : public RegNode, public icfg::ISerializable {
    public:
        virtual ~RegConfigNode();

        /**
         * Reads values from configuration file and fills the
         * appropriate children nodes in the registry.
         * Returns false when all nodes from configuration file are not
         * loaded into the registry.
         */
        bool OnLoadConfiguration();

        void CreateXsdFileOnSave(const icfg::Configuration::CreateXsd_e& a_createXsd);

    protected:

        explicit RegConfigNode(const std::string& a_name,
                               const std::string& a_fileName,
                               icfg::Configuration *a_configuration = NULL,
                               Flags a_flags = 0);

        mci_CREATE_NODE_FRIEND;

        bool OnDeserialize(const icfg::ElementPtr &a_prop);
        icfg::ElementPtr OnSerialize();

        //ISerializable
        bool Deserialize(const icfg::ElementList &a_properties);
        bool Serialize(icfg::ElementList &a_properties);

        /**
         * Reads values from children persistent nodes and writes them into the
         * configuration file. Returns false when file cannot be stored.
         */
        bool OnWriteConfiguration();

        // Here we automatically store nodes into config file
        bool OnCleanup(const Path& a_relPath);

    private:
        std::string          m_fileName;
        icfg::Configuration *m_configuration;
        bool                 m_configOwner;
        bool                 m_configOk;

        icfg::Configuration::CreateXsd_e m_createXsd;
    };
    /**@}*/
}


#endif /* REG_CONFIG_NODE_H_ */

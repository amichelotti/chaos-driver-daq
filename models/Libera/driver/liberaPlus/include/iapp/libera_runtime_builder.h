/*
 * Copyright (c) 2008-2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * Helper class which builds registry tree from runtime configuration file.
 *
 * $Id: libera_runtime_builder.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef LIBERA_RUNTIME_BUILDER_H
#define LIBERA_RUNTIME_BUILDER_H

#include <string>

#include "istd/log.h"

#include "icfg/serializable.h"
#include "icfg/configuration.h"

#include "ireg/reg_node.h"
#include "ireg/tree_node.h"
#include "ireg/declarations.h"

namespace iapp {

    class RuntimeBuilderConfig : public icfg::Configuration 
        {
        public:
            RuntimeBuilderConfig() { };
    
        protected:
            void ErrorCallback(const std::string &a_message) {
                istd_LOGc("Runtime configuration error: " << a_message);
            }
        
        private:
            std::string GetApplicationName() {
                return "runtime_config";
            }
        };

    class LiberaRuntimeBuilder : public icfg::ISerializable
        {
        public:
            /* a_addrAlignemnt defines alignment of the address in bytes. */
            LiberaRuntimeBuilder(ireg::TreeNodePtr &a_parent, 
                                 void *a_baseAddr, const size_t a_baseSize, 
                                 const std::string &a_cfgFilename,
                                 const size_t a_addrAlign = 4);
            virtual ~LiberaRuntimeBuilder();

            bool BuildRegistry();

        protected:

            bool ParseRegNode(const icfg::ElementPtr &a_regNode,
                              ireg::TreeNodePtr &a_parent);
            bool ParseBitNode(const icfg::ElementPtr &a_regNode, 
                              ireg::TreeNodePtr &a_parent);
            bool ParseNode(const icfg::ElementPtr &a_regNode, 
                           ireg::TreeNodePtr &a_parent);


            ireg::TreeNodePtr AddNode(const icfg::ElementPtr &a_node,
                                      ireg::TreeNodePtr &a_parent);

            template <typename TYPE>
                volatile TYPE *CalculateAddr(const uint32_t &a_offset);

            template <typename TYPE>
                ireg::TreeNodePtr AddRegistryNode(ireg::TreeNodePtr &a_parent,
                                                  const std::string &a_name,
                                                  const uint32_t &a_offset,
                                                  const ireg::Flags &a_flags,
                                                  const size_t a_size = 1);

            ireg::TreeNodePtr AddStringRegistryNode(ireg::TreeNodePtr &a_parent,
                                                    const std::string &a_name,
                                                    const uint32_t &a_offset,
                                                    const ireg::Flags &a_flags,
                                                    const size_t a_stringSize,
                                                    const size_t a_arraySize);

            template <typename TYPE>
                ireg::TreeNodePtr AddBitRegistryNode(ireg::TreeNodePtr &a_parent,
                                                     const std::string &a_name,
                                                     const uint32_t &a_offset,
                                                     const uint8_t &a_bitOffset,
                                                     const uint8_t &a_bitSize,
                                                     const ireg::Flags &a_flags);


            ireg::Flags ParseFlags(const std::string &a_flags);
            uint32_t ParseType(const std::string &a_type);

            //ISerializable
            bool Deserialize(const icfg::ElementList &a_properties);

        private:
            RuntimeBuilderConfig  m_config;
            ireg::TreeNodePtr     m_parent;
            void                 *m_baseAddr;
            std::string           m_filename;
            size_t                m_baseSize;
            const size_t          m_addrAlign;
        };

} // namespace

#endif // LIBERA_RUNTIME_BUILDER_H

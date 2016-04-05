/*
 * Copyright (c) 2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: node_factory_decl.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef MCI_NODE_FACTORY_DECL_H
#define MCI_NODE_FACTORY_DECL_H

#include "mci/declarations.h"

namespace mci {

    class NodeStructure;
    class LocalNodeStructure;
    class RemoteNodeStructure;

    namespace factory {
        // Declaration of templates

        /**
         * Factory function to create any node from the hierarchy.
         * @param NODE_S    type of NodeStructure
         * @param NODE_TYPE type of the registry node
         * @param ARGS      constructor arguments forwarded to EXPR_TYPE
         * @return Shared pointer to heap-allocated node
         */
        template<class NODE_S, class NODE_TYPE, typename ...ARGS>
            TreeNodePtr Create(ARGS&& ...args);

        template<typename NODE_TYPE, typename ...ARGS>
            TreeNodePtr Create(ARGS&& ...args);

        /**
         * Factory function to create any node from the hierarchy when the first
         * parameter is a C-string literal.
         */
        template<typename NODE_TYPE, typename ...ARGS>
            TreeNodePtr Create(const char* a_name, ARGS&& ...args);

    } // namespace

    using namespace factory;

} // namespace


#define mci_CREATE_NODE_FRIEND \
    template<class NODE_S, class NODE_TYPE, typename ...ARGS> \
        friend mci::TreeNodePtr mci::factory::Create(ARGS&& ...args)

#endif /* MCI_NODE_FACTORY_DECL_H */

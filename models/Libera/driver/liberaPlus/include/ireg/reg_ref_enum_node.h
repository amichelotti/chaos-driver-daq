/*
 * Copyright (c) 2012 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: reg_ref_enum_node.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */


#ifndef REG_REF_ENUM_NODE_H_
#define REG_REF_ENUM_NODE_H_

#include "ireg/reg_ref_node.h"

namespace ireg {

    template <typename ET, typename NT = ET, size_t BITSIZE = 0, size_t OFF = 0>
        class RegRefEnumNode:
            public virtual RegRefNode<ET, BITSIZE, OFF, NT>,
            public RegEnumMixin<ET, RegRefNode<ET, BITSIZE, OFF, NT>> {

        public:
            typedef ET                                              EnumType;
            typedef NT                                              NodeType;
            typedef RegRefNode<EnumType, BITSIZE, OFF, NodeType>    BaseNode;
            typedef RegEnumMixin<EnumType, BaseNode>                EnumMixin;

            virtual ~RegRefEnumNode()
            {
            }

        protected:

            /**
             * Enum node forwards all arguments to base classes.
             */
            template <typename ...ARGS>
                RegRefEnumNode(ARGS&& ...args)
                : BaseNode(args...), EnumMixin(args...)
                {
                }

            mci_CREATE_NODE_FRIEND;
        };

}

#endif /* REG_REF_ENUM_NODE_H_ */

/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: reg_fnc_enum_node.h 14606 2011-12-21 12:43:41Z matej.kenda $
 */

#ifndef REG_FNC_ENUM_NODE_H_
#define REG_FNC_ENUM_NODE_H_

#include "ireg/reg_enum_mixin.h"
#include "ireg/reg_fnc_node.h"

namespace ireg {

    template <class ENUM>
        class RegFncEnumNode:
            public virtual RegFncNode<ENUM>,
            public RegEnumMixin<ENUM, RegFncNode<ENUM>> {

        public:
            typedef ENUM                                EnumType;
            typedef RegFncNode<EnumType>                BaseNode;
            typedef RegEnumMixin<EnumType, BaseNode>    EnumMixin;

            virtual ~RegFncEnumNode()
            {

            }

        protected:

            /**
             * Enum node forwards all arguments to base classes.
             */
            template <typename ...ARGS>
                RegFncEnumNode(ARGS&& ...args)
                : BaseNode(args...), EnumMixin(args...)
                {
                }

            mci_CREATE_NODE_FRIEND;

        };
}

#endif /* REG_FNC_ENUM_NODE_H_ */

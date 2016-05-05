/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: reg_reference_enum_node.h 14606 2011-12-21 12:43:41Z matej.kenda $
 */

#ifndef REG_REFERENCE_ENUM_NODE_H_
#define REG_REFERENCE_ENUM_NODE_H_

#include "ireg/reg_enum_mixin.h"
#include "reg_reference_node.h"

namespace ireg {

    template <class ENUM>
        class RegReferenceEnumNode:
            public virtual RegReferenceNode<ENUM, ENUM>,
            public RegEnumMixin<ENUM, RegReferenceNode<ENUM, ENUM>> {

        public:
            typedef ENUM                                    EnumType;
            typedef RegReferenceNode<EnumType, EnumType>    BaseNode;
            typedef RegEnumMixin<EnumType, BaseNode>        EnumMixin;

            virtual ~RegReferenceEnumNode()
            {
            }

        protected:

            /**
             * Enum node forwards all arguments to base classes.
             */
            template <typename ...ARGS>
                RegReferenceEnumNode(ARGS&& ...args)
                : BaseNode(args...), EnumMixin(args...)
                {
                }

            mci_CREATE_NODE_FRIEND;
        };

}

#endif /* REG_REFERENCE_ENUM_NODE_H_ */

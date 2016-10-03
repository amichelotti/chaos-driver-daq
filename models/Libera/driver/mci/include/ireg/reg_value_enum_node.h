/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: reg_value_enum_node.h 14606 2011-12-21 12:43:41Z matej.kenda $
 */

#ifndef REG_VALUE_ENUM_NODE_H_
#define REG_VALUE_ENUM_NODE_H_

#include "ireg/reg_enum_mixin.h"
#include "ireg/reg_value_node.h"

namespace ireg {

    template <class ENUM>
        /**
         * The node contains an instance of the enum ENUM.
         *
         * The node can be also treated as string for values, defined in the
         * EnumCast, int32_t or int64_t.
         *
         * @see istd::EnumCast
         */
        class RegValueEnumNode:
            public virtual RegValueNode<ENUM>,
            public RegEnumMixin<ENUM, RegValueNode<ENUM>> {

        public:
            typedef ENUM                                EnumType;
            typedef RegValueNode<EnumType>              BaseNode;
            typedef RegEnumMixin<EnumType, BaseNode>    EnumMixin;

            virtual ~RegValueEnumNode()
            {
            }

        protected:

            /**
             * Enum node forwards all arguments to base classes.
             */
            template <typename ...ARGS>
                RegValueEnumNode(ARGS&& ...args)
                : BaseNode(args...), EnumMixin(args...)
                {
                }

            mci_CREATE_NODE_FRIEND;
        };
}

#endif /* REG_VALUE_ENUM_NODE_H_ */

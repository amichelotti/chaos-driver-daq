/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * RegReferenceEnumBitNode() class is intended to access certain range of bits from referenced variable
 * and represent them as enumeration data type.
 *
 * $Id: reg_reference_enum_bit_node.h 14606 2011-12-21 12:43:41Z matej.kenda $
 */


#ifndef REG_REFERENCE_ENUM_BIT_NODE_H_
#define REG_REFERENCE_ENUM_BIT_NODE_H_

#include "ireg/reg_enum_mixin.h"
#include "reg_reference_bit_node.h"

namespace ireg {

    template <class AT, class RT>
        class RegReferenceEnumBitNode:
            public virtual RegReferenceBitNode<AT, RT>,
            public RegEnumMixin<AT, RegReferenceBitNode<AT, RT>> {

        public:
            typedef AT                                  EnumType;
            typedef RegReferenceBitNode<AT,RT>          BaseNode;
            typedef RegEnumMixin<EnumType, BaseNode>    EnumMixin;

            virtual ~RegReferenceEnumBitNode()
            {

            }

        protected:

            /**
             * Enum node forwards all arguments to base classes.
             */
            template <typename ...ARGS>
                RegReferenceEnumBitNode(ARGS&& ...args)
                : BaseNode(args...), EnumMixin(args...)
                {
                }

            mci_CREATE_NODE_FRIEND;

        };
}



#endif /* REG_REFERENCE_ENUM_BIT_NODE_H_ */

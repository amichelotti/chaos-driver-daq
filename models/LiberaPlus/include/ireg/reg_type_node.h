/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: reg_type_node.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */


#ifndef REG_TYPE_NODE_H_
#define REG_TYPE_NODE_H_

#include "reg_node.h"

namespace ireg {

    template <class T>
        class RegTypeNode: public RegNode {
        public:

            virtual ~RegTypeNode()
            {

            }

            virtual NodeValType_e OnGetValueType(const Path& a_relPath = c_noRelPath) const
            {
                return eNvUndefined;
            }

        protected:
            RegTypeNode(const std::string& a_name, Flags a_flags)
            : RegNode(a_name, a_flags)
            {

            }

        };

    // Specialization for std::string
    template<> inline NodeValType_e RegTypeNode<std::string>::OnGetValueType(
        const Path& a_relPath) const
    {
        return eNvString;
    }

    // Specialization for bool
    template<> inline NodeValType_e RegTypeNode<bool>::OnGetValueType(
        const Path& a_relPath) const
    {
        return eNvBool;
    }

    // Specialization for double
    template<> inline NodeValType_e RegTypeNode<double>::OnGetValueType(
        const Path& a_relPath) const
    {
        return eNvDouble;
    }

    // Specialization for float
    template<> inline NodeValType_e RegTypeNode<float>::OnGetValueType(
        const Path& a_relPath) const
    {
        return eNvFloat;
    }

    // Specialization for int32_t
    template<> inline NodeValType_e RegTypeNode<int32_t>::OnGetValueType(
        const Path& a_relPath) const
    {
        return eNvLong;
    }

    // Specialization for uint32_t
    template<> inline NodeValType_e RegTypeNode<uint32_t>::OnGetValueType(
        const Path& a_relPath) const
    {
        return eNvULong;
    }

    // Specialization for int64_t
    template<> inline NodeValType_e RegTypeNode<int64_t>::OnGetValueType(
        const Path& a_relPath) const
    {
        return eNvLongLong;
    }

    // Specialization for uint64_t
    template<> inline NodeValType_e RegTypeNode<uint64_t>::OnGetValueType(
        const Path& a_relPath) const
    {
        return eNvULongLong;
    }

}



#endif /* REG_TYPE_NODE_H_ */

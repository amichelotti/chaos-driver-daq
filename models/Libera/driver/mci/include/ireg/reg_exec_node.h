/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: reg_exec_node.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef REG_EXECUTABLE_NODE_H_
#define REG_EXECUTABLE_NODE_H_

#include <boost/functional.hpp>

#include "reg_node.h"
#include "reg_value_enum_node.h"

namespace ireg {

    typedef RegValueEnumNode<NodeCommand_e>   CommandEnumNode;

    class RegExecNode: public CommandEnumNode {
    public:
        typedef std::function<bool()>           ExecFnc;

        static const Flags execFlags_c;

        virtual ~RegExecNode();

        virtual void OnSet(const int64_t* a_in, size_t a_pos, size_t a_size);
        virtual void OnSet(const int32_t* a_in, size_t a_pos, size_t a_size);
        virtual void OnSet(const std::string* a_in, size_t a_pos, size_t a_size);

    protected:
        RegExecNode(const std::string& a_name, ExecFnc a_fnc, const Flags a_Flags = 0);

        template<typename FNC, class O>
            RegExecNode(const std::string& a_name, O a_instance, FNC a_fnc, const Flags a_Flags = 0)
            : CommandEnumNode::BaseNode(a_name, eExecute, a_Flags | execFlags_c),
              CommandEnumNode(a_name, eExecute, a_Flags | execFlags_c),
              m_fnc(std::bind(std::mem_fun_ref(a_fnc), std::ref(*a_instance)))
            {

            }

        mci_CREATE_NODE_FRIEND;

        virtual bool OnResize(size_t a_size, const Path& a_relPath = c_noRelPath);

    private:
        void _OnSet(const int64_t a_in);

        ExecFnc m_fnc;
    };
}

#endif /* REG_EXECUTABLE_NODE_H_ */

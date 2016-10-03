/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * Trigger generation fpga object for TIM board
 *
 * $Id: multi_gigabit_transciever_node.h 15770 2012-04-20 12:35:13Z matej.kenda $
 */

#ifndef MULTI_GIGABIT_TRANSCIEVER_NODE_H
#define MULTI_GIGABIT_TRANSCIEVER_NODE_H

#include "bmc/types.h"
#include "ireg/reg_node.h"

/*-----------------------------------------------------------------------------*/

struct MgtRegisters;

/*-----------------------------------------------------------------------------*/

class MultiGigabitTranscieverNode : public ireg::RegNode
{
public:

    virtual ~MultiGigabitTranscieverNode();
    static size_t  Size();

    bool Reset();

private:
    volatile MgtRegisters* m_reg;

    MultiGigabitTranscieverNode(const std::string& a_name, FPGAMem_t* a_reg);
    virtual bool Init();
    mci_CREATE_NODE_FRIEND;

    std::mutex  m_reset_x;
};


#endif // MULTI_GIGABIT_TRANSCIEVER_NODE_H

/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * Trigger generation fpga object for TIM board
 *
 * $Id: evrx_leds_node.h 15770 2012-04-20 12:35:13Z matej.kenda $
 */

#ifndef EVRX_LEDS_NODE_H
#define EVRX_LEDS_NODE_H

#include "bmc/types.h"
#include "ireg/reg_node.h"

class EvrxLedsNode : public ireg::RegNode
{
public:

    virtual ~EvrxLedsNode();
    static size_t  Size();

private:
    volatile uint64_t* m_reg;

    EvrxLedsNode(const std::string& a_name, FPGAMem_t* a_reg);
    virtual bool Init();
    mci_CREATE_NODE_FRIEND;
};


#endif // EVRX_LEDS_NODE_H

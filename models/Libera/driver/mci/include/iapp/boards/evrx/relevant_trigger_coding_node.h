/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * Trigger generation fpga object for TIM board
 *
 * $Id: relevant_trigger_coding_node.h 16165 2012-05-21 09:12:40Z matej.kenda $
 */

#ifndef RELEVANT_TRIGGER_CODING_NODE_H
#define RELEVANT_TRIGGER_CODING_NODE_H

#include <array>
#include "bmc/types.h"
#include "ireg/reg_node.h"

/*-----------------------------------------------------------------------------*/

struct RtcDecoder;
struct RelevantTc;

enum RtcDecodeSwitch_e { eRtcOff  = 0, eRtcOn = 1, eRtcDebug = 2 };

enum MgtOutputSwitch_e { eMgtOff  = 0, eMgtSfpIn = 1, eMgtDebug = 2 };

/*-----------------------------------------------------------------------------*/

class RelevantTriggerCodingNode : public ireg::RegNode
{
public:

    virtual ~RelevantTriggerCodingNode();
    static size_t  Size();

private:
    volatile RelevantTc* m_reg;

    RelevantTriggerCodingNode(const std::string& a_name, FPGAMem_t* a_reg);
    virtual bool Init();
    mci_CREATE_NODE_FRIEND;

    void AddDecoder(
        const std::string&      a_name,
        volatile RtcDecoder&    a_dec,
        const ireg::Flags       a_flags);

    bool ApplyGenerator();

    static const size_t c_genLen = 32;

    std::mutex                      m_dbg_x;
    std::array<uint32_t, c_genLen>  m_dbgCodes;
    std::array<uint32_t, c_genLen>  m_dbgDelays;
};


#endif // RELEVANT_TRIGGER_CODING_NODE_H

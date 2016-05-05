/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * Trigger State Machine
 *
 * $Id: tsm.h 7920 2010-07-22 14:44:05Z damijan.skvarc $
 */

#ifndef TSM_H
#define TSM_H

#include <array>
#include "iapp/libera_fpga_object.h"
#include "iapp/libera_board.h"
#include "ireg/node.h"

namespace ifeature {

/*-----------------------------------------------------------------------------*/
enum TsmPolarity_e { ePositive, eNegative};

/*-----------------------------------------------------------------------------*/
struct tTSM {
/* 0x00000 */    uint64_t duration;  /* bits 0..24 */
/* 0x00008 */    uint64_t offset;    /* bits 0..24 */
/* 0x00010 */    uint64_t control;   /* bits 0,1,2 */
};



const size_t c_numOfTsmRegItems(5);

/*-----------------------------------------------------------------------------*/

class Tsm : public iapp::LiberaFpgaObject
{
public:
    enum RegNodeId_e { eDuration   = 0,
                       eOffset     = 1,
                       ePolarity   = 2,
                       eEnabled    = 3,
                       eContinuous = 4 };

    struct RegNodeProperty {
        const char* name;
        ireg::Flags flags;
    };

    struct RegNodeConfiguration {
        enum RegNodeId_e  id;
        RegNodeProperty   property;
    };

    typedef const std::vector<RegNodeConfiguration>  RegistryConfiguration;

    Tsm(iapp::LiberaBoard &a_board, FPGAMem_t *a_reg, const std::string &a_name, const RegistryConfiguration *a_config = NULL);
    virtual ~Tsm();

    static size_t  Size() { return sizeof(tTSM); }

    bool           SetDuration(const uint32_t& a_val);
    bool           GetDuration(uint32_t& a_val) const;
    bool           SetOffset(const uint32_t& a_val);
    bool           GetOffset(uint32_t& a_val) const;
    void           SetMaxOffset(const uint32_t& a_val);
    void           Enable();
    void           Disable();
    void           SetContinuous(const bool& a_val);

protected:
    virtual void   OnRegistryAdd(ireg::TreeNodePtr &parent);

private:
    typedef std::array<RegNodeProperty, c_numOfTsmRegItems>  RegistryProperties;

    bool           _SetDuration(const uint32_t& a_val);
    bool           _SetOffset(const uint32_t& a_val);

    static const RegistryProperties c_defRegProperties;

    iapp::LiberaBoard   &m_board;
    volatile tTSM       *m_reg;
    RegistryProperties   m_regProps;

    ireg::TreeNodePtr    m_durationNode;
    ireg::TreeNodePtr    m_offsetNode;
    ireg::TreeNodePtr    m_enableNode;
    ireg::TreeNodePtr    m_continuousNode;
};


} // namespace

#endif // TSM_H

/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * Machine Clock Prescaler
 *
 * $Id: mc_pll.h 15770 2012-04-20 12:35:13Z damijan.skvarc $
 */

#ifndef MACHINE_CLOCK_PLL_H
#define MACHINE_CLOCK_PLL_H

#include "iapp/libera_board.h"
#include "ireg/tree_node.h"

/*-----------------------------------------------------------------------------*/
struct tMcPll;
namespace iboard {class TimBoard;}

/*-----------------------------------------------------------------------------*/
class McPll : public iapp::LiberaFpgaObject
{
public:
    McPll(iapp::LiberaObject &a_parent, FPGAMem_t *a_reg, const std::string &a_name);

    static size_t  Size();

    bool GetMcLock() const;
    void SetMcPrescaler(uint32_t a_value);

protected:
    virtual void OnRegistryAdd(ireg::TreeNodePtr &parent);

private:
    volatile tMcPll   *m_reg;
    iboard::TimBoard  &m_board;
};


#endif // MACHINE_CLOCK_PLL_H

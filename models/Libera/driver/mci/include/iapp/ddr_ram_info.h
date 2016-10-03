/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: ddr_ram_info.h 19990 2014-01-07 09:35:55Z damijan.skvarc $
 */

#ifndef _DDR_RAM_INFO_H
#define _DDR_RAM_INFO_H

#include "iapp/libera_board.h"
#include "iapp/libera_fpga_object.h"
#include "ireg/reg_value_node.h"
#include "bmc/i2c_device_spd_eeprom.h"
#include "bmc/i2c_fpga_amba.h"

/*-----------------------------------------------------------------------------*/

struct tRamCtrlPar {
    uint32_t Capacity;

    uint32_t NumOfRanks;
    uint32_t RankWidth;
    uint32_t BankWidth;
    uint32_t RowWidth;
    uint32_t ColumnWidth;
    uint32_t Trcd;
    uint32_t Tfaw;
    uint32_t Tras;
    uint32_t Tck;
    uint32_t Twtr;
    uint32_t Trtp;
    uint32_t Trrd;
    uint32_t Trp;
    uint32_t Trfc;
    uint32_t Trefi;
};

/*-----------------------------------------------------------------------------*/

class DdrRamInfo : public iapp::LiberaFpgaObject, public bmc::SpdEeprom
{
public:
    static size_t  Size() { return sizeof(tI2cChannel); }

    bool ParMatchingCheck(void);

protected:
    virtual void      OnRegistryAdd(ireg::TreeNodePtr &parent);
    virtual void      Init();

    DdrRamInfo(iapp::LiberaBoard    &a_board,
            FPGAMem_t               *a_reg,
            const std::string&      a_name,
            const uint8_t           a_i2cSlvSel,
            tRamCtrlPar             a_ramCtrlPar);

    virtual ~DdrRamInfo();

    void AddRamParNodes(ireg::TreeNodePtr &parent, tRamCtrlPar a_ramCtrlPar);
    void EepromRamParGet(void);

    template<typename OBJ_TYPE, typename T, typename ...ARGS>
           friend typename std::enable_if<std::is_base_of<LiberaObject,OBJ_TYPE>::value, OBJ_TYPE>::type *
           iapp::Create(T &a_parent, ARGS&& ...a_args);

private:
    /* ram par. -> from eeprom */
    tRamCtrlPar m_eepromRamPar;
    /* ram ctrl. par. -> from fpga */
    tRamCtrlPar m_fpgaRamPar;

    /* eeprom memory space */
    tSpdEepromRegMap m_regArea;
    /* interface to FPGA PCI memory */
    volatile tI2cChannel *m_reg;
};


#endif // _DDR_RAM_INFO_H

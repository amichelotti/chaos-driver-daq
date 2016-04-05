/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: i2c_SPD_EEPROM.h 18145 2012-11-08 11:38:35Z ales.bardorfer $
 */


#ifndef I2C_SPD_EEPROM_H
#define I2C_SPD_EEPROM_H

#include "iapp/libera_i2c_bus_device.h"
#include "iapp/libera_i2c_bus_controller.h"

#include "ireg/reg_value_node.h"

#include "iapp/ddr_ram_info.h"
#include "bmc/i2c_device_spd_eeprom.h"


namespace iapp {

class SPD_EEPROM : public LiberaI2cBusDevice
{
public:
    SPD_EEPROM(LiberaI2cBusController  &a_parent,
               const std::string       &a_name,
               bmc::BusDeviceId         a_id,
               uint8_t                  a_device_address,
               uint8_t                  a_bus_address);
    virtual ~SPD_EEPROM();

    bmc::SpdEeprom&   Device()     { return m_device; }
    tRamCtrlPar&      GetRamPar()  { return m_RamPar; }

protected:
    virtual void      OnRegistryAdd(ireg::TreeNodePtr &parent);

private:

    void AddRamParNodes(ireg::TreeNodePtr &parent, tRamCtrlPar a_ramCtrlPar);
    void EepromRamParGet(void);

    /* BMC SPD device */
    bmc::SpdEeprom        &m_device;

    /* Ram parameters -> from eeprom */
    tRamCtrlPar            m_RamPar;

    /* Eeprom memory space */
    tSpdEepromRegMap       m_regArea;
};

} // namespace

#endif // I2C_SPD_EEPROM_H

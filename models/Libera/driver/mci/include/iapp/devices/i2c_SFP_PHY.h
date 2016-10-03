/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: i2c_SFP_PHY.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef I2C_SFP_PHY_H
#define I2C_SFP_PHY_H

#include "bmc/i2c_device_SFP_PHY.h"

#include "iapp/declarations.h"
#include "iapp/libera_i2c_bus_device.h"
#include "iapp/libera_i2c_bus_controller.h"

namespace iapp {

    class SFP_PHY : public LiberaI2cBusDevice
    {
    public:
        SFP_PHY(LiberaI2cBusController  &a_parent,
                const std::string       &a_name,
                bmc::BusDeviceId         a_id,
                uint8_t                  a_device_address,
                uint8_t                  a_bus_address);
        virtual ~SFP_PHY();

        bmc::SfpPhy&   Device() { return m_device; }

        bool GetStatus(uint16_t& a_val) const;
        void SetSgmii();
        void IsolateDisable();

    protected:
        virtual void  OnRegistryAdd(ireg::TreeNodePtr &parent);

    private:
        bmc::SfpPhy      &m_device;
    };

} //namespace


#endif // I2C_SFP_PHY_H

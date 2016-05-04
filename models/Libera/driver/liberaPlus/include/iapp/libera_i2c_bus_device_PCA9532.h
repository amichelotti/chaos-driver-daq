/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: libera_i2c_bus_device_PCA9532.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef LIBERA_I2C_BUS_DEVICE_PCA9532_H
#define LIBERA_I2C_BUS_DEVICE_PCA9532_H

#include "bmc/i2c_device_PCA9532.h"

#include "declarations.h"
#include "libera_i2c_bus_device.h"
#include "libera_i2c_bus_controller.h"

namespace iapp {

    class LiberaBoard;

    class LiberaPCA9532 : public LiberaI2cBusDevice
    {
    public:
        LiberaPCA9532(LiberaI2cBusController  &a_parent,
                      const std::string       &a_name,
                      bmc::BusDeviceId         a_id,
                      uint8_t                  a_device_address,
                      uint8_t                  a_bus_address);
        virtual ~LiberaPCA9532();

        bmc::PCA9532& Device() { return m_device; }
        bool SetMode(const std::string &a_mode, const size_t a_pos);
        bool GetMode(std::string &a_mode, const size_t a_pos) const;

    protected:
        virtual void  OnRegistryAdd(ireg::TreeNodePtr &parent);

    private:
        bool   GetPinState(uint32_t& a_val) const;

        bool   GetPrescaler0(uint32_t& a_val) const;
        bool   SetPrescaler0(const uint32_t& a_val);

        bool   GetPrescaler1(uint32_t& a_val) const;
        bool   SetPrescaler1(const uint32_t& a_val);

        bool   GetPWM0(uint32_t& a_val) const;
        bool   SetPWM0(const uint32_t& a_val);

        bool   GetPWM1(uint32_t& a_val) const;
        bool   SetPWM1(const uint32_t& a_val);

        size_t GetModes(std::string *a_values, const size_t a_pos, const size_t a_size) const;
        bool   SetModes(const std::string *a_values, const size_t a_pos, const size_t a_size);

        bmc::PCA9532 &m_device;
        ireg::TreeNodePtr m_modesNode;
    };

} //namespace


#endif // LIBERA_I2C_BUS_DEVICE_PCA9532_H

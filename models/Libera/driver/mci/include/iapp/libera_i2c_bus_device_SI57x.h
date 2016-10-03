/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: libera_i2c_bus_device_SI57x.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef LIBERA_I2C_BUS_DEVICE_SI57X_H
#define LIBERA_I2C_BUS_DEVICE_SI57X_H

#include "bmc/i2c_device_SI57x.h"

#include "declarations.h"
#include "libera_i2c_bus_device.h"
#include "libera_i2c_bus_controller.h"

namespace iapp {

    class LiberaBoard;

    class SI57x : public LiberaI2cBusDevice
    {
    public:
        SI57x(LiberaI2cBusController  &a_parent,
              const std::string       &a_name,
              bmc::BusDeviceId         a_id,
              uint8_t                  a_device_address,
              uint8_t                  a_bus_address);
        virtual ~SI57x();

        bmc::Si57x&   Device() { return m_device; }

    protected:
        virtual void  OnRegistryAdd(ireg::TreeNodePtr &parent);

    private:
        bool ApplyFrequency();
        bmc::Si57x   &m_device;
    };

} //namespace


#endif // LIBERA_I2C_BUS_DEVICE_SI57x_H

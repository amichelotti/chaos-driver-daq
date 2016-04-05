/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: libera_i2c_bus_device.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef LIBERA_I2C_BUS_DEVICE_H
#define LIBERA_I2C_BUS_DEVICE_H


#include "declarations.h"
#include "libera_bus_device.h"

namespace iapp {

    class LiberaI2cBusDevice : public LiberaBusDevice
    {
    public:

    protected:
        LiberaI2cBusDevice(LiberaObject &a_parent, bmc::BusDevicePtr a_device);
        virtual ~LiberaI2cBusDevice();

    private:
    };
} //namespace


#endif // LIBERA_I2C_BUS_DEVICE_H

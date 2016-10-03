/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: libera_spi_bus_device.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef LIBERA_SPI_BUS_DEVICE_H
#define LIBERA_SPI_BUS_DEVICE_H


#include "declarations.h"
#include "libera_bus_device.h"

namespace iapp {

    class LiberaSpiBusDevice : public LiberaBusDevice
    {
    public:

    protected:
        LiberaSpiBusDevice(LiberaObject &a_parent, bmc::BusDevicePtr a_device);
        virtual ~LiberaSpiBusDevice();

    private:
    };
} //namespace


#endif // LIBERA_SPI_BUS_DEVICE_H

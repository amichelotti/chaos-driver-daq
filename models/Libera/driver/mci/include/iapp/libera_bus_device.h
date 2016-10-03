/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: libera_bus_device.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef LIBERA_BUS_DEVICE_H
#define LIBERA_BUS_DEVICE_H

#include "bmc/bus_device.h"
#include "declarations.h"
#include "libera_object.h"

namespace iapp {

    class LiberaBusDevice : public LiberaObject
    {
    public:
        bmc::BusDevicePtr  DevicePtr() { return m_device; }

    protected:
        LiberaBusDevice(LiberaObject &a_parent, bmc::BusDevicePtr a_device);
        virtual ~LiberaBusDevice();

    private:
        bmc::BusDevicePtr  m_device;
    };


} //namespace


#endif // LIBERA_PCA9532_OBJECT_H

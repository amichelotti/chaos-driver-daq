/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: libera_spi_bus_device_DAC8581.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef LIBERA_SPI_BUS_DEVICE_DAC8581_H
#define LIBERA_SPI_BUS_DEVICE_DAC8581_H

#include "bmc/spi_device_DAC8581.h"

#include "declarations.h"
#include "libera_spi_bus_device.h"
#include "libera_spi_bus_controller.h"

namespace iapp {

    class LiberaBoard;

    class DAC8581 : public LiberaSpiBusDevice
    {
    public:
        DAC8581(LiberaSpiBusController  &a_parent,
                      const std::string       &a_name,
                      bmc::BusDeviceId         a_id);
        virtual ~DAC8581();

        bmc::DAC8581& Device() { return m_device; }
        void Set(const int32_t& a_val);

    protected:
        virtual void  OnRegistryAdd(ireg::TreeNodePtr &parent);

    private:
        bool GetValue(int32_t& a_val) const;
        bool SetValue(const int32_t& a_val);

        bmc::DAC8581      &m_device;
        ireg::TreeNodePtr  m_node;
    };

} //namespace


#endif // LIBERA_SPI_BUS_DEVICE_DAC8581_H

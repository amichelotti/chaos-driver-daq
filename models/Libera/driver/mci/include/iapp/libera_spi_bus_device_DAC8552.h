/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: libera_spi_bus_device_DAC8552.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef LIBERA_SPI_BUS_DEVICE_DAC8552_H
#define LIBERA_SPI_BUS_DEVICE_DAC8552_H

#include "bmc/spi_device_DAC8552.h"
#include "ireg/node.h"

#include "declarations.h"
#include "libera_spi_bus_device.h"
#include "libera_spi_bus_controller.h"

namespace iapp {

    class LiberaBoard;

    class DAC8552 : public LiberaSpiBusDevice
    {
    public:
        DAC8552(LiberaSpiBusController  &a_parent,
                      const std::string       &a_name,
                      bmc::BusDeviceId         a_id);
        virtual ~DAC8552();

        bool SetValueA(const uint32_t& a_val);
        bool SetValueB(const uint32_t& a_val);
        bool SetMode(const bmc::DAC8552::tMode& a_val);

        bmc::DAC8552& Device() { return m_device; }

    protected:
        virtual void  OnRegistryAdd(ireg::TreeNodePtr &parent);

    private:
        bool GetValueA(uint32_t& a_val) const;
        bool _SetValueA(const uint32_t& a_val);

        bool GetValueB(uint32_t& a_val) const;
        bool _SetValueB(const uint32_t& a_val);

        bool GetMode(bmc::DAC8552::tMode& a_val) const;
        bool _SetMode(const bmc::DAC8552::tMode& a_val);

        bmc::DAC8552 &m_device;

        ireg::Node    m_NodeA;
        ireg::Node    m_NodeB;
        ireg::Node    m_NodeMode;
    };

} //namespace


#endif // LIBERA_SPI_BUS_DEVICE_DAC8552_H

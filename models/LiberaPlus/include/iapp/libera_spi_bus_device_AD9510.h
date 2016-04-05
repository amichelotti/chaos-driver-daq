/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: libera_spi_bus_device_AD9510.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef LIBERA_SPI_BUS_DEVICE_AD9510_H
#define LIBERA_SPI_BUS_DEVICE_AD9510_H

#include "bmc/spi_device_AD9510.h"
#include "ireg/node.h"

#include "declarations.h"
#include "libera_spi_bus_device.h"
#include "libera_spi_bus_controller.h"

namespace iapp {

    class LiberaBoard;

    class AD9510 : public LiberaSpiBusDevice
    {
    public:
        AD9510(LiberaSpiBusController                &a_parent,
                const std::string                    &a_name,
                bmc::BusDeviceId                      a_id,
                const bmc::AD9510::ConfigurationItem *a_defaultRegs = NULL);
        virtual ~AD9510();

        bool SetPllEnabled(const bool& a_val);
        bool GetPllEnabled(bool& a_val) const;

        bool SetDividerA(const uint32_t& a_val);
        bool SetDividerB(const uint32_t& a_val);
        bool SetDividerR(const uint32_t& a_val);
        bool GetDividerA(uint32_t& a_val) const;
        bool GetDividerB(uint32_t& a_val) const;
        bool GetDividerR(uint32_t& a_val) const;

        bool SetRegister(const uint32_t& a_val);
        bool GetRegister(uint32_t& a_val) const;

        bmc::AD9510& Device() { return m_device; }

    protected:
        virtual void  OnRegistryAdd(ireg::TreeNodePtr &parent);

    private:
        bool _SetPllEnabled(const bool& a_val);
        bool SetDivider(const enum bmc::AD9510::tDivider& a_divider, const uint16_t& a_val);
        bool GetDivider(const enum bmc::AD9510::tDivider& a_divider, uint16_t& a_val) const;
        bool _SetDividerA(const uint32_t& a_val);
        bool _SetDividerB(const uint32_t& a_val);
        bool _SetDividerR(const uint32_t& a_val);

        bool Flush();
        bool Init();

        bmc::AD9510  &m_device;

        uint32_t      m_register;

        ireg::Node    m_enableNode;
        ireg::Node    m_divANode;
        ireg::Node    m_divBNode;
        ireg::Node    m_divRNode;

    };

} //namespace


#endif // LIBERA_SPI_BUS_DEVICE_AD9510_H

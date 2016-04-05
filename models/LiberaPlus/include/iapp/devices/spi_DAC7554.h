/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: spi_DAC7554.h 15880 2012-05-08 13:32:54Z damijan.skvarc $
 */

#ifndef LIBERA_SPI_BUS_DEVICE_DAC7554_H
#define LIBERA_SPI_BUS_DEVICE_DAC7554_H

#include "bmc/spi_device_DAC7554.h"

#include "iapp/declarations.h"
#include "iapp/libera_spi_bus_device.h"
#include "iapp/libera_spi_bus_controller.h"

namespace iapp {

    class LiberaBoard;
    class DAC7554;

    /*----------------------------------------------------------------------*/
    class DAC7554Channel
    {
    private:
        friend class DAC7554;
        DAC7554Channel(bmc::DAC7554 &m_device, uint32_t a_channel);

        bool GetValue(uint32_t& a_val) const;
        bool SetValue(const uint32_t& a_val);

        bool GetPowerDownMode(bmc::DAC7554::PowerDownMode_e& a_val) const;
        bool SetPowerDownMode(const bmc::DAC7554::PowerDownMode_e& a_val);

        void AddReg(ireg::TreeNodePtr a_parent);
        void Set(const uint32_t& a_val);

    private:
        bmc::DAC7554      &m_device;
        uint32_t           m_channel;
        ireg::TreeNodePtr  m_valueNode;
        ireg::TreeNodePtr  m_pwrDownModeNode;
    };


    /*----------------------------------------------------------------------*/
    class DAC7554 : public LiberaSpiBusDevice
    {
    public:
        DAC7554(LiberaSpiBusController  &a_parent,
                const std::string       &a_name,
                bmc::BusDeviceId         a_id);

        bmc::DAC7554& Device() { return m_device; }

        void     Set(uint32_t a_channel, const uint32_t& a_val);

        void SetPowerDownMode(int32_t& a_channel, const bmc::DAC7554::PowerDownMode_e& a_val);

    protected:
        virtual void    OnRegistryAdd(ireg::TreeNodePtr &parent);

    private:
        bmc::DAC7554    &m_device;
        DAC7554Channel   m_channel_0;
        DAC7554Channel   m_channel_1;
        DAC7554Channel   m_channel_2;
        DAC7554Channel   m_channel_3;
        DAC7554Channel*  m_channels[NUM_OF_CHANNELS];
    };

} //namespace


#endif // LIBERA_SPI_BUS_DEVICE_DAC7554_H

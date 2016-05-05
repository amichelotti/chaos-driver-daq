/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: libera_spi_bus_device_LMH6518.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef LIBERA_SPI_BUS_DEVICE_LMH6518_H
#define LIBERA_SPI_BUS_DEVICE_LMH6518_H

#include "bmc/spi_device_LMH6518.h"

#include "declarations.h"
#include "libera_spi_bus_device.h"
#include "libera_spi_bus_controller.h"

namespace iapp {

    class LiberaBoard;

    class LMH6518 : public LiberaSpiBusDevice
    {
    public:
        LMH6518(LiberaSpiBusController  &a_parent,
                      const std::string &a_name,
                      bmc::BusDeviceId   a_id,
                      bool               a_write_only);
        virtual ~LMH6518();

        bmc::LMH6518& Device() { return m_device; }
        void Set(const uint32_t& a_val);

    protected:
        virtual void  OnRegistryAdd(ireg::TreeNodePtr &parent);

    private:
        bool _GetRaw(uint32_t& a_val) const;
        bool _SetRaw(const uint32_t& a_val);

        bool _GetFullPower(bool& a_val) const;
        bool _SetFullPower(const bool& a_val);

        bool _GetHiGain(bool& a_val) const;
        bool _SetHiGain(const bool& a_val);

        bool _GetFilterBandWidth(bmc::LMH6518::tFilterBandwidth &a_val) const;
        bool _SetFilterBandWidth(const bmc::LMH6518::tFilterBandwidth& a_val);

        bool _GetAttenuation(bmc::LMH6518::tAttenuator &a_val) const;
        bool _SetAttenuation(const bmc::LMH6518::tAttenuator& a_val);

        bmc::LMH6518      &m_device;
        ireg::TreeNodePtr  m_raw_node;
        ireg::TreeNodePtr  m_full_power_node;
        ireg::TreeNodePtr  m_hi_gain_node;
        ireg::TreeNodePtr  m_bandwidth_node;
        ireg::TreeNodePtr  m_attenuation_node;
    };

} //namespace


#endif // LIBERA_SPI_BUS_DEVICE_DAC8581_H

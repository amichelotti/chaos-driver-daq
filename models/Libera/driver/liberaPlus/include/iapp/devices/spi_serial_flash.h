/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: spi_serial_flash.h 15880 2012-05-08 13:32:54Z damijan.skvarc $
 */

#ifndef LIBERA_SPI_BUS_DEVICE_SERIAL_FLASH_H
#define LIBERA_SPI_BUS_DEVICE_SERIAL_FLASH_H

#include "bmc/spi_device_serial_flash.h"

#include "iapp/declarations.h"
#include "iapp/libera_spi_bus_device.h"
#include "iapp/libera_spi_bus_controller.h"

namespace iapp {


    /*----------------------------------------------------------------------*/
    class SerialFlash : public LiberaSpiBusDevice
    {
    public:
        SerialFlash(LiberaSpiBusController  &a_parent,
                    const std::string       &a_name,
                    bmc::BusDeviceId         a_id,
                    bool                     a_writeOnly);

    protected:
        virtual void      OnRegistryAdd(ireg::TreeNodePtr &a_parent);

    private:
        bool              GetId(uint32_t& a_val) const;
        bool              GetMemType(uint32_t& a_val) const;
        bool              GetCapacity(uint32_t& a_val) const;

    private:
        bmc::SerialFlash  &m_device;
    };

} //namespace


#endif // LIBERA_SPI_BUS_DEVICE_SERIAL_FLASH_H

/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: spi_CDCE72010.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef LIBERA_SPI_BUS_DEVICE_CDCE72010_H
#define LIBERA_SPI_BUS_DEVICE_CDCE72010_H

#include "bmc/spi_device_CDCE72010.h"
#include "ireg/node.h"

#include "declarations.h"
#include "libera_spi_bus_device.h"
#include "libera_spi_bus_controller.h"

namespace iapp
{
    class CDCE72010 : public LiberaSpiBusDevice
    {
        /* helper class for accessing individual CDCE72010 register through the ireg interface */
        class Register
        {
        public:
            Register(bmc::CDCE72010 &a_device, uint32_t a_id);

            void Add(ireg::TreeNodePtr &parent);

        private:
            bool Set(const uint32_t &a_val);
            bool Get(uint32_t &a_val);

            bmc::CDCE72010  &m_device;
            const uint32_t  m_id;
        };

        /* class declaration */
    public:
        CDCE72010(LiberaSpiBusController  &a_parent,
                const std::string         &a_name,
                bmc::BusDeviceId           a_id);

    protected:
        virtual void  OnRegistryAdd(ireg::TreeNodePtr &parent);

    private:
        bmc::CDCE72010  &m_device;
        Register         m_reg0;
        Register         m_reg1;
        Register         m_reg2;
        Register         m_reg3;
        Register         m_reg4;
        Register         m_reg5;
        Register         m_reg6;
        Register         m_reg7;
        Register         m_reg8;
        Register         m_reg9;
        Register         m_reg10;
        Register         m_reg11;
    };

} //namespace


#endif // LIBERA_SPI_BUS_DEVICE_CDCE72010_H

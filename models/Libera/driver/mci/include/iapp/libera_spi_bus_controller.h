/*
 * Copyright (c) 2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: libera_spi_bus_controller.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef LIBERA_SPI_BUS_CONTROLLER_H
#define LIBERA_SPI_BUS_CONTROLLER_H

#include "istd/exception.h"
#include "bmc/bus_controller.h"
#include "bmc/spi_bus_controller_fpga_amba.h"
#include "bmc/spi_device.h"

#include "declarations.h"
#include "libera_board.h"
#include "libera_fpga_object.h"
#include "libera_bus_controller.h"

/*-----------------------------------------------------------------------------*/

namespace iapp {

    class LiberaSpiBusController : public LiberaBusController
    {
    public:
        LiberaSpiBusController(LiberaBoard       &a_board,
                               FPGAMem_t         *a_reg,
                               const std::string &a_name,
                               uint32_t           a_capacity,
                               bool               a_cs_active_high);

        virtual ~LiberaSpiBusController();

        static size_t Size() { return bmc::SpiBusControllerFpgaAmba::Size(); }

    protected:
        virtual bool OnQueryChild(const LiberaObject &a_child);

    private:
    };

} //namespace


#endif // LIBERA_SPI_BUS_CONTROLLER_H

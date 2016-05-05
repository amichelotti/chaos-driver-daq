/*
 * Copyright (c) 2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: libera_i2c_bus_controller.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef LIBERA_I2C_BUS_CONTROLLER_H
#define LIBERA_I2C_BUS_CONTROLLER_H

#include "istd/exception.h"
#include "bmc/i2c_bus_controller_fpga_amba.h"

#include "declarations.h"
#include "libera_board.h"
#include "libera_bus_controller.h"

/*-----------------------------------------------------------------------------*/

namespace iapp {

    class LiberaI2cBusController : public LiberaBusController
    {
    public:
        LiberaI2cBusController(LiberaBoard       &a_board,
                               FPGAMem_t         *a_reg,
                               const std::string &a_name,
                               uint32_t           a_capacity);

        virtual ~LiberaI2cBusController();

        static size_t  Size() { return bmc::I2cBusControllerFpgaAmba::Size(); }

    protected:
        virtual bool OnQueryChild(const LiberaObject &a_child);
    private:

    };

} //namespace


#endif // LIBERA_I2C_BUS_CONTROLLER_H

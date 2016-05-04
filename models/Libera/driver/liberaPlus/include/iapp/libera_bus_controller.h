/*
 * Copyright (c) 2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: libera_bus_controller.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef LIBERA_BUS_CONTROLLER_H
#define LIBERA_BUS_CONTROLLER_H

#include "istd/exception.h"
#include "bmc/bus_controller.h"
#include "bmc/bus_device.h"

#include "declarations.h"
#include "libera_board.h"
#include "libera_fpga_object.h"

/*-----------------------------------------------------------------------------*/

namespace iapp {

    class LiberaBusController : public LiberaFpgaObject
    {
    public:
        LiberaBusController(LiberaBoard           &a_board,
                            std::string            a_name,
                            bmc::BusControllerPtr  a_controller);

        virtual ~LiberaBusController();

        bmc::BusControllerPtr Controller() { return m_controller; }
        bool                  SetOutput(uint32_t  a_val) { return m_controller->SetOutput(a_val); }
        bool                  GetOutput(uint32_t &a_val) { return m_controller->GetOutput(a_val); }

    protected:
        /* event functions */
        virtual void  OnRegistryAdd(ireg::TreeNodePtr &parent);
        virtual void  OnAddDevice(const bmc::BusDevice &a_device);

    private:
        void   OnEachDevice(bmc::BusDevice &a_device, uint32_t a_pos, void* a_data);
        size_t GetDevices(std::string *a_values, const size_t a_pos, const size_t a_size) ;

        ireg::TreeNodePtr     m_devices_node;
        bmc::BusControllerPtr m_controller;
    };

} //namespace


#endif // LIBERA_SPI_BUS_CONTROLLER_H

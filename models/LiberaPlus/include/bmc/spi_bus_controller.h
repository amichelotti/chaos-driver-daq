/*
 * Copyright (c) 2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: spi_bus_controller.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef SPI_BUS_CONTROLLER_H_
#define SPI_BUS_CONTROLLER_H_


#include "bus_controller.h"

/*-----------------------------------------------------------------------------*/
namespace bmc
{
    /* class definition */
    class SpiBusController : public BusController
    {
    public:

    protected:
        SpiBusController(const std::string &a_name, uint32_t a_capacity = 1, bool a_cs_active_high = true);
        virtual ~SpiBusController();
        bool     ChipSelectActiveHigh() { return m_cs_active_high; }

    private:
        bool m_cs_active_high;   /* determine if chip select is active high (true) or low (false) */
    };

} //namespace


#endif /* SPI_BUS_CONTROLLER_H_ */

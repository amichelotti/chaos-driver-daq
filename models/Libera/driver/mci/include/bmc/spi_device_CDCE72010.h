/*
 * Copyright (c) 2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: spi_device_CDCE72010.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef SPI_DEVICE_CDCE72010_H_
#define SPI_DEVICE_CDCE72010_H_

#include "spi_device.h"

/*-----------------------------------------------------------------------------*/

namespace bmc {

    /* class declaration */
    class CDCE72010 : public SpiDevice
    {
        public:
            CDCE72010(BusController &a_controller, const std::string &a_name, BusDeviceId a_id);
            virtual ~CDCE72010();

            bool Get(uint32_t a_register, uint32_t &a_val);
            bool Set(uint32_t a_register, const uint32_t &a_val);

        protected:

        private:
    };
}


#endif /* SPI_DEVICE_CDCE72010_H_ */

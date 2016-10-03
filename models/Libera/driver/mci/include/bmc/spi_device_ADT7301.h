/*
 * Copyright (c) 2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: spi_device_ADT7301.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef SPI_DEVICE_ADT7301_H_
#define SPI_DEVICE_ADT7301_H_

#include <vector>
#include "spi_device.h"

/*-----------------------------------------------------------------------------*/

namespace bmc {

    /* class declaration */
    class ADT7301 : public SpiDevice
    {
    public:
        ADT7301(BusController &a_controller, const std::string &a_name, BusDeviceId a_id);
        virtual ~ADT7301();

        bool GetTemperature(float& a_val, bool& a_error);

    protected:
    private:
    };
}

#endif /* SPI_DEVICE_ADT7301_H_ */

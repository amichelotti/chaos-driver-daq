/*
 * Copyright (c) 2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: spi_device_DAC8581.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef SPI_DEVICE_DAC_8581_H_
#define SPI_DEVICE_DAC_8581_H_

#include "spi_device.h"

/*-----------------------------------------------------------------------------*/

namespace bmc {

    /* class declaration */
    class DAC8581 : public SpiDevice
    {
    public:
        DAC8581(BusController &a_controller, const std::string &a_name, BusDeviceId a_id);
        virtual ~DAC8581();

        int16_t GetValue()  const;
        bool    SetValue(const int16_t& a_val);

    protected:

    private:
        int16_t m_value;  /* cached value, which was applied to the device */
    };
}


#endif /* SPI_DEVICE_DAC_8581_H_ */

/*
 * Copyright (c) 2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: i2c_device.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */
#ifndef I2C_DEVICE_H
#define I2C_DEVICE_H

#include "bus_device.h"

/*-----------------------------------------------------------------------------*/


namespace bmc {

    class I2cDevice : public BusDevice
    {
       public:
           /* prevent copy and assign operators */
           I2cDevice(const I2cDevice& a_other) = delete;
           I2cDevice& operator = (const I2cDevice& a_other) = delete;

           I2cDevice(BusController &a_controller, const std::string &a_name, BusDeviceId a_id);
           virtual ~I2cDevice();
       protected:
           uint8_t Get(uint8_t a_register) const;
           void    Set(uint8_t a_register, uint8_t a_val);
    };

}

#endif // I2C_DEVICE_H

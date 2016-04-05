/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * SFP EEPROM i2c device - Complies with Multi Source Agreement (MSA).
 *
 * $Id: i2c_device_SFP_EEPROM.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef SFP_EEPROM_H
#define SFP_EEPROM_H

#include "i2c_device.h"


/*-----------------------------------------------------------------------------*/

namespace bmc {

    class SfpEeprom : public I2cDevice
    {
    public:
        SfpEeprom(BusController  &a_controller,
                  std::string   a_name,
                  BusDeviceId   a_id,
                  uint8_t       a_device_address,
                  uint8_t       a_bus_address);
        virtual ~SfpEeprom();

        static const uint8_t c_i2cAddr;

        bool GetVendor(std::string& a_val) const;

    protected:
        //virtual void OnEnabled();
        //virtual void OnDisabled();

        //virtual void OnDeviceDisable() {};
        //virtual void OnDeviceEnable() {};

    private:

    };
}

#endif // SFP_EEPROM_H

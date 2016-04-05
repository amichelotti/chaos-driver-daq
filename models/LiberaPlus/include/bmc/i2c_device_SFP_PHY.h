/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * SFP PHY i2c device
 *
 * $Id: i2c_device_SFP_PHY.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef SFP_PHY_H
#define SFP_PHY_H

#include "i2c_device.h"


/*-----------------------------------------------------------------------------*/

namespace bmc {

    class SfpPhy : public I2cDevice
    {
    public:
        SfpPhy(BusController  &a_controller,
               std::string   a_name,
               BusDeviceId   a_id,
               uint8_t       a_device_address,
               uint8_t       a_bus_address);
        virtual ~SfpPhy();

        static const uint8_t c_i2cAddr;

        bool GetStatus(uint16_t& a_val) const;
        void SetSgmii();
        void IsolateDisable();

    protected:

    private:

    };
}

#endif // SFP_PHY_H

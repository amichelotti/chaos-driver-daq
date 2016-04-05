/*
 * Copyright (c) 2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: spi_device_DAC7554.h 15879 2012-05-08 13:31:33Z damijan.skvarc $
 */

#ifndef SPI_DEVICE_DAC_7554_H_
#define SPI_DEVICE_DAC_7554_H_

#include "spi_device.h"

#define NUM_OF_CHANNELS 4

/*-----------------------------------------------------------------------------*/

namespace bmc {

    /* class declaration */
    class DAC7554 : public SpiDevice
    {
    public:
        /* enumeration values are set to appropriate bit values, which are applied to DP8 & DP7 bits */
        enum PowerDownMode_e {
            ePwrDownHiZ     = 0,
            ePwrDown1K      = 1,
            ePwrDown100K    = 2,
            ePwrDownOff     = 3  /* special case, identifying channel is not in power off mode */
        };

        DAC7554(BusController &a_controller, const std::string &a_name, BusDeviceId a_id);
        virtual ~DAC7554();

        uint16_t GetValue(uint32_t a_channel) const;
        bool     SetValue(uint32_t a_channel, uint16_t a_val);

        PowerDownMode_e GetPowerDownMode(uint32_t a_channel) const;
        bool            SetPowerDownMode(uint32_t a_channel, PowerDownMode_e a_val);

    protected:

    private:
        uint16_t           m_values[NUM_OF_CHANNELS];            /* cached value, which was applied to the device */
        PowerDownMode_e    m_power_down_modes[NUM_OF_CHANNELS];  /* cached power down mode  */
    };
}


#endif /* SPI_DEVICE_DAC_7554_H_ */

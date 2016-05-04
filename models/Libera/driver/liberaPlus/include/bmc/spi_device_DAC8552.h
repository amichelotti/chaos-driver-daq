/*
 * Copyright (c) 2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: spi_device_DAC8552.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef SPI_DEVICE_DAC_8552_H_
#define SPI_DEVICE_DAC_8552_H_

#include "spi_device.h"

/*-----------------------------------------------------------------------------*/

namespace bmc {

    /* class declaration */
    class DAC8552 : public SpiDevice
    {
    public:
        enum tOutput {
            eOutputA   = 0x0,
            eOutputB   = 0x1
        };

        enum tMode {
            eModeNormal      = 0x0,
            eModePwrDown1K   = 0x1,
            eModePwrDown100K = 0x2,
            eModePwrDownHiZ  = 0x3
        };

        DAC8552(BusController &a_controller, const std::string &a_name, BusDeviceId a_id);
        virtual ~DAC8552();

        uint16_t GetValue(tOutput a_output) const;
        bool     SetValue(bool a_load_A, bool a_load_B, tOutput a_output, const uint16_t& a_val);

        void     SetMode(tMode a_mode);
        tMode    GetMode();

    protected:

    private:
        uint16_t  m_actual_values[2];  /* values, applied on DAC outputs               */
        uint16_t  m_cache_values[2];   /* values, written into cache registers         */
                                       /* NOTE: since device is write only,            */
                                       /* all written values must be cached internally */
                                       /* for reading purposes                         */
        tMode     m_actual_mode;
        tMode     m_mode;
    };
}

#endif /* SPI_DEVICE_DAC_8552_H_ */

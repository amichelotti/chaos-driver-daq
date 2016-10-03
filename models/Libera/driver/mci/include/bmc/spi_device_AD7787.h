/*
 * Copyright (c) 2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: spi_device_AD7787.h 19990 2014-01-07 09:35:55Z damijan.skvarc $
 */

#ifndef SPI_DEVICE_AD7787_H_
#define SPI_DEVICE_AD7787_H_

#include <vector>
#include "spi_device.h"

/*-----------------------------------------------------------------------------*/

namespace bmc {

    /* class declaration */
    class AD7787 : public SpiDevice
    {

    public:
        AD7787(BusController &a_controller,
               const std::string &a_name,
               BusDeviceId a_id,
               double a_refVoltage);
        virtual ~AD7787();

        bool GetTemperature(float& a_val, bool& a_error);

    private:
        bool GetRawData(uint32_t& a_val, bool& a_error);
        bool GetVoltage(float& a_val, bool& a_error);

        uint8_t GetStatusReg();
        uint32_t GetDataReg();
        void SetModeReg(uint8_t a_reg);
        void SetFilterReg(uint8_t a_reg);

        double m_refVoltage;
    };
}

#endif /* SPI_DEVICE_AD7787_H_ */

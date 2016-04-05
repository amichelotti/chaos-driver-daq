/*
 * Copyright (c) 2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: i2c_bus_params.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef I2C_BUS_PARAMS_H_
#define I2C_BUS_PARAMS_H_

#include <thread>
#include <sstream>

#include "bus_params.h"

/*-----------------------------------------------------------------------------*/

namespace bmc {

    class I2cBusParams : public BusParams
    {
    public:
        I2cBusParams() = delete;
        I2cBusParams(uint8_t   a_device_address,
                     uint8_t   a_bus_address,
                     uint32_t  a_freq_khz);
        virtual ~I2cBusParams();

        bool operator==(const struct I2cBusParams& a_other) const
        {
            return (this->m_device_address == a_other.m_device_address) &&
                   (this->m_bus_address    == a_other.m_bus_address) &&
                   (this->GetFreqKhz()     == a_other.GetFreqKhz());
        }

        const std::string  ToString() const
        {
            std::ostringstream sstr;
            sstr << "Device_address: " << m_device_address << std::endl <<
                    "Bus_address: "    << m_bus_address << std::endl <<
                    BusParams::ToString();
            return sstr.str();
        }

        uint8_t  GetDeviceAddress() const        { return m_device_address;  }
        uint8_t  GetBusAddress()    const        { return m_bus_address;     }

        void     SetDeviceAddress(uint8_t a_val) { m_device_address = a_val; }
        void     SetBusAddress   (uint8_t a_val) { m_bus_address    = a_val; }

    private:
        uint8_t  m_device_address; /* unique address for devices on the same bus   */
        uint8_t  m_bus_address;    /* in the current implementation                */
                                   /* i2c_controller controls multiple bus         */
    };

} //namespace


#endif /* SPI_BUS_PARAMS_H_ */

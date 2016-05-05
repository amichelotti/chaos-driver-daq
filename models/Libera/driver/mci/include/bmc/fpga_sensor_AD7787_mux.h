/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: fpga_sensor_AD7787_mux.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */


#ifndef BMC_FPGA_SENSOR_AD7787_MUX_H_
#define BMC_FPGA_SENSOR_AD7787_MUX_H_

#include <inttypes.h>
#include <memory>
#include <vector>
#include <thread>
#include "bmc/sensor_reading.h"
#include "bmc/sensor_id.h"
#include "bmc/types.h"
#include "bmc/sensor.h"

// devices
#include "bmc/spi_device_ADT7301.h"
#include "bmc/spi_device_AD7787.h"
#include "bmc/analog_mux.h"
#include "bmc/spi_bus_controller_fpga_amba.h"
#include "bmc/fpga_sensor_ADT7301.h"

extern "C" {
    struct sensor_record;
}

namespace istd {
    struct DeleteElement;
}

namespace bmc {

    class Board;

    /**
     * @addtogroup bmc_sensor Sensors handling Classes and functions
     * @{
     */
    class FpgaSensorAD7787Mux : public Sensor {
        public:
            int                     GetRecordType() const;
            SensorType_e            GetType() const;
            const std::string&      GetName() const;
            const std::string&      GetUnits() const;
            SensorReading           LowNonCritical() const;
            SensorReading           HighNonCritical() const;
            SensorReading           HighCritical() const;
            SensorReading           FetchSensorReading() const;

        private:
            explicit FpgaSensorAD7787Mux(bmc::Board& a_board, const bmc::SensorId& a_id,
                                         tSDRfpga a_sdr, bmc::BusDevicePtr pDevice,
                                         bmc::AnalogMux a_muxDevice, uint32_t a_muxInput);
            virtual ~FpgaSensorAD7787Mux();

            // copying not allowed
            FpgaSensorAD7787Mux(const FpgaSensorAD7787Mux& other);
            FpgaSensorAD7787Mux& operator=(const FpgaSensorAD7787Mux& other);

            tSDRfpga m_sdr;
            bmc::AD7787  &m_device;
            bmc::AnalogMux m_muxDevice;
            uint32_t m_muxInput;

        friend class Board;
        friend struct istd::DeleteElement;
    };
    /**@}*/
}


#endif /* BMC_FPGA_SENSOR_AD7787_MUX_H_ */

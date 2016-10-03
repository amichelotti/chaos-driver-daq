/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: fpga_sensor_ADT7301.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */


#ifndef BMC_FPGA_SENSOR_ADT7301_H_
#define BMC_FPGA_SENSOR_ADT7301_H_

#include <inttypes.h>
#include <memory>
#include <vector>
#include <thread>
#include "bmc/sensor_reading.h"
#include "bmc/sensor_id.h"
#include "bmc/types.h"
#include "bmc/sensor.h"
#include "bmc/internal.h"
#include "bmc/spi_device_ADT7301.h"
#include "bmc/analog_mux.h"
#include "bmc/spi_bus_controller_fpga_amba.h"

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
    class FpgaSensorADT7301 : public Sensor {
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
            explicit FpgaSensorADT7301(bmc::Board& a_board, const bmc::SensorId& a_id,
                                tSDRfpga a_sdr, bmc::BusDevicePtr pDevice);
            virtual ~FpgaSensorADT7301();

            // copying not allowed
            FpgaSensorADT7301(const FpgaSensorADT7301& other);
            FpgaSensorADT7301& operator=(const FpgaSensorADT7301& other);

            tSDRfpga m_sdr;
            bmc::ADT7301  &m_device;

        friend class Board;
        friend struct istd::DeleteElement;
    };
    /**@}*/
}


#endif /* BMC_FPGA_SENSOR_ADT7301_H_ */

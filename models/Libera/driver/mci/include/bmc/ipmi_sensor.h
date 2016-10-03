/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: ipmi_sensor.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */


#ifndef BMC_IPMI_SENSOR_H_
#define BMC_IPMI_SENSOR_H_

#include <memory>
#include <vector>
#include <mutex>
#include "bmc/sensor_reading.h"
#include "bmc/sensor_id.h"
#include "bmc/types.h"
#include "bmc/sensor.h"

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
    class IpmiSensor : public Sensor {
        public:
            int                     GetRecordType() const;
            SensorType_e            GetType() const;
            const std::string&      GetName() const;
            const std::string&      GetUnits() const;
            SensorReading           Nominal() const;
            SensorReading           LowNonCritical() const;
            SensorReading           LowCritical() const;
            SensorReading           LowNonRecover() const;
            SensorReading           HighNonCritical() const;
            SensorReading           HighCritical() const;
            SensorReading           HighNonRecover() const;

        private:
            explicit IpmiSensor(Board& a_board, const SensorId& a_id);
            virtual ~IpmiSensor();

            // copying not allowed
            IpmiSensor(const IpmiSensor& other);
            IpmiSensor& operator=(const IpmiSensor& other);

            virtual SensorReading Transform(const SensorReading& a_reading) const;
            bool UpdateSDR() const;

            // Parameters for conversion
            mutable std::unique_ptr<sensor_record>   m_sdr;
            mutable std::mutex                       m_updateSDR_x;

        friend class Board;
        friend struct istd::DeleteElement;

        // Test connection simulates sensor reads by manipulating the structures
        // directly (ugly...)
        friend class TestConnection;
    };
    /**@}*/
}


#endif /* BMC_IPMI_SENSOR_H_ */

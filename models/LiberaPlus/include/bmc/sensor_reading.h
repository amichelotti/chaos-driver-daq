/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: sensor_reading.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef BMC_SENSOR_READING_H
#define BMC_SENSOR_READING_H

#include "istd/time.h"
#include "bmc/types.h"

namespace bmc {

    class Sensor;

	/**
	 * @addtogroup bmc_sensor
	 * @{
	 */
    class SensorReading {
        public:
            SensorReading();

            bool                IsValid() const;

            SensorType_e        SensorType() const;
            uint32_t            Raw() const;
            double              Converted() const;
            const std::string&  DiscreteValue() const;
            const std::string   ToString() const;
            const istd::Time&   Timestamp() const;

        private:
            // Creates a valid reading with timestamp
            explicit SensorReading(const SensorType_e a_sensorType, uint32_t a_raw, const istd::Time& a_timestamp);

            // Creates a valid reading with timestamp
            explicit SensorReading(const SensorType_e a_sensorType, uint32_t a_raw, double a_converted, const istd::Time& a_timestamp);

            // Creates a valid reading with timestamp and performs conversion
            explicit SensorReading(const SensorType_e a_sensorType, uint32_t a_raw, const istd::Time& a_timestamp, const Sensor* a_sensor);

            // Creates a valid discrete reading with timestamp
            explicit SensorReading(const std::string& a_discreteValue, const istd::Time& a_timestamp);

            // Creates an invalid reading with timestamp
            SensorReading(const istd::Time& a_timestamp);

            bool            m_valid;
            istd::Time      m_timestamp;
            SensorType_e    m_sensorType;
            uint32_t        m_raw;
            std::string     m_discreteValue;
            double          m_converted;

            template<typename ...ARGS>
                friend SensorReading NewReading(ARGS&& ...args);

    };
    /**@}*/
}


#endif /* BMC_SENSOR_READING_H */

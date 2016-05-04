/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: sensor.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef BMC_SENSOR_H
#define BMC_SENSOR_H

#include <mutex>
#include "bmc/sensor_reading.h"
#include "bmc/sensor_id.h"
#include "bmc/types.h"

namespace istd {
    struct DeleteElement;
}

namespace bmc {

    class Board;
    class Connection;

	/**
	 * @addtogroup bmc_sensor
	 * @{
	 */
    class Sensor {
        public:

            virtual const SensorId&     GetId() const;
            virtual SensorType_e        GetType() const = 0;
            virtual const std::string&  GetName() const = 0;
            virtual const std::string&  GetUnits() const = 0;
            virtual SensorReading       Nominal() const;
            virtual SensorReading       LowNonCritical() const;
            virtual SensorReading       LowCritical() const;
            virtual SensorReading       LowNonRecover() const;
            virtual SensorReading       HighNonCritical() const;
            virtual SensorReading       HighCritical() const;
            virtual SensorReading       HighNonRecover() const;
            virtual SensorReading       FetchSensorReading() const;

            SensorLimit_e ReadingLimit(const SensorReading& a_reading) const;

            SensorReading Read() const;

            virtual SensorReading Transform(const SensorReading& a_reading) const;

            /** Minimum time between two consecutive reads of sensors from
             * the underlying device. Cached value is used otherwise.
             */
            static void SetReadingInterval(const istd::Time &a_period);

        protected:
            const SensorId m_id;

            Board         *m_board;

            mutable std::string m_name;
            mutable std::string m_units;

            virtual const istd::Time& MinReadInterval() const;

            Sensor(Board& a_board, const bmc::SensorId& a_id);
            virtual ~Sensor();

            Connection* GetConnection() const;

        private:

            // copying not allowed
            Sensor(const Sensor& other);
            Sensor& operator=(const Sensor& other);

            mutable SensorReading   m_lastReading;
            mutable std::mutex      m_reading_x;

            friend class Board;
            friend struct istd::DeleteElement;
//            friend void Transform(const Sensor *a_sensor, SensorReading& a_reading);
    };

//    void Transform(const Sensor *a_sensor, SensorReading& a_reading);

    /**@}*/
}

#endif /* BMC_SENSOR_H */

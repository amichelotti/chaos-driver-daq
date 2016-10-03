/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: os_sensor.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */


#ifndef BMC_OS_SENSOR_H_
#define BMC_OS_SENSOR_H_

#include "bmc/sensor_reading.h"
#include "bmc/types.h"
#include "bmc/sensor.h"

namespace bmc {

    class Board;

	/**
	 * @addtogroup bmc_sensor
	 * @{
	 */
    class OsSensor : public Sensor {

    public:
        SensorReading       GetSensorReading() const;
        SensorType_e        GetType() const;
        const std::string&  GetName() const;
        const std::string&  GetUnits() const;
        SensorReading       HighCritical() const;
        SensorReading       HighNonCritical() const;
        SensorReading       LowNonCritical() const;

    private:
        explicit OsSensor(Board& a_board, const SensorId& a_id);
        virtual ~OsSensor();

        virtual const istd::Time& MinReadInterval() const;

        SensorType_e m_type;

        double m_lnc;
        double m_hnc;
        double m_hc;

    friend class Board;
    friend struct istd::DeleteElement;

    };
    /**@}*/
}


#endif /* BMC_OS_SENSOR_H_ */

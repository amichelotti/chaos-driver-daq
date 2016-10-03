/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: sensor_id.h 8621 2010-09-14 05:54:19Z matej.kenda $
 */

#ifndef BMC_SENSOR_ID_H
#define BMC_SENSOR_ID_H

#include <vector>

namespace bmc {

	/**
	 * @addtogroup bmc_sensor
	 * @{
	 */
    class SensorId {
        public:
            SensorId(int a_num = -1) : m_num(a_num)
                {};

            operator int() const
                { return m_num; };

        private:
            int m_num;
    };
    /**@}*/

    typedef std::vector<SensorId> SensorIds;

}

#endif /* BMC_SENSOR_ID_H */

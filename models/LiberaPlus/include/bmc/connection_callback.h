/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: connection_callback.h 8621 2010-09-14 05:54:19Z matej.kenda $
 */

#ifndef BMC_CONNECTION_CALLBACK_H_
#define BMC_CONNECTION_CALLBACK_H_

// shared headers
#include "istd/time.h"

namespace bmc {

	/**
	 * @addtogroup istd_trace
	 * @{
	 */
    /**
     * Abstract connection callback class - interface.
     */
    class ConnectionCallback {

        public:
            virtual ~ConnectionCallback();

            virtual void UpdateUpgradeStatus(
                    int a_boardId,
                    unsigned int a_seqNum,
                    unsigned int a_totalSent,
                    unsigned int a_diffSent,
                    unsigned int a_fileSize,
                    istd::Time a_timeElapsed);
    };
    /**@}*/

}


#endif /* BMC_CONNECTION_CALLBACK_H_ */

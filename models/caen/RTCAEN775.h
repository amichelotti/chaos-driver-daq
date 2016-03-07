/*
 *	RTCAEN775.h
 *	!CHAOS
 *	Created by Andrea Michelotti
 *      Collects and align a given series of Libera BPMs
 *    	Copyright 2015 INFN, National Institute of Nuclear Physics
 *
 *    	Licensed under the Apache License, Version 2.0 (the "License");
 *    	you may not use this file except in compliance with the License.
 *    	You may obtain a copy of the License at
 *
 *    	http://www.apache.org/licenses/LICENSE-2.0
 *
 *    	Unless required by applicable law or agreed to in writing, software
 *    	distributed under the License is distributed on an "AS IS" BASIS,
 *    	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    	See the License for the specific language governing permissions and
 *    	limitations under the License.
 */
#ifndef _RTCAEN775_h
#define _RTCAEN775_h

#include <chaos/cu_toolkit/control_manager/RTAbstractControlUnit.h>
#include <common/vme/caen/caen775_drv.h>
#include "RTCAEN.h"
    namespace driver {
        namespace daq {
        namespace caen {
	  class RTCAEN775 : public RTCAEN {
		  PUBLISHABLE_CONTROL_UNIT_INTERFACE(RTCAEN775)
	  public:
    /*!
     Construct a new CU with full constructor
     */
	    RTCAEN775(const std::string& _control_unit_id, const std::string& _control_unit_param, const ControlUnitDriverList& _control_unit_drivers);
	    /*!
     Destructor a new CU
     */
    ~RTCAEN775();

protected:


public:

    void unitDefineActionAndDataset() throw(chaos::CException);
    void unitInit() throw(chaos::CException);


    void unitDeinit() throw(chaos::CException);
    void unitRun() throw(chaos::CException);
};
            }
        }
    }
#endif

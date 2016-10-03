/*
 *	RTCAEN965.h
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
#ifndef _RTCAEN965_h
#define _RTCAEN965_h

#include <chaos/cu_toolkit/control_manager/RTAbstractControlUnit.h>
#include <common/vme/caen/CaenBase.h>

#include "RTCAEN.h"
    namespace driver {
        namespace daq {
        namespace caen {
	  class RTCAEN965 : public RTCAEN< ::common::vme::caen::CaenBase> {
		  PUBLISHABLE_CONTROL_UNIT_INTERFACE(RTCAEN965)
	  public:
    /*!
     Construct a new CU with full constructor
     */
	    RTCAEN965(const std::string& _control_unit_id, const std::string& _control_unit_param, const ControlUnitDriverList& _control_unit_drivers):RTCAEN< ::common::vme::caen::CaenBase>(_control_unit_id,
					_control_unit_param,
					_control_unit_drivers) {}



protected:
    uint32_t* iped;
public:

    void unitDefineActionAndDataset() throw(chaos::CException);
    void unitInit() throw(chaos::CException);



};
            }
        }
    }
#endif

/*
 *	RTCAEN.h
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
#ifndef _RTCAEN_h
#define _RTCAEN_h

#include <chaos/cu_toolkit/control_manager/RTAbstractControlUnit.h>
#include <driver/misc/core/RTVme.h>
#include <common/vme/caen/CaenBase.h>
    namespace driver {
        namespace daq {
        namespace caen {
	  class RTCAEN : public ::driver::misc::RTVme {
		  PUBLISHABLE_CONTROL_UNIT_INTERFACE(RTCAEN)
	  public:
    /*!
     Construct a new CU with full constructor
     */
	    RTCAEN(const std::string& _control_unit_id, const std::string& _control_unit_param, const ControlUnitDriverList& _control_unit_drivers);
	    /*!
     Destructor a new CU
     */
    ~RTCAEN();

protected:
    uint64_t* events;
    uint64_t* acq_cycle;
    ::common::vme::caen::CaenBase* caen;
public:
    int32_t channels;
    int32_t crate_num;
    int32_t timeo_ms;
    void unitDefineActionAndDataset() throw(chaos::CException);
    void unitInit() throw(chaos::CException);

    void unitStart() throw(chaos::CException);
    void unitStop() throw(chaos::CException);
    void unitDeinit() throw(chaos::CException);
    void unitRun() throw(chaos::CException);
    void unitInputAttributeChangedHandler() throw(chaos::CException);
};
            }
        }
    }
#endif

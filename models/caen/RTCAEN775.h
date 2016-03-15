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
#include <common/vme/caen/Caen775.h>
#include "RTCAEN.h"
    namespace driver {
        namespace daq {
        namespace caen {
	  class RTCAEN775 : public RTCAEN< ::common::vme::caen::Caen775 > {
		  PUBLISHABLE_CONTROL_UNIT_INTERFACE(RTCAEN775);
	  public:
    /*!
     Construct a new CU with full constructor
     */
	    RTCAEN775(const std::string& _control_unit_id, const std::string& _control_unit_param, const ControlUnitDriverList& _control_unit_drivers):
	    	RTCAEN< ::common::vme::caen::Caen775 >(_control_unit_id,
	    	                        _control_unit_param,
	    	                        _control_unit_drivers) {


	    	}

protected:
    const uint32_t *fsr;

public:

    void unitDefineActionAndDataset() throw(chaos::CException);
    void unitInit() throw(chaos::CException);
    //actions
    bool setFsr(const std::string &name,int32_t value,uint32_t size){
    	DPRINT("set FSR 0x%x",value);
    	caen->setFSR(value);
    return true;
    }

};
            }
        }
    }
#endif

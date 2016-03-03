/*
 *	RTCAEN.cpp
 *	!CHAOS
 *	Andrea Michelotti
 *
 *    	Copyright 2012 INFN, National Institute of Nuclear Physics
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

#include "RTCAEN.h"
#include <stdlib.h>
#include <boost/algorithm/string.hpp>
#include <chaos/ui_toolkit/LowLevelApi/LLRpcApi.h>
#include <chaos/cu_toolkit/command_manager/CommandManager.h>
#include <common/debug/core/debug.h>

#include <chaos/common/utility/TimingUtil.h>
using namespace chaos;
using namespace chaos::common::data::cache;
using namespace chaos::common::utility;
using namespace chaos::cu::driver_manager::driver;
using namespace ::driver::daq::caen;
PUBLISHABLE_CONTROL_UNIT_IMPLEMENTATION(::driver::daq::caen::RTCAEN)

#define RTCAENLAPP_		LAPP_ << "[RTDataSync] "
#define RTCAENLDBG_		LDBG_ << "[RTDataSync] " << __PRETTY_FUNCTION__ << " "
#define RTCAENLERR_		LERR_ << "[RTDataSync] " << __PRETTY_FUNCTION__ << "("<<__LINE__<<") "



/*
 Construct
 */
RTCAEN::RTCAEN(const string& _control_unit_id,
                        const string& _control_unit_param,
                        const ControlUnitDriverList& _control_unit_drivers):
driver::misc::RTVme(_control_unit_id,
                        "",
                        _control_unit_drivers) {


}

void RTCAEN::unitInputAttributeChangedHandler() throw(CException) {
    //r_o_attr_lock->lock();
    std::vector<VariableIndexType> changed_idx;
    getAttributeCache()->getChangedInputAttributeIndex(changed_idx);
    if(changed_idx.size()){
        //something has been changed
        for(std::vector<VariableIndexType>::iterator it = changed_idx.begin();
            it != changed_idx.end();
            it++) {
            switch(*it) {
                case 0:
                    break;

                default:
                    break;
            }
        }
    }
}

void RTCAEN::unitDefineActionAndDataset() throw(chaos::CException) {

	driver::misc::RTVme::unitDefineActionAndDataset();

	addAttributeToDataSet("CHANNELS",
	                        "Number of channels available",
	                        DataType::TYPE_INT64,
	                        DataType::Input);

	addAttributeToDataSet("CRATE_NUM",
	                        "Crate Number",
	                        DataType::TYPE_INT32,
	                        DataType::Input);

	addAttributeToDataSet("TIMEOUT",
		                        "Timeout acquisition in ms (0=no timeout)",
		                        DataType::TYPE_INT32,
		                        DataType::Input);
	for(int cnt=0;cnt<32;cnt++){
		std::stringstream ss;
		ss<<"THRESHOLD"<<cnt;
		addAttributeToDataSet(ss.str().c_str(),
				                        "Threshold memory",
				                        DataType::TYPE_INT32,
				                        DataType::Input);
	}
	addAttributeToDataSet("BITSET2",
					                        "Operational control mode",
					                        DataType::TYPE_INT32,
					                        DataType::Input);
    addAttributeToDataSet("ACQ_CYCLE",
                        "Acquisition cycle",
                        DataType::TYPE_INT64,
                        DataType::Output);

    addAttributeToDataSet("EVENTS",
                            "Events counted",
                            DataType::TYPE_INT64,
                            DataType::Output);

// actions

}


 void RTCAEN::unitInit() throw(chaos::CException){
	 vme_base_address = *getAttributeCache()->getROPtr<uint64_t>(DOMAIN_INPUT, "VME_BASE");
	 channels = *getAttributeCache()->getROPtr<uint32_t>(DOMAIN_INPUT, "CHANNELS");
	 timeo_ms = *getAttributeCache()->getROPtr<int32_t>(DOMAIN_INPUT, "TIMEOUT");
	 crate_num = *getAttributeCache()->getROPtr<int32_t>(DOMAIN_INPUT, "CRATE_NUM");

	 if((channels<=0)||(vme_base_address==0)){
		 throw CException(-1,__PRETTY_FUNCTION__,"invalid CHANNELS or VME_BASE ");
	 }

	 events=getAttributeCache()->getRWPtr<uint64_t>(DOMAIN_OUTPUT, "EVENTS");
	 acq_cycle=getAttributeCache()->getRWPtr<uint64_t>(DOMAIN_OUTPUT, "ACQ_CYCLE");

	 

 }
 
 void RTCAEN::unitStart() throw(chaos::CException){


 }
 void RTCAEN::unitStop() throw(chaos::CException){

 }
 void RTCAEN::unitDeinit() throw(chaos::CException){

 }
 void RTCAEN::unitRun() throw(chaos::CException){

 }
 
/*
 Destructor
 */
RTCAEN::~RTCAEN() {
	unitDeinit();

}

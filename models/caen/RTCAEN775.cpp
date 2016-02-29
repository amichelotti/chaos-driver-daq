/*
 *	RTCAEN775.cpp
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

#include "RTCAEN775.h"
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
using namespace driver::daq::caen;
PUBLISHABLE_CONTROL_UNIT_IMPLEMENTATION(RTCAEN775)

#define RTCAEN775LAPP_		LAPP_ << "[RTDataSync] "
#define RTCAEN775LDBG_		LDBG_ << "[RTDataSync] " << __PRETTY_FUNCTION__ << " "
#define RTCAEN775LERR_		LERR_ << "[RTDataSync] " << __PRETTY_FUNCTION__ << "("<<__LINE__<<") "



/*
 Construct
 */
RTCAEN775::RTCAEN775(const string& _control_unit_id,
                        const string& _control_unit_param,
                        const ControlUnitDriverList& _control_unit_drivers):
driver::misc::RTVme(_control_unit_id,
                        _control_unit_param,
                        _control_unit_drivers) {
	 caen=NULL;


}
void RTCAEN775::unitDefineActionAndDataset() throw(chaos::CException) {

	driver::misc::RTVme::unitDefineActionAndDataset();

	addAttributeToDataSet("CHANNELS",
	                        "Number of channels available",
	                        DataType::TYPE_INT64,
	                        DataType::Input);

	addAttributeToDataSet("TIMEOUT",
		                        "Timeout acquisition in ms (0=no timeout)",
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

    addAttributeToDataSet("CH",
                            "Event number",
                            DataType::TYPE_INT64,
                            DataType::Output);

    addBinaryAttributeAsSubtypeToDataSet("CH","Vector of acquired channels",chaos::DataType::SUB_TYPE_INT32,32*sizeof(int32_t),chaos::DataType::Output);


}


 void RTCAEN775::unitInit() throw(chaos::CException){
	 vme_base_address = *getAttributeCache()->getROPtr<uint64_t>(DOMAIN_INPUT, "VME_BASE");
	 channels = *getAttributeCache()->getROPtr<uint32_t>(DOMAIN_INPUT, "CHANNELS");
	 if((channels<=0)||(vme_base_address==0)){
		 throw CException(-1,__PRETTY_FUNCTION__,"invalid CHANNELS or VME_BASE ");
	 }

	 getAttributeCache()->setOutputAttributeNewSize("CH", channels*sizeof(int32_t));
	 caen = caen775_open(vme_base_address);
	 if(caen==NULL){
		 throw CException(-1,__PRETTY_FUNCTION__,"cannot open CAEN775");

	 }
	 chp=getAttributeCache()->getRWPtr<uint32_t>(DOMAIN_OUTPUT, "CH");
	 timeo_ms=getAttributeCache()->getRWPtr<int32_t>(DOMAIN_INPUT, "TIMEOUT");
	 events=getAttributeCache()->getRWPtr<uint64_t>(DOMAIN_OUTPUT, "EVENTS");
	 acq_cycle=getAttributeCache()->getRWPtr<uint64_t>(DOMAIN_OUTPUT, "ACQ_CYCLE");


	 if((chp==NULL)||(events==NULL) || (acq_cycle==NULL) || (timeo_ms==NULL)){
		 throw CException(-1,__PRETTY_FUNCTION__,"cannot retrive dataset pointers");

	 }
 }
 
 void RTCAEN775::unitStart() throw(chaos::CException){


 }
 void RTCAEN775::unitStop() throw(chaos::CException){

 }
 void RTCAEN775::unitDeinit() throw(chaos::CException){
	 if(caen){
		 caen775_close(caen);
		 caen=NULL;
	 }
 }
 void RTCAEN775::unitRun() throw(chaos::CException){

	 int ret;
	 ret = caen775_acquire_channels_poll(caen,chp,events,*timeo_ms);


	 DPRINT("* acquired %d channels, events:%llu, loop %llu\n",ret,*events,*acq_cycle);
	 if(ret){
         getAttributeCache()->setOutputDomainAsChanged();

	 }
	 (*acq_cycle)++;
 }
 
/*
 Destructor
 */
RTCAEN775::~RTCAEN775() {
	unitDeinit();

}

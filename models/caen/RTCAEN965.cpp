/*
 *	RTCAEN965.cpp
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

#include "RTCAEN965.h"
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
PUBLISHABLE_CONTROL_UNIT_IMPLEMENTATION(::driver::daq::caen::RTCAEN965)

#define RTCAEN965LAPP_		LAPP_ << "[RTCAEN965] "
#define RTCAEN965LDBG_		LDBG_ << "[RTCAEN965] " << __PRETTY_FUNCTION__ << " "
#define RTCAEN965LERR_		LERR_ << "[RTCAEN965] " << __PRETTY_FUNCTION__ << "("<<__LINE__<<") "



/*
 Construct
 */
RTCAEN965::RTCAEN965(const string& _control_unit_id,
                        const string& _control_unit_param,
                        const ControlUnitDriverList& _control_unit_drivers):
RTCAEN(_control_unit_id,
                        _control_unit_param,
                        _control_unit_drivers) {
	 caen=NULL;


}
void RTCAEN965::unitDefineActionAndDataset() throw(chaos::CException) {

	driver::daq::caen::RTCAEN::unitDefineActionAndDataset();




    addBinaryAttributeAsSubtypeToDataSet("CHL","Vector of acquired channels (Low Resolution)",chaos::DataType::SUB_TYPE_INT32,16*sizeof(int32_t),chaos::DataType::Output);
    addBinaryAttributeAsSubtypeToDataSet("CHH","Vector of acquired channels (High Resolution)",chaos::DataType::SUB_TYPE_INT32,16*sizeof(int32_t),chaos::DataType::Output);

}


 void RTCAEN965::unitInit() throw(chaos::CException){
	 driver::daq::caen::RTCAEN::unitInit();


	 getAttributeCache()->setOutputAttributeNewSize("CHL", channels*sizeof(int32_t));
	 getAttributeCache()->setOutputAttributeNewSize("CHH", channels*sizeof(int32_t));
	 caen = caen965_open(vme_base_address);
	 if(caen==NULL){
		 throw CException(-1,__PRETTY_FUNCTION__,"cannot open CAEN775");

	 }
	 chpl=getAttributeCache()->getRWPtr<uint32_t>(DOMAIN_OUTPUT, "CHL");
	 chph=getAttributeCache()->getRWPtr<uint32_t>(DOMAIN_OUTPUT, "CHH");
	 caen965_init(caen,crate_num,true);
 }
 
 void RTCAEN965::unitStart() throw(chaos::CException){


 }
 void RTCAEN965::unitStop() throw(chaos::CException){

 }
 void RTCAEN965::unitDeinit() throw(chaos::CException){
	 if(caen){
		 caen965_close(caen);
		 caen=NULL;
	 }
 }
 void RTCAEN965::unitRun() throw(chaos::CException){

	 int ret;

	 ret = caen965_acquire_channels_poll(caen,chpl,chph,0,channels,events,timeo_ms);

	 DPRINT("* acquired %d channels, events:%llu, loop %llu",ret,*events,*acq_cycle);
	 if(ret){
         getAttributeCache()->setOutputDomainAsChanged();

	 }
	 (*acq_cycle)++;
 }
 
/*
 Destructor
 */
RTCAEN965::~RTCAEN965() {
	unitDeinit();

}

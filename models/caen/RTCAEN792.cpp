/*
 *	RTCAEN792.cpp
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

#include "RTCAEN792.h"
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
PUBLISHABLE_CONTROL_UNIT_IMPLEMENTATION(::driver::daq::caen::RTCAEN792)

#define RTCAEN792LAPP_		LAPP_ << "[RTCAEN792] "
#define RTCAEN792LDBG_		LDBG_ << "[RTCAEN792] " << __PRETTY_FUNCTION__ << " "
#define RTCAEN792LERR_		LERR_ << "[RTCAEN792] " << __PRETTY_FUNCTION__ << "("<<__LINE__<<") "



/*
 Construct
 */
RTCAEN792::RTCAEN792(const string& _control_unit_id,
                        const string& _control_unit_param,
                        const ControlUnitDriverList& _control_unit_drivers):
RTCAEN(_control_unit_id,
                        _control_unit_param,
                        _control_unit_drivers) {
	 caen=NULL;


}
void RTCAEN792::unitDefineActionAndDataset() throw(chaos::CException) {

	driver::daq::caen::RTCAEN::unitDefineActionAndDataset();




    addBinaryAttributeAsSubtypeToDataSet("CH","Vector of acquired channels (Low Resolution)",chaos::DataType::SUB_TYPE_INT32,32*sizeof(int32_t),chaos::DataType::Output);

}


 void RTCAEN792::unitInit() throw(chaos::CException){
	 driver::daq::caen::RTCAEN::unitInit();


	 getAttributeCache()->setOutputAttributeNewSize("CH", channels*sizeof(int32_t));
	 getAttributeCache()->setOutputAttributeNewSize("CHH", channels*sizeof(int32_t));
	 caen = caen965_open(vme_base_address);
	 if(caen==NULL){
		 throw CException(-1,__PRETTY_FUNCTION__,"cannot open CAEN792");

	 }
	 chp=getAttributeCache()->getRWPtr<uint32_t>(DOMAIN_OUTPUT, "CH");
	 caen965_init(caen,crate_num,true);
 }
 
 void RTCAEN792::unitStart() throw(chaos::CException){


 }
 void RTCAEN792::unitStop() throw(chaos::CException){

 }
 void RTCAEN792::unitDeinit() throw(chaos::CException){
	 if(caen){
		 caen965_close(caen);
		 caen=NULL;
	 }
 }
 void RTCAEN792::unitRun() throw(chaos::CException){

	 int ret;

	 ret = caen965_acquire_channels_poll(caen,chp,0,0,channels,events,timeo_ms);

	 DPRINT("* acquired %d channels, events:%llu, loop %llu",ret,*events,*acq_cycle);
	 if(ret){
         getAttributeCache()->setOutputDomainAsChanged();

	 }
	 (*acq_cycle)++;
 }
 
/*
 Destructor
 */
RTCAEN792::~RTCAEN792() {
	unitDeinit();

}

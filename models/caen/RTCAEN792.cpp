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
RTCAEN< ::common::vme::caen::CaenDaqBase>(_control_unit_id,
                        _control_unit_param,
                        _control_unit_drivers) {


}
void RTCAEN792::unitDefineActionAndDataset() throw(chaos::CException) {
	::driver::daq::caen::RTCAEN< ::common::vme::caen::CaenDaqBase>::unitDefineActionAndDataset();


		addAttributeToDataSet("IPED",
				"IPED QDC pedestal",
				DataType::TYPE_INT32,
				DataType::Input);
}


 void RTCAEN792::unitInit() throw(chaos::CException){
	 AttributeSharedCacheWrapper * cc=getAttributeCache();

	 	 ::driver::daq::caen::RTCAEN< ::common::vme::caen::CaenDaqBase >::unitInit();
	 	 iped = (cc->getRWPtr< uint32_t >(chaos::common::data::cache::DOMAIN_INPUT, "IPED"));
 }
 


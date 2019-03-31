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
#include <chaos/cu_toolkit/command_manager/CommandManager.h>
#include <common/debug/core/debug.h>
#include <common/vme/caen/Caen775.h>
#include <chaos/common/utility/TimingUtil.h>
using namespace chaos;
using namespace chaos::common::data::cache;
using namespace chaos::common::utility;
using namespace chaos::cu::driver_manager::driver;
using namespace ::driver::daq::caen;
PUBLISHABLE_CONTROL_UNIT_IMPLEMENTATION(::driver::daq::caen::RTCAEN775)

#define RTCAEN775LAPP_		LAPP_ << "[RTCAEN775] "
#define RTCAEN775LDBG_		LDBG_ << "[RTCAEN775] " << __PRETTY_FUNCTION__ << " "
#define RTCAEN775LERR_		LERR_ << "[RTCAEN775] " << __PRETTY_FUNCTION__ << "("<<__LINE__<<") "




void RTCAEN775::unitDefineActionAndDataset() throw(chaos::CException) {
    ::driver::daq::caen::RTCAEN< ::common::vme::caen::Caen775 >::unitDefineActionAndDataset();

    addAttributeToDataSet("FSR",
                          "FULL SCALE REGISTER TLSB=8.9/N (ns)",
                          DataType::TYPE_INT32,
                          DataType::Input);

    addHandlerOnInputAttributeName<RTCAEN775, int32_t>(this,
                                                       &RTCAEN775::setFsr,
                                                       "FSR");


}
void  RTCAEN775::unitInit() throw(chaos::CException){
    AttributeSharedCacheWrapper * cc=getAttributeCache();
    ::driver::daq::caen::RTCAEN< ::common::vme::caen::Caen775 >::unitInit();
    fsr = (uint32_t*)(cc->getROPtr< uint32_t >(chaos::common::data::cache::DOMAIN_INPUT, "FSR"));
    DPRINT("input FSR=0x%x",*fsr);
    caen->setFSR(*fsr);
    *fsr = caen->getFSR();
    DPRINT("read FSR=0x%x",*fsr);


}




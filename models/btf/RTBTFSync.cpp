/*
 *	RTBTFSync.cpp
 *	!CHAOS
 *	Created automatically
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

#include "RTBTFSync.h"
//#include "RTBTFSyncDriver.h"
#include <boost/algorithm/string.hpp>
#include <chaos/ui_toolkit/LowLevelApi/LLRpcApi.h>
#include <chaos/cu_toolkit/command_manager/CommandManager.h>
#include <common/debug/core/debug.h>
using namespace chaos;
using namespace chaos::common::data::cache;
using namespace chaos::cu::driver_manager::driver;
using namespace driver::daq::btf;
using namespace driver::misc;
PUBLISHABLE_CONTROL_UNIT_IMPLEMENTATION(RTBTFSync)


#define RTBTFSyncLAPP_		LAPP_ << "[RTBTFSync] "
#define RTBTFSyncLDBG_		LDBG_ << "[RTBTFSync] " << __PRETTY_FUNCTION__ << " "
#define RTBTFSyncLERR_		LERR_ << "[RTBTFSync] " << __PRETTY_FUNCTION__ << "("<<__LINE__<<") "



/*
 Construct
 */
RTBTFSync::RTBTFSync(const string& _control_unit_id, const string& _control_unit_param, const ControlUnitDriverList& _control_unit_drivers):
driver::misc::RTDataSync(_control_unit_id,_control_unit_param,_control_unit_drivers) {
    
    
}


    

/*
 Destructor
 */
RTBTFSync::~RTBTFSync() {

}

void RTBTFSync::unitDefineActionAndDataset() throw(chaos::CException) {
     addAttributeToDataSet("memcacheurl",
						  "Memcache server",
						  DataType::TYPE_STRING,
						  DataType::Input);
     
     addAttributeToDataSet("memcachekey",
						  "Memcache output key",
						  DataType::TYPE_STRING,
						  DataType::Input);
     
    RTDataSync::unitDefineActionAndDataset();
    
   
}



//!Initialize the Custom Control Unit
void RTBTFSync::unitInit() throw(chaos::CException) {
    char*url=getAttributeCache()->getROPtr<char>(DOMAIN_INPUT, "memcacheurl");
    char*key=getAttributeCache()->getROPtr<char>(DOMAIN_INPUT, "memcachekey");
    if(url == NULL || key ==NULL){
        throw chaos::CException(-1,__PRETTY_FUNCTION__, "cannot find 'memcacheurl' or 'memcachekey' required initialization  ");
    }
    server_url.assign(url);
    server_key.assign(key);


}

void RTBTFSync::unitRun() throw(chaos::CException) {
  
}
//!Execute the work, this is called with a determinated delay, it must be as fast as possible
void RTBTFSync::unitStart() throw(chaos::CException) {
 
}
//!Execute the Control Unit work
void RTBTFSync::unitStop() throw(chaos::CException) {

}

//!Deinit the Control Unit
void RTBTFSync::unitDeinit() throw(chaos::CException) {

}

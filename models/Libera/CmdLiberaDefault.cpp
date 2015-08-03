/*
 *	CmdLiberaDefault.cpp
 *	!CHAOS
 *	Created by Andrea Michelotti
 *
 *    	Copyright 2013 INFN, National Institute of Nuclear Physics
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

#include <string.h>
#include "CmdLiberaDefault.h"


#define CMDCU_ LAPP_ << "[CmdLiberaDefault]"
#define CMDCUDBG LDBG_ << "[CmdLiberaDefault]"

namespace chaos_batch = chaos::common::batch_command;
using namespace chaos::common::data;
using namespace chaos::common::batch_command;
using namespace chaos::cu::control_manager::slow_command;
using namespace driver::daq::libera;
CmdLiberaDefault::CmdLiberaDefault() {
  CMDCUDBG<< "Created command default:"<<driverAccessorsErogator;
  driver =NULL;
}

CmdLiberaDefault::~CmdLiberaDefault() {
    if(driver){
        delete driver;
        driver = NULL;
    }

}

    // return the implemented handler
uint8_t CmdLiberaDefault::implementedHandler() {
        //add to default hadnler the acquisition one
    return chaos_batch::HandlerType::HT_Set  | HandlerType::HT_Acquisition;

}

    // Start the command execution
void CmdLiberaDefault::setHandler(c_data::CDataWrapper *data) {

	setFeatures(features::FeaturesFlagTypes::FF_SET_SCHEDULER_DELAY, (uint64_t)1000000);
	chaos::cu::driver_manager::driver::DriverAccessor * accessor=driverAccessorsErogator->getAccessoInstanceByIndex(0);
  	CMDCUDBG<< "Created accessor:"<<accessor;
	if(accessor==NULL){
		throw chaos::CException(-1, "Cannot retrieve the requested driver", __FUNCTION__);
	}
	driver = new chaos::cu::driver_manager::driver::BasicIODriverInterface(accessor);

	if(driver==NULL){
		throw chaos::CException(-2, "Cannot allocate driver resources", __FUNCTION__);
	}
	CMDCUDBG<< "retrived BasicIODriver:"<<driver;

	 int32_t *perr=getAttributeCache()->getRWPtr<int32_t>(DOMAIN_OUTPUT, "error");
        *perr=0;

	BC_NORMAL_RUNNIG_PROPERTY

}

    // Aquire the necessary data for the command
/*!
 The acquire handler has the purpose to get all necessary data need the by CC handler.
 \return the mask for the runnign state
 */
void CmdLiberaDefault::acquireHandler() {

	CMDCUDBG << "Default Acquiring libera status";
	char * status= getAttributeCache()->getRWPtr<char>(DOMAIN_OUTPUT, "STATUS");
	if(driver->iop(LIBERA_IOP_CMD_GETENV,status,MAX_STRING)==0){
            CMDCUDBG<<"STATUS:"<<status;
            getAttributeCache()->setOutputDomainAsChanged();
        }
	//force output dataset as changed
}

//
//  CmdLiberaTime.cpp
//  
//
//  Created by Andrea Michelotti 15/5/2015
//  Copyright (c) 2013 infn. All rights reserved.
//

#include "CmdLiberaTime.h"

#include <boost/format.hpp>
#include <chaos/common/data/cache/AbstractSharedDomainCache.h>
using namespace chaos::common::data::cache;


namespace c_data = chaos::common::data;
namespace chaos_batch = chaos::common::batch_command;
BATCH_COMMAND_OPEN_DESCRIPTION_ALIAS(driver::daq::libera::,CmdLiberaTime,"time","setting time","73882f3e-36db-11e5-985f-334fcd6dff22")
BATCH_COMMAND_ADD_STRING_PARAM("time", "string time",chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_MANDATORY)

BATCH_COMMAND_CLOSE_DESCRIPTION()
// return the implemented handler
uint8_t driver::daq::libera::CmdLiberaTime::implementedHandler() {
    return chaos_batch::HandlerType::HT_Set  ;
}


driver::daq::libera::CmdLiberaTime::CmdLiberaTime():CmdLiberaDefault(){
}
driver::daq::libera::CmdLiberaTime::~CmdLiberaTime(){
}
void driver::daq::libera::CmdLiberaTime::acquireHandler() {
    CMDCUDBG_<<"Acquire not implemented ";
     BC_END_RUNNING_PROPERTY;

}

void driver::daq::libera::CmdLiberaTime::setHandler(c_data::CDataWrapper *data) {
    int32_t*perr;
    int ret;
    CmdLiberaDefault::setHandler(data);
       CMDCUDBG_<<"Setting time...";

     perr=getAttributeCache()->getRWPtr<int32_t>(DOMAIN_OUTPUT, "error");
        *perr=0;
        
        if(data->hasKey("time" )) {
            
            const char* t=data->getCStringValue("time");
            CMDCUDBG_<<"Setting time to:"<<t;

            if((ret=driver->iop(LIBERA_IOP_CMD_SETTIME,(void*)t,strlen(t)+1))!=0){
                *perr|=LIBERA_ERROR_SETTING_ENV;
              
                BC_END_RUNNING_PROPERTY;
                CMDCUERR_<<"cannot set time:\""<<t<<"\" ret: "<<ret;
            }else {
                 CMDCUERR_<<"setting time ok:\""<<t<<"\" ret: "<<ret;
            }
        }
        
          getAttributeCache()->setOutputDomainAsChanged();
        
}

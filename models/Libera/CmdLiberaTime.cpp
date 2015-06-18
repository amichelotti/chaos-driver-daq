//
//  CmdLiberaTime.cpp
//  
//
//  Created by Andrea Michelotti 15/5/2015
//  Copyright (c) 2013 infn. All rights reserved.
//

#include "CmdLiberaTime.h"

#include <boost/format.hpp>
#define CMDCU_ LAPP_ << "[CmdLiberaTime]"
#define CMDCUDBG_ LDBG_ <<"[CmdLiberaTime]"
#define CMDCUERR_ LERR_ <<"[CmdLiberaTime]"


namespace c_data = chaos::common::data;
namespace chaos_batch = chaos::common::batch_command;

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
     BC_END_RUNNIG_PROPERTY;

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
              
                BC_END_RUNNIG_PROPERTY;
                CMDCUERR_<<"cannot set time:\""<<t<<"\" ret: "<<ret;
            }else {
                 CMDCUERR_<<"setting time ok:\""<<t<<"\" ret: "<<ret;
            }
        }
        
          getAttributeCache()->setOutputDomainAsChanged();
        
}
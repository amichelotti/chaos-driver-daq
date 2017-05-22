//
//  CmdLiberaEnv.cpp
//  Commands to set environment
//
//  Created by Andrea Michelotti on 11/06/15.
//  Copyright (c) 2013 infn. All rights reserved.
//

#include "CmdLiberaEnv.h"

#include <boost/format.hpp>
#include <chaos/common/data/cache/AbstractSharedDomainCache.h>
using namespace chaos::common::data::cache;

namespace c_data = chaos::common::data;
namespace chaos_batch = chaos::common::batch_command;

uint8_t driver::daq::libera::CmdLiberaEnv::implementedHandler() {
    return chaos_batch::HandlerType::HT_Set  ;
}


driver::daq::libera::CmdLiberaEnv::CmdLiberaEnv():CmdLiberaDefault(){
}
driver::daq::libera::CmdLiberaEnv::~CmdLiberaEnv(){
}
void driver::daq::libera::CmdLiberaEnv::acquireHandler() {
    CMDCUDBG_<<"Acquire not implemented ";

}
void driver::daq::libera::CmdLiberaEnv::setHandler(c_data::CDataWrapper *data) {
	int32_t *perr;
        int ret;
        
        CmdLiberaDefault::setHandler(data);
//        setFeatures(features::FeaturesFlagTypes::FF_SET_SCHEDULER_DELAY, (uint64_t)1000000);

        
#define ADD_ENV_PARAM(param) \
CMDCUDBG_<<"checking environment "<< # param; \
        if(data->hasKey(# param )) {\
            libera_env_t env;\
            env.value = data->getInt32Value(# param);\
            env.selector=CSPI_ENV_## param;\
            CMDCUDBG_<<"Setting env \""<< # param <<"\" ("<<std::hex<<env.selector<<std::dec<<")="<<env.value ;\
            if((ret=driver->iop(LIBERA_IOP_CMD_SETENV,&env,sizeof(libera_env_t)))!=0){\
                *perr|=LIBERA_ERROR_SETTING_ENV;\
                getAttributeCache()->setOutputDomainAsChanged();\
                BC_END_RUNNING_PROPERTY;\
                throw chaos::CException(ret, "Cannot set environment", __FUNCTION__);\
            }\
            CMDCUDBG_<<"Sucessfully applied \""<< # param <<"\" ("<<std::hex<<env.selector<<std::dec<<")="<<env.value ;\
	}
        
        perr=getAttributeCache()->getRWPtr<int32_t>(DOMAIN_OUTPUT, "error");
        *perr=0;
       
         if((ret=driver->iop(LIBERA_IOP_CMD_STOP,0,0))!=0){
            *perr|=LIBERA_ERROR_STOP_ACQUIRE;
            getAttributeCache()->setOutputDomainAsChanged();

            BC_END_RUNNING_PROPERTY;
            throw chaos::CException(ret, "Cannot stop acquire", __FUNCTION__);
        }
        
        ADD_ENV_PARAM(TRIGMODE);
        ADD_ENV_PARAM(KX);
        ADD_ENV_PARAM(KY);
        ADD_ENV_PARAM(XOFFSET);
        ADD_ENV_PARAM(YOFFSET);
        ADD_ENV_PARAM(QOFFSET);
        ADD_ENV_PARAM(SWITCH);
        ADD_ENV_PARAM(GAIN);
        ADD_ENV_PARAM(AGC);
        ADD_ENV_PARAM(DSC);
        ADD_ENV_PARAM(PMOFFSET);
        ADD_ENV_PARAM(PMDEC);
        ADD_ENV_PARAM(TRIGDELAY);
        ADD_ENV_PARAM(EXTSWITCH);
        ADD_ENV_PARAM(SWDELAY);
        ADD_ENV_PARAM(DDC_MAFLENGTH);
        ADD_ENV_PARAM(DDC_MAFDELAY);
        ADD_ENV_PARAM(NOTCH1);
        ADD_ENV_PARAM(NOTCH2);
        ADD_ENV_PARAM(POLYPHASE_FIR);
        ADD_ENV_PARAM(MTVCXOFFS);
        ADD_ENV_PARAM(MTNCOSHFT);
        ADD_ENV_PARAM(MTPHSOFFS);
        ADD_ENV_PARAM(MTUNLCKTR);
        ADD_ENV_PARAM(MTSYNCIN);
        ADD_ENV_PARAM(STUNLCKTR);
        ADD_ENV_PARAM(PM);
        ADD_ENV_PARAM(SR);
        ADD_ENV_PARAM(SP);
        
        char * status= getAttributeCache()->getRWPtr<char>(DOMAIN_OUTPUT, "STATUS");
	if(driver->iop(LIBERA_IOP_CMD_GETENV,status,MAX_STRING)==0){
            CMDCUDBG_<<"AFTER ENV:STATUS:"<<status;
            getAttributeCache()->setOutputDomainAsChanged();
        }
        BC_END_RUNNING_PROPERTY;
}

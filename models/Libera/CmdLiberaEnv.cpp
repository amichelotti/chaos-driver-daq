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
using namespace chaos::cu::control_manager;
namespace c_data = chaos::common::data;
namespace chaos_batch = chaos::common::batch_command;
BATCH_COMMAND_OPEN_DESCRIPTION_ALIAS(driver::daq::libera::,CmdLiberaEnv,"env","env command","72872f3e-36db-11e5-985f-334fcd6dff22")
BATCH_COMMAND_ADD_INT32_PARAM("TRIGMODE", "trigger mode",chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_OPTIONAL)
BATCH_COMMAND_ADD_INT32_PARAM("KX", "KX",chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_OPTIONAL)
BATCH_COMMAND_ADD_INT32_PARAM("KY", "KY",chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_OPTIONAL)
BATCH_COMMAND_ADD_INT32_PARAM("XOFFSET", "XOFFSET",chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_OPTIONAL)
BATCH_COMMAND_ADD_INT32_PARAM("YOFFSET", "YOFFSET",chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_OPTIONAL)
BATCH_COMMAND_ADD_INT32_PARAM("QOFFSET", "QOFFSET",chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_OPTIONAL)
BATCH_COMMAND_ADD_INT32_PARAM("SWITCH", "SWITCH",chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_OPTIONAL)
BATCH_COMMAND_ADD_INT32_PARAM("GAIN", "GAIN",chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_OPTIONAL)
BATCH_COMMAND_ADD_INT32_PARAM("AGC", "AGC",chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_OPTIONAL)
BATCH_COMMAND_ADD_INT32_PARAM("DSC", "DSC",chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_OPTIONAL)
BATCH_COMMAND_ADD_INT32_PARAM("PMOFFSET", "PMOFFSET",chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_OPTIONAL)
BATCH_COMMAND_ADD_INT32_PARAM("PMDEC", "PMDEC",chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_OPTIONAL)
BATCH_COMMAND_ADD_INT32_PARAM("TRIGDELAY", "TRIGDELAY",chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_OPTIONAL)
BATCH_COMMAND_ADD_INT32_PARAM("EXTSWITCH", "EXTSWITCH",chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_OPTIONAL)
BATCH_COMMAND_ADD_INT32_PARAM("SWDELAY", "SWDELAY",chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_OPTIONAL)
BATCH_COMMAND_ADD_INT32_PARAM("DDC_MAFLENGTH", "DDC_MAFLENGTH",chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_OPTIONAL)
BATCH_COMMAND_ADD_INT32_PARAM("DDC_MAFDELAY", "DDC_MAFDELAY",chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_OPTIONAL)
BATCH_COMMAND_ADD_INT32_PARAM("NOTCH1", "NOTCH1",chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_OPTIONAL)
BATCH_COMMAND_ADD_INT32_PARAM("NOTCH2", "NOTCH2",chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_OPTIONAL)
BATCH_COMMAND_ADD_INT32_PARAM("POLYPHASE_FIR", "POLYPHASE_FIR",chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_OPTIONAL)
BATCH_COMMAND_ADD_INT32_PARAM("MTVCXOFFS", "MTVCXOFFS",chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_OPTIONAL)
BATCH_COMMAND_ADD_INT32_PARAM("MTNCOSHFT", "MTNCOSHFT",chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_OPTIONAL)
BATCH_COMMAND_ADD_INT32_PARAM("MTVCXOFFS", "MTVCXOFFS",chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_OPTIONAL)
BATCH_COMMAND_ADD_INT32_PARAM("MTPHSOFFS", "MTPHSOFFS",chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_OPTIONAL)
BATCH_COMMAND_ADD_INT32_PARAM("MTUNLCKTR", "MTUNLCKTR",chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_OPTIONAL)
BATCH_COMMAND_ADD_INT32_PARAM("MTSYNCIN", "MTSYNCIN",chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_OPTIONAL)
BATCH_COMMAND_ADD_INT32_PARAM("STUNLCKTR", "STUNLCKTR",chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_OPTIONAL)
BATCH_COMMAND_ADD_INT32_PARAM("PM", "PM",chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_OPTIONAL)
BATCH_COMMAND_ADD_INT32_PARAM("SR", "SR",chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_OPTIONAL)
BATCH_COMMAND_ADD_INT32_PARAM("SP", "SP",chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_OPTIONAL)
BATCH_COMMAND_CLOSE_DESCRIPTION()

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
        int ret;
        
        CmdLiberaDefault::setHandler(data);
        setStateVariableSeverity(StateVariableTypeAlarmDEV,"env_not_reached", chaos::common::alarm::MultiSeverityAlarmLevelClear);\

//        setFeatures(features::FeaturesFlagTypes::FF_SET_SCHEDULER_DELAY, (uint64_t)1000000);

        
#define ADD_ENV_PARAM(param) \
CMDCUDBG_<<"checking environment "<< # param; \
        if(data->hasKey(# param )) {\
            libera_env_t env;\
            std::stringstream ss;\
            env.value = data->getInt32Value(# param);\
            env.selector=CSPI_ENV_## param;\
            ss<<"Setting env \""<< # param <<"\" ("<<std::hex<<env.selector<<std::dec<<")="<<env.value ;\
            CMDCUDBG_<<ss.str();\
            if((ret=driver->iop(LIBERA_IOP_CMD_SETENV,&env,sizeof(libera_env_t)))!=0){\
                getAttributeCache()->setOutputDomainAsChanged();\
                BC_FAULT_RUNNING_PROPERTY;\
		        setStateVariableSeverity(StateVariableTypeAlarmDEV,"env_not_reached", chaos::common::alarm::MultiSeverityAlarmLevelHigh);\
                metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelError,"Cannot set env:"+ss.str() );\
                return;\
            } else {metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelInfo,ss.str() );}}
        
       
       
         if((ret=driver->iop(LIBERA_IOP_CMD_STOP,0,0))!=0){
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

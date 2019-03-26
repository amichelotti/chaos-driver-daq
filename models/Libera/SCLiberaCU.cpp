/*
 *	SCLiberaCU
 *	!CHAOS
 *	Created by Andrea Michelotti 15/5/2015
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

#include "SCLiberaCU.h"
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

//---comands----
#include "CmdLiberaDefault.h"
#include "CmdLiberaAcquire.h"
#include "CmdLiberaEnv.h"
#include "CmdLiberaTime.h"

using namespace chaos;

using namespace chaos::common::data;
using namespace chaos::common::batch_command;

using namespace chaos::cu::control_manager::slow_command;
using namespace chaos::cu::driver_manager::driver;
using namespace chaos::cu::control_manager;


#define SCCUAPP LAPP_ << "[SCLiberaCU - " << getDeviceID() << "] - "<<__PRETTY_FUNCTION__<<":"
#define SCCULDBG LDBG_ << "[SCLiberaCU - " <<getDeviceID() << "] - "<<__PRETTY_FUNCTION__<<":"

PUBLISHABLE_CONTROL_UNIT_IMPLEMENTATION(::driver::daq::libera::SCLiberaCU)

using namespace ::driver::daq::libera;
/*
 Construct a new CU with an identifier
 */
SCLiberaCU::SCLiberaCU(const string& _control_unit_id,
														const string& _control_unit_param,
														const ControlUnitDriverList& _control_unit_drivers):
//call base constructor
chaos::cu::control_manager::SCAbstractControlUnit(_control_unit_id,
												  _control_unit_param,
												  _control_unit_drivers){

    driver = NULL;
   
        
/*    if(!_control_unit_param.empty()){
        bpm_type = atoi(control_unit_param.c_str());
    }*/
}

/*
 Base destructor
 */
SCLiberaCU::~SCLiberaCU() {
	
}


/*
 Return the default configuration
 */
void SCLiberaCU::unitDefineActionAndDataset() throw(chaos::CException) {
  SCCULDBG<<"defining commands";
	//install all command

    installCommand(BATCH_COMMAND_GET_DESCRIPTION(CmdLiberaDefault), true,true);
	//installCommand<CmdLiberaAcquire>("acquire");
    installCommand(BATCH_COMMAND_GET_DESCRIPTION(CmdLiberaEnv));
    installCommand(BATCH_COMMAND_GET_DESCRIPTION(CmdLiberaTime));

	installCommand(BATCH_COMMAND_GET_DESCRIPTION(CmdLiberaAcquire));

	//set it has default
	setDefaultCommand("default");
	SCCULDBG<<"defining dataset";
        
        addAttributeToDataSet("timeout",
						  "Time out triggering",
						  DataType::TYPE_INT32,
						  DataType::Input);
        
        
        //// Configuration
        addAttributeToDataSet("config","Coefficients for polynomial filtering Voltage to Position",chaos::DataType::TYPE_CLUSTER,chaos::DataType::Input);


       
        
        
        ////
        addAttributeToDataSet("DD",
						  "Enable DataOnDemand acquisition",
						  DataType::TYPE_BOOLEAN,
						  DataType::Bidirectional);
        
        
        addHandlerOnInputAttributeName< SCLiberaCU, bool >(this,&SCLiberaCU::setDD,"DD");

        
        addAttributeToDataSet("SA",
						  "Enable Slow acquisition",
						  DataType::TYPE_BOOLEAN,
						  DataType::Bidirectional);
        
        
        addHandlerOnInputAttributeName< SCLiberaCU, bool >(this,&SCLiberaCU::setSA,"SA");

        
        
         addAttributeToDataSet("ADC",
						  "Enable ADC acquisition",
						  DataType::TYPE_BOOLEAN,
						  DataType::Bidirectional);
        
        
        addHandlerOnInputAttributeName< SCLiberaCU, bool >(this,&SCLiberaCU::setADC,"ADC");

        
        addAttributeToDataSet("TRIGGER",
						  "Enable Trigger acquisition(not in SA)",
						  DataType::TYPE_BOOLEAN,
						  DataType::Input);
        
        
        addAttributeToDataSet("OFFSET",
						  "Offset in acquisition",
						  DataType::TYPE_INT32,
						  DataType::Input);
        
        addAttributeToDataSet("MODE",
						  "Libera Mode",
						  DataType::TYPE_INT32,
						  DataType::Bidirectional);
        
       addHandlerOnInputAttributeName< SCLiberaCU, int32_t >(this,&SCLiberaCU::setMode,"MODE");

        
        addAttributeToDataSet("SAMPLES",
						  "Samples to acquire",
						  DataType::TYPE_INT32,
						  DataType::Input);

	addAttributeToDataSet("ACQUISITION",
						  "Acquisition number",
						  DataType::TYPE_INT64,
						  DataType::Output);
        addAttributeToDataSet("MT",
						  "Machine Time",
						  DataType::TYPE_INT64,
						  DataType::Bidirectional);
        addAttributeToDataSet("ST",
						  "System Time",
						  DataType::TYPE_INT64,
						  DataType::Bidirectional);
        
        
        addAttributeToDataSet("VA","Volt A",DataType::TYPE_INT32,chaos::DataType::Output);
        addAttributeToDataSet("VB","Volt B",DataType::TYPE_INT32,chaos::DataType::Output);
        addAttributeToDataSet("VC","Volt C",DataType::TYPE_INT32,chaos::DataType::Output);
        addAttributeToDataSet("VD","Volt D",DataType::TYPE_INT32,chaos::DataType::Output);
        
    
        addAttributeToDataSet("X","X mm calculated",DataType::TYPE_DOUBLE,chaos::DataType::Output);
        addAttributeToDataSet("Y","Y mm calculated",DataType::TYPE_DOUBLE,chaos::DataType::Output);
        addAttributeToDataSet("Q","Q",DataType::TYPE_INT32,chaos::DataType::Output);
        addAttributeToDataSet("SUM","SUM",DataType::TYPE_INT32,chaos::DataType::Output);
        addAttributeToDataSet("Q1","Q1",DataType::TYPE_INT32,chaos::DataType::Output);
        addAttributeToDataSet("Q2","Q2",DataType::TYPE_INT32,chaos::DataType::Output);

        
	
      
        addAttributeToDataSet("STATUS",
						  "status",
						  DataType::TYPE_STRING,
						  DataType::Output,MAX_STRING);
        
	//setup the dataset
	/*
         * addAttributeToDataSet("DD",
						  "Data on Demand",
						  DataType::TYPE_BYTEARRAY,
						  DataType::Output,1 * sizeof(libera_dd_t));
         */ 
        addBinaryAttributeAsSubtypeToDataSet("VA_ACQ","VA ACQUIRED",chaos::DataType::SUB_TYPE_INT32,1,chaos::DataType::Output);
        addBinaryAttributeAsSubtypeToDataSet("VB_ACQ","VB ACQUIRED",chaos::DataType::SUB_TYPE_INT32,1,chaos::DataType::Output);
        addBinaryAttributeAsSubtypeToDataSet("VC_ACQ","VC ACQUIRED",chaos::DataType::SUB_TYPE_INT32,1,chaos::DataType::Output);
        addBinaryAttributeAsSubtypeToDataSet("VD_ACQ","VD ACQUIRED",chaos::DataType::SUB_TYPE_INT32,1,chaos::DataType::Output);

        addBinaryAttributeAsSubtypeToDataSet("X_ACQ","X ACQUIRED",chaos::DataType::SUB_TYPE_DOUBLE,1,chaos::DataType::Output);
        addBinaryAttributeAsSubtypeToDataSet("Y_ACQ","Y ACQUIRED",chaos::DataType::SUB_TYPE_DOUBLE,1,chaos::DataType::Output);
    	addBinaryAttributeAsSubtypeToDataSet("SUM_ACQ","SUM ACQUIRED",chaos::DataType::SUB_TYPE_INT32,1,chaos::DataType::Output);

     /*
      *    addAttributeToDataSet("SA",
						  "Data Streaming",
						  DataType::TYPE_BYTEARRAY,
						  DataType::Output,1 * sizeof(libera_sa_t));
      */
        
         addAttributeToDataSet("ADC_CW",
						  "Data ADC Continuous",
						  DataType::TYPE_BYTEARRAY,
						  DataType::Output,1 * sizeof(libera_cw_t));
	
        addAttributeToDataSet("ADC_SP",
						  "Data ADC Single Pass",
						  DataType::TYPE_BYTEARRAY,
						  DataType::Output,1 * sizeof(libera_sp_t));
        
        addAttributeToDataSet("AVG",
						  "Data Average",
						  DataType::TYPE_BYTEARRAY,
						  DataType::Output,1 * sizeof(libera_avg_t));
        

        addStateVariable(StateVariableTypeAlarmDEV,"mode_not_reached",
            "Notify mode is not reached");

        addStateVariable(StateVariableTypeAlarmDEV,"acquisition_error",
            "Notify an error");	
}

void SCLiberaCU::unitDefineCustomAttribute() {
	
}

// Abstract method for the initialization of the control unit
void SCLiberaCU::unitInit() throw(CException) {
        metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelInfo,"Initializing");
	chaos::cu::driver_manager::driver::DriverAccessor * accessor=AbstractControlUnit::getAccessoInstanceByIndex(0);
	if(accessor==NULL){
		throw chaos::CFatalException(-1, "Cannot retrieve the requested driver", __FUNCTION__);
	}
	driver = new chaos::cu::driver_manager::driver::BasicIODriverInterface(accessor);
	if(driver==NULL){
		throw chaos::CFatalException(-2, "Cannot allocate driver resources", __FUNCTION__);
	}
	
        if(driver->initIO(0,0)!=0){
            throw chaos::CFatalException(-3, "Cannot initialize driver", __FUNCTION__);

        }
	itrigger=getAttributeCache()->getRWPtr<bool>(DOMAIN_INPUT, "TRIGGER");
    imode = getAttributeCache()->getRWPtr<int32_t>(DOMAIN_INPUT, "MODE");
    isamples=getAttributeCache()->getRWPtr<int32_t>(DOMAIN_INPUT, "SAMPLES");
    ioffset = getAttributeCache()->getRWPtr<int32_t>(DOMAIN_INPUT, "OFFSET");
    if(!(itrigger && imode && isamples && ioffset)){
    	throw CFatalException(-1,"cannot retrieve cache pointers",__PRETTY_FUNCTION__);
    }
	SCCULDBG << "Initialization done";	
}

// Abstract method for the start of the control unit
void SCLiberaCU::unitStart() throw(CException) {
        metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelInfo,"Starting");
	//setSA("SA", true, 0);
	
}

// Abstract method for the stop of the control unit
void SCLiberaCU::unitStop() throw(CException) {
            metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelInfo,"Stopping");

	
}

// Abstract method for the deinit of the control unit
void SCLiberaCU::unitDeinit() throw(CException) {
            metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelInfo,"Deinit");

    if(driver!=NULL){
        delete driver;
        driver = NULL;
    }
	
}
bool SCLiberaCU::sendAcquire(int32_t mode, bool enable,int32_t loops, int32_t samples,int32_t offset,bool sync){
    
     uint64_t cmd_id;
    bool result = false;
    std::auto_ptr<chaos::common::data::CDataWrapper> cmd_pack(new CDataWrapper());
    
    cmd_pack->addInt32Value("mode", mode);
    cmd_pack->addInt32Value("enable", enable);
    cmd_pack->addInt32Value("offset", offset);
    cmd_pack->addInt32Value("samples", samples);
    cmd_pack->addInt32Value("loops", loops);

    //send command

        submitBatchCommand("acquire",
                cmd_pack.release(),
                cmd_id,
                0,
                50,
                (enable==0)?SubmissionRuleType::SUBMIT_AND_KILL:SubmissionRuleType::SUBMIT_NORMAL);
   
    if (sync) {
        //! whait for the current command id to finish
        result = waitOnCommandID(cmd_id);
    }
    return result;

}

bool SCLiberaCU::setDD(const std::string &name, bool value, uint32_t size){
    int32_t mode=LIBERA_IOP_MODE_DD | ((*itrigger)?LIBERA_IOP_MODE_TRIGGERED:0);
    if(value){

       return sendAcquire(mode,1,-1,*isamples,*ioffset,false);
    } 
    
    
    
    return sendAcquire(0,0,-1,*isamples,0,false);
    
     
}

bool SCLiberaCU::setSA(const std::string &name, bool value, uint32_t size){
    int32_t mode=LIBERA_IOP_MODE_SA ;
    if(value){
       return sendAcquire(mode,1,-1,*isamples,*ioffset,false);
    } 
    
    return sendAcquire(0,0,-1,*isamples,0,true);
}
bool SCLiberaCU::setADC(const std::string &name, bool value, uint32_t size){
     int32_t mode=LIBERA_IOP_MODE_ADC ;
    if(value){
       return sendAcquire(mode,1,-1,*isamples,*ioffset,false);
    } 
    
    return sendAcquire(0,0,-1,*isamples,*ioffset,false);
}

                        
bool  SCLiberaCU::setMode(const std::string &name, int32_t value, uint32_t size){
    return sendAcquire(value,1,-1,*isamples,*ioffset,false);
}

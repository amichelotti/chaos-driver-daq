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



#define SCCUAPP LAPP_ << "[SCLiberaCU - " << getCUID() << "] - "<<__FUNCTION__<<":"
#define SCCULDBG LDBG_ << "[SCLiberaCU - " << getCUID() << "] - "<<__FUNCTION__<<":"

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
	installCommand<CmdLiberaDefault>("default");
	installCommand<CmdLiberaAcquire>("acquire");
	installCommand<CmdLiberaEnv>("env");
	installCommand<CmdLiberaTime>("time");
	
	//set it has default
	setDefaultCommand("default");
	SCCULDBG<<"defining dataset";
        addAttributeToDataSet("MODE",
						  "Libera Mode",
						  DataType::TYPE_INT32,
						  DataType::Output);
        
        addAttributeToDataSet("SAMPLES",
						  "Samples to acquire",
						  DataType::TYPE_INT32,
						  DataType::Output);
	addAttributeToDataSet("ACQUISITION",
						  "Acquisition number",
						  DataType::TYPE_INT64,
						  DataType::Output);
        addAttributeToDataSet("MT",
						  "Machine Time",
						  DataType::TYPE_INT64,
						  DataType::Output);
        addAttributeToDataSet("ST",
						  "System Time",
						  DataType::TYPE_INT64,
						  DataType::Output);
        
        
        addAttributeToDataSet("VA","Volt A",DataType::TYPE_INT32,chaos::DataType::Output);
        addAttributeToDataSet("VB","Volt B",DataType::TYPE_INT32,chaos::DataType::Output);
        addAttributeToDataSet("VC","Volt C",DataType::TYPE_INT32,chaos::DataType::Output);
        addAttributeToDataSet("VD","Volt D",DataType::TYPE_INT32,chaos::DataType::Output);
        addAttributeToDataSet("X","X",DataType::TYPE_INT32,chaos::DataType::Output);
        addAttributeToDataSet("Y","Y",DataType::TYPE_INT32,chaos::DataType::Output);
        
        addAttributeToDataSet("Q","Q",DataType::TYPE_INT32,chaos::DataType::Output);
        addAttributeToDataSet("SUM","SUM",DataType::TYPE_INT32,chaos::DataType::Output);
        addAttributeToDataSet("Q1","Q1",DataType::TYPE_INT32,chaos::DataType::Output);
        addAttributeToDataSet("Q2","Q2",DataType::TYPE_INT32,chaos::DataType::Output);

        
	addAttributeToDataSet("timeout",
						  "Time out triggering",
						  DataType::TYPE_INT32,
						  DataType::Input);
        
        
        addAttributeToDataSet("error",
						  "error status",
						  DataType::TYPE_INT32,
						  DataType::Output);
        addAttributeToDataSet("STATUS",
						  "status",
						  DataType::TYPE_STRING,
						  DataType::Output,MAX_STRING);
        
	//setup the dataset
	addAttributeToDataSet("DD",
						  "Data on Demand",
						  DataType::TYPE_BYTEARRAY,
						  DataType::Output,1 * sizeof(libera_dd_t));
	
        addAttributeToDataSet("SA",
						  "Data Streaming",
						  DataType::TYPE_BYTEARRAY,
						  DataType::Output,1 * sizeof(libera_sa_t));
        
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
        
	
}

void SCLiberaCU::unitDefineCustomAttribute() {
	
}

// Abstract method for the initialization of the control unit
void SCLiberaCU::unitInit() throw(CException) {
	SCCUAPP "unitInit";
	chaos::cu::driver_manager::driver::DriverAccessor * accessor=AbstractControlUnit::getAccessoInstanceByIndex(0);
	if(accessor==NULL){
		throw chaos::CException(-1, "Cannot retrieve the requested driver", __FUNCTION__);
	}
	driver = new chaos::cu::driver_manager::driver::BasicIODriverInterface(accessor);
	if(driver==NULL){
		throw chaos::CException(-2, "Cannot allocate driver resources", __FUNCTION__);
	}
	
        if(driver->initIO(0,0)!=0){
            throw chaos::CException(-3, "Cannot initialize driver", __FUNCTION__);

        }
	
	SCCULDBG << "Initialization done";	
}

// Abstract method for the start of the control unit
void SCLiberaCU::unitStart() throw(CException) {
	
}

// Abstract method for the stop of the control unit
void SCLiberaCU::unitStop() throw(CException) {
	
}

// Abstract method for the deinit of the control unit
void SCLiberaCU::unitDeinit() throw(CException) {
    if(driver!=NULL){
        delete driver;
        driver = NULL;
    }
	
}

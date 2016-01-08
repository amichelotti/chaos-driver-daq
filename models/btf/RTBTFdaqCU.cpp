/*
 *	RTBTFdaqCU
 *	!CHAOS
 *	Created by Andrea Michelotti 8/1/2016
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

#include "RTBTFdaqCU.h"
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

using namespace chaos;

using namespace chaos::common::data;
using namespace chaos::common::batch_command;

using namespace chaos::cu::control_manager::slow_command;
using namespace chaos::cu::driver_manager::driver;



#define SCCUAPP LAPP_ << "[RTBTFdaqCU - " << getCUID() << "] - "<<__PRETTY_FUNCTION__<<":"
#define SCCULDBG LDBG_ << "[RTBTFdaqCU - " << getCUID() << "] - "<<__PRETTY_FUNCTION__<<":"

PUBLISHABLE_CONTROL_UNIT_IMPLEMENTATION(::driver::daq::btf::RTBTFdaqCU)

using namespace ::driver::daq::btf;
/*
 Construct a new CU with an identifier
 */
RTBTFdaqCU::RTBTFdaqCU(const string& _control_unit_id,
														const string& _control_unit_param,
														const ControlUnitDriverList& _control_unit_drivers):
//call base constructor
chaos::cu::control_manager::RTAbstractControlUnit(_control_unit_id,
												  _control_unit_param,
												  _control_unit_drivers){

  
}

/*
 Base destructor
 */
RTBTFdaqCU::~RTBTFdaqCU() {
	
}


/*
 Return the default configuration
 */
void RTBTFdaqCU::unitDefineActionAndDataset() throw(chaos::CException) {
  
	SCCULDBG<<"defining dataset";
     
        
	addAttributeToDataSet("scaleradd",
						  "VME Scaler Base Address",
						  DataType::TYPE_INT32,
						  DataType::Input);
        
        addAttributeToDataSet("qdc965add",
						  "VME QDC965 base Address",
						  DataType::TYPE_INT32,
						  DataType::Input);
        
        addAttributeToDataSet("qdc792add",
						  "VME QDC792 base Address",
						  DataType::TYPE_INT32,
						  DataType::Input);
        
        addAttributeToDataSet("ACQUISITION",
						  "Acquisition number",
						  DataType::TYPE_INT64,
						  DataType::Output);
        addAttributeToDataSet("TRIGGER",
						  "Number of triggers",
						  DataType::TYPE_INT64,
						  DataType::Output);
        addAttributeToDataSet("TRIGGER LOST",
						  "Number of lost trigger",
						  DataType::TYPE_INT64,
						  DataType::Output);
        
        
        addBinaryAttributeAsSubtypeToDataSet("QDC965HI","Vector of 16 Channels High Resolution",chaos::DataType::SUB_TYPE_INT32,1,chaos::DataType::Output);
        addBinaryAttributeAsSubtypeToDataSet("QDC965LO","Vector of 16 Channels Low Resolution",chaos::DataType::SUB_TYPE_INT32,1,chaos::DataType::Output);
        addBinaryAttributeAsSubtypeToDataSet("QDC792","Vector of 32 Channels ",chaos::DataType::SUB_TYPE_INT32,1,chaos::DataType::Output);
        addBinaryAttributeAsSubtypeToDataSet("SCALER","Vector of 32 Counters ",chaos::DataType::SUB_TYPE_INT32,1,chaos::DataType::Output);
       
	
}

void RTBTFdaqCU::unitDefineCustomAttribute() {
	
}

// Abstract method for the initialization of the control unit
void RTBTFdaqCU::unitInit() throw(CException) {
	
	SCCULDBG << "Initialization done";	
}

// Abstract method for the start of the control unit
void RTBTFdaqCU::unitStart() throw(CException) {
	
}
// Abstract method for the start of the control unit
void RTBTFdaqCU::unitRun() throw(CException) {
	
}

// Abstract method for the stop of the control unit
void RTBTFdaqCU::unitStop() throw(CException) {
	
}

// Abstract method for the deinit of the control unit
void RTBTFdaqCU::unitDeinit() throw(CException) {
  
}

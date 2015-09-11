/*
 *	RTBPMLibera.cpp
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

#include "RTBPMLibera.h"
//#include "RTBPMLiberaDriver.h"

using namespace chaos;
using namespace chaos::common::data::cache;
using namespace chaos::cu::driver_manager::driver;
using namespace chaos::driver::daq;
PUBLISHABLE_CONTROL_UNIT_IMPLEMENTATION(RTBPMLibera)

#define RTBPMLiberaLAPP_		LAPP_ << "[RTBPMLibera] "
#define RTBPMLiberaLDBG_		LDBG_ << "[RTBPMLibera] " << __PRETTY_FUNCTION__ << " "
#define RTBPMLiberaLERR_		LERR_ << "[RTBPMLibera] " << __PRETTY_FUNCTION__ << "("<<__LINE__<<") "

/*
 Construct
 */
RTBPMLibera::RTBPMLibera(const string& _control_unit_id, const string& _control_unit_param, const ControlUnitDriverList& _control_unit_drivers):
RTAbstractControlUnit(_control_unit_id, _control_unit_param, _control_unit_drivers) {

}

/*
 Destructor
 */
RTBPMLibera::~RTBPMLibera() {

}

//!Return the definition of the control unit
/*!
The api that can be called withi this method are listed into
"Control Unit Definition Public API" module into html documentation
(chaosframework/Documentation/html/group___control___unit___definition___api.html)
*/
void RTBPMLibera::unitDefineActionAndDataset() throw(chaos::CException) {
    //insert your definition code here
    /*

    addAttributeToDataSet("<my_double_variable_name>", // this is the name of the variable that is shown in interfaces and in MDS
			      "variable Description",
			      DataType::TYPE_DOUBLE,
			      DataType::Output);


	 addAttributeToDataSet("<my_int32_variable>",
			      "variable Description",
			      DataType::TYPE_INT32,
			      DataType::Output);

	 addAttributeToDataSet("<my_int64_variable>",
			      "variable Description",
			      DataType::TYPE_INT64,
			      DataType::Output);

	 addAttributeToDataSet("<my_string_variable name>",
			      "variable Description",
			      DataType::TYPE_STRING,
			      DataType::Output,256); // max string size


	 addAttributeToDataSet("< my_buffer name>",
			      "variable Description",
			      DataType::TYPE_BYTEARRAY,
			      DataType::Output,
			      10000000); // max buffer size

	 addActionDescritionInstance<RTBPMLibera>(this,
						       &RTBPMLibera::my_custom_action,
						       "customFunctionName,
						       "custom function desctiption");

    */

    //add two int32_t channel for example

    //output channel
    addAttributeToDataSet("out_1",
                          "Int32 output channel",
                          DataType::TYPE_INT32,
                          DataType::Output);

    //input channel
    addAttributeToDataSet("in_1",
                          "Int32 output channel",
                          DataType::TYPE_INT32,
                          DataType::Input);
}


//!Define custom control unit attribute
void RTBPMLibera::unitDefineCustomAttribute() {

}

//!Initialize the Custom Control Unit
void RTBPMLibera::unitInit() throw(chaos::CException) {

  //check the value set on MDS for in_1 channel
  //int32_t in_1 = getAttributeCache()->getValue<int32_t>(DOMAIN_INPUT, "in_1");

}

//!Execute the work, this is called with a determinated delay, it must be as fast as possible
void RTBPMLibera::unitStart() throw(chaos::CException) {

}

//!Execute the Control Unit work
void RTBPMLibera::unitRun() throw(chaos::CException) {
  //get the output attribute pointer form the internal cache
 
}

//!Execute the Control Unit work
void RTBPMLibera::unitStop() throw(chaos::CException) {

}

//!Deinit the Control Unit
void RTBPMLibera::unitDeinit() throw(chaos::CException) {

}

//! pre imput attribute change
void RTBPMLibera::unitInputAttributePreChangeHandler() throw(chaos::CException) {

}

//! attribute changed handler
void RTBPMLibera::unitInputAttributeChangedHandler() throw(chaos::CException) {

  
}

/*
CDataWrapper *RTBPMLibera::my_custom_action(CDataWrapper *actionParam, bool& detachParam) {
	CDataWrapper *result =  new CDataWrapper();
	return result;
}
*/

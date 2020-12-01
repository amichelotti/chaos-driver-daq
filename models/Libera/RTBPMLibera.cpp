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
   
addAttributeToDataSet("BPBA1001X","BPBA1001 X",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);
addAttributeToDataSet("BPBA1001Y","BPBA1001 Y",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);

addAttributeToDataSet("BPSA1001X","BPSA1001 X",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);
addAttributeToDataSet("BPSA1001Y","BPSA1001 Y",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);

addAttributeToDataSet("BPBA1002X","BPBA1002 X",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);
addAttributeToDataSet("BPBA1002Y","BPBA1002 Y",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);

addAttributeToDataSet("BPBA2001X","BPBA2001 X",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);
addAttributeToDataSet("BPBA2001Y","BPBA2001 Y",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);

addAttributeToDataSet("BPSA2001X","BPSA2001 X",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);
addAttributeToDataSet("BPSA2001Y","BPSA2001 Y",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);

addAttributeToDataSet("BPBA2002X","BPBA2002 X",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);
addAttributeToDataSet("BPBA2002Y","BPBA2002 Y",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);

addAttributeToDataSet("BPBA3001X","BPBA3001 X",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);
addAttributeToDataSet("BPBA3001Y","BPBA3001 Y",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);

addAttributeToDataSet("BPSA3001X","BPSA3001 X",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);
addAttributeToDataSet("BPSA3001Y","BPSA3001 Y",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);

addAttributeToDataSet("BPBA3002X","BPBA3002 X",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);
addAttributeToDataSet("BPBA3002Y","BPBA3002 Y",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);

addAttributeToDataSet("BPBA4001X","BPBA4001 X",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);
addAttributeToDataSet("BPBA4001Y","BPBA4001 Y",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);

addAttributeToDataSet("BPBA4002X","BPBA4002 X",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);
addAttributeToDataSet("BPBA4002Y","BPBA4002 Y",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);

addAttributeToDataSet("BPSA4001X","BPSA4001 X",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);
addAttributeToDataSet("BPSA4001Y","BPSA4001 Y",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);
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

//! attribute changed handler
void RTBPMLibera::unitInputAttributeChangedHandler() throw(chaos::CException) {

  
}


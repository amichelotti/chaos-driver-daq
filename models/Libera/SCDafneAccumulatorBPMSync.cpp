/*
 *	SCDafneAccumulatorBPMSync.cpp
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

#include "SCDafneAccumulatorBPMSync.h"
//#include "SCDafneAccumulatorBPMSyncDriver.h"
#include <boost/algorithm/string.hpp>
#include <chaos/ui_toolkit/LowLevelApi/LLRpcApi.h>
#include <chaos/cu_toolkit/command_manager/CommandManager.h>
#include <driver/daq/models/Libera/ChaosControllerLibera.h>
#include "CmdDefaultDafneAccumulatorBPM.h"
#include "CmdAcquireDafneAccumulatorBPM.h"
#include "CmdEnvDafneAccumulatorBPM.h"
using namespace chaos;
using namespace chaos::common::data::cache;
using namespace chaos::cu::driver_manager::driver;
using namespace ::driver::daq::libera;
using namespace ::driver::misc;
PUBLISHABLE_CONTROL_UNIT_IMPLEMENTATION(SCDafneAccumulatorBPMSync)

#define SCDafneAccumulatorBPMSyncLAPP_		LAPP_ << "[SCDafneAccumulatorBPMSync] "
#define SCDafneAccumulatorBPMSyncLDBG_		LDBG_ << "[SCDafneAccumulatorBPMSync] " << __PRETTY_FUNCTION__ << " "
#define SCDafneAccumulatorBPMSyncLERR_		LERR_ << "[SCDafneAccumulatorBPMSync] " << __PRETTY_FUNCTION__ << "("<<__LINE__<<") "



/*
 Construct
 */
SCDafneAccumulatorBPMSync::SCDafneAccumulatorBPMSync(const string& _control_unit_id, const string& _control_unit_param, const ControlUnitDriverList& _control_unit_drivers):
SCDataSync(_control_unit_id, _control_unit_param, _control_unit_drivers) {
   
   
}
    

/*
 Destructor
 */
SCDafneAccumulatorBPMSync::~SCDafneAccumulatorBPMSync() {

}

using namespace driver::daq::libera;
//!Return the definition of the control unit
/*!
The api that can be called withi this method are listed into
"Control Unit Definition Public API" module into html documentation
(chaosframework/Documentation/html/group___control___unit___definition___api.html)
*/
void SCDafneAccumulatorBPMSync::unitDefineActionAndDataset() throw(chaos::CException) {
    //insert your definition code here
      installCommand<CmdDefaultDafneAccumulatorBPM>("default");
      installCommand(BATCH_COMMAND_GET_DESCRIPTION(CmdAcquireDafneAccumulatorBPM));

	installCommand<CmdEnvDafneAccumulatorBPM>("env");
	//installCommand<CmdDafneAccumulatorBPM>("time");
        setDefaultCommand("default");

        std::vector<ChaosDatasetAttribute*> rattrs= driver->getRemoteVariables();
        
    for (std::vector<ChaosDatasetAttribute*>::iterator i=rattrs.begin();i!=rattrs.end();i++){
        std::string name=(*i)->getGroup()+chaos::PATH_SEPARATOR+(*i)->getName();
        
        if((*i)->getType()!=chaos::DataType::TYPE_BYTEARRAY){
            addAttributeToDataSet(name,(*i)->getDesc(),(*i)->getType(),(*i)->getDir());
        } else if((*i)->getBinaryType()!=chaos::DataType::SUB_TYPE_NONE){
            addBinaryAttributeAsSubtypeToDataSet(name,(*i)->getDesc(),(*i)->getBinaryType(),(*i)->getSize(),(*i)->getDir());
        }
    }
        /*
//libera 07
addAttributeToDataSet("BPSA1001X","BPSA1001 X",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);
addAttributeToDataSet("BPSA1001Y","BPSA1001 Y",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);

addBinaryAttributeAsSubtypeToDataSet("BPSA1001X_ACQ","Buffer of Acquired X coordinate in mm",chaos::DataType::SUB_TYPE_DOUBLE,1,chaos::DataType::Output);
addBinaryAttributeAsSubtypeToDataSet("BPSA1001Y_ACQ","Buffer of Acquired Y coordinate in mm",chaos::DataType::SUB_TYPE_DOUBLE,1,chaos::DataType::Output);

//libera01
addAttributeToDataSet("BPSA2001X","BPSA2001 X",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);
addAttributeToDataSet("BPSA2001Y","BPSA2001 Y",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);

addBinaryAttributeAsSubtypeToDataSet("BPSA2001X_ACQ","Acquired BPSA2001 X",chaos::DataType::SUB_TYPE_DOUBLE,1,chaos::DataType::Output);
addBinaryAttributeAsSubtypeToDataSet("BPSA2001Y_ACQ","Acquired BPSA2001 Y",chaos::DataType::SUB_TYPE_DOUBLE,1,chaos::DataType::Output);

//libera09
addAttributeToDataSet("BPSA3001X","BPSA3001 X",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);
addAttributeToDataSet("BPSA3001Y","BPSA3001 Y",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);

addBinaryAttributeAsSubtypeToDataSet("BPSA3001X_ACQ","Acquired BPSA3001 X",chaos::DataType::SUB_TYPE_DOUBLE,1,chaos::DataType::Output);
addBinaryAttributeAsSubtypeToDataSet("BPSA3001Y_ACQ","Acquired BPSA3001 Y",chaos::DataType::SUB_TYPE_DOUBLE,1,chaos::DataType::Output);

//libera03
addAttributeToDataSet("BPSA4001X","BPSA4001 X",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);
addAttributeToDataSet("BPSA4001Y","BPSA4001 Y",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);

addBinaryAttributeAsSubtypeToDataSet("BPSA4001X_ACQ","Acquired BPSA4001X X",chaos::DataType::SUB_TYPE_DOUBLE,1,chaos::DataType::Output);
addBinaryAttributeAsSubtypeToDataSet("BPSA4001Y_ACQ","Acquired BPSA4001Y Y",chaos::DataType::SUB_TYPE_DOUBLE,1,chaos::DataType::Output);

//libera08
addAttributeToDataSet("BPBA1001X","BPBA1001 X",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);
addAttributeToDataSet("BPBA1001Y","BPBA1001 Y",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);

addBinaryAttributeAsSubtypeToDataSet("BPBA1001X_ACQ","Acquired BPBA1001X X",chaos::DataType::SUB_TYPE_DOUBLE,1,chaos::DataType::Output);
addBinaryAttributeAsSubtypeToDataSet("BPBA1001Y_ACQ","Acquired BPBA1001Y Y",chaos::DataType::SUB_TYPE_DOUBLE,1,chaos::DataType::Output);

//libera02
addAttributeToDataSet("BPBA2002X","BPBA2002 X",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);
addAttributeToDataSet("BPBA2002Y","BPBA2002 Y",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);

addBinaryAttributeAsSubtypeToDataSet("BPBA2002X_ACQ","Acquired BPBA2002X X",chaos::DataType::SUB_TYPE_DOUBLE,1,chaos::DataType::Output);
addBinaryAttributeAsSubtypeToDataSet("BPBA2002Y_ACQ","Acquired BPBA2002Y Y",chaos::DataType::SUB_TYPE_DOUBLE,1,chaos::DataType::Output);


// libera 13
addAttributeToDataSet("BPBA1002X","BPBA1002 X",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);
addAttributeToDataSet("BPBA1002Y","BPBA1002 Y",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);

addBinaryAttributeAsSubtypeToDataSet("BPBA1002X_ACQ","Acquired BPBA1002X X",chaos::DataType::SUB_TYPE_DOUBLE,1,chaos::DataType::Output);
addBinaryAttributeAsSubtypeToDataSet("BPBA1002Y_ACQ","Acquired BPBA1002Y Y",chaos::DataType::SUB_TYPE_DOUBLE,1,chaos::DataType::Output);

addAttributeToDataSet("BPBA2001X","BPBA2001 X",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);
addAttributeToDataSet("BPBA2001Y","BPBA2001 Y",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);

addBinaryAttributeAsSubtypeToDataSet("BPBA2001X_ACQ","Acquired BPBA2001X X",chaos::DataType::SUB_TYPE_DOUBLE,1,chaos::DataType::Output);
addBinaryAttributeAsSubtypeToDataSet("BPBA2001Y_ACQ","Acquired BPBA2001Y Y",chaos::DataType::SUB_TYPE_DOUBLE,1,chaos::DataType::Output);

//

// libera 12

addAttributeToDataSet("BPBA3001X","BPBA3001 X",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);
addAttributeToDataSet("BPBA3001Y","BPBA3001 Y",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);

addBinaryAttributeAsSubtypeToDataSet("BPBA3001X_ACQ","Acquired BPBA3001X X",chaos::DataType::SUB_TYPE_DOUBLE,1,chaos::DataType::Output);
addBinaryAttributeAsSubtypeToDataSet("BPBA3001Y_ACQ","Acquired BPBA3001Y Y",chaos::DataType::SUB_TYPE_DOUBLE,1,chaos::DataType::Output);

addAttributeToDataSet("BPBA3002X","BPBA3002 X",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);
addAttributeToDataSet("BPBA3002Y","BPBA3002 Y",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);

addBinaryAttributeAsSubtypeToDataSet("BPBA3002X_ACQ","Acquired BPBA3002X X",chaos::DataType::SUB_TYPE_DOUBLE,1,chaos::DataType::Output);
addBinaryAttributeAsSubtypeToDataSet("BPBA3002Y_ACQ","Acquired BPBA3002Y Y",chaos::DataType::SUB_TYPE_DOUBLE,1,chaos::DataType::Output);

addAttributeToDataSet("BPBA4001X","BPBA4001 X",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);
addAttributeToDataSet("BPBA4001Y","BPBA4001 Y",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);

addBinaryAttributeAsSubtypeToDataSet("BPBA4001X_ACQ","Acquired BPBA4001X X",chaos::DataType::SUB_TYPE_DOUBLE,1,chaos::DataType::Output);
addBinaryAttributeAsSubtypeToDataSet("BPBA4001Y_ACQ","Acquired BPBA4001Y Y",chaos::DataType::SUB_TYPE_DOUBLE,1,chaos::DataType::Output);

addAttributeToDataSet("BPBA4002X","BPBA4002 X",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);
addAttributeToDataSet("BPBA4002Y","BPBA4002 Y",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);

addBinaryAttributeAsSubtypeToDataSet("BPBA4002X_ACQ","Acquired BPBA4002X X",chaos::DataType::SUB_TYPE_DOUBLE,1,chaos::DataType::Output);
addBinaryAttributeAsSubtypeToDataSet("BPBA4002Y_ACQ","Acquired BPBA4002Y Y",chaos::DataType::SUB_TYPE_DOUBLE,1,chaos::DataType::Output);






  addBinaryAttributeAsSubtypeToDataSet("DoubleTest",
                                         "test",
                                         DataType::SUB_TYPE_DOUBLE,
                                         1,
                                         DataType::Output);
         * */

////

    addAttributeToDataSet("MODE",
						  "BPM Mode",
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
						  "Machine Acquisition Time",
						  DataType::TYPE_INT64,
						  DataType::Output);
       
   
}



//!Initialize the Custom Control Unit
void SCDafneAccumulatorBPMSync::unitInit() throw(chaos::CException) {
    
    
 if(driver->init(1)!=0){
     throw chaos::CException(-100,"## cannot initialize devices",__PRETTY_FUNCTION__);
 }

 

}

//!Execute the work, this is called with a determinated delay, it must be as fast as possible
void SCDafneAccumulatorBPMSync::unitStart() throw(chaos::CException) {
 if(driver->start(1)!=0){
         throw chaos::CException(-101,"## cannot start devices",__PRETTY_FUNCTION__);
 }   
 
}
#if 0
//!Execute the Control Unit work
void SCDafneAccumulatorBPMSync::unitRun() throw(chaos::CException) {
    data_group->sync();
    for(int cnt=0,out=0;cnt<cu_names.size();cnt++,out+=2){
                bpmpos mm; 
                int32_t va,vb,vc,vd;
                
                va= *libera_va[cnt];
                vb= *libera_vb[cnt];
                vc= *libera_vc[cnt];
                vd= *libera_vd[cnt];
                mm=bpm_voltage_to_mm((cnt>3)?1:0,va,vb,vc,vd);

                SCDafneAccumulatorBPMSyncLDBG_<<" "<<cnt<<" :"<<libera_va[cnt]->getName()<<" "<<libera_va[cnt]->getInfo().getTimeStamp()<<": ("<<mm.x<<" mm, "<<mm.y<<" mm) Voltages:"<<va <<" "<<vb <<" "<<(int32_t)vc<<" "<<vd;
                
                getAttributeCache()->setOutputAttributeValue(out,(void*)&mm.x,sizeof(double));
                getAttributeCache()->setOutputAttributeValue(out+1,(void*)&mm.y,sizeof(double));
    }
    getAttributeCache()->setOutputDomainAsChanged();

}
#endif
//!Execute the Control Unit work
void SCDafneAccumulatorBPMSync::unitStop() throw(chaos::CException) {
driver->stop(1);
}

//!Deinit the Control Unit
void SCDafneAccumulatorBPMSync::unitDeinit() throw(chaos::CException) {
driver->deinit(1);
}

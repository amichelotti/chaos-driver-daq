/*
 *	DafneAccumulatorBPMSync.cpp
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

#include "DafneAccumulatorBPMSync.h"
//#include "DafneAccumulatorBPMSyncDriver.h"
#include <boost/algorithm/string.hpp>
#include <chaos/ui_toolkit/LowLevelApi/LLRpcApi.h>
#include <chaos/cu_toolkit/CommandManager/CommandManager.h>
#include <driver/daq/models/Libera/ChaosControllerLibera.h>

using namespace chaos;
using namespace chaos::common::data::cache;
using namespace chaos::cu::driver_manager::driver;
using namespace ::driver::daq::libera;
using namespace ::driver::misc;
PUBLISHABLE_CONTROL_UNIT_IMPLEMENTATION(DafneAccumulatorBPMSync)

#define DafneAccumulatorBPMSyncLAPP_		LAPP_ << "[DafneAccumulatorBPMSync] "
#define DafneAccumulatorBPMSyncLDBG_		LDBG_ << "[DafneAccumulatorBPMSync] " << __PRETTY_FUNCTION__ << " "
#define DafneAccumulatorBPMSyncLERR_		LERR_ << "[DafneAccumulatorBPMSync] " << __PRETTY_FUNCTION__ << "("<<__LINE__<<") "



struct bpmpos {
    double x;
    double y;
};

bpmpos bpm_voltage_to_mm(uint32_t type,int32_t va,int32_t vb,int32_t vc,int32_t vd){
    bpmpos pos;
    double x=0,y=0;
    double U= ((double)(vb +vd -va -vc))/(va +vb +vc+vd);
    double V= ((double)(va +vb -vc -vd))/(va +vb +vc+vd);
    double a[2][6]={{28.5574,-0.046125,5.43125e-5,0.0172085,-1.15991e-5,1.94837e-7},{9.8435,-0.022408,0.034859,-1.4584e-6,-9.9279e-6}};
    double b[2][6]={{28.5574,-0.0172085,1.94837e-7,-0.046125,-1.15991e-5,5.43125e-5},{32.0137,0.0432143,0.000222447,-0.000318269,0.00167884}};
    if(type>1){
        return pos;
    }
    for(int cnt=0;cnt<7;cnt++){
        x = a[type][0] * U + a[type][1] * pow(y,2)*U +  a[type][2]*pow(y,4)*U + a[type][3] *pow(x,2)*U +a[type][4]*pow(x,2)*pow(y,2)*U+a[type][5]*pow(x,4)*U;
        y = b[type][1] * V + b[type][1] * pow(y,2)*V +  b[type][2]*pow(y,4)*V + b[type][3] *pow(x,2)*V +b[type][4]*pow(x,2)*pow(y,2)*V+b[type][5]*pow(x,4)*V;
    }
    
    /*MATLAB*/
    /*Xs=0;
Ys=0;

for i=1:7

x=a(1)*U+a(2)*Ys^2*U+a(3)*Ys^4*U+a(4)*Xs^2*U+a(5)*Xs^2*Ys^2*U+a(6)*Xs^4*U;
y=b(1)*V+b(2)*Ys^2*V+b(3)*Ys^4*V+b(4)*Xs^2*V+b(5)*Xs^2*Ys^2*V+b(6)*Xs^4*V;

Xs=x;
Ys=y;

end*/
    

    pos.x=x;
    pos.y=y;
    return pos;
}

/*
 Construct
 */
DafneAccumulatorBPMSync::DafneAccumulatorBPMSync(const string& _control_unit_id, const string& _control_unit_param, const ControlUnitDriverList& _control_unit_drivers):
RTAbstractControlUnit(_control_unit_id, _control_unit_param, _control_unit_drivers) {
    int cnt=0;
    std::vector<std::string>::iterator i;
   
     boost::split(cu_names,_control_unit_param,boost::is_any_of(" \n"));
     int cu=cu_names.size();
       DafneAccumulatorBPMSyncLDBG_<<" "<<cu<<" BPMS:"<<_control_unit_param;

       group=new ChaosControllerGroup<ChaosControllerLibera>();
       data_group=new ChaosDatasetAttributeSinchronizer();
       if(group==NULL || data_group==NULL){
           throw chaos::CException(-100,"## cannot create group access",__PRETTY_FUNCTION__);
       
       }
     if(cu>0){
    
    libera_va = new ChaosDatasetAttribute*[cu];
    libera_vb = new ChaosDatasetAttribute*[cu];
    libera_vc = new ChaosDatasetAttribute*[cu];
    libera_vd = new ChaosDatasetAttribute*[cu];
    libera_acquisition = new ChaosDatasetAttribute*[cu];
    libera_devs = new ChaosControllerLibera*[cu];
    i=cu_names.begin();
    
    for(cnt=0;cnt<cu;cnt++){
        DafneAccumulatorBPMSyncLDBG_<<" ["<<cnt<<"] Adding "<<*i<<" to the set";
        libera_devs[cnt]= new ChaosControllerLibera(*i+ "/LIBERA_ACQUIRE0");
        
        libera_va[cnt] = new ChaosDatasetAttribute(*i + "/LIBERA_ACQUIRE0/VA");
        libera_vb[cnt] = new ChaosDatasetAttribute(*i + "/LIBERA_ACQUIRE0/VB");
        libera_vc[cnt] = new ChaosDatasetAttribute(*i + "/LIBERA_ACQUIRE0/VC");
        libera_vd[cnt] = new ChaosDatasetAttribute(*i + "/LIBERA_ACQUIRE0/VD");
        libera_acquisition[cu] = new ChaosDatasetAttribute(*i + "/LIBERA_ACQUIRE0/ACQUISITION");

        
        data_group->add(libera_acquisition[cu] );
        
        if(libera_devs[cnt]){
            group->add(libera_devs[cnt]);
        }
        i++;
    }
    group->setSchedule(200);
    group->setTimeout(30000000);
    data_group->setInterval(3000000);
    data_group->setTimeout (6000000);
   
    }
    
}

/*
 Destructor
 */
DafneAccumulatorBPMSync::~DafneAccumulatorBPMSync() {

}

//!Return the definition of the control unit
/*!
The api that can be called withi this method are listed into
"Control Unit Definition Public API" module into html documentation
(chaosframework/Documentation/html/group___control___unit___definition___api.html)
*/
void DafneAccumulatorBPMSync::unitDefineActionAndDataset() throw(chaos::CException) {
    //insert your definition code here
   
//libera 07
addAttributeToDataSet("BPSA1001X","BPSA1001 X",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);
addAttributeToDataSet("BPSA1001Y","BPSA1001 Y",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);


//libera01
addAttributeToDataSet("BPSA2001X","BPSA2001 X",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);
addAttributeToDataSet("BPSA2001Y","BPSA2001 Y",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);

//libera09
addAttributeToDataSet("BPSA3001X","BPSA3001 X",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);
addAttributeToDataSet("BPSA3001Y","BPSA3001 Y",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);

//libera03
addAttributeToDataSet("BPSA4001X","BPSA4001 X",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);
addAttributeToDataSet("BPSA4001Y","BPSA4001 Y",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);
//libera08
addAttributeToDataSet("BPBA1001X","BPBA1001 X",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);
addAttributeToDataSet("BPBA1001Y","BPBA1001 Y",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);
//libera02
addAttributeToDataSet("BPBA2002X","BPBA2002 X",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);
addAttributeToDataSet("BPBA2002Y","BPBA2002 Y",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);



// libera 13
addAttributeToDataSet("BPBA1002X","BPBA1002 X",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);
addAttributeToDataSet("BPBA1002Y","BPBA1002 Y",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);

addAttributeToDataSet("BPBA2001X","BPBA2001 X",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);
addAttributeToDataSet("BPBA2001Y","BPBA2001 Y",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);
//

// libera 12

addAttributeToDataSet("BPBA3001X","BPBA3001 X",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);
addAttributeToDataSet("BPBA3001Y","BPBA3001 Y",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);

addAttributeToDataSet("BPBA3002X","BPBA3002 X",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);
addAttributeToDataSet("BPBA3002Y","BPBA3002 Y",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);

addAttributeToDataSet("BPBA4001X","BPBA4001 X",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);
addAttributeToDataSet("BPBA4001Y","BPBA4001 Y",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);

addAttributeToDataSet("BPBA4002X","BPBA4002 X",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);
addAttributeToDataSet("BPBA4002Y","BPBA4002 Y",chaos::DataType::TYPE_DOUBLE,chaos::DataType::Output);
////
}


//!Define custom control unit attribute
void DafneAccumulatorBPMSync::unitDefineCustomAttribute() {

}

//!Initialize the Custom Control Unit
void DafneAccumulatorBPMSync::unitInit() throw(chaos::CException) {
 if(group->init(1)!=0){
     throw chaos::CException(-100,"## cannot initialize devices",__PRETTY_FUNCTION__);
 }
 sleep(5);
 if(group->start(1)!=0){
         throw chaos::CException(-101,"## cannot start devices",__PRETTY_FUNCTION__);
 }
 sleep(5);
 group->acquire_sa(1,1000000,2000000,0);

}

//!Execute the work, this is called with a determinated delay, it must be as fast as possible
void DafneAccumulatorBPMSync::unitStart() throw(chaos::CException) {
    
 
}

//!Execute the Control Unit work
void DafneAccumulatorBPMSync::unitRun() throw(chaos::CException) {
    data_group->sync();
    for(int cnt=0,out=0;cnt<cu_names.size();cnt++,out+=2){
                bpmpos mm; 
                int32_t va,vb,vc,vd;
                
                va= *libera_va[cnt];
                vb= *libera_vb[cnt];
                vc= *libera_vc[cnt];
                vd= *libera_vd[cnt];
                mm=bpm_voltage_to_mm((cnt>3)?1:0,va,vb,vc,vd);

                DafneAccumulatorBPMSyncLDBG_<<" "<<cnt<<" :"<<libera_va[cnt]->getName()<<" "<<libera_va[cnt]->getInfo().getTimeStamp()<<": ("<<mm.x<<" mm, "<<mm.y<<" mm) Voltages:"<<va <<" "<<vb <<" "<<(int32_t)vc<<" "<<vd;
                
                getAttributeCache()->setOutputAttributeValue(out,(void*)&mm.x,sizeof(double));
                getAttributeCache()->setOutputAttributeValue(out+1,(void*)&mm.y,sizeof(double));
    }
    getAttributeCache()->setOutputDomainAsChanged();

}

//!Execute the Control Unit work
void DafneAccumulatorBPMSync::unitStop() throw(chaos::CException) {
group->stop(1);
}

//!Deinit the Control Unit
void DafneAccumulatorBPMSync::unitDeinit() throw(chaos::CException) {
group->deinit(1);
}

//! pre imput attribute change
void DafneAccumulatorBPMSync::unitInputAttributePreChangeHandler() throw(chaos::CException) {

}

//! attribute changed handler
void DafneAccumulatorBPMSync::unitInputAttributeChangedHandler() throw(chaos::CException) {

  
}


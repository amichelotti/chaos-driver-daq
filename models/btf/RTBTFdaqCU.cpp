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
#include <common/debug/core/debug.h>
using namespace chaos;

using namespace chaos::common::data;
using namespace chaos::common::data::cache;

using namespace chaos::common::batch_command;

using namespace chaos::cu::control_manager::slow_command;
using namespace chaos::cu::driver_manager::driver;
using namespace ::driver::daq::btf;
namespace chaos_batch = chaos::common::batch_command;

#define ENABLE_VETO 0x0
#define DISABLE_VETO 0x1

#define CLOSEDEV(_x) \
    _x ## _close(_x ## _handle);

#define OPENDEV(_x)						\
    DPRINT("opening %s at address 0x%x",# _x ,(uint32_t) * _x ## _addr);\
    if(_x ## _addr && (_x ## _handle = _x ## _open(VME_UNIVERSE2_DRIVER,(uint32_t) * _x ## _addr))){ \
    DPRINT("* " # _x " successfully mapped\n");\
    } else {\
    throw chaos::CException(-4,"## cannot map " # _x " ", __PRETTY_FUNCTION__);\
    }


#define SCCUAPP LAPP_ << "[RTBTFdaqCU - " << getCUID() << "] - "<<__PRETTY_FUNCTION__<<":"
#define SCCULDBG LDBG_ << "[RTBTFdaqCU - " << getCUID() << "] - "<<__PRETTY_FUNCTION__<<":"

PUBLISHABLE_CONTROL_UNIT_IMPLEMENTATION(::driver::daq::btf::RTBTFdaqCU)

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
    // vme:{"driver":"stringa","params":"stringa"}
    // qdc:{"channels":16,"address":"0xaaaa0000"}
    // tdc:{"channels":16,"address":"0xaaaa0000"}
    // pio:{"channels":16,"address":"0xaaaa0000"}
    // sis:{"channels":16,"address":"0xaaaa0000"}
   caen965_handle=NULL;
   caen792_handle=NULL;
   sis3800_handle=NULL;
   caen513_handle=NULL;
   caen792_chans=16;
   caen965_chans=16;

   CDataWrapper params;
   std::string vme_param,vme_driver;
   params.setSerializedJsonData(_control_unit_param.c_str());
   if(!params.hasKey("vme")){
       throw chaos::CException(-1,"missing 'vme' key",_control_unit_id);
   }
   chaos::common::data::CDWUniquePtr vmep=params.getCSDataValue("vme");
   if(!vmep->hasKey("driver")){
       throw chaos::CException(-1,"missing 'driver' in 'vme'",_control_unit_id);
   } else {
       vme_driver=vmep->getStringValue("driver");
   }
   if(vmep->hasKey("params")){
       vme_param=vmep->getStringValue("params");
   }
    vme=vmewrap_init_driver(vme_driver.c_str(),(void*)vme_param.c_str());
    if(vme==NULL){
        throw chaos::CException(-1,"error initializing driver:"+vmep->getJSONString(),_control_unit_id);
    }
    if(params.hasKey("caen965")){
        chaos::common::data::CDWUniquePtr p=params.getCSDataValue("caen965");
        if(!p->hasKey("address")){
            throw chaos::CException(-1,"missing 'address' key :"+p->getJSONString(),_control_unit_id);
        }
        if(p->hasKey("channels")){
               caen965_chans=p->getInt32Value("channels");
        }
        unsigned add=strtoul(p->getStringValue("address").c_str(),0,0);
        caen965_handle=caen965_open(vme,add);
        if(!caen965_handle){
            throw chaos::CException(-1,"cannot initialize caen965",_control_unit_id);
        }
    }
    if(params.hasKey("caen792")){
        chaos::common::data::CDWUniquePtr p=params.getCSDataValue("caen792");
        if(!p->hasKey("address")){
            throw chaos::CException(-1,"missing 'address' key :"+p->getJSONString(),_control_unit_id);
        }
         if(p->hasKey("channels")){
               caen792_chans=p->getInt32Value("channels");
        }
        unsigned add=strtoul(p->getStringValue("address").c_str(),0,0);
        caen792_handle=caen792_open(vme,add);
        if(!caen792_handle){
            throw chaos::CException(-1,"cannot initialize caen792",_control_unit_id);
        }
    }

   

    if(params.hasKey("sis")){
        chaos::common::data::CDWUniquePtr p=params.getCSDataValue("sis");
        if(!p->hasKey("address")){
            throw chaos::CException(-1,"missing 'address' key :"+p->getJSONString(),_control_unit_id);
        }
        unsigned add=strtoul(p->getStringValue("address").c_str(),0,0);
        sis3800_handle=sis3800_open(vme,add);
        if(!sis3800_handle){
            throw chaos::CException(-1,"cannot initialize sis",_control_unit_id);
        }
    }
    if(params.hasKey("pio")){
        chaos::common::data::CDWUniquePtr p=params.getCSDataValue("pio");
        if(!p->hasKey("address")){
            throw chaos::CException(-1,"missing 'address' key :"+p->getJSONString(),_control_unit_id);
        }
        unsigned add=strtoul(p->getStringValue("address").c_str(),0,0);
        caen513_handle=caen513_open(vme,add);
        if(!caen513_handle){
            throw chaos::CException(-1,"cannot initialize pio",_control_unit_id);
        }
    }

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

    if(caen965_handle){
        if(caen965_chans==16){
            addBinaryAttributeAsSubtypeToDataSet("QDC965HI","Vector of Channels High Resolution",chaos::DataType::SUB_TYPE_INT32,caen965_chans*sizeof(int32_t),chaos::DataType::Output);
            addBinaryAttributeAsSubtypeToDataSet("QDC965LO","Vector of Channels Low Resolution",chaos::DataType::SUB_TYPE_INT32,caen965_chans*sizeof(int32_t),chaos::DataType::Output);
        } else if(caen965_chans==32){
            addBinaryAttributeAsSubtypeToDataSet("QDC965","Vector of Channels Low Resolution",chaos::DataType::SUB_TYPE_INT32,caen965_chans*sizeof(int32_t),chaos::DataType::Output);
        }

    }
    if(caen792_handle){
        addBinaryAttributeAsSubtypeToDataSet("QDC792","Vector of Channels ",chaos::DataType::SUB_TYPE_INT32,caen792_chans*sizeof(int32_t),chaos::DataType::Output);
    }
    if(sis3800_handle){
        addBinaryAttributeAsSubtypeToDataSet("SCALER","Vector of 32 Counters ",chaos::DataType::SUB_TYPE_INT32,32*sizeof(int32_t),chaos::DataType::Output);
    }

}

void RTBTFdaqCU::unitDefineCustomAttribute() {

}

// Abstract method for the initialization of the control unit
void RTBTFdaqCU::unitInit() throw(CException) {

    SCCULDBG << "Initializating";
    int cnt;
    counter=counter_old=0;
    tot_lost=0;
    loop=0;

    if(caen965_handle){
        if(caen965_chans==16){
             qdchi=getAttributeCache()->getRWPtr<uint32_t>(DOMAIN_OUTPUT,"QDC965HI");
            qdclow=getAttributeCache()->getRWPtr<uint32_t>(DOMAIN_OUTPUT,"QDC965LO");
        } else {
            qdclow=getAttributeCache()->getRWPtr<uint32_t>(DOMAIN_OUTPUT,"QDC965");
            qdchi=&qdclow[16];


        }
       
    }
    if(caen792_handle){
        qdc792=getAttributeCache()->getRWPtr<uint32_t>(DOMAIN_OUTPUT,"QDC792");
    }

    if(sis3800_handle){
        counters=getAttributeCache()->getRWPtr<uint32_t>(DOMAIN_OUTPUT,"SCALER");
    }
    trigger_lost=getAttributeCache()->getRWPtr<uint64_t>(DOMAIN_OUTPUT,"TRIGGER LOST");

    acquisition=getAttributeCache()->getRWPtr<uint64_t>(DOMAIN_OUTPUT,"ACQUISITION");

    triggers=getAttributeCache()->getRWPtr<uint64_t>(DOMAIN_OUTPUT,"TRIGGER");

    
    //  caen513_init(caen513_handle,V513_CHANMODE_NEG|V513_CHANMODE_OUTPUT);
    //
    caen513_init(caen513_handle,1); //use board defaults
    
  for(cnt=8;cnt<16;cnt++)
    caen513_setChannelMode(caen513_handle,cnt, V513_CHANMODE_NEG|V513_CHANMODE_IGLITCHED|V513_CHANMODE_INPUT); // 15 trigger in
  
  for(cnt=0;cnt<8;cnt++)
    caen513_setChannelMode(caen513_handle,cnt, V513_CHANMODE_NEG|V513_CHANMODE_OUTPUT);
  




    caen965_init(caen965_handle,0,1);
    caen792_init(caen792_handle,0,1);
    sis3800_init(sis3800_handle);
    caen513_reset(caen513_handle);

    //resetTM(caen513_handle);
    // caen513_set(caen513_handle,DISABLE_VETO); // SW veto OFF



}

// Abstract method for the start of the control unit
void RTBTFdaqCU::unitStart() throw(CException) {
    setDefaultScheduleDelay(0);
    loop=0;
    counter_old=counter=0;
    tot_lost=0;
    if(sis3800_handle)
        sis3800_clear(sis3800_handle);

}
// Abstract method for the start of the control unit
void RTBTFdaqCU::unitRun() throw(CException) {
    int ret,cnt;
    uint64_t cycle0,cycle1;
    counter_old=counter;
    caen513_set(caen513_handle,((counter&0xF)<<1) |ENABLE_VETO); // SW veto ON

    if(sis3800_handle){
       /* for(cnt=0;cnt<32;cnt++){
            counters[cnt]=sis3800_readCounter(sis3800_handle,cnt);
        }*/
        sis3800_readCounter(sis3800_handle,counters,32);
        counter=counters[30];
    }
    DPRINT("start acquisition SW:%10llu HW %10u",loop,counter);
    if(loop==0){
        loop=counter;
    }
    if(counter>counter_old){
        tot_lost+=(counter-counter_old)-1;
    }
    if(caen965_handle){
        ret = caen965_acquire_channels_poll(caen965_handle,qdclow,qdchi,0,16,&cycle0,0);
    }
    //    dump_channels(out,low,cycle0,ret);
    // dump_channels(out,hi,cycle0,ret);
    if(caen792_handle){
        ret = caen792_acquire_channels_poll(caen792_handle,qdc792,0,caen792_chans,&cycle1,0);
    }
    //    dump_channels(out,ch,cycle1,ret);
    //counter_middle=sis3800_readCounter(sis3800_handle,30);
    
    *acquisition=loop;
    *trigger_lost=tot_lost;
    *triggers=*triggers+ (counter_middle-counter);
   /* 
    if(counter_middle>counter){
        int discard;

        discard=(counter_middle-counter-1);

        if(discard){
            DERR("acquisition SW %llu HW:%llu discarded, lost %d trigger(s)",loop,counter,discard);
        } else {
//////            getAttributeCache()->setOutputDomainAsChanged();

        }

    }*/
    loop++;
    getAttributeCache()->setOutputDomainAsChanged();


    //    caen513_reset(caen513_handle);
    //    caen513_set(caen513_handle,DISABLE_VETO); // SW veto OFF
    caen513_set(caen513_handle,((counter&0xF)<<1) |DISABLE_VETO); // SW veto OF
}

// Abstract method for the stop of the control unit
void RTBTFdaqCU::unitStop() throw(CException) {
    DPRINT("ACQUISITION STOP SW %llu HW %u",loop,counter);


}

// Abstract method for the deinit of the control unit
void RTBTFdaqCU::unitDeinit() throw(CException) {
    if(caen513_handle){
        caen513_close(caen513_handle);
    }
    if(caen965_handle){
        caen965_close(caen965_handle);
    }
    if(caen792_handle){
        caen792_close(caen792_handle);
    }
    if(sis3800_handle){
        sis3800_close(sis3800_handle);
    }

}

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

#define ENABLE_VETO 0x0
#define DISABLE_VETO 0xffff

#define CLOSEDEV(_x) \
  _x ## _close(_x ## _handle);

#define OPENDEV(_x)						\
  if(_x ## _addr && (_x ## _handle = _x ## _open((uint32_t)_x ## _addr))){ \
    DPRINT("* " # _x " successfully mapped\n");\
  } else {\
       throw chaos::CException(-4,"## cannot map " # _x " ", __PRETTY_FUNCTION__);\
  }


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
        addAttributeToDataSet("caen513add",
						  "VME CAEN 513 base Address (PIO)",
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
        
        
        addBinaryAttributeAsSubtypeToDataSet("QDC965HI","Vector of 16 Channels High Resolution",chaos::DataType::SUB_TYPE_INT32,16*sizeof(int32_t),chaos::DataType::Output);
        addBinaryAttributeAsSubtypeToDataSet("QDC965LO","Vector of 16 Channels Low Resolution",chaos::DataType::SUB_TYPE_INT32,16*sizeof(int32_t),chaos::DataType::Output);
        addBinaryAttributeAsSubtypeToDataSet("QDC792","Vector of 32 Channels ",chaos::DataType::SUB_TYPE_INT32,32*sizeof(int32_t),chaos::DataType::Output);
        addBinaryAttributeAsSubtypeToDataSet("SCALER","Vector of 32 Counters ",chaos::DataType::SUB_TYPE_INT32,32*sizeof(int32_t),chaos::DataType::Output);
       
	
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
       sis3800_addr=getAttributeCache()->getROPtr<uint32_t>(DOMAIN_INPUT, "scaleradd");
       caen965_addr=getAttributeCache()->getROPtr<uint32_t>(DOMAIN_INPUT,"qdc965add");
       caen792_addr=getAttributeCache()->getROPtr<uint32_t>(DOMAIN_INPUT,"qdc792add");
        caen513_addr=getAttributeCache()->getROPtr<uint32_t>(DOMAIN_INPUT,"caen513add");
        qdchi=getAttributeCache()->getRWPtr<uint32_t>(DOMAIN_INPUT,"QDC965HI");
        qdclow=getAttributeCache()->getRWPtr<uint32_t>(DOMAIN_INPUT,"QDC965LO");
        qdc792=getAttributeCache()->getRWPtr<uint32_t>(DOMAIN_INPUT,"QDC792");
        counters=getAttributeCache()->getRWPtr<uint32_t>(DOMAIN_INPUT,"SCALER");
        trigger_lost=getAttributeCache()->getRWPtr<uint64_t>(DOMAIN_OUTPUT,"TRIGGER LOST");
         acquisition=getAttributeCache()->getRWPtr<uint64_t>(DOMAIN_OUTPUT,"ACQUISITION");
        triggers=getAttributeCache()->getRWPtr<uint64_t>(DOMAIN_OUTPUT,"TRIGGER");
       if(sis3800_addr  == NULL || caen965_addr==NULL || caen792_addr==NULL ||caen513_addr==NULL){
           throw chaos::CException(-2, "BAD VME START ADDRESS", __PRETTY_FUNCTION__);
       }
       
        if(qdchi  == NULL || qdclow==NULL || qdc792==NULL ||counters==NULL){
           throw chaos::CException(-3, "BAD DATEASET", __PRETTY_FUNCTION__);
       }
       
       DPRINT("scaler add 0x%x qdc965 0x%x qdc 792 0x%x",*sis3800_addr,*caen965_addr,*caen792_addr);
       
    OPENDEV(caen965);
    sleep(1);
    OPENDEV(caen792);
    sleep(1);
    OPENDEV(sis3800);
    sleep(1);
    OPENDEV(caen513);
    sleep(1);
    caen513_reset(caen513_handle);
  //  caen513_init(caen513_handle,V513_CHANMODE_NEG|V513_CHANMODE_OUTPUT); 
  //  
  //caen513_init(caen513_handle,1); //use board defaults
  /*
  for(cnt=8;cnt<16;cnt++)
    caen513_setChannelMode(caen513_handle,cnt, V513_CHANMODE_NEG|V513_CHANMODE_IGLITCHED|V513_CHANMODE_INPUT); // 15 trigger in
  */
  for(cnt=0;cnt<16;cnt++)
    caen513_setChannelMode(caen513_handle,cnt, V513_CHANMODE_NEG|V513_CHANMODE_OUTPUT); 
  




  caen965_init(caen965_handle,0,1);
  caen792_init(caen792_handle,0,1);
  sis3800_init(sis3800_handle);

  //resetTM(caen513_handle);
  caen513_set(caen513_handle,DISABLE_VETO); // SW veto OFF  
  
            

}

// Abstract method for the start of the control unit
void RTBTFdaqCU::unitStart() throw(CException) {
	
}
// Abstract method for the start of the control unit
void RTBTFdaqCU::unitRun() throw(CException) {
    int ret;
    uint64_t cycle0,cycle1;
    loop++;
    counter_old=counter;
    counter=sis3800_readCounter(sis3800_handle,30);
    DPRINT("start acquisition SW:%10llu HW %10u",loop,counter);

    if(counter>counter_old){
      tot_lost+=(counter_old)-1;
    }
    ret = caen965_acquire_channels_poll(caen965_handle,qdclow,qdchi,0,16,&cycle0,0);
    //caen513_set(caen513_handle,ENABLE_VETO); // SW veto ON

    //    dump_channels(out,low,cycle0,ret);
    // dump_channels(out,hi,cycle0,ret);
    ret = caen792_acquire_channels_poll(caen792_handle,qdc792,0,16,&cycle1,0);
    
    //    dump_channels(out,ch,cycle1,ret);
    counter_middle=sis3800_readCounter(sis3800_handle,30);
    
    *acquisition=loop;
    *trigger_lost=tot_lost;
    *triggers=*triggers+ (counter_middle-counter);
    
    if(counter_middle>counter){
      int discard;

      discard=(counter_middle-counter-1);
      
      if(discard){
          PRINT("acquisition SW %llu HW:%llu discarded, lost %d trigger(s)",loop,counter,discard)
      } else {
          getAttributeCache()->setOutputDomainAsChanged();

      }
      
    }
    

    //    caen513_reset(caen513_handle);
    //    caen513_set(caen513_handle,DISABLE_VETO); // SW veto OFF
    //caen513_set(caen513_handle,DISABLE_VETO); // SW veto OFF


    
    loop++;
	
}

// Abstract method for the stop of the control unit
void RTBTFdaqCU::unitStop() throw(CException) {
        DPRINT("ACQUISITION STOP SW %llu HW %u",loop,counter);

	
}

// Abstract method for the deinit of the control unit
void RTBTFdaqCU::unitDeinit() throw(CException) {
  CLOSEDEV(caen513);
  CLOSEDEV(caen965);
  CLOSEDEV(caen792);
  CLOSEDEV(sis3800);
  
}

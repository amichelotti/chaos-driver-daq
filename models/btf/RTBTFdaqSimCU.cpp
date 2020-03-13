/*
 *	RTBTFdaqSimCU
 *	!CHAOS
 *	Created by Andrea Michelotti 1/2/2016
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

#include "RTBTFdaqSimCU.h"
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


#define SCCUAPP LAPP_ << "[RTBTFdaqSimCU - " << getCUID() << "] - "<<__PRETTY_FUNCTION__<<":"
#define SCCULDBG LDBG_ << "[RTBTFdaqSimCU - " << getCUID() << "] - "<<__PRETTY_FUNCTION__<<":"

PUBLISHABLE_CONTROL_UNIT_IMPLEMENTATION(::driver::daq::btf::RTBTFdaqSimCU)

/*
 Construct a new CU with an identifier
 */
RTBTFdaqSimCU::RTBTFdaqSimCU(const string& _control_unit_id,
														const string& _control_unit_param,
														const ControlUnitDriverList& _control_unit_drivers):
//call base constructor
RTBTFdaqCU(_control_unit_id,
												  _control_unit_param,
												  _control_unit_drivers){

  
}

/*
 Base destructor
 */
RTBTFdaqSimCU::~RTBTFdaqSimCU() {
	
}


/*
 Return the default configuration
 */
void RTBTFdaqSimCU::unitDefineActionAndDataset() throw(chaos::CException) {
  
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

void RTBTFdaqSimCU::unitDefineCustomAttribute() {
	
}

// Abstract method for the initialization of the control unit
void RTBTFdaqSimCU::unitInit() throw(CException) {
	
	SCCULDBG << "Initializating";
        int cnt;
        counter=counter_old=0;
        tot_lost=0;
        loop=0;
        
       sis3800_addr=getAttributeCache()->getROPtr<uint32_t>(DOMAIN_INPUT, "scaleradd");
       caen965_addr=getAttributeCache()->getROPtr<uint32_t>(DOMAIN_INPUT,"qdc965add");

       caen792_addr=getAttributeCache()->getROPtr<uint32_t>(DOMAIN_INPUT,"qdc792add");

        //caen513_addr=getAttributeCache()->getROPtr<uint32_t>(DOMAIN_INPUT,"caen513add");
          //    DPRINT("CAEN513 0x%x",caen513_addr);
       //getAttributeCache()->setOutputAttributeNewSize("QDC965HI",16*sizeof(int32_t));
       
        qdchi=getAttributeCache()->getRWPtr<uint32_t>(DOMAIN_OUTPUT,"QDC965HI");

        qdclow=getAttributeCache()->getRWPtr<uint32_t>(DOMAIN_OUTPUT,"QDC965LO");

        qdc792=getAttributeCache()->getRWPtr<uint32_t>(DOMAIN_OUTPUT,"QDC792");

        counters=getAttributeCache()->getRWPtr<uint32_t>(DOMAIN_OUTPUT,"SCALER");

        trigger_lost=getAttributeCache()->getRWPtr<uint64_t>(DOMAIN_OUTPUT,"TRIGGER LOST");

         acquisition=getAttributeCache()->getRWPtr<uint64_t>(DOMAIN_OUTPUT,"ACQUISITION");

        triggers=getAttributeCache()->getRWPtr<uint64_t>(DOMAIN_OUTPUT,"TRIGGER");

       if((sis3800_addr  == NULL) || (caen965_addr==NULL) || (caen792_addr==NULL)){
           throw chaos::CException(-2, "BAD VME START ADDRESS", __PRETTY_FUNCTION__);
       }
       
        if((qdchi  == NULL) || (qdclow==NULL) || (qdc792==NULL) ||(counters==NULL)){
            DPRINT("qdchi 0x%x qdclo 0x%x qdc792 0x%x counters 0x%x",qdchi,qdclow,qdc792,counters);
       //    throw chaos::CException(-3, "BAD DATASET", __PRETTY_FUNCTION__);
       }
       
       DPRINT("scaler add 0x%x qdc965 0x%x qdc 792 0x%x",*sis3800_addr,*caen965_addr,*caen792_addr);
       

}

// Abstract method for the start of the control unit
void RTBTFdaqSimCU::unitStart() throw(CException) {
    setDefaultScheduleDelay(REFRESH_RATE);
    loop=0;
    counter_old=counter=0;
    tot_lost=0;
	
}
// Abstract method for the start of the control unit
void RTBTFdaqSimCU::unitRun() throw(CException) {
    int ret,cnt;
    uint64_t cycle0,cycle1;
    counter_old=counter;
    for(cnt=0;cnt<32;cnt++){
        counters[cnt]=0;
    }
    counter=loop;
    
    DPRINT("start acquisition SW:%10lu HW %10u",loop,counter);

    if(counter>counter_old){
      tot_lost+=(counter-counter_old)-1;
    }
   
   
    
    *acquisition=loop;
    *trigger_lost=tot_lost;
    *triggers=*triggers+ (counter_middle-counter);
    
    if(counter_middle>counter){
      int discard;

      discard=(counter_middle-counter-1);
      
      if(discard){
          DERR("acquisition SW %u HW:%lu discarded, lost %d trigger(s)",loop,counter,discard);
      } else {
          getAttributeCache()->setOutputDomainAsChanged();

      }
      
    }
    loop++;


    //    caen513_reset(caen513_handle);
    //    caen513_set(caen513_handle,DISABLE_VETO); // SW veto OFF
    //caen513_set(caen513_handle,DISABLE_VETO); // SW veto OF	
}

// Abstract method for the stop of the control unit
void RTBTFdaqSimCU::unitStop() throw(CException) {
        DPRINT("ACQUISITION STOP SW %lu HW %u",loop,counter);

	
}

// Abstract method for the deinit of the control unit
void RTBTFdaqSimCU::unitDeinit() throw(CException) {
 // CLOSEDEV(caen513);

  
}

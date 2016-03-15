/*
 *	RTCAEN.h
 *	!CHAOS
 *	Created by Andrea Michelotti
 *      Collects and align a given series of Libera BPMs
 *    	Copyright 2015 INFN, National Institute of Nuclear Physics
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
#ifndef _RTCAEN_h
#define _RTCAEN_h

#include <chaos/cu_toolkit/control_manager/RTAbstractControlUnit.h>
#include <driver/misc/core/RTVme.h>
#include <boost/algorithm/string.hpp>
#include <chaos/ui_toolkit/LowLevelApi/LLRpcApi.h>
#include <chaos/cu_toolkit/command_manager/CommandManager.h>
#include <common/debug/core/debug.h>
#include <common/vme/caen/CaenBase.h>
#include <chaos/common/utility/TimingUtil.h>
#include <stdint.h>
using namespace chaos::common::data::cache;
using namespace chaos::common::utility;
using namespace chaos::cu::driver_manager::driver;
using namespace chaos::cu::control_manager;

    namespace driver {
        namespace daq {
        namespace caen {
        template <class CAEN>
	  class RTCAEN : public ::driver::misc::RTVme {
	  public:
    /*!
     Construct a new CU with full constructor
     */
	    RTCAEN<CAEN>(const std::string& _control_unit_id, const std::string& _control_unit_param, const ControlUnitDriverList& _control_unit_drivers):
	    	::driver::misc::RTVme(_control_unit_id,
	    	                        "",
	    	                        _control_unit_drivers) {
	    		caen =NULL;

	    	}
	    /*!
     Destructor a new CU
     */
    ~RTCAEN<CAEN>(){
    	unitDeinit();

    }


protected:
    uint64_t* events;
    uint64_t* acq_cycle;
    CAEN* caen;
    uint32_t *chp;
    const uint32_t *bset,*bclr;
  	uint32_t last_event,event;

public:
    int32_t channels;
    int32_t crate_num;
    int32_t timeo_ms;
    void unitDefineActionAndDataset() throw(chaos::CException){
    	::driver::misc::RTVme::unitDefineActionAndDataset();

    		addAttributeToDataSet("CHANNELS",
    		                        "Number of channels available",
    		                        chaos::DataType::TYPE_INT64,
    		                        chaos::DataType::Input);

    		addAttributeToDataSet("CRATE_NUM",
    		                        "Crate Number",
    		                        chaos::DataType::TYPE_INT32,
    		                        chaos::DataType::Input);

    		addAttributeToDataSet("TIMEOUT",
    			                        "Timeout acquisition in ms (0=no timeout)",
    			                        chaos::DataType::TYPE_INT32,
    			                        chaos::DataType::Input);
    		for(int cnt=0;cnt<32;cnt++){
    			std::stringstream ss;
    			ss<<"THRESHOLD"<<cnt;
    			addAttributeToDataSet(ss.str().c_str(),
    					                        "Threshold memory",
    					                        chaos::DataType::TYPE_INT32,
    					                        chaos::DataType::Input);
    			addHandlerOnInputAttributeName< ::driver::daq::caen::RTCAEN<CAEN>, int32_t >(this,
    			                                                            &::driver::daq::caen::RTCAEN<CAEN>::setThreshold,
    																		ss.str().c_str());

    		}
    		addAttributeToDataSet("BITSET2",
    						                        "Set Operational control mode",
    						                        chaos::DataType::TYPE_INT32,
    						                        chaos::DataType::Input);

    		addAttributeToDataSet("BITCLR2",
    							                        "Clear Operational control mode",
    							                        chaos::DataType::TYPE_INT32,
    							                        chaos::DataType::Input);
    	    addAttributeToDataSet("ACQUISITION",
    	                        "Acquisition cycle",
    	                        chaos::DataType::TYPE_INT64,
    	                        chaos::DataType::Output);

    	    addAttributeToDataSet("EVENTS",
    	                            "Events counted",
    	                            chaos::DataType::TYPE_INT64,
    	                            chaos::DataType::Output);
    	    addBinaryAttributeAsSubtypeToDataSet("CH","Vector of acquired channels",chaos::DataType::SUB_TYPE_INT32,32*sizeof(int32_t),chaos::DataType::Output);


    	// actions
    	    addHandlerOnInputAttributeName< ::driver::daq::caen::RTCAEN<CAEN>, int32_t >(this,
    	                                                            &::driver::daq::caen::RTCAEN<CAEN>::setMode,
    	                                                            "BITSET2");

    	    addHandlerOnInputAttributeName< ::driver::daq::caen::RTCAEN<CAEN>, int32_t >(this,
    	                                                                &::driver::daq::caen::RTCAEN<CAEN>::clrMode,
    	                                                                "BITCLR2");

    }


    void unitInit() throw(chaos::CException){
	    AttributeSharedCacheWrapper * cc=getAttributeCache();

    	vme_base_address = *(cc->getROPtr< uint64_t >(chaos::common::data::cache::DOMAIN_INPUT, "VME_BASE"));


    	vme_driver_type= (vme_driver_t)*(cc->getROPtr<uint64_t>(DOMAIN_INPUT, "VME_DRIVER"));
    	channels = *(cc->getROPtr<uint32_t>(DOMAIN_INPUT, "CHANNELS"));
    	timeo_ms = *(cc->getROPtr<int32_t>(DOMAIN_INPUT, "TIMEOUT"));
    	crate_num = *(cc->getROPtr<int32_t>(DOMAIN_INPUT, "CRATE_NUM"));
    	bset = cc->getROPtr<uint32_t>(DOMAIN_INPUT, "BITSET2");
    	bclr =cc->getROPtr<uint32_t>(DOMAIN_INPUT, "BITCLR2");

    	events=cc->getRWPtr<uint64_t>(DOMAIN_OUTPUT, "EVENTS");
    	acq_cycle=cc->getRWPtr<uint64_t>(DOMAIN_OUTPUT, "ACQUISITION");
    	chp=cc->getRWPtr<uint32_t>(DOMAIN_OUTPUT, "CH");
    	if(caen==NULL){
    		caen =new CAEN();
			if(caen==NULL){
					 throw chaos::CException(-1,__PRETTY_FUNCTION__,"cannot allocate device");
			 }

    	}
    	if(vme_base_address==0){
    		  throw chaos::CException(-1,__PRETTY_FUNCTION__,"invalid VME_BASE address ");
    	}

    	if(caen->open(vme_driver_type,vme_base_address)){
    			 throw chaos::CException(-1,__PRETTY_FUNCTION__,"cannot open device");

    	}
    	last_event=event=0;
    	caen->init(crate_num,true);
    	cc->setOutputAttributeNewSize("CH", caen->getNumberOfChannels()*sizeof(int32_t));
    	DPRINT("detected %s",caen->getBoard().c_str());
    	*events=0;
    	*acq_cycle=0;
    	last_event=event=0;


    }

    void unitStart() throw(chaos::CException){

   	 if(*bset){
   		 DPRINT("setting BITSET2 0x%x",*bset);
   		 caen->setMode((caen_modes_t)*bset);
   	 }
   	 if(*bset){
   	 		 DPRINT("setting BITCLR2 0x%x",*bset);
   	 		 caen->clrMode((caen_modes_t)*bclr);
   	 }
   	 caen->resetEventBuffer();
   	 *events=0;
   	 *acq_cycle=0;
   	 last_event=event=0;
    }
    void unitStop() throw(chaos::CException){

    }
    void unitDeinit() throw(chaos::CException){
   	 if(caen){
   		 DPRINT("deallocating caen 0x%x",caen);
   			 caen->close();
   			 delete caen;
   			 caen=NULL;
   		 }
    }
    void unitRun() throw(chaos::CException){
   	 int ret;

   	 	 bzero((void*)chp,caen->getNumberOfChannels()*sizeof(uint32_t));
   		 if(caen->waitEvent(timeo_ms)==0){

   			 ret=caen->acquireChannels(chp,&event);
   			 *events+=(event-last_event);
   			 last_event=event;

   			 if(ret){
   				 LDBG_<<"* acquired "<< ret <<" channels, event tag "<<event<<" events:"<<*events<<" loop:"<<*acq_cycle;

   				 getAttributeCache()->setOutputDomainAsChanged();
   			 }
   			 (*acq_cycle)++;
   			}
    }

    bool setMode(const std::string &name,int32_t value,uint32_t size){
    	DPRINT("set mode 0x%x",value);
    	caen->setMode((caen_modes_t)value);
    	return true;
    }
    bool clrMode(const std::string &name,int32_t value,uint32_t size){
    	DPRINT("set CLR mode 0x%x",value);
    	caen->clrMode((caen_modes_t)value);
    	return true;
    }
    bool setThreshold(const std::string &name,int32_t value,uint32_t size){

    		std:string channel=name.substr(sizeof("THRESHOLD"));
    		int ich=atoi(channel.c_str());
    		DPRINT("set threshold channel %d = 0x%x",ich,value);
    		caen->setThreashold(ich,value);
    		return true;

    }
};
            }
        }
    }
#endif


/*
 *	daqClient.cpp
 *	!CHOAS
 *	Created by Andrea Michelotti
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

#include <stdio.h>

#include <chaos/ui_toolkit/ChaosUIToolkit.h>
#include <chaos/ui_toolkit/LowLevelApi/LLRpcApi.h>
#include <chaos/ui_toolkit/HighLevelApi/HLDataApi.h>
#include "LiberaData.h"
using namespace chaos;
using namespace chaos::ui;

void print_state(CUStateKey::ControlUnitState state) {
  switch (state) {
    case CUStateKey::INIT:
    std::cout << "Initialized" << std::endl;
    break;
    case CUStateKey::START:
    std::cout << "Started" << std::endl;
    break;
    case CUStateKey::STOP:
    std::cout << "Stopped" << std::endl;
    break;
    case CUStateKey::DEINIT:
    std::cout << "Deinitilized" << std::endl;
    break;
  }
}


int main (int argc, char* argv[] ) {
  int err = 0;
  int mode=0,offset=0,sched=0;
  bool triggered=false,decimated=false;
  int samples=1;
  std::string attribute_value_tmp_str;
  std::string ofile;
  CUStateKey::ControlUnitState device_state;
  try{
    
    ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption("acquire", po::value<int>(&mode)->default_value(0), "acquire [0=OFF,1=DD,2=SA,3=ADC_SP,4=ADC_CW]");
    ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption("triggered", po::value<bool>(&triggered)->default_value(false), "trigger on/off");
    ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption("samples", po::value<int>(&samples)->default_value(1), "acquires samples");
    ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption("offset", po::value<int>(&offset)->default_value(0), "in DD ofset of acquisition");
    ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption("ofile", po::value<std::string>(&ofile), "output on file");

    ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption("decimated", po::value<bool>(&decimated)->default_value(true), "decimated data on/off");
    ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption("sched", po::value<int>(&sched)->default_value(0), "acquire time");

      
      //init UIToolkit client
    ChaosUIToolkit::getInstance()->init(argc, argv);

    
    DeviceController *controller = HLDataApi::getInstance()->getControllerForDeviceID("device_name", 40000);
    if(!controller) return -1;

    //init device
    std::cout << "Init the device" << std::endl;
    err = controller->initDevice();
    if(err == ErrorCode::EC_TIMEOUT) return -1;
    print_state(device_state);
    sleep(2);

    //check the state
    err = controller->getState(device_state);
    if(err == ErrorCode::EC_TIMEOUT) return -1;
    print_state(device_state);
    sleep(2);

    //start the device
    std::cout << "Start the device" << std::endl;
    err = controller->startDevice();
    sleep(2);

    //check the state
    err = controller->getState(device_state);
    if(err == ErrorCode::EC_TIMEOUT) return -1;
    print_state(device_state);
    sleep(2);
    uint64_t command_id = 0;
    int mode_dev=0;
    if(triggered){
        mode_dev=LIBERA_IOP_MODE_TRIGGERED;
    }
    
    if(decimated){
        mode_dev|=LIBERA_IOP_MODE_DECIMATED;
    }
    CDataWrapper param_mode;

   	
    switch(mode){
        case 0:
            param_mode.addInt32Value("enable",0);
            break;
            
       case 1:
           mode_dev|=LIBERA_IOP_MODE_DD;
            break;
        case 2:
           mode_dev|=LIBERA_IOP_MODE_SA;
            break;
            
        case 3:
            mode_dev|=LIBERA_IOP_MODE_SINGLEPASS;
            break;
        case 4:
            mode_dev|=LIBERA_IOP_MODE_CONTINUOUS;
            break;
            
            
    }
    param_mode.addInt32Value("mode",mode_dev);
    if(mode_dev && (samples>0)){
        param_mode.addInt32Value("samples",samples);
    }
    
    err = controller->submitSlowControlCommand("acquire",
     //         					       chaos_batch::SubmissionRuleType::SUBMIT_AND_Stack,
              chaos_batch::SubmissionRuleType::SUBMIT_AND_Kill,
              100,
              command_id,
              0,
              0, // delay
              0,
              &param_mode);

     if(err == ErrorCode::EC_TIMEOUT) throw CException(2, "Time out on connection", "Set device to deinit state");

    //print all dataset
    controller->fetchCurrentDatatasetFromDomain((DatasetDomain)0);
    if(controller->getCurrentDatasetForDomain((DatasetDomain)0) != NULL) {
      if(!ofile.empty()){
          if(freopen(ofile.c_str(),"w",stdout)== NULL){
              std::cerr<<" cannot open :"<<ofile <<" for write"<<std::endl;
              return -1;
          }
          
      } 
      switch(mode){
            
       case 1:{
           CDataWrapper *wrapped_data =controller->getCurrentData();
           if(wrapped_data){
               libera_dd_t* data=(libera_dd_t*)wrapped_data->getRawValuePtr("DD");
               int samp=wrapped_data->getInt32Value("SAMPLES");
               if(samp!=samples){
                   std::cerr<<"## sample required:"<<samples<<" different from acquired:"<<samp;
                   return -2;
               }
               
               if(data){
                libera_dd_desc_t h;
                std::cout<<h;
                for(int cnt=0;cnt<samples;cnt++){
                       std::cout<<data[cnt];
                   }
               }
           }
           
           
       }
           break;
        case 2:
           mode_dev|=LIBERA_IOP_MODE_SA;
            break;
            
        case 3:
            mode_dev|=LIBERA_IOP_MODE_SINGLEPASS;
            break;
        case 4:
            mode_dev|=LIBERA_IOP_MODE_CONTINUOUS;
            break;
      }
      std::cout << controller->getCurrentDatasetForDomain((DatasetDomain)0)->getJSONString() <<std::endl;
    }
  
  

    controller->setScheduleDelay(sched);
    sleep(1);
  } catch(CException& e) {
    std::cerr << e.errorCode << " - "<< e.errorDomain << " - " << e.errorMessage << std::endl;
    return -3;
  }

    return 0;
}

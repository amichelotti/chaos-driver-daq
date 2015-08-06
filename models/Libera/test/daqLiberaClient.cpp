
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

//#include <fstream>
#include "LiberaData.h"
using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::ui;
using namespace chaos::common::batch_command;
template <typename T>
void print_header(int ts_enable,std::ofstream &fout){
    T h;
    if(ts_enable){
      fout<<"TS,";
      LDBG_<<"TS,";
    }
    fout<<h<<flush;
    LDBG_<<h;
}

template <typename T>
void print_data(T data,int ts_enable,int samples,uint64_t tstamp,std::ofstream &fout){
    for(int cnt=0;cnt<samples;cnt++){
            if(ts_enable){
               fout<<tstamp<<",";
               LDBG_<<tstamp<<",";
            }
            fout<<data[cnt]<<flush;
            LDBG_<<data[cnt];
    }

}
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
  bool triggered=false,decimated=false,timestamp=false;
  int samples=1,loops=1,max_acquire_time;
  std::string attribute_value_tmp_str;
  std::string ofile;
  std::ofstream ofs_out;
  CUStateKey::ControlUnitState device_state;
  std::string device_name;
  uint64_t old_acquisition=0;
  try{

    ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption("acquire", po::value<int>(&mode)->default_value(0), "acquire [0=OFF,1=DD,2=SA,3=ADC_SP,4=ADC_CW]");
    ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption("triggered", po::value<bool>(&triggered)->default_value(false), "trigger on/off");
    ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption("samples", po::value<int>(&samples)->default_value(1), "acquires samples");
    ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption("offset", po::value<int>(&offset)->default_value(0), "in DD ofset of acquisition");
    ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption("ofile", po::value<std::string>(&ofile)->default_value("libera.out"), "output on file");
    ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption("loops", po::value<int>(&loops)->default_value(1), "acquires loops <0 for continuous acquisition, SA is continuos");

    ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption("decimated", po::value<bool>(&decimated)->default_value(false), "decimated data on/off");
    ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption("timestamp", po::value<bool>(&timestamp)->default_value(false), "dump timestamp");
    ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption("max_acquire_time", po::value<int>(&max_acquire_time)->default_value(0), "max acquire time in seconds 0=continuos ");

    ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption("sched", po::value<int>(&sched)->default_value(1000000), "acquire time");

    ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption("device", po::value<std::string>(&device_name), "libera device name");




      //init UIToolkit client
    ChaosUIToolkit::getInstance()->init(argc, argv);

    if(device_name.empty()){
        std::cerr<<"## device name is required"<<std::endl;
        return -1;
    }
    DeviceController *controller = HLDataApi::getInstance()->getControllerForDeviceID(device_name, 40000);
    if(!controller) {
        std::cerr<<"## cannot connect to "<<device_name<<std::endl;

        return -2;
    }
    //init device
    err = controller->getState(device_state);
    if(err == ErrorCode::EC_TIMEOUT) return -1;

    if(device_state == CUStateKey::DEINIT){
        err = controller->initDevice();
        if(err == ErrorCode::EC_TIMEOUT) {
            throw CException(2, "Initialization", "timeout");
        }
        //print_state(device_state);
        sleep(1);
    }
    //check the state
    //print_state(device_state);
    //sleep(2);
    controller->setScheduleDelay(sched);
    //start the device
    err = controller->getState(device_state);
    if(err == ErrorCode::EC_TIMEOUT) {
        throw CException(2, "Initialization", "timeout");
    }

    if(device_state != CUStateKey::START){
        err = controller->startDevice();
        if(err == ErrorCode::EC_TIMEOUT) {
            throw CException(2, "Initialization", "timeout");
        }
        //print_state(device_state);
        sleep(2);
    }
    //std::cout << "Start the device" << std::endl;


    //check the state
    err = controller->getState(device_state);
    if(err == ErrorCode::EC_TIMEOUT || device_state!= CUStateKey::START) {
        throw CException(2, "Initialization", "device not in run state");

    }
    //print_state(device_state);
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
    if(mode_dev && (samples>=0)){
        param_mode.addInt32Value("samples",samples);
    }
    param_mode.addInt32Value("duration",max_acquire_time);
    param_mode.addInt32Value("loops",loops);

    err = controller->submitSlowControlCommand("acquire",
     //         					       SubmissionRuleType::SUBMIT_AND_Stack,
              SubmissionRuleType::SUBMIT_AND_Kill,
              100,
              command_id,
              0,
              sched, // delay
              0,
              &param_mode);

     if(err == ErrorCode::EC_TIMEOUT) throw CException(2, "Time out on connection", "Set device to deinit state");
    sleep(1);
    if(mode==0){
         err = controller->submitSlowControlCommand("default",
     //         					       SubmissionRuleType::SUBMIT_AND_Stack,
              SubmissionRuleType::SUBMIT_AND_Kill,
              100,
              command_id,
              0,
              sched, // delay
              0,
              &param_mode);

     if(err == ErrorCode::EC_TIMEOUT) throw CException(2, "Time out on connection", "Set device to deinit state");
        sleep(1);

        return 0;
    }
    //print all dataset
    if(!ofile.empty()){
           LAPP_<<"opening "<<ofile;

           ofs_out.open(ofile.c_str(),std::ofstream::out );
           if((ofs_out.good()==false) || (ofs_out == NULL)){
                LERR_<<" cannot open :"<<ofile <<" for write";
                return -3;
           }
           LAPP_<<"opening "<<ofile << " for writing.";

      }
    LAPP_<<"dumping:"<<mode << " samples:"<<samples;

    int*pmode;
    uint64_t tstamp;
    uint64_t*acquisition;
    libera_dd_t* data1;
    libera_sa_t* data2;
    libera_cw_t* data3;
    libera_sp_t* data4;
    libera_avg_t* data5;
    uint64_t counter=0;
    do {
    controller->fetchCurrentDeviceValue();

      CDataWrapper *wrapped_data =controller->getCurrentData();

       if(wrapped_data==NULL){
          throw CException(2, "Error fetching", "Dataset");
      }

      pmode=(int*)wrapped_data->getRawValuePtr("MODE");
      controller->getTimeStamp(tstamp);
      data1=(libera_dd_t*)wrapped_data->getRawValuePtr("DD");
      data2=(libera_sa_t*)wrapped_data->getRawValuePtr("SA");
      data3=(libera_cw_t*)wrapped_data->getRawValuePtr("ADC_CW");
      data4=(libera_sp_t*)wrapped_data->getRawValuePtr("ADC_SP");
      data5=(libera_avg_t*)wrapped_data->getRawValuePtr("AVG");

      acquisition=(uint64_t*)wrapped_data->getRawValuePtr("ACQUISITION");

      if(!(data1 && data2 && data3 && data4 && data5&&acquisition&& pmode )){
          throw CException(2, "Error fetching", "pointers");

      }
      int samp=wrapped_data->getInt32Value("SAMPLES");
      if(samp!=samples){
            std::cerr<<"## sample required:"<<samples<<" different from acquired:"<<samp;
            return -2;
      }

    if(old_acquisition==*acquisition){
       LDBG_<<" no new data received, old acquire:"<<old_acquisition<<" current:"<<*acquisition;
       continue;

    }

      switch(mode){

       case 1:
           if(counter==0){
               print_header<libera_dd_desc_t> (timestamp,ofs_out);
           }
           print_data(data1,timestamp,samples,tstamp,ofs_out);


       break;

         case 2:
         if(counter==0){
               print_header<libera_sa_desc_t> (timestamp,ofs_out);
           }
           print_data(data2,timestamp,samples,tstamp,ofs_out);

        break;
        case 3:
         if(counter==0){
               print_header<libera_cw_desc_t> (timestamp,ofs_out);
           }
           print_data(data3,timestamp,samples,tstamp,ofs_out);

       break;
       case 4:
        if(counter==0){
               print_header<libera_sp_desc_t> (timestamp,ofs_out);
           }
           print_data(data4,timestamp,samples,tstamp,ofs_out);

       break;

         case 5:
             if(counter==0){
               print_header<libera_avg_desc_t> (timestamp,ofs_out);
           }
           print_data(data5,timestamp,samples,tstamp,ofs_out);

       break;

      }
      LDBG_<<" mode:"<<*pmode<<" acquisition:"<<*acquisition<<" timestamp:"<<tstamp;
      old_acquisition=*acquisition;
      counter++;
    } while (*pmode!=0);
     // std::cout << controller->getCurrentDatasetForDomain((DatasetDomain)0)->getJSONString() <<std::endl;

          ofs_out.close();



  } catch(CException& e) {
    std::cerr << e.errorCode << " - "<< e.errorDomain << " - " << e.errorMessage << std::endl;
    return -3;
  }

    return 0;
}

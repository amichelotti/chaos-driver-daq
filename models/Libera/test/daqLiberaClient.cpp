
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

#include <driver/daq/models/Libera/ChaosControllerLibera.h>
#include <driver/misc/core/ChaosControllerGroup.h>
#include <driver/misc/core/ChaosDatasetAttribute.h>
#include <driver/misc/core/ChaosDatasetAttributeSinchronizer.h>
#include <chaos_metadata_service_client/ChaosMetadataServiceClient.h>


//#include <fstream>
#include "LiberaData.h"
using namespace chaos;
using namespace std;
using namespace chaos::common::data;
using namespace chaos::common::batch_command;
using namespace driver::misc;
using namespace driver::daq::libera;
using namespace chaos::metadata_service_client;

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


int main (int argc, const char* argv[] ) {
  int err = 0;
  int mode=0,offset=0,sched=0;
  bool triggered=false,decimated=false,timestamp=false;
  int samples=1,loops=1,max_acquire_time;
  std::string attribute_value_tmp_str;
  std::string ofile;
  std::ofstream ofs_out;
  CUStateKey::ControlUnitState device_state;
  std::vector<std::string> device_name;
  uint64_t old_acquisition=0;
 
  try{
    ChaosMetadataServiceClient::getInstance()->getGlobalConfigurationInstance()->addOption("acquire", po::value<int>(&mode)->default_value(0), "acquire [0=OFF,1=DD,2=SA,3=ADC_SP,4=ADC_CW]");
    ChaosMetadataServiceClient::getInstance()->getGlobalConfigurationInstance()->addOption("triggered", po::value<bool>(&triggered)->default_value(false), "trigger on/off");
    ChaosMetadataServiceClient::getInstance()->getGlobalConfigurationInstance()->addOption("samples", po::value<int>(&samples)->default_value(1), "acquires samples");
    ChaosMetadataServiceClient::getInstance()->getGlobalConfigurationInstance()->addOption("offset", po::value<int>(&offset)->default_value(0), "in DD ofset of acquisition");
    ChaosMetadataServiceClient::getInstance()->getGlobalConfigurationInstance()->addOption("ofile", po::value<std::string>(&ofile)->default_value("libera.out"), "output on file");
    ChaosMetadataServiceClient::getInstance()->getGlobalConfigurationInstance()->addOption("loops", po::value<int>(&loops)->default_value(1), "acquires loops <0 for continuous acquisition, SA is continuos");

    ChaosMetadataServiceClient::getInstance()->getGlobalConfigurationInstance()->addOption("decimated", po::value<bool>(&decimated)->default_value(false), "decimated data on/off");
    ChaosMetadataServiceClient::getInstance()->getGlobalConfigurationInstance()->addOption("timestamp", po::value<bool>(&timestamp)->default_value(false), "dump timestamp");
    ChaosMetadataServiceClient::getInstance()->getGlobalConfigurationInstance()->addOption("max_acquire_time", po::value<int>(&max_acquire_time)->default_value(0), "max acquire time in seconds 0=continuos ");

    ChaosMetadataServiceClient::getInstance()->getGlobalConfigurationInstance()->addOption("sched", po::value<int>(&sched)->default_value(100000), "acquire time");

    ChaosMetadataServiceClient::getInstance()->getGlobalConfigurationInstance()->addOption("device,d", po::value<std::vector<std::string> >(&device_name), "libera device name");




      //init UIToolkit client
    ChaosMetadataServiceClient::getInstance()->init(argc, argv);

    if(device_name.empty()){
        std::cerr<<"## device name is required"<<std::endl;
        return -1;
    }
    
    ChaosControllerLibera* libera_devs[device_name.size()];
    ChaosDatasetAttribute* libera_va[device_name.size()];
    ChaosDatasetAttribute* libera_vb[device_name.size()];
    ChaosDatasetAttribute* libera_vc[device_name.size()];
    ChaosDatasetAttribute* libera_vd[device_name.size()];
    
    ChaosDatasetAttribute* libera_acquisition[device_name.size()];
    
    ChaosControllerGroup<ChaosControllerLibera> group;
    ChaosDatasetAttributeSinchronizer data_group;
    int cu=0;
    for(vector<std::string>::iterator i = device_name.begin();i!=device_name.end();i++,cu++){
        libera_devs[cu] = new ChaosControllerLibera(i->c_str());
        
        libera_va[cu] = new ChaosDatasetAttribute(*i + "/VA");
        libera_vb[cu] = new ChaosDatasetAttribute(*i + "/VB");
        libera_vc[cu] = new ChaosDatasetAttribute(*i + "/VC");
        libera_vd[cu] = new ChaosDatasetAttribute(*i + "/VD");
        libera_acquisition[cu] = new ChaosDatasetAttribute(*i + "/ACQUISITION");
        data_group.add(libera_acquisition[cu] );
        
        if(libera_devs[cu]){
            group.add(*libera_devs[cu]);
        }
        
    }
    sleep(5);
    int rett;
     if(rett=group.init(1)){
         LERR_<<" error forcing init:"<<rett;
         return -4;
     }
    sleep(5);
    if(group.setSchedule(sched)!=0){
        LERR_<<" ## cannot set schedule at:"<<sched;
        return -2;
    }
    if(rett=group.start(1)){
        LERR_<<" error forcing start:"<<rett;
        return -5;
    }
    sleep(5);
   
    
    
    
    switch(mode){
        case 0:
            group.acquire_disable();
            break;
        case 1:
            if(group.acquire_dd(samples,loops,2000000,triggered)!=0){
                LERR_<<" error giving group command DD";
                return -3;
                
            }
            break;
        case 2:
            if(group.acquire_sa(samples,loops,2000000,triggered)!=0){
                LERR_<<" error giving group command SA";
                return -3;
            }
            break;
         
            
    }
    //print all dataset
    if(!ofile.empty()){
           LAPP_<<"opening "<<ofile;

           ofs_out.open(ofile.c_str(),std::ofstream::out );
           if((ofs_out.good()==false) ){
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
    
    data_group.setInterval(3000000);
    data_group.setTimeout (6000000);
    
    while(loops--){
        data_group.sync();
        cu=0;
        for(vector<std::string>::iterator i = device_name.begin();i!=device_name.end();i++,cu++){
            bpmpos mm;
            mm=bpm_voltage_to_mm(0,*libera_va[cu],*libera_vb[cu],*libera_vc[cu],*libera_vd[cu]);
            ofs_out<<*i<< " [" <<(uint64_t)*libera_acquisition[cu]<<" "<<libera_va[cu]->getInfo().getTimeStamp()<<": ("<<mm.x<<" mm, "<<mm.y<<" mm) Voltages:"<<(int32_t)*libera_va[cu] <<" "<<(int32_t)*libera_vb[cu] <<" "<<(int32_t)*libera_vc[cu]<<" "<<(int32_t)*libera_vd[cu]<<std::endl;

        }
    }
/*    do {
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


*/
  } catch(CException& e) {
    std::cerr << e.errorCode << " - "<< e.errorDomain << " - " << e.errorMessage << std::endl;
    return -3;
  }

    return 0;
}

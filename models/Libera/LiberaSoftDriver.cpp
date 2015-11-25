/*  
 * LiberaSoftDriver.cpp
 * @author michelo
Copyright Apr 29, 2015 michelo

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
 */

#include "LiberaSoftDriver.h"
#include <boost/thread/mutex.hpp>
#include <boost/regex.hpp>
#include <stdlib.h>
#define ILK_PARAMCOUNT 8
#include <chaos/cu_toolkit/driver_manager/driver/AbstractDriverPlugin.h>
#include <common/misc/wavegenerators/WaveFactory.h>
#include <boost/lexical_cast.hpp>

#define LiberaBrillianceCSPILAPP_		LAPP_ << "[LiberaSoftDriver] "
#define LiberaBrillianceCSPILDBG_		LDBG_ << "[LiberaSoftDriver "<<__PRETTY_FUNCTION__<<" ]"
#define LiberaBrillianceCSPILERR_		LERR_ << "[LiberaSoftDriver "<<__PRETTY_FUNCTION__<<" ]"
using namespace chaos::cu::driver_manager::driver;
static boost::mutex io_mux;
OPEN_CU_DRIVER_PLUGIN_CLASS_DEFINITION(LiberaSoftDriver, 1.0.0, LiberaSoftDriver)
REGISTER_CU_DRIVER_PLUGIN_CLASS_INIT_ATTRIBUTE(LiberaSoftDriver, http_address / dnsname : port)
CLOSE_CU_DRIVER_PLUGIN_CLASS_DEFINITION




static volatile size_t _event_id = 0;
static pthread_cond_t eventc = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t eventm = PTHREAD_MUTEX_INITIALIZER;

//GET_PLUGIN_CLASS_DEFINITION
//we need to define the driver with alias version and a class that implement it
//default constructor definition
LiberaSoftDriver::LiberaSoftDriver() {
    int rc;
    cfg.operation =liberaconfig::deinit;
/*
    if((rc=initIO(0,0))!=0){
        throw chaos::CException(rc,"Initializing","LiberaSoftDriver::LiberaSoftDriver");    
    }
 */
}




//default descrutcor

LiberaSoftDriver::~LiberaSoftDriver() {
  deinitIO();  
}
int LiberaSoftDriver::wait_trigger(){
    	int rc = 0;
        usleep(trigger_time_ms*1000);
        return 0;
}
int LiberaSoftDriver::read(void *buffer, int addr, int bcount) {
  	int rc;
	// Allways seek(), not just the first time.
         if((cfg.operation == liberaconfig::acquire)&& (cfg.datasize>0)){
          size_t nread=0; //initialize variable to 0
          
          if (cfg.mask & liberaconfig::want_trigger) {
	    if((rc=wait_trigger())!=0){
                LiberaBrillianceCSPILERR_<<"Error waiting trigger:"<<rc;

                return -rc;
            }
          }
	  boost::mutex::scoped_lock lock(io_mux);
	  
          if(cfg.mode ==CSPI_MODE_SA){
              
              libera_sa_t* tt=(libera_sa_t*)buffer;
              memset(tt,0,bcount);
              tt->Va =wave->generate();
              tt->Vb =wave->generate();
              tt->Vc =wave->generate();
              tt->Vd =wave->generate();
              return 1;
          }
          if(bcount<(cfg.atom_count*cfg.datasize)){
              LiberaBrillianceCSPILERR_<<"POSSIBLE error, buffer is smaller than required"<<rc;
          }
          int count = std::min(bcount/cfg.datasize,cfg.atom_count);
          
          
          
	  rc = (cfg.mask & liberaconfig::want_trigger) ? CSPI_SEEK_TR : CSPI_SEEK_MT;
              
	  if(addr==CHANNEL_DD){
	   
            libera_dd_t*dd=(libera_dd_t*)buffer;
            for(int cnt=0;cnt<count;cnt++){
                memset(&dd[cnt],0,sizeof(libera_dd_t));
                dd[cnt].Va=wave->generate();
                dd[cnt].Vb=wave->generate();
                dd[cnt].Vc=wave->generate();
                dd[cnt].Vd=wave->generate();
            }
	 
	    return count;
	  }
         }
        
        
       return 0;        
      
}

int LiberaSoftDriver::write(void *buffer, int addr, int bcount) {
    //TODO: implement the method
    return 0;
}
// assign MT and ST from a string formatted as [MT]:[YYYYMMDDhhmm.ss]

int LiberaSoftDriver::assign_time(const char*time ){
      const char delim = ':';
        const char delim_phase = '.';
        std::string s(time);

        size_t p = s.find(delim);
        if (std::string::npos == p) {
            LiberaBrillianceCSPILERR_<<"Invalid argument -- 'TIME' missing delimiter \":\"" ;
             return -4;
        }
        std::string s2(s.substr(0, p-0));
        if (!s2.empty()) {

                cfg.mask |= liberaconfig::want_setmt;

                size_t p_phase = s2.find(delim_phase);
                if (std::string::npos == p_phase) {
                        // No LMT Phase specified
                        cfg.time.mt = atoll(s2.c_str());
                        cfg.time.phase = 0;

                } else {
                        // MT + LMT Phase specified
                        std::string s_mt(s2.substr(0, p_phase-0));
                        std::string s_phase(s2.substr(p_phase+1));

                        cfg.time.mt = atoll(s_mt.c_str());
                        if (!s_phase.empty())
                                cfg.time.phase = atoll(s_phase.c_str());
                        else
                                cfg.time.phase = 0;
                }
        }

        s2 = s.substr(p+1);
        if (!s2.empty()) {

                for (p=4; p < (s2.size()-3); ++p)
                        if (p%3 == 1) s2.insert(p, 1, delim);

                         struct tm t;
                if (!strptime(s2.c_str(), "%Y:%m:%d:%H:%M.%S", &t)){
                    LiberaBrillianceCSPILERR_<<"Invalid argument -- 'strptime'";
                    return -1;
                }
                cfg.time.st = mktime(&t);
                if (-1 == cfg.time.st){
                         LiberaBrillianceCSPILERR_<<"Invalid argument -- 'mkTIME'";

                    return -2;
                }

                cfg.mask |= liberaconfig::want_setst;
        }
        
        return 0;
       
}
// trigger_time,WaveGen:init_params
static boost::regex drv_opt("(\\d+),([a-zA-Z]+):(.+)");

int LiberaSoftDriver::initIO(void *buffer, int sizeb) {
    
    boost::smatch match;
    std::string param=(const char*)buffer;
    if(boost::regex_match(param,match,drv_opt,boost::match_extra)){
        wave = common::misc::wavegenerators::WaveFactory::getGenerator(match[2],match[3]);
    } else {
        
         LiberaBrillianceCSPILERR_<< "bad option parameters:"<<param;
         return -1;
    }
    
   
    cfg.operation = liberaconfig::init;
    cfg.atom_count=1;
    cfg.datasize=0;

    ep.trig_mode = CSPI_TRIGMODE_GET;
     
   
    
    return 0;
}

int LiberaSoftDriver::deinitIO() {
    if(cfg.operation == liberaconfig::deinit){
          LiberaBrillianceCSPILERR_<<"Already de-initializad";
    }
    wave.reset();
    return 0;
}


/*
 
#define LIBERA_IOP_MODE_DD 0x1 // data acquire on demand
#define LIBERA_IOP_MODE_SS 0x2 // streaming data acquire
#define LIBERA_IOP_MODE_ADC 0x4 // ADC data acquire
#define LIBERA_IOP_MODE_PM 0x8 // Post Mortem data acquire
#define LIBERA_IOP_MODE_AVG 0x10 // Average data acquire
#define LIBERA_IOP_MODE_TRIGGERED 0x100
#define LIBERA_IOP_MODE_DECIMATED 0x200
#define LIBERA_IOP_MODE_CONTINUOUS 0x400
#define LIBERA_IOP_MODE_SINGLEPASS 0x800

#define LIBERA_IOP_CMD_ACQUIRE 0x1
#define LIBERA_IOP_CMD_SETENV 0x2 // Setting environment
#define LIBERA_IOP_CMD_GETENV 0x3 // getting environment
#define LIBERA_IOP_CMD_SETTIME 0x4 // Setting Time
#define LIBERA_IOP_CMD_SET_OFFSET 0x5 // set offset in buffer
#define LIBERA_IOP_CMD_SET_SAMPLES 0x6 // set offset in buffer


 */
int LiberaSoftDriver::iop(int operation, void*data, int sizeb) {
    int rc;
    CSPI_ENVPARAMS ep;
    boost::mutex::scoped_lock lock(io_mux);

#define SET_ENV(cpimask,param) \
if(cmd_env->selector & CSPI_ENV_## cpimask ){\
    env.param =cmd_env->value;\
    LiberaBrillianceCSPILDBG_<<"IO SET ENV \""<<#cpimask<<"\" bitmask:"<<cmd_env->selector<<" value="<<cmd_env->value;\
}
    
            
    switch(operation){
        case LIBERA_IOP_CMD_GET_TS:
            CSPI_TIMESTAMP ts;
            struct timeval tts;
            gettimeofday(&tts,NULL);
            ts.st.tv_sec=tts.tv_sec;
            ts.st.tv_nsec=tts.tv_usec*1000;
            memcpy(data,&ts,std::min((uint32_t)sizeb,(uint32_t)sizeof(CSPI_TIMESTAMP)));
            return 0;
             
            break;
        case LIBERA_IOP_CMD_STOP:
            LiberaBrillianceCSPILDBG_<<"IOP STOP"<<driver_mode;

            cfg.operation = liberaconfig::unknown;

            break;
        case LIBERA_IOP_CMD_ACQUIRE:
            driver_mode = *(int*)data;
            LiberaBrillianceCSPILDBG_<<"IOP Acquire driver mode:"<<driver_mode;
            if(driver_mode&LIBERA_IOP_MODE_TRIGGERED){
                cfg.mask|=cfg.want_trigger;
                LiberaBrillianceCSPILDBG_<<"Enable Trigger";
            
            } else {
                cfg.mask&=~cfg.want_trigger;
                LiberaBrillianceCSPILDBG_<<"Disable Trigger";
                myenv.trig_mode = 0;

            }
            if(driver_mode&LIBERA_IOP_MODE_DECIMATED){
                cfg.dd.decimation =1;
                LiberaBrillianceCSPILDBG_<<"Enable Decimation";

            } else {
                cfg.dd.decimation =0;
                LiberaBrillianceCSPILDBG_<<"Disable Decimation";

            }
                 //const size_t modes[] = {CSPI_MODE_DD, CSPI_MODE_SA, CSPI_MODE_PM, CSPI_MODE_ADC, CSPI_MODE_AVERAGE};

            if(driver_mode&LIBERA_IOP_MODE_DD){
              cfg.mode =CSPI_MODE_DD;
              LiberaBrillianceCSPILDBG_<<"Acquire Data on Demand";
              cfg.operation = liberaconfig::acquire;
              cfg.datasize=sizeof(CSPI_DD_ATOM);

            }
            if(driver_mode&LIBERA_IOP_MODE_SA){
                cfg.mode =CSPI_MODE_SA;
                LiberaBrillianceCSPILDBG_<<"Acquire Data on Streaming";
                cfg.operation = liberaconfig::acquire;
                cfg.datasize=sizeof(CSPI_SA_ATOM);
            }
            if(driver_mode&LIBERA_IOP_MODE_PM){
                cfg.mode =CSPI_MODE_PM;
                LiberaBrillianceCSPILDBG_<<"Acquire Data Post Mortem";
                cfg.operation = liberaconfig::acquire;
                cfg.datasize=sizeof(CSPI_DD_ATOM);
            }
            if(driver_mode&LIBERA_IOP_MODE_ADC){
                cfg.mode =CSPI_MODE_ADC;
                LiberaBrillianceCSPILDBG_<<"Acquire ADC Data";
                cfg.datasize=sizeof(CSPI_ADC_ATOM);
                cfg.operation = liberaconfig::acquire;

                if(driver_mode&LIBERA_IOP_MODE_CONTINUOUS){
                    cfg.adc.mode|=liberaconfig::adc_specific::cw;
                    cfg.mode = CSPI_MODE_ADC_CW;
                    LiberaBrillianceCSPILDBG_<<"Acquire ADC Data Continuous";
                     cfg.operation = liberaconfig::acquire;
                     cfg.datasize=sizeof(CSPI_ADC_CW_ATOM);

                } else {
                    cfg.adc.mode&=~liberaconfig::adc_specific::cw;
                }
                if(driver_mode&LIBERA_IOP_MODE_SINGLEPASS){
                    cfg.adc.mode|=liberaconfig::adc_specific::sp;
                    cfg.mode = CSPI_MODE_ADC_SP;
                    LiberaBrillianceCSPILDBG_<<"Acquire ADC Data Single Pass";
                     cfg.operation = liberaconfig::acquire;
                     cfg.datasize=sizeof(CSPI_ADC_SP_ATOM);
                } else {
                    cfg.adc.mode&=~liberaconfig::adc_specific::sp;
                }
            }
            if(operation&LIBERA_IOP_MODE_AVG){
                cfg.mode =CSPI_MODE_AVERAGE;
                cfg.atom_count = 1;
                LiberaBrillianceCSPILDBG_<<"Acquire Average Data";
                cfg.operation = liberaconfig::acquire;
                cfg.datasize=sizeof(CSPI_AVERAGE_ATOM);

            }
            
            break;
        case LIBERA_IOP_CMD_SET_SAMPLES:
               cfg.atom_count = *(int *)data;
                LiberaBrillianceCSPILDBG_<<"Setting Samples:"<<cfg.atom_count;

               break;    
        case LIBERA_IOP_CMD_SET_OFFSET:
               cfg.dd.offset = *(int *)data;
               LiberaBrillianceCSPILDBG_<<"Setting Offset:"<<cfg.dd.offset;

               break;
        case LIBERA_IOP_CMD_SETENV:{
            cfg.operation = liberaconfig::setenv;
            libera_env_t* cmd_env=(libera_env_t*)data;
            CSPI_ENVPARAMS &env=myenv;
            
            SET_ENV(KX,Kx);
            SET_ENV(KY,Ky);
            SET_ENV(XOFFSET,Xoffset);
            SET_ENV(YOFFSET,Yoffset);
            SET_ENV(QOFFSET,Qoffset);
            SET_ENV(SWITCH,switches);
            SET_ENV(GAIN,gain);
            SET_ENV(AGC,agc);
            SET_ENV(DSC,dsc);
            SET_ENV(ILK,ilk.mode);
            
                SET_ENV(ILKSTATUS, ilk_status);
                SET_ENV(PMOFFSET, PMoffset);
                
                SET_ENV(PMDEC, PMdec);

               SET_ENV(TRIGDELAY, trig_delay);
               
                SET_ENV(EXTSWITCH, external_switching);
                SET_ENV(SWDELAY, switching_delay);
                SET_ENV(TRIGMODE,trig_mode);
                SET_ENV(DDC_MAFLENGTH, ddc_maflength);
               SET_ENV(DDC_MAFDELAY, ddc_mafdelay);

               SET_ENV(NOTCH1, notch1[0]);
               SET_ENV( NOTCH2, notch2[0]);
                SET_ENV(POLYPHASE_FIR, polyphase_fir[0]);

                SET_ENV(MTVCXOFFS, mtvcxoffs);
                SET_ENV(MTNCOSHFT, mtncoshft);
                SET_ENV(MTPHSOFFS, mtphsoffs);
                SET_ENV( MTUNLCKTR, mtunlcktr);
                SET_ENV( MTSYNCIN, mtsyncin);
                SET_ENV(STUNLCKTR, stunlcktr);

                SET_ENV( PM, pm.mode);
               SET_ENV( SR, sr.enable);
                SET_ENV(SP, sp.threshold);
               
            }
        
            break;
        case LIBERA_IOP_CMD_GETENV:{
            char *pdata=(char*)data;
            CSPI_BITMASK mask = ~(0LL);
            cfg.operation = liberaconfig::listenv;
            LiberaBrillianceCSPILDBG_<<"GET ENV";
            
            std::stringstream ss;
            ss<<myenv;
            strncpy(pdata,ss.str().c_str(),std::min((uint32_t)sizeb,(uint32_t)ss.str().size()));
            break;
        }
        case LIBERA_IOP_CMD_SETTIME:{
            

        return 0;

        }
         
    }
    
    if(cfg.operation == liberaconfig::acquire){
        CSPI_BITMASK event_mask=0;
        if (cfg.mask & liberaconfig::want_trigger) {
            event_mask |= CSPI_EVENT_TRIGGET;
        }

	
        LiberaBrillianceCSPILDBG_<<"connecting to HW..cfg:x"<<std::hex<<cfg.mask<<std::dec;

       /* raw_data = (char*)realloc(raw_data,cfg.atom_count*cfg.datasize);
        if(raw_data==NULL){
            cfg.operation = liberaconfig::unknown;
             LiberaBrillianceCSPILERR_<<"Cannot allocate buffer of:"<<cfg.atom_count*cfg.datasize <<" bytes";
             return -100;
        }*/
	p.mode = cfg.mode;
        
	CSPI_BITMASK param_mask = CSPI_CON_MODE;
	if (event_mask) param_mask |= (CSPI_CON_HANDLER|CSPI_CON_EVENTMASK);
        LiberaBrillianceCSPILDBG_<<"Setting connection parameters on:"<<con_handle<<" :"<<param_mask;

	
        LiberaBrillianceCSPILDBG_<<"Connected with HW mode:"<<cfg.mode;

    }
  
    return 0;
}

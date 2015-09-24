/*  
 * LiberaBrillianceCSPI.cpp
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

#include "LiberaBrillianceCSPIDriver.h"
#include <boost/thread/mutex.hpp>
#define ILK_PARAMCOUNT 8
#include <chaos/cu_toolkit/driver_manager/driver/AbstractDriverPlugin.h>

#include <boost/lexical_cast.hpp>

#define LiberaBrillianceCSPILAPP_		LAPP_ << "[LiberaBrillianceCSPI] "
#define LiberaBrillianceCSPILDBG_		LDBG_ << "[LiberaBrillianceCSPI] "
#define LiberaBrillianceCSPILERR_		LERR_ << "[LiberaBrillianceCSPI] "
using namespace chaos::cu::driver_manager::driver;
static boost::mutex io_mux;
OPEN_CU_DRIVER_PLUGIN_CLASS_DEFINITION(LiberaBrillianceCSPIDriver, 1.0.0, LiberaBrillianceCSPIDriver)
REGISTER_CU_DRIVER_PLUGIN_CLASS_INIT_ATTRIBUTE(LiberaBrillianceCSPIDriver, http_address / dnsname : port)
CLOSE_CU_DRIVER_PLUGIN_CLASS_DEFINITION



/*
int LiberaBrillianceCSPIDriver::read_libera(liberaData_t *data,int size){
    //	rc = cfg.mask & config::want_trigger ? CSPI_SEEK_TR : CSPI_SEEK_MT;
    int cnt=0;
    int ret;
    size_t re=0;
    int _type = CSPI_SEEK_MT;
    unsigned long long offset=0;
    CSPI_DD_ATOM *p =(CSPI_DD_ATOM *)malloc(size*sizeof(CSPI_DD_ATOM)); 
    cspi_seek(con_handle,&offset,_type);
    ret=cspi_read(con_handle,p,size,&re);
     
          
    while((cnt<size)){
        data[cnt].libera.va = p[cnt].Va;
        data[cnt].libera.vb = p[cnt].Vb;
        data[cnt].libera.vc = p[cnt].Vc;
        data[cnt].libera.vd = p[cnt].Vd;
        data[cnt].libera.x = p[cnt].X;
        data[cnt].libera.y = p[cnt].Y;
        data[cnt].libera.q = p[cnt].Q;
        data[cnt].libera.sum = p[cnt].Sum;
        LiberaBrillianceCSPILDBG_<<re<<"] read :"<<ret<<" params, "<<data[cnt].libera.va<<","<<data[cnt].libera.vb<<","<<data[cnt].libera.vc<<","<<data[cnt].libera.vd<<","<<data[cnt].libera.x<<","<<data[cnt].libera.y<<","<<data[cnt].libera.q<<","<<data[cnt].libera.sum<<","<<data[cnt].libera.q1<<","<<data[cnt].libera.q2;
        cnt++;

    }
    free(p);
    
}
*/
static volatile size_t _event_id = 0;
static pthread_cond_t eventc = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t eventm = PTHREAD_MUTEX_INITIALIZER;

int event_callback(CSPI_EVENT *p)
{
	_event_id = p->hdr.id;
        LiberaBrillianceCSPILDBG_<<"Trigger Event arised:"<<_event_id;
	pthread_cond_signal(&eventc);

	return 0;
}
//GET_PLUGIN_CLASS_DEFINITION
//we need to define the driver with alias version and a class that implement it
//default constructor definition
LiberaBrillianceCSPIDriver::LiberaBrillianceCSPIDriver() {
    int rc;
    cfg.operation =liberaconfig::deinit;
/*
    if((rc=initIO(0,0))!=0){
        throw chaos::CException(rc,"Initializing","LiberaBrillianceCSPIDriver::LiberaBrillianceCSPIDriver");    
    }
 */
}


std::stringstream& operator<<(std::stringstream& os, const CSPI_ENVPARAMS& obj){
	const size_t tab = 17;
	const char* labels[] = {
		"Kx [nm]", "Ky [nm]",
		"Xoffset [nm]", "Yoffset [nm]", "Qoffset [nm]",
		"Switches",
		"Level [dBm]",
		"AGC",
		"DSC",
		//"Interlock",
		0
	};

	const int *p = reinterpret_cast<const int*>(&obj);
	
	// Health
	os << std::setw(tab) << "Temp [C]" << ": ";
	os << *p++ << std::endl;

	os << std::setw(tab) << "Fans [rpm]" << ": ";
	os << *p++ << " " << *p++ << std::endl;

	os << std::setw(tab) << "Voltages [mV]" << ": ";

	for (size_t i=0; i<8; ++i) {
		os << *p++ << " ";
	}
	os << std::endl;

	os << std::setw(tab) << "SC PLL" << ": ";
	os << (*p++ ? "locked" : "unlocked") << std::endl;

	os << std::setw(tab) << "MC PLL" << ": ";
	os << (*p++ ? "locked" : "unlocked") << std::endl;

	// TRIGmode
	os << std::setw(tab) << "TRIGmode" << ": ";
	os << *p++ << std::endl;

	// Feature registers
	os << std::setw(tab) << "Feature" << ": 0x";
	os << std::hex << std::setfill('0');
	os << std::setw(8) << *p++ << " 0x";
	unsigned long feature = *p++;
	os << std::setw(8) << feature;
	os << std::dec << std::setfill(' ') ;
	if ( LIBERA_IS_BRILLIANCE(feature) )
		os << ", Brilliance";
	if ( LIBERA_IS_GBETHERNET(feature) ) {
		os << ", GbE";
		if ( LIBERA_IS_GBE_DEMO(feature) )
			os << "_demo";
	}
	if ( LIBERA_IS_DESY_MOLEX(feature) )
		os << ", Desy Molex";
	if ( LIBERA_IS_GROUPING(feature) )
		os << ", Grouping (RIO)";
	if ( LIBERA_IS_MAF(feature) )
		os << ", MAF";
	if ( LIBERA_IS_DCC(feature) )
		os << ", Communication Controller";
	os << std::endl;
	// From Kx/y on
	for (size_t i=0; labels[i]; ++i) {
		os << std::setw(tab) << labels[i] << ": " << *p++ << std::endl;
	}

	os << std::setw(tab) << "Interlock" << ": ";
	const int *q = p + ILK_PARAMCOUNT - 1;

	std::copy(p, q, std::ostream_iterator<int>(std::cout," "));
	os << *q << std::endl;
	p += ILK_PARAMCOUNT;

	os << std::setw(tab) << "InterlockStatus" << ": ";
	unsigned long ilkStatus = *p++;
	os << ilkStatus;
	if ( INTERLOCK_XPOSITION(ilkStatus) )
			os << ", X pos";
	if ( INTERLOCK_YPOSITION(ilkStatus) )
			os << ", Y pos";
	if ( INTERLOCK_ATT_LIMIT(ilkStatus) )
			os << ", attenuator limit";
	if ( INTERLOCK_ADC_OVERFLOW(ilkStatus) )
			os << ", ADC overflow";
	if ( INTERLOCK_ADC_NOT_FILTERED(ilkStatus) )
			os << ", ADC not filtered";
	os << std::endl;
	os << std::setw(tab) << "PostMortem" << ": ";
	os << obj.pm.mode << " ";
	os << obj.pm.min_x_limit << " ";
	os << obj.pm.max_x_limit << " ";
	os << obj.pm.min_y_limit << " ";
	os << obj.pm.max_y_limit << " ";
	os << obj.pm.overflow_limit << " ";
	os << obj.pm.overflow_dur << " ";
	os << std::endl;
	os << std::setw(tab) << "PMoffset, PMdec" << ": ";
	os << *p++ << ", ";
	os << *p++ << std::endl;
	os << std::setw(tab) << "TrigDelay" << ": ";
	os << *p++ << std::endl;

	os << std::setw(tab) << "ExternalSwitching" << ": ";
	os << (*p++ ? "external" : "internal") << std::endl;
	os << std::setw(tab) << "SwitchingDelay" << ": ";
	os << *p++ << std::endl;

	if ( LIBERA_IS_MAF(feature) ) {
		os << std::setw(tab) << "MAF Length, Delay" << ": ";
		os << obj.ddc_maflength << ", " << obj.ddc_mafdelay << std::endl;
	}

	const mt_status_t& mt_stat = obj.pll_status.mt_stat;
	const st_status_t& st_stat = obj.pll_status.st_stat;

	os << std::setw(tab) << "MT controller" << ": ";
	os << "RtmSts= " << mt_stat.status << " "
	<< "OffsTune= " << mt_stat.vcxo_offset << " "
	<< "CompTune= " << (mt_stat.nco_shift ? "yes":"no") << " "
	<< std::endl;
	os << std::setw(tab) << "ST controller" << ": ";
	os << "RtmSts= " << st_stat.status << " "
	<< std::endl;

	os << std::setw(tab) << "SpikeRemoval" << ": ";
	os << obj.sr.enable << " ";
	os << obj.sr.cspi_enable << " ";
	os << obj.sr.averaging_stop << " ";
	os << obj.sr.average_window << " ";
	os << obj.sr.start << " ";
	os << obj.sr.window << " ";
	os << std::endl;

	os << std::setw(tab) << "SinglePass" << ": ";
	os << obj.sp.threshold << " ";
	os << obj.sp.n_before << " ";
	os << obj.sp.n_after << " ";
	os << std::endl;

	os << std::setw(tab) << "MaxADC" << ": ";
	os << obj.max_adc << " "	<< std::endl;

	return os ;
}

//default descrutcor

LiberaBrillianceCSPIDriver::~LiberaBrillianceCSPIDriver() {
  deinitIO();  
}
int LiberaBrillianceCSPIDriver::wait_trigger(){
    	int rc = 0;
	struct timeval  now;
	struct timespec timeout;


	do {

		gettimeofday( &now, 0 );
		timeout.tv_sec = now.tv_sec + 30;
		timeout.tv_nsec = now.tv_usec * 1000;
                LiberaBrillianceCSPILDBG_<<" waiting Trigger";
                pthread_mutex_lock(&eventm);
		rc = pthread_cond_timedwait( &eventc, &eventm, &timeout );
                pthread_mutex_unlock(&eventm);

                LiberaBrillianceCSPILDBG_<<" exit wait stat:"<<rc;
	} while((0 == rc) && (CSPI_EVENT_TRIGGET != _event_id));


	if (ETIMEDOUT==rc) {
            LiberaBrillianceCSPILERR_<<"trigger timeout:"<<rc;
            return rc;

        }
        return 0;
}
int LiberaBrillianceCSPIDriver::read(void *buffer, int addr, int bcount) {
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
              rc= cspi_get(con_handle,buffer);
               if (CSPI_OK != rc) {
                     LiberaBrillianceCSPILERR_<<"Error reading"<<rc;
                     return -rc;
               }
              return 1;
          }
          if(bcount<(cfg.atom_count*cfg.datasize)){
              LiberaBrillianceCSPILERR_<<"POSSIBLE error, buffer is smaller than required"<<rc;
          }
          int count = std::min(bcount/cfg.datasize,cfg.atom_count);
          
          
          
	  rc = (cfg.mask & liberaconfig::want_trigger) ? CSPI_SEEK_TR : CSPI_SEEK_MT;
              
	  if(addr==CHANNEL_DD){
            //  LiberaBrillianceCSPILDBG_ << "DD seeeking off: "<<std::dec<<cfg.dd.offset<<" tr:"<<rc;
	    rc = cspi_seek(con_handle, &cfg.dd.offset, rc);
	    if (CSPI_OK != rc) {
	      LiberaBrillianceCSPILERR_<<"Error seeking"<<rc;
	      return -rc;
	    }
	    rc=cspi_read(con_handle,buffer,count,&nread);
	    if (CSPI_OK != rc) {
	      LiberaBrillianceCSPILERR_<<"Error reading"<<rc;
	      return -rc;
	    }
	    return nread;
	  }
         }
        
        
       return 0;        
      
}

int LiberaBrillianceCSPIDriver::write(void *buffer, int addr, int bcount) {
    //TODO: implement the method
    return 0;
}
// assign MT and ST from a string formatted as [MT]:[YYYYMMDDhhmm.ss]

int LiberaBrillianceCSPIDriver::assign_time(const char*time ){
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

int LiberaBrillianceCSPIDriver::initIO(void *buffer, int sizeb) {
    size_t ef = CSPI_ENV_TRIGMODE;
    if(cfg.operation!=liberaconfig::deinit){
       LiberaBrillianceCSPILERR_<<"not in deinit state:"<<cfg.operation;
       return 0;
    }
    cfg.operation = liberaconfig::init;
    cfg.atom_count=1;
    cfg.datasize=0;

    ep.trig_mode = CSPI_TRIGMODE_GET;
     
    cspi_setlibparam(&lib, CSPI_LIB_SUPERUSER);
    int rc = cspi_allochandle(CSPI_HANDLE_ENV, 0, &env_handle);
    if (CSPI_OK != rc) {
        LiberaBrillianceCSPILERR_<<"Cannot allocate CSPI env resources";
        return rc;
    }
    rc = cspi_setenvparam(env_handle,&ep,ef);
    if (CSPI_OK != rc) {
        LiberaBrillianceCSPILERR_<<"Cannot set env";
        return rc;
    }
    rc = cspi_allochandle(CSPI_HANDLE_CON, env_handle, &con_handle);
    
    if (CSPI_OK != rc) {
        LiberaBrillianceCSPILERR_<<"Cannot allocate CSPI connection resources";    
        return rc;
    }
    
    return 0;
}

int LiberaBrillianceCSPIDriver::deinitIO() {
    if(cfg.operation == liberaconfig::deinit){
          LiberaBrillianceCSPILERR_<<"Already de-initializad";
    }
    cfg.operation =liberaconfig::deinit;
   /* if(raw_data){
        free(raw_data);
        raw_data=NULL;
    }*/
    if(env_handle){
        int rc = cspi_freehandle(CSPI_HANDLE_ENV, env_handle);
        if (CSPI_OK != rc) {
            LiberaBrillianceCSPILERR_<<"Cannot de-allocate CSPI ENV resources";
            return rc;
        }
        env_handle =NULL;
    }
        
    
    if(con_handle){
        int rc = cspi_freehandle(CSPI_HANDLE_CON, con_handle);
        if (CSPI_OK != rc) {
            LiberaBrillianceCSPILERR_<<"Cannot de-allocate CSPI CONN resources";
            return rc;
        }
        con_handle =NULL;
    }
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
int LiberaBrillianceCSPIDriver::iop(int operation, void*data, int sizeb) {
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
             rc = cspi_gettimestamp(con_handle,&ts);
             if(rc==CSPI_OK){
                 LiberaBrillianceCSPILDBG_<<" TS:"<<ts.st.tv_sec<<" :"<<ts.st.tv_nsec;
                 memcpy(data,&ts,std::min((uint32_t)sizeb,(uint32_t)sizeof(CSPI_TIMESTAMP)));
                 return 0;
             } else {
                 LiberaBrillianceCSPILERR_<<"# Error getting timestamp err:"<<rc;
                 return rc;
             }
             
            break;
        case LIBERA_IOP_CMD_STOP:
            LiberaBrillianceCSPILDBG_<<"IOP STOP"<<driver_mode;

            cspi_disconnect(con_handle);
            cfg.operation = liberaconfig::unknown;

            break;
        case LIBERA_IOP_CMD_ACQUIRE:
            driver_mode = *(int*)data;
            LiberaBrillianceCSPILDBG_<<"IOP Acquire driver mode:"<<driver_mode;
            if(driver_mode&LIBERA_IOP_MODE_TRIGGERED){
                cfg.mask|=cfg.want_trigger;
                LiberaBrillianceCSPILDBG_<<"Enable Trigger";
                /*ep.trig_mode = CSPI_TRIGMODE_SET;
                cspi_setenvparam(env_handle, &ep, CSPI_ENV_TRIGMODE);
                 */
            } else {
                cfg.mask&=~cfg.want_trigger;
                LiberaBrillianceCSPILDBG_<<"Disable Trigger";
                 ep.trig_mode = 0;
                cspi_setenvparam(env_handle, &ep, CSPI_ENV_TRIGMODE);

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
            CSPI_ENVPARAMS env;
            
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
                rc = cspi_setenvparam(env_handle,(CSPI_ENVPARAMS*) &env, cmd_env->selector);
            
                if (CSPI_OK != rc) {
                 LiberaBrillianceCSPILERR_<<"Error setting env:"<<rc;
                 return rc;
            }
        }
            break;
        case LIBERA_IOP_CMD_GETENV:{
            char *pdata=(char*)data;
            CSPI_BITMASK mask = ~(0LL);
            cfg.operation = liberaconfig::listenv;
            LiberaBrillianceCSPILDBG_<<"GET ENV";
            CSPI_ENVPARAMS env;
            rc = cspi_getenvparam(env_handle,(CSPI_ENVPARAMS*) &env, mask);
            if(data && sizeb) *pdata=0;
            if (CSPI_OK != rc) {
                 LiberaBrillianceCSPILERR_<<"Error getting env:"<<rc;

                return rc;
            }
            std::stringstream ss;
            ss<<env;
            strncpy(pdata,ss.str().c_str(),std::min((uint32_t)sizeb,(uint32_t)ss.str().size()));
            break;
        }
        case LIBERA_IOP_CMD_SETTIME:{
            CSPI_SETTIMESTAMP ts;
            CSPI_BITMASK mask = 0LL;

            cfg.operation = liberaconfig::settime;
            char*t=(char*)data;
            LiberaBrillianceCSPILDBG_<<"set time to:"<<t;

            if((rc=assign_time(t))!=0){
                 LiberaBrillianceCSPILERR_<<"Error setting time:"<<rc;
                 return rc;
            }
            if (cfg.mask & liberaconfig::want_setmt) {

                mask |= CSPI_TIME_MT;
                ts.mt    = cfg.time.mt;
                ts.phase = cfg.time.phase;
        }
        if (cfg.mask & liberaconfig::want_setst) {

                mask |= CSPI_TIME_ST;
                ts.st.tv_sec = cfg.time.st;
                ts.st.tv_nsec = 0;
        }

        
        ep.trig_mode = CSPI_TRIGMODE_SET;

        rc = cspi_setenvparam(env_handle, &ep, CSPI_ENV_TRIGMODE);
        if (CSPI_OK != rc) {
                 LiberaBrillianceCSPILERR_<<"Error setting time: setting trigger mode:"<<rc;
                 return rc;
        }

        rc = cspi_settime(env_handle, &ts, mask);
        if (CSPI_OK != rc) {
            LiberaBrillianceCSPILERR_<<"Error setting time: setting time:"<<rc;
              return rc;        
        }
        LiberaBrillianceCSPILDBG_<<"set time OK:"<<t;

        return 0;

            break;
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
        p.handler = event_callback;
	p.mode = cfg.mode;
	p.event_mask = event_mask;
        
	CSPI_BITMASK param_mask = CSPI_CON_MODE;
	if (event_mask) param_mask |= (CSPI_CON_HANDLER|CSPI_CON_EVENTMASK);
        LiberaBrillianceCSPILDBG_<<"Setting connection parameters on:"<<con_handle<<" :"<<param_mask;

	int rc = cspi_setconparam(con_handle, &p, param_mask);
	if (CSPI_OK != rc) {
            LiberaBrillianceCSPILERR_<<"Error setting connection parameters on acquire"<<rc;
            return rc;
        }
        rc = cspi_connect(con_handle);
        if (CSPI_OK != rc) {
            LiberaBrillianceCSPILERR_<<"Error connecting to the HW acquire"<<rc;
            return rc;   
        }
        LiberaBrillianceCSPILDBG_<<"Connected with HW mode:"<<cfg.mode;

    }
  
    return 0;
}

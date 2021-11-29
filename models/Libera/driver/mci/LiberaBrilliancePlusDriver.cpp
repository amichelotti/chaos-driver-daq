/*  
 * LiberaBrilliancePlusDriver.cpp
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

#include "LiberaBrilliancePlusDriver.h"
#include <boost/thread/mutex.hpp>
#include <boost/regex.hpp>
#include <stdlib.h>
#define ILK_PARAMCOUNT 8
#include <chaos/cu_toolkit/driver_manager/driver/AbstractDriverPlugin.h>
#include <boost/lexical_cast.hpp>

#if 0
const std::string cSaStream          = "boards.raf3.signals.sa";                          /* name of SA stream signal                                                */
const std::string cFaStream          = "boards.raf3.signals.fa";                          /* name of FA stream signal                                                */
const std::string cAdcDod            = "boards.raf3.signals.adc";                         /* name of adc "data on demand" signal                                     */
const std::string cTbtDod            = "boards.raf3.signals.ddc_synthetic";               /* name of turn-by-turn (DDC) "data on demand"                             */
const std::string cTdpDod            = "boards.raf3.signals.tdp_synthetic";               /* name of turn-by-turn (TDP) "data on demand"                             */
const std::string cAGCNode           = "boards.raf3.conditioning.tuning.agc.enabled";     /* path to the AGC node                                                    */
const std::string cPowerLevelNode    = "boards.raf3.conditioning.tuning.agc.power_level"; /* path to the Power Level node                                            */
#endif


#define LiberaSoftLAPP_		LAPP_ << "[LiberaBrilliancePlusDriver] "
#define LiberaSoftDBG		LDBG_ << "[LiberaBrilliancePlusDriver "<<__PRETTY_FUNCTION__<<" ]"
#define LiberaSoftERR		LERR_ << "[LiberaBrilliancePlusDriver "<<__PRETTY_FUNCTION__<<" ]"
using namespace chaos::cu::driver_manager::driver;
static ChaosMutex io_mux;
OPEN_CU_DRIVER_PLUGIN_CLASS_DEFINITION(LiberaBrilliancePlusDriver, 1.0.0, LiberaBrilliancePlusDriver)
REGISTER_CU_DRIVER_PLUGIN_CLASS_INIT_ATTRIBUTE(LiberaBrilliancePlusDriver, http_address / dnsname : port)
CLOSE_CU_DRIVER_PLUGIN_CLASS_DEFINITION




static volatile size_t _event_id = 0;
static pthread_cond_t eventc = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t eventm = PTHREAD_MUTEX_INITIALIZER;

//GET_PLUGIN_CLASS_DEFINITION
//we need to define the driver with alias version and a class that implement it
//default constructor definition
LiberaBrilliancePlusDriver::LiberaBrilliancePlusDriver() {
	int rc;
	cfg.operation =liberaconfig::deinit;
	sclient=NULL;

	/*
    if((rc=initIO(0,0))!=0){
        throw chaos::CException(rc,"Initializing","LiberaBrilliancePlusDriver::LiberaBrilliancePlusDriver");    
    }
	 */
}




//default descrutcor

LiberaBrilliancePlusDriver::~LiberaBrilliancePlusDriver() {
	deinitIO();
}
int LiberaBrilliancePlusDriver::wait_trigger(){
	int rc = 0;
	usleep(trigger_time_ms*1000);
	return 0;
}
int LiberaBrilliancePlusDriver::read(void *buffer, int addr, int bcount) {
	int rc;
	// Allways seek(), not just the first time.
	if((cfg.operation == liberaconfig::acquire)&& (cfg.datasize>0)){
		size_t nread=0; //initialize variable to 0

		if (cfg.mask & liberaconfig::want_trigger) {
			if((rc=wait_trigger())!=0){
				LiberaSoftERR<<"Error waiting trigger:"<<rc;

				return -rc;
			}
		}
		ChaosLockGuard lock(io_mux);

		if(cfg.mode ==CSPI_MODE_SA){
			RStream::MetaBufferPtr buf;
			int ret = sclient->Read(buf);

			if (ret == isig::eSuccess) {
				std::stringstream ss;
				/* print out data content */

				RStream::Buffer& mci_buffer(buf->data);
				const auto columns = mci_buffer[0].GetComponents();
				libera_sa_t* tt=(libera_sa_t*)buffer;
				memset(tt,0,bcount);
				tt->Va =mci_buffer[0][0];
				tt->Vb =mci_buffer[0][1];
				tt->Vc =mci_buffer[0][2];
				tt->Vd =mci_buffer[0][3];
				tt->Sum = tt->Va + tt->Vb + tt->Vc + tt->Vd;
				LiberaSoftDBG<<" SA VA:"<<tt->Va<<" VB:"<<tt->Vb<<" VC:"<<tt->Vc<<" VD:"<<tt->Vd << " Sum:"<<tt->Sum;
				for (size_t i = 0; i < mci_buffer.GetLength(); ++i) {
					for (size_t j = 0; j < columns ; ++j) {
						ss << "["<<i<<"]"<<std::setw(11) <<  mci_buffer[i][j] <<  "  ";
					}
					ss << std::endl;
				}
				LiberaSoftDBG<<"Raw:"<<ss.str();
			} else {
				LiberaSoftERR <<  "[SA] Read error: " << ret;
				return -3;
			}

			return 1;
		}

		 if(bcount<(cfg.atom_count*cfg.datasize)){
			 LiberaSoftERR<<"POSSIBLE error, buffer is smaller than required"<<rc;
		 }
		int count = std::min(bcount/cfg.datasize,cfg.atom_count);


		if(addr==CHANNEL_DD){

			LiberaSoftDBG<<" DA read count:"<<count;
			libera_dd_t*dd=(libera_dd_t*)buffer;
			auto mci_buffer(dodclient.CreateBuffer(count));
			isig::SignalMeta meta;
			int ret = dodclient.Read(mci_buffer, meta);
			if (ret == isig::eSuccess) {
				std::stringstream ss;

				/* print out data content */
				const auto columns = mci_buffer[0].GetComponents();

				for (size_t i = 0; i < mci_buffer.GetLength(); ++i) {
					for (size_t j = 0; j < columns ; ++j) {
						ss << "["<<i<<"]"<<std::setw(11) <<  mci_buffer[i][j] <<  "  ";
					}
					ss << std::endl;
				}
				LiberaSoftDBG<<"Raw:"<<ss.str();

				for(int cnt=0;cnt<count;cnt++){
								memset(&dd[cnt],0,sizeof(libera_dd_t));
								dd[cnt].Va=mci_buffer[cnt][0];
								dd[cnt].Vb=mci_buffer[cnt][1];
								dd[cnt].Vc=mci_buffer[cnt][2];
								dd[cnt].Vd=mci_buffer[cnt][3];
								
								LiberaSoftDBG<<" DD["<<cnt<<"] VA:"<<dd[cnt].Va<<" VB:"<<dd[cnt].Vb<<" VC:"<<dd[cnt].Vc<<" VD:"<<dd[cnt].Vd;
				}
			}else {
			  LiberaSoftERR <<  " [DOD] Read error reading samples: " << count<<" ret:"<<ret;
			  return -5;
			}


			return count;

		}
	}


	return 0;

}

int LiberaBrilliancePlusDriver::write(void *buffer, int addr, int bcount) {
	//TODO: implement the method
	return 0;
}
// assign MT and ST from a string formatted as [MT]:[YYYYMMDDhhmm.ss]

int LiberaBrilliancePlusDriver::assign_time(const char*time ){
	const char delim = ':';
	const char delim_phase = '.';
	std::string s(time);

	size_t p = s.find(delim);
	if (std::string::npos == p) {
		LiberaSoftERR<<"Invalid argument -- 'TIME' missing delimiter \":\"" ;
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
			LiberaSoftERR<<"Invalid argument -- 'strptime'";
			return -1;
		}
		cfg.time.st = mktime(&t);
		if (-1 == cfg.time.st){
			LiberaSoftERR<<"Invalid argument -- 'mkTIME'";

			return -2;
		}

		cfg.mask |= liberaconfig::want_setst;
	}

	return 0;

}
// trigger_time,WaveGen:init_params
static boost::regex drv_opt("(\\d+),(.+)");

int LiberaBrilliancePlusDriver::initIO(void *buffer, int sizeb) {
	//const char* argv[1];
	//argv[0]="LiberaBrilliancePlusDriver";
  if(buffer==NULL){
    LiberaSoftDBG<<" skipping initialization no initialization string done" ;
    return 0;
  }
	if( cfg.operation!=liberaconfig::deinit){
		LiberaSoftERR<<"must be deinitialized to initialize";
		return 0;
	}
	try{
	mci::Init();
	root = mci::Connect();
	std::string board_name_stream=(char*)buffer;
	std::string board_name_dod=(char*)buffer;
	board_name_stream+=".signals.sa";
	board_name_dod+=".signals.adc";
	nodeBase = (char*)buffer;
	snode = root.GetNode(mci::Tokenize(board_name_stream));
	dodnode = root.GetNode(mci::Tokenize(board_name_dod));
	std::string gain=nodeBase+".conditioning.switching";
	LiberaSoftDBG<<"Gain Node:"<<gain;
	gainNode = root.GetNode(mci::Tokenize(gain));
	LiberaSoftDBG<<"Gain Node opened:"<<gain;
	LiberaSoftDBG<<" getting node:\""<<board_name_stream<<"\" \""<<board_name_dod<<"\"" ;
	signal_sa = mci::CreateRemoteSignal(snode);
	signal_dod = mci::CreateRemoteSignal(dodnode);
	DOD *rSignal = dynamic_cast<DOD*> (signal_dod.get());
	RStream *rStream = dynamic_cast<RStream*> (signal_sa.get());

	/* instantiate client for reading data from the signal */

	  dodclient =rSignal->CreateClient("mydodClient");


	  sclient = new RStream::Client(rStream,"myStreamClient");
	  sclient->SetReadTimeout(std::chrono::seconds(5));
#if BASE_VER_MAJOR >= 3
	  dodclient.SetReadTimeout(std::chrono::seconds(5));
#endif
	} catch (std::exception e){
	  LiberaSoftERR<<"error creating client:"<<e.what();
	  return -1;
	} catch(...){
		LiberaSoftERR<<"error uknown exception creating client";
		return -2;
	}
	cfg.operation=liberaconfig::init;
	LiberaSoftDBG<<" Init Done";
	return 0;
}

int LiberaBrilliancePlusDriver::deinitIO() {
	if(cfg.operation == liberaconfig::deinit){
		LiberaSoftERR<<"Already de-initializad";
	}
	mci::Shutdown();
	if(sclient){
	  delete sclient;
	  sclient=NULL;
	}
	cfg.operation = liberaconfig::deinit;
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
int LiberaBrilliancePlusDriver::iop(int operation, void*data, int sizeb) {
	int rc;
	CSPI_ENVPARAMS ep;
	ChaosLockGuard lock(io_mux);

#define SET_ENV(cpimask,param) \
		if(cmd_env->selector & CSPI_ENV_## cpimask ){\
			env.param =cmd_env->value;\
			LiberaSoftDBG<<"IO SET ENV \""<<#cpimask<<"\" bitmask:"<<cmd_env->selector<<" value="<<cmd_env->value;\
		}

	try{

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
		LiberaSoftDBG<<"IOP STOP"<<driver_mode;
		if(cfg.operation = liberaconfig::acquire && cfg.mode == CSPI_MODE_DD){
		  dodclient.Close();
		}
		if(cfg.operation = liberaconfig::acquire && cfg.mode == CSPI_MODE_SA){
	  sclient->Close();
		}
		cfg.operation = liberaconfig::unknown;

		break;
	case LIBERA_IOP_CMD_ACQUIRE:
		driver_mode = *(int*)data;
		LiberaSoftDBG<<"IOP Acquire driver mode:"<<driver_mode;
		if(driver_mode&LIBERA_IOP_MODE_TRIGGERED){
			cfg.mask|=cfg.want_trigger;
			LiberaSoftDBG<<"Enable Trigger";

		} else {
			cfg.mask&=~cfg.want_trigger;
			LiberaSoftDBG<<"Disable Trigger";
			myenv.trig_mode = 0;

		}
		if(driver_mode&LIBERA_IOP_MODE_DECIMATED){
			cfg.dd.decimation =1;
			LiberaSoftDBG<<"Enable Decimation";

		} else {
			cfg.dd.decimation =0;
			LiberaSoftDBG<<"Disable Decimation";

		}
		//const size_t modes[] = {CSPI_MODE_DD, CSPI_MODE_SA, CSPI_MODE_PM, CSPI_MODE_ADC, CSPI_MODE_AVERAGE};

		if(driver_mode&LIBERA_IOP_MODE_DD){
	  isig::AccessMode_e acc=isig::eModeDodNow;
	  LiberaSoftDBG<<"Acquire Data on Demand samples:"<<cfg.atom_count;
	  if(cfg.mask&cfg.want_trigger){
	  acc=isig::eModeDodOnEvent;
	}
	  auto ret = dodclient.Open(acc, cfg.atom_count,0);
			if (ret != isig::eSuccess) {
			  LiberaSoftERR<<"Cannot open DOD client";
			  return -1;
			}
			LiberaSoftDBG<<"Setting Gain";
			bool ok = gainNode.SetValue(false);
			if (ok==false) {
			  LiberaSoftERR<<"Cannot set gain to false";
			}
			LiberaSoftDBG<<"Open DOD client succeeded";
			cfg.mode =CSPI_MODE_DD;
			cfg.operation = liberaconfig::acquire;
			cfg.datasize=sizeof(CSPI_DD_ATOM);
			return 0;
		}
		if(driver_mode&LIBERA_IOP_MODE_SA){
			LiberaSoftDBG<<"Acquire Data on Streaming";

			LiberaSoftDBG<<"Setting Gain";
			bool ok = gainNode.SetValue(true);
			if (ok==false) {
			  LiberaSoftERR<<"Cannot set gain to true";
			}

			
			auto ret = sclient->Open();
			if (ret != isig::eSuccess) {
				LiberaSoftERR<<"Cannot open stream client";

				return -1;
			}
			cfg.datasize=sizeof(CSPI_SA_ATOM);
			cfg.mode =CSPI_MODE_SA;
			cfg.operation = liberaconfig::acquire;
			LiberaSoftDBG<<"Open stream client succeeded";
			return 0;
		}
		if(driver_mode&LIBERA_IOP_MODE_PM){
			cfg.mode =CSPI_MODE_PM;
			LiberaSoftDBG<<"Acquire Data Post Mortem";
			cfg.operation = liberaconfig::acquire;
			cfg.datasize=sizeof(CSPI_DD_ATOM);
		}
		if(driver_mode&LIBERA_IOP_MODE_ADC){
			cfg.mode =CSPI_MODE_ADC;
			LiberaSoftDBG<<"Acquire ADC Data";
			cfg.datasize=sizeof(CSPI_ADC_ATOM);
			cfg.operation = liberaconfig::acquire;

			if(driver_mode&LIBERA_IOP_MODE_CONTINUOUS){
				cfg.adc.mode|=liberaconfig::adc_specific::cw;
				cfg.mode = CSPI_MODE_ADC_CW;
				LiberaSoftDBG<<"Acquire ADC Data Continuous";
				cfg.operation = liberaconfig::acquire;
				cfg.datasize=sizeof(CSPI_ADC_CW_ATOM);

			} else {
				cfg.adc.mode&=~liberaconfig::adc_specific::cw;
			}
			if(driver_mode&LIBERA_IOP_MODE_SINGLEPASS){
				cfg.adc.mode|=liberaconfig::adc_specific::sp;
				cfg.mode = CSPI_MODE_ADC_SP;
				LiberaSoftDBG<<"Acquire ADC Data Single Pass";
				cfg.operation = liberaconfig::acquire;
				cfg.datasize=sizeof(CSPI_ADC_SP_ATOM);
			} else {
				cfg.adc.mode&=~liberaconfig::adc_specific::sp;
			}
		}
		if(operation&LIBERA_IOP_MODE_AVG){
			cfg.mode =CSPI_MODE_AVERAGE;
			cfg.atom_count = 1;
			LiberaSoftDBG<<"Acquire Average Data";
			cfg.operation = liberaconfig::acquire;
			cfg.datasize=sizeof(CSPI_AVERAGE_ATOM);

		}

		break;
	case LIBERA_IOP_CMD_SET_SAMPLES:
		cfg.atom_count = *(int *)data;
		LiberaSoftDBG<<"Setting Samples:"<<cfg.atom_count;

		break;
	case LIBERA_IOP_CMD_SET_OFFSET:
		cfg.dd.offset = *(int *)data;
		LiberaSoftDBG<<"Setting Offset:"<<cfg.dd.offset;

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
		LiberaSoftDBG<<"GET ENV";

		std::stringstream ss;
		myenv.health.fan[0]=(int)10;

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


		LiberaSoftDBG<<"connecting to HW..cfg:x"<<std::hex<<cfg.mask<<std::dec;

		/* raw_data = (char*)realloc(raw_data,cfg.atom_count*cfg.datasize);
        if(raw_data==NULL){
            cfg.operation = liberaconfig::unknown;
             LiberaSoftERR<<"Cannot allocate buffer of:"<<cfg.atom_count*cfg.datasize <<" bytes";
             return -100;
        }*/
		p.mode = cfg.mode;

		CSPI_BITMASK param_mask = CSPI_CON_MODE;
		if (event_mask) param_mask |= (CSPI_CON_HANDLER|CSPI_CON_EVENTMASK);
		LiberaSoftDBG<<"Setting connection parameters on:"<<con_handle<<" :"<<param_mask;


		LiberaSoftDBG<<"Connected with HW mode:"<<cfg.mode;

	}
	
	} catch(std::exception e){
	  LiberaSoftERR<<"Error performing operation:"<<e.what();
	  return -100;
	}
	return 0;
}

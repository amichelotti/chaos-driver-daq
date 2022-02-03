/*
 * LiberaEpicsDriver.cpp
 * @author Andrea Michelotti
Copyright Gen 18, 2022 

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

#include "LiberaEpicsDriver.h"
#include <driver/epics/driver/EpicsGenericDriver.h>

#define ILK_PARAMCOUNT 8
#include <chaos/cu_toolkit/driver_manager/driver/AbstractDriverPlugin.h>

#define LiberaSoftLAPP_ LAPP_ << "[LiberaEpicsDriver] "
#define LiberaSoftDBG LDBG_ << "[LiberaEpicsDriver " << __PRETTY_FUNCTION__ << " ]"
#define LiberaSoftERR LERR_ << "[LiberaEpicsDriver " << __PRETTY_FUNCTION__ << " ]"
#define MAX_RETRY 0

using namespace ::driver::daq::libera;

OPEN_CU_DRIVER_PLUGIN_CLASS_DEFINITION(LiberaEpicsDriver, 1.0.0, ::driver::daq::libera::LiberaEpicsDriver)
REGISTER_CU_DRIVER_PLUGIN_CLASS_INIT_ATTRIBUTE(::driver::daq::libera::LiberaEpicsDriver, http_address / dnsname: port)
CLOSE_CU_DRIVER_PLUGIN_CLASS_DEFINITION

static volatile size_t _event_id = 0;
static pthread_cond_t  eventc    = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t eventm    = PTHREAD_MUTEX_INITIALIZER;

OPEN_REGISTER_PLUGIN
REGISTER_PLUGIN(::driver::daq::libera::LiberaEpicsDriver)
CLOSE_REGISTER_PLUGIN

// GET_PLUGIN_CLASS_DEFINITION
// we need to define the driver with alias version and a class that implement it
// default constructor definition
LiberaEpicsDriver::LiberaEpicsDriver() {
  int rc;
  cfg.operation = liberaconfig::deinit;
  LiberaSoftDBG<<"Driver @"<<std::hex<<this;
  
}

// default descrutcor

LiberaEpicsDriver::~LiberaEpicsDriver() {
 // deinitIO();
}

void LiberaEpicsDriver::driverInit(const chaos::common::data::CDataWrapper &json) throw(chaos::CException){
  // add pvconfig
  std::vector<std::string> pvlist={"adc.ACQM","adc.ChannelA","adc.ChannelB","adc.ChannelC","adc.ChannelD","adc.PROC","adc.NGRP",\
  "ddc_synth.SCAN","ddc_synth.ACQM","ddc_synth.OFFS","ddc_synth.MT","ddc_synth.Va","ddc_synth.Vb","ddc_synth.Vc","ddc_synth.Vd",\
  "ddc_synth.Sum","ddc_synth.Q","ddc_synth.X","ddc_synth.Y","ddc_synth.PROC","ddc_synth.NGRP",\
  "sa.SCAN","sa.Va","sa.Vb","sa.Vc","sa.Vd","sa.Sum","sa.Q","sa.X","sa.Y","sa.LMT_l","sa.LMT_h"/*,\
  "pm.ddc_synth.SCAN","pm.ddc_synth.ACQM","pm.ddc_synth.Va","pm.ddc_synth.Vb","pm.ddc_synth.Vc","pm.ddc_synth.Vd","pm.ddc_synth.Sum","pm.ddc_synth.Q","pm.ddc_synth.X","pm.ddc_synth.Y","pm.ddc_synth.PROC","pm.ddc_synth.NGRP"*/};
  chaos::common::data::CDWUniquePtr newconf=json.clone();
  ::driver::epics::common::EpicsGenericDriver::addPVListConfig(*(newconf.get()),pvlist);
  LiberaEpicsBase::driverInit(*newconf.get());

}

void LiberaEpicsDriver::driverInit(const char *initParameter) throw(chaos::CException) {

  if (initParameter != NULL) {
    chaos::common::data::CDataWrapper cw;
    cw.setSerializedJsonData(initParameter);
    driverInit(cw);

  }
  throw chaos::CException(-1,"invalid configuration",__PRETTY_FUNCTION__);

}
int LiberaEpicsDriver::read(void *buffer, int addr, int bcount) {
  int rc;
  // Allways seek(), not just the first time.
  if(addr==CHANNEL_SA){
      libera_sa_t *tt = (libera_sa_t *)buffer;

      if((rc=devicedriver->waitChange("sa.Va"))!=0){
        return rc;
      }

      devicedriver->read("sa.Va",tt->Va );
      devicedriver->read("sa.Vb",tt->Vb );
      devicedriver->read("sa.Vc",tt->Vc );
      devicedriver->read("sa.Vd",tt->Vd );
      devicedriver->read("sa.Sum",tt->Sum );
      devicedriver->read("sa.Q",tt->Q );
      devicedriver->read("sa.X",tt->X );
      devicedriver->read("sa.Y",tt->Y );
      devicedriver->read("sa.LMT_l",tt->reserved[0] );
      devicedriver->read("sa.LMT_h",tt->reserved[1] );
      uint64_t mt = (tt->reserved[0])|(((uint64_t)tt->reserved[1])<<32);


      LiberaSoftDBG <<mt <<"- SA VA:" << tt->Va << " VB:" << tt->Vb << " VC:" << tt->Vc << " VD:" << tt->Vd;
      return 1;
  } else if (addr == CHANNEL_DD){

      if (!(cfg.mask & liberaconfig::want_trigger)) {
        WRITEPV("ddc_synth.PROC",0);
    }
      if((rc=devicedriver->waitChange("ddc_synth.Va"))!=0){
          return rc;
        }
     // int32_t va[count],vb[count],vc[count],vd[count],sum[count],q[count],x[count],y[count];
        libera_data_handle_t *dd = (libera_data_handle_t *)buffer;

        READPVARRAY("ddc_synth.Va",dd->Va,dd->samples );
        READPVARRAY("ddc_synth.Vb",dd->Vb,dd->samples );
        READPVARRAY("ddc_synth.Vc",dd->Vc,dd->samples);
        READPVARRAY("ddc_synth.Vd",dd->Vd,dd->samples );
        READPVARRAY("ddc_synth.Sum",dd->Sum,dd->samples );
        if(dd->Q){
          READPVARRAY("ddc_synth.Q",dd->Q,dd->samples);
        }
        if(dd->X){
          READPVARRAY("ddc_synth.X",dd->X,dd->samples);
        }
        if(dd->Y){
          READPVARRAY("ddc_synth.Y",dd->Y,dd->samples);
        }
        *dd->ts=0;
        devicedriver->read("ddc_synth.MT",*(int32_t*)dd->ts);
        LiberaSoftDBG <<*dd->ts<< " DD[" << 0 << "] VA:" << dd->Va[0] << " VB:" << dd->Vb[0]  << " VC:" << dd->Vc[0]  << " VD:" << dd->Vd[0] <<" Sum:"<<dd->Sum[0];
        //<<" Q:"<<dd-dd->Q[0]<<" X:"<<dd->X[0]<<" Y:"<<dd->Y[0];

      return dd->samples;
    }

  

  return 0;
}

int LiberaEpicsDriver::write(void *buffer, int addr, int bcount) {
  // TODO: implement the method
  return 0;
}
// assign MT and ST from a string formatted as [MT]:[YYYYMMDDhhmm.ss]

// trigger_time,WaveGen:init_params

int LiberaEpicsDriver::initIO(void *buffer, int sizeb) {
  LiberaSoftLAPP_<<"initIO";
  return 0;
}

int LiberaEpicsDriver::deinitIO() {
    LiberaSoftLAPP_<<"deinitIO";

  if (cfg.operation == liberaconfig::deinit) {
    LiberaSoftERR << "Already de-initializad";
    return 0;
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
int LiberaEpicsDriver::iop(int operation, void *data, int sizeb) {
  int                       rc;
  CSPI_ENVPARAMS            ep;
  std::string               trigger_mode ;
  int                       scan_mode ;
#define SET_ENV(cpimask, param)                                                                                        \
  if (cmd_env->selector & CSPI_ENV_##cpimask) {                                                                        \
    env.param = cmd_env->value;                                                                                        \
    LiberaSoftDBG << "IO SET ENV \"" << #cpimask << "\" bitmask:" << cmd_env->selector << " value=" << cmd_env->value; \
  }

  switch (operation) {
    case LIBERA_IOP_CMD_GET_TS:
      CSPI_TIMESTAMP ts;
      struct timeval tts;
      gettimeofday(&tts, NULL);
      ts.st.tv_sec  = tts.tv_sec;
      ts.st.tv_nsec = tts.tv_usec * 1000;
      memcpy(data, &ts, std::min((uint32_t)sizeb, (uint32_t)sizeof(CSPI_TIMESTAMP)));
      return 0;

      break;
    case LIBERA_IOP_CMD_STOP:
      LiberaSoftDBG << "IOP STOP" << driver_mode;

      cfg.operation = liberaconfig::unknown;

      break;
    case LIBERA_IOP_CMD_ACQUIRE:
      driver_mode = *(int *)data;
      trigger_mode = ((driver_mode & LIBERA_IOP_MODE_TRIGGERED) ? "Event" : "Now");
      scan_mode    = ((driver_mode & LIBERA_IOP_MODE_TRIGGERED) ? 2 : 0);
      LiberaSoftDBG << "IOP Acquire driver mode:" << driver_mode;
      if (driver_mode & LIBERA_IOP_MODE_TRIGGERED) {
        cfg.mask |= cfg.want_trigger;
        LiberaSoftDBG << "Enable Trigger";

      } else {
        cfg.mask &= ~cfg.want_trigger;
        LiberaSoftDBG << "Disable Trigger";
        myenv.trig_mode = 0;
      }
      if (driver_mode & LIBERA_IOP_MODE_DECIMATED) {
        cfg.dd.decimation = 1;
        LiberaSoftDBG << "Enable Decimation";

      } else {
        cfg.dd.decimation = 0;
        LiberaSoftDBG << "Disable Decimation";
      }
      // const size_t modes[] = {CSPI_MODE_DD, CSPI_MODE_SA, CSPI_MODE_PM, CSPI_MODE_ADC, CSPI_MODE_AVERAGE};

      if (driver_mode & LIBERA_IOP_MODE_DD) {
        cfg.mode = CSPI_MODE_DD;
        LiberaSoftDBG << "Acquire Data on Demand";
        if ((driver_mode & LIBERA_IOP_MODE_TRIGGERED)==0) {
          WRITEPV("ddc_synth.PROC",0);
        }
        WRITEPV("ddc_synth.NGRP", (int32_t)cfg.atom_count);
        WRITEPV("ddc_synth.SCAN", scan_mode);
        WRITEPV("ddc_synth.ACQM", trigger_mode);
        
        cfg.operation = liberaconfig::acquire;
        cfg.datasize  = sizeof(CSPI_DD_ATOM);
      }
      if (driver_mode & LIBERA_IOP_MODE_SA) {
        cfg.mode = CSPI_MODE_SA;
        LiberaSoftDBG << "Acquire Data on Streaming";
        WRITEPV("ssa.SCAN", 2);

        cfg.operation = liberaconfig::acquire;
        cfg.datasize  = sizeof(CSPI_SA_ATOM);
      }
      if (driver_mode & LIBERA_IOP_MODE_PM) {
        cfg.mode = CSPI_MODE_PM;
        LiberaSoftDBG << "Acquire Data Post Mortem";
        WRITEPV("pm.ddc_synth.SCAN", scan_mode);
        WRITEPV("pm.ddc_synth.ACQM", trigger_mode);

        cfg.operation = liberaconfig::acquire;
        cfg.datasize  = sizeof(CSPI_DD_ATOM);
      }
      if (driver_mode & LIBERA_IOP_MODE_ADC) {
        cfg.mode = CSPI_MODE_ADC;
        LiberaSoftDBG << "Acquire ADC Data";
        cfg.datasize  = sizeof(CSPI_ADC_ATOM);
        cfg.operation = liberaconfig::acquire;
        WRITEPV("adc.SCAN", scan_mode);
        WRITEPV("adc.ACQM", trigger_mode);

        if (driver_mode & LIBERA_IOP_MODE_CONTINUOUS) {
          cfg.adc.mode |= liberaconfig::adc_specific::cw;
          cfg.mode = CSPI_MODE_ADC_CW;
          LiberaSoftDBG << "Acquire ADC Data Continuous";
          cfg.operation = liberaconfig::acquire;
          cfg.datasize  = sizeof(CSPI_ADC_CW_ATOM);

        } else {
          cfg.adc.mode &= ~liberaconfig::adc_specific::cw;
        }
        if (driver_mode & LIBERA_IOP_MODE_SINGLEPASS) {
          cfg.adc.mode |= liberaconfig::adc_specific::sp;
          cfg.mode = CSPI_MODE_ADC_SP;
          LiberaSoftDBG << "Acquire ADC Data Single Pass";
          cfg.operation = liberaconfig::acquire;
          cfg.datasize  = sizeof(CSPI_ADC_SP_ATOM);
        } else {
          cfg.adc.mode &= ~liberaconfig::adc_specific::sp;
        }
      }
      if (operation & LIBERA_IOP_MODE_AVG) {
        cfg.mode       = CSPI_MODE_AVERAGE;
        cfg.atom_count = 1;
        LiberaSoftDBG << "Acquire Average Data";
        cfg.operation = liberaconfig::acquire;
        cfg.datasize  = sizeof(CSPI_AVERAGE_ATOM);
      }

      break;
    case LIBERA_IOP_CMD_SET_SAMPLES:
      cfg.atom_count = *(int *)data;
      LiberaSoftDBG << "Setting Samples:" << cfg.atom_count;
      switch (cfg.mode) {
        case CSPI_MODE_DD:
        //  WRITEPV("ddc_synth.NGRP", cfg.atom_count);
          break;
        case CSPI_MODE_PM:
       //   WRITEPV("pm.ddc_synth.NGRP", cfg.atom_count);
          break;
      }
      break;
    case LIBERA_IOP_CMD_SET_OFFSET:
      cfg.dd.offset = *(int *)data;
      LiberaSoftDBG << "Setting Offset:" << cfg.dd.offset;
      switch (cfg.mode) {
        case CSPI_MODE_DD:
          WRITEPV("ddc_synth.OFFS", (int32_t)cfg.dd.offset);
          break;
        case CSPI_MODE_PM:
          WRITEPV("pm.ddc_synth.OFFS", (int32_t)cfg.dd.offset);
          break;
      }
      break;
    case LIBERA_IOP_CMD_SETENV: {
      cfg.operation           = liberaconfig::setenv;
      libera_env_t   *cmd_env = (libera_env_t *)data;
      CSPI_ENVPARAMS &env     = myenv;

      SET_ENV(KX, Kx);
      SET_ENV(KY, Ky);
      SET_ENV(XOFFSET, Xoffset);
      SET_ENV(YOFFSET, Yoffset);
      SET_ENV(QOFFSET, Qoffset);
      SET_ENV(SWITCH, switches);
      SET_ENV(GAIN, gain);
      SET_ENV(AGC, agc);
      SET_ENV(DSC, dsc);
      SET_ENV(ILK, ilk.mode);

      SET_ENV(ILKSTATUS, ilk_status);
      SET_ENV(PMOFFSET, PMoffset);

      SET_ENV(PMDEC, PMdec);

      SET_ENV(TRIGDELAY, trig_delay);

      SET_ENV(EXTSWITCH, external_switching);
      SET_ENV(SWDELAY, switching_delay);
      SET_ENV(TRIGMODE, trig_mode);
      SET_ENV(DDC_MAFLENGTH, ddc_maflength);
      SET_ENV(DDC_MAFDELAY, ddc_mafdelay);

      SET_ENV(NOTCH1, notch1[0]);
      SET_ENV(NOTCH2, notch2[0]);
      SET_ENV(POLYPHASE_FIR, polyphase_fir[0]);

      SET_ENV(MTVCXOFFS, mtvcxoffs);
      SET_ENV(MTNCOSHFT, mtncoshft);
      SET_ENV(MTPHSOFFS, mtphsoffs);
      SET_ENV(MTUNLCKTR, mtunlcktr);
      SET_ENV(MTSYNCIN, mtsyncin);
      SET_ENV(STUNLCKTR, stunlcktr);

      SET_ENV(PM, pm.mode);
      SET_ENV(SR, sr.enable);
      SET_ENV(SP, sp.threshold);

    }

    break;
    case LIBERA_IOP_CMD_GETENV: {
      char        *pdata = (char *)data;
      CSPI_BITMASK mask  = ~(0LL);
      cfg.operation      = liberaconfig::listenv;

      std::stringstream ss;
      // myenv.health.fan[0]=(int)wave->generate();

      // ss<<myenv;
      LiberaSoftDBG<<"GET ENV size:"<<std::min((uint32_t)sizeb,(uint32_t)ss.str().size())<<" val:"<<ss.str();

      strncpy(pdata, ss.str().c_str(), std::min((uint32_t)sizeb, (uint32_t)ss.str().size()));
      break;
    }
    case LIBERA_IOP_CMD_SETTIME: {
      return 0;
    }
  }

  if (cfg.operation == liberaconfig::acquire) {
    CSPI_BITMASK event_mask = 0;
    if (cfg.mask & liberaconfig::want_trigger) {
      event_mask |= CSPI_EVENT_TRIGGET;
    }

    LiberaSoftDBG << "connecting to HW..cfg:x" << std::hex << cfg.mask << std::dec;

    /* raw_data = (char*)realloc(raw_data,cfg.atom_count*cfg.datasize);
     if(raw_data==NULL){
         cfg.operation = liberaconfig::unknown;
          LiberaSoftERR<<"Cannot allocate buffer of:"<<cfg.atom_count*cfg.datasize <<" bytes";
          return -100;
     }*/
    p.mode = cfg.mode;

    CSPI_BITMASK param_mask = CSPI_CON_MODE;
    if (event_mask) param_mask |= (CSPI_CON_HANDLER | CSPI_CON_EVENTMASK);
    LiberaSoftDBG << "Setting connection parameters on:" << con_handle << " :" << param_mask;

    LiberaSoftDBG << "Connected with HW mode:" << cfg.mode;
  }

  return 0;
}

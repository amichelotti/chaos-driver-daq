/*
 * LiberaBrillianceSPEpicsDriver.cpp
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

#include "LiberaBrillianceSPEpicsDriver.h"
#include <driver/epics/driver/EpicsGenericDriver.h>

#define ILK_PARAMCOUNT 8
#include <chaos/cu_toolkit/driver_manager/driver/AbstractDriverPlugin.h>

#define LiberaSoftLAPP_ LAPP_ << "[LiberaBrillianceSPEpicsDriver] "
#define LiberaSoftDBG LDBG_ << "[LiberaBrillianceSPEpicsDriver " << __PRETTY_FUNCTION__ << " ]"
#define LiberaSoftERR LERR_ << "[LiberaBrillianceSPEpicsDriver " << __PRETTY_FUNCTION__ << " ]"
#define MAX_RETRY 0

using namespace ::driver::daq::libera;

OPEN_CU_DRIVER_PLUGIN_CLASS_DEFINITION(LiberaBrillianceSPEpicsDriver, 1.0.0, ::driver::daq::libera::LiberaBrillianceSPEpicsDriver)
REGISTER_CU_DRIVER_PLUGIN_CLASS_INIT_ATTRIBUTE(::driver::daq::libera::LiberaBrillianceSPEpicsDriver, http_address / dnsname
                                               : port)
CLOSE_CU_DRIVER_PLUGIN_CLASS_DEFINITION

static volatile size_t _event_id = 0;
static pthread_cond_t  eventc    = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t eventm    = PTHREAD_MUTEX_INITIALIZER;

OPEN_REGISTER_PLUGIN
REGISTER_PLUGIN(::driver::daq::libera::LiberaBrillianceSPEpicsDriver)
CLOSE_REGISTER_PLUGIN

// GET_PLUGIN_CLASS_DEFINITION
// we need to define the driver with alias version and a class that implement it
// default constructor definition
LiberaBrillianceSPEpicsDriver::LiberaBrillianceSPEpicsDriver() {
  int rc;
  cfg.operation = liberaconfig::deinit;
  maxSamples  = 1024;

  LiberaSoftDBG << "Driver @" << std::hex << this;
  /*
      if((rc=initIO(0,0))!=0){
          throw chaos::CException(rc,"Initializing","LiberaBrillianceSPEpicsDriver::LiberaBrillianceSPEpicsDriver");
      }
   */
}

// default descrutcor

LiberaBrillianceSPEpicsDriver::~LiberaBrillianceSPEpicsDriver() {
  // deinitIO();
}

void LiberaBrillianceSPEpicsDriver::driverInit(const chaos::common::data::CDataWrapper &json) throw(chaos::CException) {
  // add pvconfig
  if (json.hasKey("maxSamples")) {
    maxSamples = json.getInt32Value("maxSamples");
    
  }
  std::vector<std::string> pvlist =
      {"ADC:ADC_MONITOR", "ADC:ADC_A_MONITOR", "ADC:ADC_B_MONITOR", "ADC:ADC_C_MONITOR", "ADC:ADC_D_MONITOR", "ADC:ADC_FINISHED_MONITOR", "ADC:ADC_IGNORE_TRIG_SP", "ADC:ADC_ON_NEXT_TRIG_CMD", "SA:SA_A_MONITOR", "SA:SA_B_MONITOR", "SA:SA_C_MONITOR", "SA:SA_D_MONITOR", "SA:SA_X_MONITOR", "SA:SA_Y_MONITOR", "SA:SA_Q_MONITOR", "SA:SA_SUM_MONITOR", "SA:SA_FINISHED_MONITOR"};
  chaos::common::data::CDWUniquePtr newconf = json.clone();
  ::driver::epics::common::EpicsGenericDriver::addPVListConfig(*(newconf.get()), pvlist);
  std::map<std::string, std::string> pvprprop = {
    //{"ENV:ENV_SWITCHES_MONITOR", "ENV_SWITCHES_MONITOR"},
                                                 {"ENV:ENV_GAIN_MONITOR", "ENV_GAIN_MONITOR"},
                                                 {"ENV:ENV_GAIN_SP", "Gain"},
                                                 {"ENV:ENV_AGC_MONITOR", "ENV_AGC_MONITOR"},
                                                 {"ENV:ENV_AGC_SP", "ENV_AGC_SP"},
                                             //    {"ENV:ENV_DSC_MONITOR", "ENV_DSC_MONITOR"},
                                             //    {"ENV:ENV_TEMP_MONITOR", "ENV_TEMP_MONITOR"},
                                                 {"ENV:ENV_TRIGDELAY_MONITOR", "ENV_TRIGDELAY_MONITOR"},
                                                 {"ENV:ENV_TRIGDELAY_SP", "ENV_TRIGDELAY_SP"},
                                             //    {"ENV:ENV_RESET_COUNTER_MONITOR", "ENV_RESET_COUNTER_MONITOR"},
                                             //    {"ENV:ENV_RESET_COUNTER_SP", "ENV_RESET_COUNTER_SP"},
                                                 {"ENV:ENV_ERROR_MONITOR", "ENV_ERROR_MONITOR"},
                                                 {"ENV:ENV_SAFPGA_ERR_MONITOR", "ENV_SAFPGA_ERR_MONITOR"},
                                                 {"ENV:ENV_SADRV_ERR_MONITOR", "ENV_SADRV_ERR_MONITOR"},
                                                 {"ENV:ENV_TEMP_REMOTE_MONITOR", "ENV_TEMP_REMOTE_MONITOR"},
                                                 {"ENV:ENV_TEMP_ANALOG_MONITOR", "ENV_TEMP_ANALOG_MONITOR"},
                                                 {"ENV:ENV_TEMP_DIGITAL_MONITOR", "ENV_TEMP_DIGITAL_MONITOR"},
                                                 {"ENV:ENV_TEMP_MAX_MONITOR", "ENV_TEMP_MAX_MONITOR"},
                                                 {"ENV:ENV_TEMP_MIN_MONITOR", "ENV_TEMP_MIN_MONITOR"},
                                                 {"ENV:ENV_FRONT_VENT_ACT_MONITOR", "ENV_FRONT_VENT_ACT_MONITOR"},
                                                 {"ENV:ENV_BACK_VENT_ACT_MONITOR", "ENV_BACK_VENT_ACT_MONITOR"},
                                                 {"ENV:ENV_ILK_STATUS_MONITOR", "ENV_ILK_STATUS_MONITOR"},
                                                 {"ENV:ENV_ILK_STATUS_SP", "ENV_ILK_STATUS_SP"}

  };

  ::driver::epics::common::EpicsGenericDriver::addPVListConfig(*(newconf.get()), pvprprop);

  LiberaEpicsBase::driverInit(*newconf.get());
}

void LiberaBrillianceSPEpicsDriver::driverInit(const char *initParameter) throw(chaos::CException) {
  if (initParameter != NULL) {
    chaos::common::data::CDataWrapper cw;
    cw.setSerializedJsonData(initParameter);
    driverInit(cw);
  }
  throw chaos::CException(-1, "invalid configuration", __PRETTY_FUNCTION__);
}

int LiberaBrillianceSPEpicsDriver::read(void *buffer, int addr, int bcount) {
  int rc;
  // Allways seek(), not just the first time.
  if (addr == CHANNEL_SA) {
    libera_sa_t *tt = (libera_sa_t *)buffer;

    int ret = devicedriver->waitChange("SA:SA_A_MONITOR");
    if (ret != 0) {
      return ret;
    }
    READPV("SA:SA_A_MONITOR", tt->Va);

    READPV("SA:SA_B_MONITOR", tt->Vb);
    READPV("SA:SA_C_MONITOR", tt->Vc);
    READPV("SA:SA_D_MONITOR", tt->Vd);
    READPV("SA:SA_SUM_MONITOR", tt->Sum);
    READPV("SA:SA_Q_MONITOR", tt->Q);
    READPV("SA:SA_X_MONITOR", tt->X);
    READPV("SA:SA_Y_MONITOR", tt->Y);
    READPV("SA:SA_COUNTER_MONITOR", tt->counter[0]);
    tt->counter[1]=0;
    tt->Sum/=4;
    LiberaSoftDBG << tt->counter[0] << "- SA VA:" << tt->Va << " VB:" << tt->Vb << " VC:" << tt->Vc << " VD:" << tt->Vd;
    return 1;
  } else if (addr == CHANNEL_DD || (addr==CHANNEL_ADC)) {
    if(cfg.mask & liberaconfig::want_trigger){
     devicedriver->write("ADC:ADC_ON_NEXT_TRIG_CMD",1);
    }

    if ((rc = devicedriver->waitChange("ADC:ADC_FINISHED_MONITOR")) != 0) {
      
      return rc;
    }

    // int32_t va[count],vb[count],vc[count],vd[count],sum[count],q[count],x[count],y[count];
    libera_data_handle_t *dd          = (libera_data_handle_t *)buffer;
    uint32_t              realsamples = std::min((uint32_t)dd->samples, maxSamples);

    READPVARRAY("ADC:ADC_A_MONITOR", dd->Va, realsamples);
    READPVARRAY("ADC:ADC_B_MONITOR", dd->Vb, realsamples);
    READPVARRAY("ADC:ADC_C_MONITOR", dd->Vc, realsamples);
    READPVARRAY("ADC:ADC_D_MONITOR", dd->Vd, realsamples);
   // READPVARRAY("ADC:ADC_SUM_MONITOR", dd->Sum, realsamples);
    for(int cnt=0;cnt<realsamples;cnt++){
      dd->Sum[cnt]=(dd->Va[cnt]+dd->Vb[cnt]+dd->Vc[cnt]+dd->Vd[cnt])/4;
    }
    int count;
    READPV("ADC:ADC_FINISHED_MONITOR", count);

    *dd->ts =count;
    // devicedriver->waitChange(ADC_port+":ADC_FINISHED_MONITOR");

    LiberaSoftDBG << *dd->ts << " ADC[" << 0 << "] VA[0]:" << dd->Va[0] << "VA[" << realsamples - 1 << "]:" << dd->Va[realsamples - 1] << "  VB:" << dd->Vb[0] << " VC:" << dd->Vc[0] << " VD:" << dd->Vd[0] << " Sum:" << dd->Sum[0];
    //<<" Q:"<<dd-dd->Q[0]<<" X:"<<dd->X[0]<<" Y:"<<dd->Y[0];

    if(cfg.mask & liberaconfig::want_trigger){
     devicedriver->write("ADC:ADC_ON_NEXT_TRIG_CMD",1);
    }

    
    return realsamples;
  }

  return 0;
}

int LiberaBrillianceSPEpicsDriver::write(void *buffer, int addr, int bcount) {
  // TODO: implement the method
  return 0;
}
// assign MT and ST from a string formatted as [MT]:[YYYYMMDDhhmm.ss]

int LiberaBrillianceSPEpicsDriver::initIO(void *buffer, int sizeb) {
  LiberaSoftLAPP_ << "initIO";
  return 0;
}

int LiberaBrillianceSPEpicsDriver::deinitIO() {
  LiberaSoftLAPP_ << "deinitIO";

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
int LiberaBrillianceSPEpicsDriver::iop(int operation, void *data, int sizeb) {
  int            rc;
  CSPI_ENVPARAMS ep;
  bool           trigger_mode;
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
      devicedriver->write("ADC:ADC_IGNORE_TRIG_SP", 1);
      //devicedriver->write("ADC:ADC_IGNORE_TRIG_SP",1);

      cfg.operation = liberaconfig::unknown;

      break;
    case LIBERA_IOP_CMD_ACQUIRE:
      driver_mode  = *(int *)data;
      trigger_mode = ((driver_mode & LIBERA_IOP_MODE_TRIGGERED) ? true : false);

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

        devicedriver->write("ADC:ADC_IGNORE_TRIG_SP",((trigger_mode==true)?1:0));

        cfg.operation = liberaconfig::acquire;
        cfg.datasize  = sizeof(CSPI_DD_ATOM);
      }
      if (driver_mode & LIBERA_IOP_MODE_SA) {
        cfg.mode = CSPI_MODE_SA;
        //devicedriver->write("ADC:ADC_IGNORE_TRIG_SP", 1);
        //devicedriver->write("ADC:ADC_IGNORE_TRIG_SP",1);
        LiberaSoftDBG << "Acquire Data on Streaming";
        // devicedriver->write("ssa.SCAN", 2);

        cfg.operation = liberaconfig::acquire;
        cfg.datasize  = sizeof(CSPI_SA_ATOM);
      }
      if (driver_mode & LIBERA_IOP_MODE_PM) {
        cfg.mode = CSPI_MODE_PM;
        LiberaSoftDBG << "Acquire Data Post Mortem";

        cfg.operation = liberaconfig::acquire;
        cfg.datasize  = sizeof(CSPI_ADC_ATOM);
      }
      if (driver_mode & LIBERA_IOP_MODE_ADC) {
        cfg.mode = CSPI_MODE_ADC;
        LiberaSoftDBG << "Acquire ADC Data";
        devicedriver->write("ADC:ADC_IGNORE_TRIG_SP",((trigger_mode==true)?1:0));
        cfg.datasize  = sizeof(CSPI_ADC_ATOM);
        cfg.operation = liberaconfig::acquire;
        // devicedriver->write("adc.SCAN", scan_mode);
        // devicedriver->write("adc.ACQM", trigger_mode);

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
                  devicedriver->write("ADC:ADC_IGNORE_TRIG_SP",((trigger_mode==true)?1:0));

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

          break;
        case CSPI_MODE_PM:
          //  devicedriver->write("pm.ddc_synth.NGRP", cfg.atom_count);
          break;
      }
      break;
    case LIBERA_IOP_CMD_SET_OFFSET:
      cfg.dd.offset = *(int *)data;
      LiberaSoftDBG << "Setting Offset:" << cfg.dd.offset;
      switch (cfg.mode) {
        case CSPI_MODE_DD:
          //  devicedriver->write("ddc_synth.OFFS", cfg.dd.offset);
          break;
        case CSPI_MODE_PM:
          //   devicedriver->write("pm.ddc_synth.OFFS", cfg.dd.offset);
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
      LiberaSoftDBG << "GET ENV size:" << std::min((uint32_t)sizeb, (uint32_t)ss.str().size()) << " val:" << ss.str();

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

    p.mode = cfg.mode;

    CSPI_BITMASK param_mask = CSPI_CON_MODE;
    if (event_mask) param_mask |= (CSPI_CON_HANDLER | CSPI_CON_EVENTMASK);
    LiberaSoftDBG << "Setting connection parameters on:" << con_handle << " :" << param_mask;

    LiberaSoftDBG << "Connected with HW mode:" << cfg.mode;
  }

  return 0;
}
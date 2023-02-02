/*
 *	RTBTFdaqSimCU
 *	!CHAOS
 *	Created by Andrea Michelotti 8/1/2016
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
#include <common/debug/core/debug.h>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
using namespace chaos;

using namespace chaos::common::data;
using namespace chaos::common::data::cache;

using namespace chaos::common::batch_command;
using namespace chaos::cu::control_manager;

using namespace chaos::cu::control_manager::slow_command;
using namespace chaos::cu::driver_manager::driver;
using namespace ::driver::daq::btf;
namespace chaos_batch = chaos::common::batch_command;

#define ENABLE_VETO 0x3  // first two
#define DISABLE_VETO 0x0
#define COUNTER_ALL_TRIGGER 30
#define COUNTER_VALID_TRIGGER 31
#define CLOSEDEV(_x) _x##_close(_x##_handle);

#define OPENDEV(_x)                                                             \
  DPRINT("opening %s at address 0x%x", #_x, (uint32_t)*_x##_addr);              \
  if (_x##_addr &&                                                              \
      (_x##_handle = _x##_open(VME_UNIVERSE2_DRIVER, (uint32_t)*_x##_addr))) {  \
    DPRINT("* " #_x " successfully mapped\n");                                  \
  } else {                                                                      \
    throw chaos::CException(-4, "## cannot map " #_x " ", __PRETTY_FUNCTION__); \
  }

#define SCCUAPP \
  LAPP_ << "[RTBTFdaqSimCU - " << getCUID() << "] - " << __PRETTY_FUNCTION__ << ":"
#define SCCULDBG \
  LDBG_ << "[RTBTFdaqSimCU - " << getCUID() << "] - " << __PRETTY_FUNCTION__ << ":"

PUBLISHABLE_CONTROL_UNIT_IMPLEMENTATION(::driver::daq::btf::RTBTFdaqSimCU)
inline static double beamFunc(double *x, double *p) {
  double amplitude = p[0];
  double mean_x = p[1];
  double sigma_x = p[2];
  double mean_y = p[3];
  double sigma_y = p[4];
  double rho = p[5];
  double u = (x[0] - mean_x) / sigma_x;
  double v = (x[1] - mean_y) / sigma_y;
  double c = 1 - rho * rho;
  double result = amplitude * exp(-(u * u - 2 * rho * u * v + v * v) / (2 * c));

  //(1 / (2 * TMath::Pi() * sigma_x * sigma_y * sqrt(c)))

  return result;
}
/*
 Construct a new CU with an identifier
 */
RTBTFdaqSimCU::RTBTFdaqSimCU(const string&                _control_unit_id,
                       const string&                _control_unit_param,
                       const ControlUnitDriverList& _control_unit_drivers)
    :  // call base constructor
    chaos::cu::control_manager::RTAbstractControlUnit(
        _control_unit_id,
        _control_unit_param,
        _control_unit_drivers) {
  // vme:{"driver":"stringa","params":"stringa"}
  // qdc:{"channels":16,"address":"0xaaaa0000"}
  // tdc:{"channels":16,"address":"0xaaaa0000"}
  // pio:{"channels":16,"address":"0xaaaa0000"}
  // sis:{"channels":16,"address":"0xaaaa0000"}
  
  timeout_ms       = 0;
  caen792_chans    = 16;
  caen965_chans    = 16;
  last_eval = last_eval_trigger = 0;
  veto_enable                   = true;
  pio_latch                     = true;
  counter_trigger = counter_etrigger = 0;
  CDataWrapper params;
  std::string  vme_param, vme_driver;
  params.setSerializedJsonData(_control_unit_param.c_str());
  
  if (params.hasKey("timeout")) {
    timeout_ms = params.getInt32Value("timeout");
  }
  if (params.hasKey("veto")) {
    veto_enable = params.getBoolValue("veto");
  }

  if (params.hasKey("pio_latch")) {
    pio_latch = params.getBoolValue("pio_latch");
  }
  
  if (params.hasKey("pio")) {
    chaos::common::data::CDWUniquePtr p = params.getCSDataValue("pio");
    if (!p->hasKey("address")) {
      throw chaos::CException(
          -1, "missing 'address' key :" + p->getJSONString(), _control_unit_id);
    }
  }
  if (params.hasKey("caen965")) {
    chaos::common::data::CDWUniquePtr p = params.getCSDataValue("caen965");
    if (!p->hasKey("address")) {
      throw chaos::CException(
          -1, "missing 'address' key :" + p->getJSONString(), _control_unit_id);
    }
    if (p->hasKey("channels")) {
      caen965_chans = p->getInt32Value("channels");
    }
    if (p->hasKey("outchannels") && p->isVector("outchannels")) {
      CMultiTypeDataArrayWrapperSPtr arr = p->getVectorValue("outchannels");
      for (int cnt = 0; cnt < arr->size(); cnt++) {
        output_channels["caen965"].push_back(arr->getInt32ElementAtIndex(cnt));
        out_channels_965 = true;
      }
    }    
  }
  if (params.hasKey("caen792")) {
    chaos::common::data::CDWUniquePtr p = params.getCSDataValue("caen792");
    if (!p->hasKey("address")) {
      throw chaos::CException(
          -1, "missing 'address' key :" + p->getJSONString(), _control_unit_id);
    }
    if (p->hasKey("channels")) {
      caen792_chans = p->getInt32Value("channels");
    }
    if (p->hasKey("outchannels") && p->isVector("outchannels")) {
      CMultiTypeDataArrayWrapperSPtr arr = p->getVectorValue("outchannels");
      for (int cnt = 0; cnt < arr->size(); cnt++) {
        output_channels["caen792"].push_back(arr->getInt32ElementAtIndex(cnt));
        out_channels_792 = true;
      }
    }
  }

  if (params.hasKey("sis")) {
    chaos::common::data::CDWUniquePtr p = params.getCSDataValue("sis");
    if (!p->hasKey("address")) {
      throw chaos::CException(
          -1, "missing 'address' key :" + p->getJSONString(), _control_unit_id);
    }
    unsigned add   = strtoul(p->getStringValue("address").c_str(), 0, 0);
   
  }
}

/*
 Base destructor
 */
RTBTFdaqSimCU::~RTBTFdaqSimCU() {}

/*
 Return the default configuration
 */
void RTBTFdaqSimCU::unitDefineActionAndDataset()  {
  SCCULDBG << "defining dataset timeout ms:" << timeout_ms << " latch:" << pio_latch << " veto:" << veto_enable;

  addAttributeToDataSet("ACQUISITION", "Acquisition number", DataType::TYPE_INT64, DataType::Output);
  addAttributeToDataSet("TRIGGERS", "Triggers", DataType::TYPE_INT32, DataType::Output);
  addAttributeToDataSet("TRIGGERS_VALID", "Triggers after veto", DataType::TYPE_INT32, DataType::Output);

  addAttributeToDataSet("TRIGGER_LOST", "Number of lost trigger", DataType::TYPE_INT64, DataType::Output);
  addAttributeToDataSet("TRIGGER_FREQ", "Evaluated Trigger Freq", DataType::TYPE_DOUBLE, DataType::Output);
  addAttributeToDataSet("TRIGGER_EFREQ", "Evaluated effective trigger", DataType::TYPE_DOUBLE, DataType::Output);

    if (out_channels_965) {
      std::vector<int> arr = output_channels["caen965"];
      for (int cnt = 0; cnt < arr.size(); cnt++) {
        std::stringstream ss;
        ss << "QDC" << arr[cnt];
        addAttributeToDataSet(ss.str(), "Channel " + ss.str(), DataType::TYPE_INT32, DataType::Output);
        SCCULDBG<<"CHANNEL "<<ss.str();
        }
    } else {
      if (caen965_chans == 16) {
        addBinaryAttributeAsSubtypeToDataSet(
            "QDC965HI", "Vector of Channels High Resolution", chaos::DataType::SUB_TYPE_INT32, caen965_chans * sizeof(int32_t), chaos::DataType::Output);
        addBinaryAttributeAsSubtypeToDataSet(
            "QDC965LO", "Vector of Channels Low Resolution", chaos::DataType::SUB_TYPE_INT32, caen965_chans * sizeof(int32_t), chaos::DataType::Output);
      } else if (caen965_chans == 32) {
        addBinaryAttributeAsSubtypeToDataSet(
            "QDC965", "Vector of Channels Low Resolution", chaos::DataType::SUB_TYPE_INT32, caen965_chans * sizeof(int32_t), chaos::DataType::Output);
      }
    }
  
    if (out_channels_792) {
      std::vector<int> arr = output_channels["caen792"];
      for (int cnt = 0; cnt < arr.size(); cnt++) {
        std::stringstream ss;
        ss << "TDC" << arr[cnt];
        addAttributeToDataSet(ss.str(), "Channel " + ss.str(), DataType::TYPE_INT32, DataType::Output);
        SCCULDBG<<"CHANNEL "<<ss.str();

      }
    } else {
      addBinaryAttributeAsSubtypeToDataSet(
          "QDC792", "Vector of Channels ", chaos::DataType::SUB_TYPE_INT32, caen792_chans * sizeof(int32_t), chaos::DataType::Output);
    }
  
  /* if (sis3800_handle) {
    addBinaryAttributeAsSubtypeToDataSet(
        "SCALER", "Vector of 32 Counters ", chaos::DataType::SUB_TYPE_INT32, 32 * sizeof(int32_t), chaos::DataType::Output);
  }*/
  if (pio_latch) {
    addStateVariable(StateVariableTypeAlarmCU, "missing_pio_trigger", "No PIO trigger received", 5000);
  }
  if (timeout_ms > 0) {
    addStateVariable(StateVariableTypeAlarmCU, "965_timeout", "No trigger received in the configured timeout", 5000);
    addStateVariable(StateVariableTypeAlarmCU, "792_timeout", "No trigger received in the configured timeout", 5000);
  }
}

void RTBTFdaqSimCU::unitDefineCustomAttribute() {}

// Abstract method for the initialization of the control unit
void RTBTFdaqSimCU::unitInit() {
  SCCULDBG << "Initializating";
  int cnt;
  counter = counter_old = 0;
  tot_lost              = 0;
  loop                  = 0;

    if (out_channels_965) {
      uint32_t* arr = (uint32_t*)malloc(sizeof(int32_t) * 32);
      qdclow        = arr;
      qdchi         = arr + 16;
    } else {
      if (caen965_chans == 16) {
        qdchi =
            getAttributeCache()->getRWPtr<uint32_t>(DOMAIN_OUTPUT, "QDC965HI");
        qdclow =
            getAttributeCache()->getRWPtr<uint32_t>(DOMAIN_OUTPUT, "QDC965LO");
      } else {
        qdclow = getAttributeCache()->getRWPtr<uint32_t>(DOMAIN_OUTPUT, "QDC965");
        qdchi  = &qdclow[16];
      }
    }
  
    if (out_channels_792) {
      uint32_t* arr = (uint32_t*)malloc(sizeof(int32_t) * 32);
      qdc792        = arr;
    } else {
      qdc792 = getAttributeCache()->getRWPtr<uint32_t>(DOMAIN_OUTPUT, "QDC792");
    }
  
  trigger_lost =
      getAttributeCache()->getRWPtr<uint64_t>(DOMAIN_OUTPUT, "TRIGGER_LOST");

  acquisition =
      getAttributeCache()->getRWPtr<uint64_t>(DOMAIN_OUTPUT, "ACQUISITION");

  freq           = getAttributeCache()->getRWPtr<double>(DOMAIN_OUTPUT, "TRIGGER_FREQ");
  efreq          = getAttributeCache()->getRWPtr<double>(DOMAIN_OUTPUT, "TRIGGER_EFREQ");
  triggers_valid = getAttributeCache()->getRWPtr<uint32_t>(DOMAIN_OUTPUT, "TRIGGERS_VALID");
  triggers       = getAttributeCache()->getRWPtr<uint32_t>(DOMAIN_OUTPUT, "TRIGGERS");

  //  caen513_init(caen513_handle,V513_CHANMODE_NEG|V513_CHANMODE_OUTPUT);
  //
  }

// Abstract method for the start of the control unit
void RTBTFdaqSimCU::unitStart() {
  loop        = 0;
  counter_old = counter = 0;
  tot_lost              = 0;
  
  last_eval         = chaos::common::utility::TimingUtil::getTimeStamp();
  last_eval_trigger = last_eval;
    //   sis3800_readCounter(sis3800_handle, counters, 32);
  
  periodic_task = chaos::common::utility::TimingUtil::getTimeStamp();
  loop          = counter_all;
  *trigger_lost = counter_all - loop;
}
// Abstract method for the start of the control unit
void RTBTFdaqSimCU::unitRun() {
  int      ret, cnt;
  uint64_t cycle0, cycle1;
  counter_old = counter;
  uint32_t pio;
  bool     timeout_arose = false;
  uint64_t now           = chaos::common::utility::TimingUtil::getTimeStamp();
  if ((periodic_task - now) > PERIODIC_TASK) {
   
    *freq         = (double)1000.0 * (counter - counter_trigger) / (now - periodic_task);
    *efreq        = (double)1000.0 * (counter_all - counter_etrigger) / (now - periodic_task);
    *trigger_lost = counter_all - loop;
    setStateVariableSeverity(StateVariableTypeAlarmCU, "missing_trigger", chaos::common::alarm::MultiSeverityAlarmLevelClear);
    counter_etrigger = counter_all;
    counter_trigger  = counter;

    getAttributeCache()->setOutputDomainAsChanged();
    periodic_task = now;
  }

   
 
  counter++;
  counter_all++;
  for(int c=0;c<caen965_chans;c++){
    qdclow[c]=qdchi[c]=(rand()%4095);
  }

    if (out_channels_965) {
      std::vector<int> arr = output_channels["caen965"];
      for (int cnt = 0; cnt < arr.size(); cnt++) {
        std::stringstream ss;
        ss << "QDC" << arr[cnt];
        getAttributeCache()->setOutputAttributeValue(ss.str(), qdclow[cnt]);
      }
    }
   for(int c=0;c<caen792_chans;c++){
    qdc792[c]=(rand()%4095);
  }
  //    dump_channels(out,low,cycle0,ret);
  // dump_channels(out,hi,cycle0,ret);
  
    if (out_channels_792) {
      std::vector<int> arr = output_channels["caen792"];
      for (int cnt = 0; cnt < arr.size(); cnt++) {
        std::stringstream ss;
        ss << "TDC" << arr[cnt];
        getAttributeCache()->setOutputAttributeValue(ss.str(), qdc792[cnt]);
      }
    }
  

  *acquisition    = ++loop;
  *triggers       = counter_all;
  *triggers_valid = counter;

  getAttributeCache()->setOutputDomainAsChanged();
}

// Abstract method for the stop of the control unit
void RTBTFdaqSimCU::unitStop() {
  DPRINT("ACQUISITION STOP SW %lu HW %u", loop, counter);
}

// Abstract method for the deinit of the control unit
void RTBTFdaqSimCU::unitDeinit() {
  
  if (out_channels_965) {
    free(qdclow);
  }
  if (out_channels_792) {
    free(qdc792);
  }
}

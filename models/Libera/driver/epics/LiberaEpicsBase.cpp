/*
 * LiberaEpicsBase.cpp
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

#include "LiberaEpicsBase.h"
#include <driver/epics/driver/EpicsCAccessDriver.h>

#define ILK_PARAMCOUNT 8
#include <chaos/cu_toolkit/driver_manager/driver/AbstractDriverPlugin.h>

#define LiberaSoftLAPP_ LAPP_ << "[LiberaEpicsBase] "
#define LiberaSoftDBG LDBG_ << "[LiberaEpicsBase " << __PRETTY_FUNCTION__ << " ]"
#define LiberaSoftERR LERR_ << "[LiberaEpicsBase " << __PRETTY_FUNCTION__ << " ]"
#define MAX_RETRY 0

using namespace ::driver::daq::libera;

// GET_PLUGIN_CLASS_DEFINITION
// we need to define the driver with alias version and a class that implement it
// default constructor definition
DEFAULT_CU_DRIVER_PLUGIN_CONSTRUCTOR_WITH_NS(::driver::daq::libera,LiberaEpicsBase) {
  int rc;
  cfg.operation = liberaconfig::deinit;
  devicedriver=NULL;
  LiberaSoftDBG<<"Driver @"<<std::hex<<this;
 
}

// default descrutcor

LiberaEpicsBase::~LiberaEpicsBase() {
 // deinitIO();
 if(devicedriver){
   delete devicedriver;
 }
}
void LiberaEpicsBase::createProperties() {
  std::vector<std::string>           listPV = devicedriver->pvList();
  std::vector<std::string>::iterator i      = listPV.begin();
  int retry=MAX_RETRY;
  while (i != listPV.end()) {
    LDBG_ << "retriving information of " << *i;  //<<" ="<<r->getJSONString();

    chaos::common::data::CDWUniquePtr r = devicedriver->readRecord(*i);
    if (r.get()) {
      chaos::common::data::CDWUniquePtr conf = devicedriver->getPVConfig(*i);
      if (conf.get()) {
        std::string cname;
        if (conf->hasKey(KEY_CNAME) && (r->hasKey(PROPERTY_VALUE_KEY))) {
          cname = conf->getStringValue(KEY_CNAME);
          LDBG_ << "create PUBLIC property:" << *i << " CNAME:" << cname;  //<<" ="<<r->getJSONString();
          createProperty(
            *i,
            [](AbstractDriver *thi, const std::string &name, const chaos::common::data::CDataWrapper &p)
                -> chaos::common::data::CDWUniquePtr {
              //read handler
              return ((LiberaEpicsBase *)thi)->devicedriver->readRecord(name);
            },
            [](AbstractDriver *thi, const std::string &name, const chaos::common::data::CDataWrapper &p)
                -> chaos::common::data::CDWUniquePtr {
              ((LiberaEpicsBase *)thi)->devicedriver->writeRecord(name, p);
              return chaos::common::data::CDWUniquePtr();
            },
            cname);

        }/* else {
          LDBG_ << "create  property:" << *i;  //<<" ="<<r->getJSONString();
        }*/
        
      }
      i++;
    } else {
        if(retry--){
		      devicedriver->waitChange(*i);
        } else {
          i++;
          retry=MAX_RETRY;
        }

	}
  }
}
void LiberaEpicsBase::driverInit(const chaos::common::data::CDataWrapper &json) throw(chaos::CException){
  LiberaSoftDBG<<"Configuration:"<<json.getJSONString();
  devicedriver = new ::driver::epics::common::EpicsCAccessDriver(json);
  
  createProperties();

}
void LiberaEpicsBase::driverDeinit(){
  if(devicedriver){
    LiberaSoftDBG<<"Destroy";

    delete devicedriver;
    devicedriver=NULL;
  }
}

void LiberaEpicsBase::driverInit(const char *initParameter) throw(chaos::CException) {

  if (initParameter != NULL) {
    chaos::common::data::CDataWrapper cw;
    cw.setSerializedJsonData(initParameter);
    driverInit(cw);

  }
  throw chaos::CException(-1,"invalid configuration",__PRETTY_FUNCTION__);

}
int LiberaEpicsBase::assign_time(const char *time) {
  const char  delim       = ':';
  const char  delim_phase = '.';
  std::string s(time);

  size_t p = s.find(delim);
  if (std::string::npos == p) {
    LiberaSoftERR << "Invalid argument -- 'TIME' missing delimiter \":\"";
    return -4;
  }
  std::string s2(s.substr(0, p - 0));
  if (!s2.empty()) {
    cfg.mask |= liberaconfig::want_setmt;

    size_t p_phase = s2.find(delim_phase);
    if (std::string::npos == p_phase) {
      // No LMT Phase specified
      cfg.time.mt    = atoll(s2.c_str());
      cfg.time.phase = 0;

    } else {
      // MT + LMT Phase specified
      std::string s_mt(s2.substr(0, p_phase - 0));
      std::string s_phase(s2.substr(p_phase + 1));

      cfg.time.mt = atoll(s_mt.c_str());
      if (!s_phase.empty())
        cfg.time.phase = atoll(s_phase.c_str());
      else
        cfg.time.phase = 0;
    }
  }

  s2 = s.substr(p + 1);
  if (!s2.empty()) {
    for (p = 4; p < (s2.size() - 3); ++p)
      if (p % 3 == 1) s2.insert(p, 1, delim);

    struct tm t;
    if (!strptime(s2.c_str(), "%Y:%m:%d:%H:%M.%S", &t)) {
      LiberaSoftERR << "Invalid argument -- 'strptime'";
      return -1;
    }
    cfg.time.st = mktime(&t);
    if (-1 == cfg.time.st) {
      LiberaSoftERR << "Invalid argument -- 'mkTIME'";

      return -2;
    }

    cfg.mask |= liberaconfig::want_setst;
  }

  return 0;
}

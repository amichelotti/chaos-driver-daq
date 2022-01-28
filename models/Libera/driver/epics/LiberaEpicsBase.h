/*
 * LiberaEpicsBase.h
 * @author michelo
  Copyright January 2022 Andrea Michelotti
 * epics driver for Libera Brilliance

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

#ifndef __LiberaEpicsBase_H__
#define __LiberaEpicsBase_H__

#include <driver/daq/models/Libera/LiberaData.h>
#include <chaos/cu_toolkit/driver_manager/driver/AbstractDriverPlugin.h>
#include <chaos/cu_toolkit/driver_manager/driver/ReadWriteInterface.h>

namespace cu_driver = chaos::cu::driver_manager::driver;
namespace driver {
namespace epics {
namespace common {
class EpicsGenericDriver;
}
}  // namespace epics
}  // namespace driver
namespace driver {

namespace daq {
namespace libera {

#define READPV(pv,val) {\
int ret;\
if((ret=devicedriver->read(pv,val))<=0){ LERR_<<"["<<__PRETTY_FUNCTION__<<"] cannot read \""<<pv<<"\" , ret:"<<ret;}}

#define READPVARRAY(pv,val,count) {\
int ret;\
if((ret=devicedriver->readArray(pv,val,count))<=0){  LERR_<<"["<<__PRETTY_FUNCTION__<<"] cannot readarray \""<<pv<<"\" count:"<<count<<", ret:"<<ret;}}

class LiberaEpicsBase:  public chaos::cu::driver_manager::driver::AbstractDriverPlugin,public chaos::cu::driver_manager::driver::ReadWriteInterface   {
 protected:
  int            driver_mode;
  int            nacquire;
  char          *raw_data;
  CSPIHENV       env_handle;
  CSPIHCON       con_handle;
  CSPI_LIBPARAMS lib;
  CSPI_ENVPARAMS ep;
  CSPI_CONPARAMS p;

  struct liberaconfig {
    liberaconfig()
        : operation(unknown), mode(CSPI_MODE_UNKNOWN), atom_count(0), loop_count(1), mask(0), offset(0) {}

    enum { unknown = 0,
           init,
           deinit,
           acquire,
           setenv,
           listenv,
           settime };
    size_t operation;  // Main operation mode

    size_t mode;        // Acquisition mode
    struct dd_specific  // Specifics for DD acq. mode
    {
      dd_specific()
          : decimation(0), offset(0){};

      size_t             decimation;  // 1 or 64, 0=ignore
      unsigned long long offset;      // history buffer offset in MT units
    } dd;
    struct adc_specific {
      adc_specific()
          : mode(adc_specific::none), rotate(0){};
      int mode;

      enum {
        none = 0x00,
        cw   = 0x01,
        sp   = 0x02
      };
      int rotate;
    } adc;

    size_t atom_count;  // number of samples to retrieve
    size_t loop_count;  // number of iterations (repetitions)
    size_t offset;
    size_t datasize;
    struct settime_specific {
      settime_specific()
          : mt(0), st(0) {}

      unsigned long long mt;     // machine time
      unsigned long      phase;  // LMT phase
      time_t             st;     // system time (seconds since 1/1/1970)
    } time;

    enum {
      want_timestamp = 0x01,
      want_raw       = 0x02,
      want_trigger   = 0x04,
      want_binary    = 0x08,
      want_setmt     = 0x10,
      want_setst     = 0x20,
      want_reserved  = 0x40,
      want_dcc       = 0x80,
    };
    CSPI_BITMASK mask;  // command-line switches (flags)
  };

  struct liberaconfig cfg;
  int                 assign_time(const char *time);

  int                                          trigger_time_ms;
  CSPI_ENVPARAMS                               myenv;
  uint64_t                                     libera_ts;
  ::driver::epics::common::EpicsGenericDriver *devicedriver;
  void createProperties();
 public:
  void driverInit(const char *initParameter) throw(chaos::CException);
  virtual void driverInit(const chaos::common::data::CDataWrapper &json) throw(chaos::CException)=0;


  LiberaEpicsBase();

  ~LiberaEpicsBase();
  
};
}  // namespace libera
}  // namespace daq
}  // namespace driver

#endif  //__LiberaBrillianceSlow_H__

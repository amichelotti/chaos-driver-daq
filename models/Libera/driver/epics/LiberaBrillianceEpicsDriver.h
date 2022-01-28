/*
 * LiberaBrillianceEpicsDriver.h
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

#ifndef __LiberaBrillianceEpicsDriver_H__
#define __LiberaBrillianceEpicsDriver_H__
#include "LiberaEpicsBase.h"
#include <driver/daq/models/Libera/LiberaData.h>
namespace cu_driver = chaos::cu::driver_manager::driver;
DEFINE_CU_DRIVER_DEFINITION_PROTOTYPE(LiberaBrillianceEpicsDriver);
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
#define DD1_NELM 1000
#define DD2_NELM 10000
#define DD3_NELM 30000
#define DD4_NELM 400000

class LiberaBrillianceEpicsDriver : public LiberaEpicsBase{
 
  uint32_t maxDDSamples;// epics driver cannot set dinamically size, so the right port is selected on the base of the maxSamples
  std::string dd_port;
 public:
  LiberaBrillianceEpicsDriver();

  ~LiberaBrillianceEpicsDriver();
  //! Execute a command

  /**
   \brief Read  from the physical device
   \param buffer[out] destination buffer
   \param addr[in]  address or identification
   \param bcout[in] buffer count
   \return the number of succesful read items, negative error
   */
  virtual int read(void *buffer, int addr, int bcount);
  /**
   \brief Write a channel of the device
   \param buffer[out] destination buffer
   \param addr[in] channel address or identification
   \param bcout[in] buffer count
   \return the number of succesful written items, negative error
   */
  virtual int write(void *buffer, int addr, int bcount);

  /**
   \brief Initialize the specific driver
   \param buffer[in] initialisation opaque parameter
   \return 0 if success, error otherwise

   */
  virtual int initIO(void *buffer, int sizeb);

  /**
   \brief deinit the driver
   \param buffer[in] initialisation opaque parameter
   \return 0 if success, error otherwise
   */
  virtual int deinitIO();

  /**
   \brief perform a specific I/O operation  the device
   \param data[inout] operation data opaque parameter
   \param sizeb[in] operation data size/maxsize
   \return 0 if success, error otherwise
   */
  virtual int  iop(int operation, void *data, int sizeb);
  virtual void driverInit(const chaos::common::data::CDataWrapper &json) throw(chaos::CException);
  virtual void driverInit(const char *initParameter) throw(chaos::CException);
};
}  // namespace libera
}  // namespace daq
}  // namespace driver

#endif  //__LiberaBrillianceSlow_H__

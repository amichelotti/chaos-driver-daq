/*  
 * LiberaBrillianceSlow.h
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

#ifndef __LiberaBrillianceSlow_H__
#define __LiberaBrillianceSlow_H__
#include <chaos/cu_toolkit/driver_manager/driver/BasicIODriver.h>
DEFINE_CU_DRIVER_DEFINITION_PROTOTYPE(LiberaBrillianceSlow);

class LiberaBrillianceSlow : public chaos::cu::driver_manager::driver::BasicIODriver {
public:
    LiberaBrillianceSlow();

    ~LiberaBrillianceSlow();
    //! Execute a command

    /**
     \brief Read  from the physical device
     \param buffer[out] destination buffer
     \param addr[in]  address or identification
     \param bcout[in] buffer count
     \return the number of succesful read items, negative error
     */
    int read(void *buffer, int addr, int bcount);
    /**
     \brief Write a channel of the device
     \param buffer[out] destination buffer
     \param addr[in] channel address or identification
     \param bcout[in] buffer count
     \return the number of succesful written items, negative error
     */
    int write(void *buffer, int addr, int bcount);

    /**
     \brief Initialize the specific driver
     \param buffer[in] initialisation opaque parameter
     \return 0 if success, error otherwise
     
     */
    int initIO(void *buffer, int sizeb);

    /**
     \brief deinit the driver
     \param buffer[in] initialisation opaque parameter
     \return 0 if success, error otherwise
     */
    int deinitIO();


    /**
     \brief perform a specific I/O operation  the device
     \param data[inout] operation data opaque parameter
     \param sizeb[in] operation data size/maxsize
     \return 0 if success, error otherwise
     */
    int iop(int operation, void*data, int sizeb);

};

#endif //__LiberaBrillianceSlow_H__
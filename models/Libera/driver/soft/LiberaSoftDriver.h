/*  
 * LiberaSoftDriver.h
 * @author michelo
Copyright Apr 29, 2015 michelo
 * simulator driver for Libera Brilliance + 

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

#ifndef __LiberaSoftDriver_H__
#define __LiberaSoftDriver_H__
#include <chaos/cu_toolkit/driver_manager/driver/ReadWriteInterface.h>
#define CSPI
#include <driver/daq/models/Libera/LiberaData.h>
#include <common/misc/wavegenerators/WaveBase.h>
DEFINE_CU_DRIVER_DEFINITION_PROTOTYPE(LiberaSoftDriver);
namespace driver {

namespace daq {
namespace libera {


class LiberaSoftDriver : public chaos::cu::driver_manager::driver::AbstractDriverPlugin,public chaos::cu::driver_manager::driver::ReadWriteInterface {
protected:
    int driver_mode;
    int nacquire;
    char*raw_data;
    CSPIHENV env_handle;
    CSPIHCON con_handle;
    CSPI_LIBPARAMS lib;
    CSPI_ENVPARAMS ep;
    CSPI_CONPARAMS p;

void driverInit(const char *initParameter) throw(chaos::CException);
  virtual void driverInit(const chaos::common::data::CDataWrapper &json) throw(chaos::CException);
  virtual void driverDeinit();

struct liberaconfig
{
	liberaconfig() :
		operation(unknown),
		mode(CSPI_MODE_UNKNOWN),
		atom_count(0),
		loop_count(1),
		mask(0),
                offset(0)
		{}

	enum {unknown=0, init,deinit,acquire, setenv, listenv, settime};
	size_t operation;		// Main operation mode

	size_t mode;			// Acquisition mode
	struct dd_specific		// Specifics for DD acq. mode
	{
		dd_specific() : decimation(0), offset(0) {};

		size_t decimation;			// 1 or 64, 0=ignore
		unsigned long long offset;	// history buffer offset in MT units
	} dd;
	struct adc_specific
	{
		adc_specific() : mode(adc_specific::none), rotate(0) {};
		int mode;

		enum {
			none = 0x00,
			cw   = 0x01,
			sp   = 0x02
		};
		int rotate;
	} adc;

	size_t atom_count;		// number of samples to retrieve
	size_t loop_count;		// number of iterations (repetitions)
        size_t offset;
        size_t datasize;
	struct settime_specific
	{
		settime_specific() : mt(0), st(0) {}

		unsigned long long mt;	// machine time
		unsigned long phase;	// LMT phase
		time_t st;				// system time (seconds since 1/1/1970)
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
	CSPI_BITMASK mask;			// command-line switches (flags)
};
    
    struct liberaconfig cfg;
    int wait_trigger();
    int assign_time(const char*time );
    
    int trigger_time_ms;
    ::common::misc::wavegenerators::WaveBase_t wave;
    CSPI_ENVPARAMS myenv;
public:
    LiberaSoftDriver();

    ~LiberaSoftDriver();
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
    virtual int initIO(void *buffer, int sizeb);

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
}}}
#endif //__LiberaBrillianceSlow_H__

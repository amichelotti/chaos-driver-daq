/*
*	daqServer.cpp
*	!CHAOS
*	Created by Andrea Michelotti
*
*    	Copyright 2012 INFN, National Institute of Nuclear Physics
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


#include <chaos/cu_toolkit/control_manager/IOCU.h>
#ifdef LIBERA
#include "driver/daq/models/Libera/LiberaBrillianceCSPIDriver.h"
#include <driver/daq/models/Libera/LiberaSoftDriver.h>
#endif
#include "SCLiberaCU.h"
#include <string>

#include <chaos/cu_toolkit/ChaosCUToolkit.h>


using namespace chaos;
using namespace chaos::cu;
using namespace chaos::cu::driver_manager;


int main(int argc, char *argv[])
{
	try {
		// initialize the control unit toolkit
		ChaosCUToolkit::getInstance()->init(argc, argv);
#ifdef LIBERA
		// allocate the instance and inspector for driver
		REGISTER_DRIVER(,LiberaBrillianceCSPIDriver);
		//                REGISTER_DRIVER(,LiberaSoftDriver);

                REGISTER_CU(::driver::daq::libera::SCLiberaCU);
#endif
		// start control unit toolkit until someone will close it
		ChaosCUToolkit::getInstance()->start();
	} catch (CException& ex) {
		DECODE_CHAOS_EXCEPTION(ex)
	} catch (program_options::error &e){
		cerr << "Unable to parse command line: " << e.what() << endl;
	} catch (...){
		cerr << "unexpected exception caught.. " << endl;
	}
	return 0;
}

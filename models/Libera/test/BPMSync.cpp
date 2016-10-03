/*
*	BPMSync.cpp
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


#include <string>

#include <chaos/cu_toolkit/ChaosCUToolkit.h>
#include "DafneAccumulatorBPMSync.h"
#include "SCDafneAccumulatorBPMSync.h"
#include <driver/misc/core/remoteGroupAccessDriver.h>


using namespace chaos;
using namespace chaos::cu;
using namespace chaos::ui;

using namespace chaos::cu::driver_manager;


int main(int argc, char *argv[])
{
	try {
		// initialize the control unit toolkit
		ChaosCUToolkit::getInstance()->init(argc, argv);
                //REGISTER_CU(::driver::daq::libera::SCLiberaCU);

                REGISTER_CU(::driver::daq::libera::DafneAccumulatorBPMSync);
                REGISTER_CU(::driver::daq::libera::SCDafneAccumulatorBPMSync);
                REGISTER_DRIVER(::driver::misc,remoteGroupAccessDriver);
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

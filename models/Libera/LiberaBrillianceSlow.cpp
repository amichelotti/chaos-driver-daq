/*  
 * LiberaBrillianceSlow.cpp
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

#include "LiberaBrillianceSlow.h"


#include <chaos/cu_toolkit/driver_manager/driver/AbstractDriverPlugin.h>

#include <boost/lexical_cast.hpp>


#define LiberaBrillianceSlowLAPP_		LAPP_ << "[LiberaBrillianceSlow] "
#define LiberaBrillianceSlowLDBG_		LDBG_ << "[LiberaBrillianceSlow] "
#define LiberaBrillianceSlowLERR_		LERR_ << "[LiberaBrillianceSlow] "
using namespace chaos::cu::driver_manager::driver;

OPEN_CU_DRIVER_PLUGIN_CLASS_DEFINITION(LiberaBrillianceSlow, 1.0.0, LiberaBrillianceSlow)
REGISTER_CU_DRIVER_PLUGIN_CLASS_INIT_ATTRIBUTE(LiberaBrillianceSlow, http_address / dnsname : port)
CLOSE_CU_DRIVER_PLUGIN_CLASS_DEFINITION


DEF_IO_DRIVER_DATASET
//DEF_IO_CHANNEL("channel","Desc",chaos::DataType::Output,chaos::DataType::TYPE_DOUBLE,sizeof(double))
ENDDEF_IO_DRIVER_DATASET

//GET_PLUGIN_CLASS_DEFINITION
//we need to define the driver with alias version and a class that implement it
//default constructor definition
LiberaBrillianceSlow::LiberaBrillianceSlow() {
    INIT_IO_DRIVER_DATASET
}


//default descrutcor

LiberaBrillianceSlow::~LiberaBrillianceSlow() {

}

int LiberaBrillianceSlow::read(void *buffer, int addr, int bcount) {
    //TODO: implement the method
    return 0;
}

int LiberaBrillianceSlow::write(void *buffer, int addr, int bcount) {
    //TODO: implement the method
    return 0;
}

int LiberaBrillianceSlow::initIO(void *buffer, int sizeb) {
    //TODO: implement the method
    return 0;
}

int LiberaBrillianceSlow::deinitIO() {
    //TODO: implement the method
    return 0;
}

int LiberaBrillianceSlow::iop(int operation, void*data, int sizeb) {
    //TODO: implement the method
    return 0;
}

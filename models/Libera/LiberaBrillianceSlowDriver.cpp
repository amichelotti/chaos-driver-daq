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

#include "LiberaBrillianceSlowDriver.h"


#include <chaos/cu_toolkit/driver_manager/driver/AbstractDriverPlugin.h>

#include <boost/lexical_cast.hpp>

#define LiberaBrillianceSlowLAPP_		LAPP_ << "[LiberaBrillianceSlow] "
#define LiberaBrillianceSlowLDBG_		LDBG_ << "[LiberaBrillianceSlow] "
#define LiberaBrillianceSlowLERR_		LERR_ << "[LiberaBrillianceSlow] "
using namespace chaos::cu::driver_manager::driver;

OPEN_CU_DRIVER_PLUGIN_CLASS_DEFINITION(LiberaBrillianceSlowDriver, 1.0.0, LiberaBrillianceSlowDriver)
REGISTER_CU_DRIVER_PLUGIN_CLASS_INIT_ATTRIBUTE(LiberaBrillianceSlowDriver, http_address / dnsname : port)
CLOSE_CU_DRIVER_PLUGIN_CLASS_DEFINITION


DEF_IO_DRIVER_DATASET
DEF_IO_CHANNEL("VA","Volt A",chaos::DataType::Output,chaos::DataType::TYPE_INT32,sizeof(int32_t))
DEF_IO_CHANNEL("VB","Volt B",chaos::DataType::Output,chaos::DataType::TYPE_INT32,sizeof(int32_t))

DEF_IO_CHANNEL("VC","Volt C",chaos::DataType::Output,chaos::DataType::TYPE_INT32,sizeof(int32_t))
DEF_IO_CHANNEL("VD","Volt D",chaos::DataType::Output,chaos::DataType::TYPE_INT32,sizeof(int32_t))

DEF_IO_CHANNEL("X","X",chaos::DataType::Output,chaos::DataType::TYPE_INT32,sizeof(int32_t))
DEF_IO_CHANNEL("Y","Y",chaos::DataType::Output,chaos::DataType::TYPE_INT32,sizeof(int32_t))

DEF_IO_CHANNEL("Q","Q",chaos::DataType::Output,chaos::DataType::TYPE_INT32,sizeof(int32_t))
DEF_IO_CHANNEL("SUM","SUM",chaos::DataType::Output,chaos::DataType::TYPE_INT32,sizeof(int32_t))
DEF_IO_CHANNEL("Q1","Q1",chaos::DataType::Output,chaos::DataType::TYPE_INT32,sizeof(int32_t))
DEF_IO_CHANNEL("Q2","Q2",chaos::DataType::Output,chaos::DataType::TYPE_INT32,sizeof(int32_t))
DEF_IO_CHANNEL("N","Number of Acquisitions",chaos::DataType::Output,chaos::DataType::TYPE_INT32,sizeof(int32_t))
DEF_IO_CHANNEL("Data","Data Array VA VB VC VD X Y Q SUM",chaos::DataType::Output,chaos::DataType::TYPE_CLUSTER|chaos::DataType::TYPE_ACCESS_ARRAY,sizeof(liberaData_t)*64000)

ENDDEF_IO_DRIVER_DATASET



int LiberaBrillianceSlowDriver::read_libera(liberaData_t *data,int size){
    char cmd[256];
    int ret;
    FILE *f;
    cmd[0]=0;
     int cnt=0;
     switch(driver_mode){
        case LIBERA_IOP_TURN_BY_TURN_DATA:
            sprintf(cmd,"libera -0 %d",size);
            break;
        case LIBERA_IOP_TURN_BY_TURN_DATA_TRIGGERED:
             sprintf(cmd,"libera -0 -t %d",size);
            break;
        case LIBERA_IOP_DATA_DECIMATED:
             sprintf(cmd,"libera -0 -d64 %d",size);
            break;
        case LIBERA_IOP_DATA_SLOW_DAQ:
             sprintf(cmd,"libera -1 %d",size);
            break;
        case LIBERA_IOP_DATA_ADC_OUT:
             sprintf(cmd,"libera -3 -t %d",size);
            break;
         default:
             ;
    }
     if(cmd[0]!=0 && (data!=NULL)){
        
         f=popen(cmd,"r");
         if(f==NULL){
             LiberaBrillianceSlowLERR_<<"## cannot execute command \""<<cmd<<"\"";
             return 0;
         }
         while(!feof(f)){
            memset(&data[cnt],0,sizeof(liberaData_t));
            if((ret=fscanf(f,"%d %d %d %d %d %d %d %d %d %d",&data[cnt].libera.va,&data[cnt].libera.vb,&data[cnt].libera.vc,&data[cnt].libera.vd,&data[cnt].libera.x,&data[cnt].libera.y,&data[cnt].libera.q,&data[cnt].libera.sum,&data[cnt].libera.q1,&data[cnt].libera.q2))>0){
                LiberaBrillianceSlowLDBG_<<"read :"<<ret<<" params, "<<data[cnt].libera.va<<","<<data[cnt].libera.vb<<","<<data[cnt].libera.vc<<","<<data[cnt].libera.vd<<","<<data[cnt].libera.x<<","<<data[cnt].libera.y<<","<<data[cnt].libera.q<<","<<data[cnt].libera.sum<<","<<data[cnt].libera.q1<<","<<data[cnt].libera.q2;
                if(cnt<size)
                    cnt++;
            } else {
                LiberaBrillianceSlowLERR_<<"Error decoding ret="<<ret;
            }
     }
         pclose(f);
    }
     return cnt;
}

//GET_PLUGIN_CLASS_DEFINITION
//we need to define the driver with alias version and a class that implement it
//default constructor definition
LiberaBrillianceSlowDriver::LiberaBrillianceSlowDriver() {
    INIT_IO_DRIVER_DATASET
    driver_mode =LIBERA_IOP_TURN_BY_TURN_DATA;
    nacquire=1;
    data = (liberaData_t*)malloc(sizeof(liberaData_t));
    if(data==NULL){
        throw chaos::CException(0,"Cannot allocate buffer","LiberaBrillianceSlowDriver");
    }
}


//default descrutcor

LiberaBrillianceSlowDriver::~LiberaBrillianceSlowDriver() {
    if(data){
        free(data);
        data=NULL;
    }
}

int LiberaBrillianceSlowDriver::read(void *buffer, int addr, int bcount) {
    int ret=0;
    if(addr==0){
        ret=read_libera(data,nacquire);
        LiberaBrillianceSlowLDBG_<<"read libera items :"<<ret;    
    }
    int32_t*t=(int32_t*)buffer;

    if(addr<10){
        memcpy(buffer,&data[0].raw[addr],sizeof(int32_t));
    } else if(addr==10){
        *t= nacquire;
    } else {

       memcpy(buffer,data,sizeof(liberaData_t)*std::min(nacquire,(int)(bcount/sizeof(liberaData_t))));

    }
    
    //TODO: implement the method
    return 1;
}

int LiberaBrillianceSlowDriver::write(void *buffer, int addr, int bcount) {
    //TODO: implement the method
    return 0;
}

int LiberaBrillianceSlowDriver::initIO(void *buffer, int sizeb) {
    //TODO: implement the method
    return 0;
}

int LiberaBrillianceSlowDriver::deinitIO() {
    //TODO: implement the method
    return 0;
}

int LiberaBrillianceSlowDriver::iop(int operation, void*data, int sizeb) {
    switch(operation){
        case LIBERA_IOP_TURN_BY_TURN_DATA:
        case LIBERA_IOP_TURN_BY_TURN_DATA_TRIGGERED:
        case LIBERA_IOP_DATA_DECIMATED:
        case LIBERA_IOP_DATA_SLOW_DAQ:
        case LIBERA_IOP_DATA_ADC_OUT:
            driver_mode = operation;
            break;
        case LIBERA_IO_DATA_ACQUIRE:
            nacquire = *(int32_t*)data;
             data = (liberaData_t*)realloc(data,nacquire*sizeof(liberaData_t));
            if(data==NULL){
                throw chaos::CException(0,"Cannot rellocate buffer","LiberaBrillianceSlowDriver::iop");
            }
            break;
                   
        default:;
    }
    return 0;
}

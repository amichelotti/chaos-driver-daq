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

#include "LiberaBrillianceCSPIDriver.h"


#include <chaos/cu_toolkit/driver_manager/driver/AbstractDriverPlugin.h>

#include <boost/lexical_cast.hpp>

#define LiberaBrillianceSlowLAPP_		LAPP_ << "[LiberaBrillianceSlow] "
#define LiberaBrillianceSlowLDBG_		LDBG_ << "[LiberaBrillianceSlow] "
#define LiberaBrillianceSlowLERR_		LERR_ << "[LiberaBrillianceSlow] "
using namespace chaos::cu::driver_manager::driver;

OPEN_CU_DRIVER_PLUGIN_CLASS_DEFINITION(LiberaBrillianceCSPIDriver, 1.0.0, LiberaBrillianceCSPIDriver)
REGISTER_CU_DRIVER_PLUGIN_CLASS_INIT_ATTRIBUTE(LiberaBrillianceCSPIDriver, http_address / dnsname : port)
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
DEF_IO_CHANNEL("Data","Data Array VA VB VC VD X Y Q SUM",chaos::DataType::Output,chaos::DataType::TYPE_CLUSTER|chaos::DataType::TYPE_ACCESS_ARRAY,sizeof(liberaData_t)*100)

ENDDEF_IO_DRIVER_DATASET



int LiberaBrillianceCSPIDriver::read_libera(liberaData_t *data,int size){
    //	rc = cfg.mask & config::want_trigger ? CSPI_SEEK_TR : CSPI_SEEK_MT;
    int cnt=0;
    int ret;
    size_t re=0;
    cspi_seek(con_handle,0,CSPI_SEEK_MT);
    ret=cspi_read(con_handle,&data[cnt],size,&re);
     
          
    while((cnt<size)){
        LiberaBrillianceSlowLDBG_<<re<<"] read :"<<ret<<" params, "<<data[cnt].libera.va<<","<<data[cnt].libera.vb<<","<<data[cnt].libera.vc<<","<<data[cnt].libera.vd<<","<<data[cnt].libera.x<<","<<data[cnt].libera.y<<","<<data[cnt].libera.q<<","<<data[cnt].libera.sum<<","<<data[cnt].libera.q1<<","<<data[cnt].libera.q2;
        cnt++;

    }
    
}

static volatile size_t _event_id = 0;
static pthread_cond_t eventc = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t eventm = PTHREAD_MUTEX_INITIALIZER;

int event_callback(CSPI_EVENT *p)
{
	_event_id = p->hdr.id;
	pthread_cond_signal(&eventc);

	return 0;
}
//GET_PLUGIN_CLASS_DEFINITION
//we need to define the driver with alias version and a class that implement it
//default constructor definition
LiberaBrillianceCSPIDriver::LiberaBrillianceCSPIDriver() {
    INIT_IO_DRIVER_DATASET
    driver_mode =LIBERA_IOP_TURN_BY_TURN_DATA;
    nacquire=1;
    data = (liberaData_t*)malloc(sizeof(liberaData_t));
    if(data==NULL){
        throw chaos::CException(0,"Cannot allocate buffer","LiberaBrillianceCSPIDriver");
    }
    CSPI_LIBPARAMS lib = {1,1};
    cspi_setlibparam(&lib, CSPI_LIB_SUPERUSER);
    int rc = cspi_allochandle(CSPI_HANDLE_ENV, 0, &env_handle);
    if (CSPI_OK != rc) {
        throw chaos::CException(rc,"Cannot allocate CSPI env resources","LiberaBrillianceCSPIDriver");    
    }
    rc = cspi_allochandle(CSPI_HANDLE_CON, env_handle, &con_handle);
    if (CSPI_OK != rc) {
        throw chaos::CException(rc,"Cannot allocate CSPI connection resources","LiberaBrillianceCSPIDriver");    
    }
     cfg.operation=liberaconfig::acquire;
     cfg.mode =0;
     cfg.atom_count=1;
     CSPI_CONPARAMS p;
     CSPI_BITMASK event_mask= CSPI_EVENT_TRIGGET;
     CSPI_BITMASK param_mask = CSPI_CON_MODE;
     param_mask |= (CSPI_CON_HANDLER|CSPI_CON_EVENTMASK);
     //const size_t modes[] = {CSPI_MODE_DD, CSPI_MODE_SA, CSPI_MODE_PM, CSPI_MODE_ADC, CSPI_MODE_AVERAGE};
     //DD data on demand
     p.mode =0;
     p.handler=event_callback;
     p.event_mask=CSPI_EVENT_TRIGGET;
     rc = cspi_setconparam(con_handle, &p, param_mask);
     if (CSPI_OK != rc) {
        throw chaos::CException(rc,"Cannot connect to CSPI connection","LiberaBrillianceCSPIDriver");    
     }
}


//default descrutcor

LiberaBrillianceCSPIDriver::~LiberaBrillianceCSPIDriver() {
    if(data){
        free(data);
        data=NULL;
    }
    if(env_handle){
        int rc = cspi_freehandle(CSPI_HANDLE_ENV, env_handle);
        if (CSPI_OK != rc) {
        throw chaos::CException(rc,"Cannot de-allocate CSPI ENV resources","~LiberaBrillianceCSPIDriver");    
    }
        env_handle =NULL;
    }
    if(con_handle){
        int rc = cspi_freehandle(CSPI_HANDLE_CON, con_handle);
        if (CSPI_OK != rc) {
        throw chaos::CException(rc,"Cannot de-allocate CSPI CONN resources","~LiberaBrillianceCSPIDriver");    
    }
        con_handle =NULL;
    }
}

int LiberaBrillianceCSPIDriver::read(void *buffer, int addr, int bcount) {
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

int LiberaBrillianceCSPIDriver::write(void *buffer, int addr, int bcount) {
    //TODO: implement the method
    return 0;
}

int LiberaBrillianceCSPIDriver::initIO(void *buffer, int sizeb) {
    //TODO: implement the method
    return 0;
}

int LiberaBrillianceCSPIDriver::deinitIO() {
    //TODO: implement the method
    return 0;
}

int LiberaBrillianceCSPIDriver::iop(int operation, void*data, int sizeb) {
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
                throw chaos::CException(0,"Cannot rellocate buffer","LiberaBrillianceCSPIDriver::iop");
            }
            break;
                   
        default:;
    }
    return 0;
}

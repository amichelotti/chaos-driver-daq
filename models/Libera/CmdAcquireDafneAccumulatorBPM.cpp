/* 
 * File:   CmdAcquireDafneAccumulatorBPM.cpp
 * Author: michelo
 * 
 * Created on October 15, 2015, 10:21 AM
 */

#include "CmdAcquireDafneAccumulatorBPM.h"
#include "LiberaData.h"
#include <boost/format.hpp>
#include <chaos/cu_toolkit/control_manager/slow_command/SlowCommand.h>

using namespace ::driver::daq::libera;
using namespace ::driver::misc;

BATCH_COMMAND_OPEN_DESCRIPTION_ALIAS(driver::daq::libera::,CmdAcquireDafneAccumulatorBPM,"acquire","acquire command","72882f3e-36db-11e5-985f-334fcd6dff22")
BATCH_COMMAND_ADD_INT32_PARAM("enable", "enable acquisition =1, disable =0",chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_MANDATORY)
BATCH_COMMAND_ADD_INT32_PARAM("mode", "acquisition modes, =1 SlowAcquisition, =2 Data on Demand, add 0x100 for DD triggering",chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_MANDATORY)
BATCH_COMMAND_ADD_INT32_PARAM("samples", "in DataOnDemand number of samples",chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_OPTIONAL)
BATCH_COMMAND_ADD_INT32_PARAM("loops", "acquisition loops, -1 means continuos, to break launch a acquire command with enable=0",chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_OPTIONAL)

BATCH_COMMAND_CLOSE_DESCRIPTION()

CmdAcquireDafneAccumulatorBPM::CmdAcquireDafneAccumulatorBPM() {
    
}



CmdAcquireDafneAccumulatorBPM::~CmdAcquireDafneAccumulatorBPM() {
}

uint8_t CmdAcquireDafneAccumulatorBPM::implementedHandler(){
       return chaos::common::batch_command::HandlerType::HT_Set  | chaos::common::batch_command::HandlerType::HT_Acquisition;

}
void  CmdAcquireDafneAccumulatorBPM::setHandler(c_data::CDataWrapper *data){
  ::driver::misc::CmdSync::setHandler(data);
  int tomode=0;
    if(data->hasKey("enable")) {
            if(data->getInt32Value("enable")==0){
               
                BC_END_RUNNIG_PROPERTY;
                return;
            }
   }
  
   if(data->hasKey("mode")) {
       tomode=data->getInt32Value("mode");
        CTRLDBG_<<" Going into mode:"<<tomode;
            if(tomode==0){
               
                BC_END_RUNNIG_PROPERTY;
                return;
            }
   }
    va=driver->getRemoteVariables("VA");
    vb=driver->getRemoteVariables("VB");
    vc=driver->getRemoteVariables("VC");;
    vd=driver->getRemoteVariables("VD");;
    va_acq=driver->getRemoteVariables("VA_ACQ");
    vb_acq=driver->getRemoteVariables("VB_ACQ");
    vc_acq=driver->getRemoteVariables("VC_ACQ");;
    vd_acq=driver->getRemoteVariables("VD_ACQ");; 
    x_acq=driver->getRemoteVariables("X_ACQ");
    y_acq=driver->getRemoteVariables("Y_ACQ");
    x=driver->getRemoteVariables("X");
    y=driver->getRemoteVariables("Y");
    mode=driver->getRemoteVariables("MODE");
    acquire=driver->getRemoteVariables("ACQUISITION");
    samples=driver->getRemoteVariables("SAMPLES");
    poly_type=driver->getRemoteVariables("POLYTYPE");
    if((va.size()==vb.size())&&(vb.size()==vc.size())&&(vc.size()==vd.size())&&(vd.size()==mode.size())&&(mode.size()==acquire.size())&&(acquire.size()==samples.size())){
        CTRLDBG_<<" Array BPM size:"<<va.size();
        elem_size = va.size();
    } else {
        CTRLERR_<<"Different array size, check driver input parameters: VA:"<<va.size()<<" VB:"<<vb.size()<<" VC:"<<vc.size()<<" VD:"<<vd.size()<<" MODE:"<<mode.size()<<" acquire:"<<acquire.size()<<" samples:"<<samples.size();
        chaos::CException(-1,"## bad array sizes ",__PRETTY_FUNCTION__);
    }
    
   
   for(int cnt=0;cnt<elem_size;cnt++){
       mode_sync.add(mode[cnt]);
   }
    CTRLDBG_<<" WAITING for mode:"<<tomode;
    mode_sync.setTimeout(10000000);
    mode_sync.sync(tomode);
    CTRLDBG_<<" EXITING from waiting mode:"<<tomode;
    rattrs= driver->getRemoteVariables();

}

void CmdAcquireDafneAccumulatorBPM::acquireHandler() {
    uint64_t acquire_v,mt_v;
    int32_t mode_v,samples_v;
    try {
    mode_v= *mode[0];
    samples_v = *samples[0];
    acquire_v = *acquire[0];
    int cntt=0;
    int cnt=0;
   
 
    for(cnt=0;cnt<elem_size;cnt++){
          bpmpos mm; 
          int32_t a,b,c,d;
          uint32_t size;
          int poly;
          a= *va[cnt];
          b= *vb[cnt];
          c= *vc[cnt];
          d= *vd[cnt];
          poly=*poly_type[cnt];
          mm=bpm_voltage_to_mm(poly,a,b,c,d);
          *x[cnt]=mm.x;
          *x[cnt]=mm.y;
          if(mode_v&LIBERA_IOP_MODE_DD){
              
              int32_t *vva_acq=(int32_t*) *va_acq[cnt];
              int32_t *vvb_acq=(int32_t*) *vb_acq[cnt];
              int32_t *vvc_acq=(int32_t*) *vc_acq[cnt];
              int32_t *vvd_acq=(int32_t*) *vd_acq[cnt];
              double *vx_acq=(double*) *x_acq[cnt];
              double *vy_acq=(double*) *y_acq[cnt];
              for(int cntt=0;cntt<samples_v;cntt++){
                 mm=bpm_voltage_to_mm(poly,vva_acq[cntt],vvb_acq[cntt],vvc_acq[cntt],vvd_acq[cntt]);
                 vx_acq[cntt]=mm.x;
                 vy_acq[cntt]=mm.y;
              }
          }
          CTRLDBG_<<getAlias()<<" "<<cnt<<"."<<" :"<<va[cnt]->getName()<<" "<<va[cnt]->getInfo().getTimeStamp()<<": ("<<mm.x<<" mm, "<<mm.y<<" mm) Voltages:"<<a <<" "<<b <<" "<<c<<" "<<d;
    }
    cnt=0;
    for (std::vector<ChaosDatasetAttribute*>::iterator i=rattrs.begin();i!=rattrs.end();i++,cnt++){
        
        if((*i)->getDir()==chaos::DataType::Output){
            uint32_t size;
            void*ptr=(*i)->get(&size);
            if((*i)->getType()==chaos::DataType::TYPE_BYTEARRAY){
                getAttributeCache()->setOutputAttributeNewSize(cnt,size);
            }
                    
            getAttributeCache()->setOutputAttributeValue(cnt,ptr,size);
            
        } 
    }
       } catch(chaos::CException e){
        ATTRDBG_<<"%% WARNING "<<e.errorMessage;
    }
 
     if(mode_v==0){
        ATTRDBG_<<"exiting from acquire, by mode =0";
        BC_END_RUNNIG_PROPERTY;
        }
    
     getAttributeCache()->setOutputAttributeValue("MODE",(void*)&mode_v,sizeof(mode_v));
     getAttributeCache()->setOutputAttributeValue("SAMPLES",(void*)&samples_v,sizeof(samples_v));
     getAttributeCache()->setOutputAttributeValue("ACQUISITION",(void*)&acquire_v,sizeof(acquire_v));
     getAttributeCache()->setOutputDomainAsChanged();

}

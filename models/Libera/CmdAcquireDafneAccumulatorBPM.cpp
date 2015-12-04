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
      
    x_acq=driver->getRemoteVariables("X_ACQ");
    y_acq=driver->getRemoteVariables("Y_ACQ");
    mode=driver->getRemoteVariables("MODE");
    acquire=driver->getRemoteVariables("ACQUISITION");
    samples=driver->getRemoteVariables("SAMPLES");
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
 
    /*
    for(int cnt=0,cntt=0;cnt<elem_size;cnt++,cntt+=4){
          bpmpos mm; 
          int32_t a,b,c,d;
          libera_dd_t*buf;
          uint32_t size;
            a= *va[cnt];
            b= *vb[cnt];
            c= *vc[cnt];
            d= *vd[cnt];
            mm=bpm_voltage_to_mm((cnt>3)?1:0,a,b,c,d);

            CTRLDBG_<<getAlias()<<" "<<cnt<<"."<<cntt<<" :"<<va[cnt]->getName()<<" "<<va[cnt]->getInfo().getTimeStamp()<<": ("<<mm.x<<" mm, "<<mm.y<<" mm) Voltages:"<<a <<" "<<b <<" "<<c<<" "<<d;
                
            getAttributeCache()->setOutputAttributeValue(cntt,(void*)&mm.x,sizeof(double));
            getAttributeCache()->setOutputAttributeValue(cntt+1,(void*)&mm.y,sizeof(double));
            if((buf=(libera_dd_t*)dd[cnt]->get(&size))&& (size>sizeof(libera_dd_t))){
                int elems=size/sizeof(libera_dd_t);
                double x[elems];
                double y[elems];
                getAttributeCache()->setOutputAttributeNewSize("DoubleTest",elems*sizeof(double));
              //  double*tst= getAttributeCache()->getRWPtr<double>(DOMAIN_OUTPUT, "DoubleTest");

                getAttributeCache()->setOutputAttributeNewSize(cntt+2,elems*sizeof(double));
                getAttributeCache()->setOutputAttributeNewSize(cntt+3,elems*sizeof(double));
                for(int c=0;c<elems;c++){
                    mm= bpm_voltage_to_mm((cnt>3)?1:0,buf[c].Va,buf[c].Vb,buf[c].Vc,buf[c].Vd);
                    x[c]=mm.x;
                    y[c]=mm.y;
                 //   tst[c]=mm.y;
                }
               getAttributeCache()->setOutputAttributeValue(cntt+2,(void*)x,elems*sizeof(double));
               getAttributeCache()->setOutputAttributeValue(cntt+3,(void*)y,elems*sizeof(double));
            }
            
    }
    */
    if(mode_v==0){
            ATTRDBG_<<"exiting from acquire, by mode =0";
           BC_END_RUNNIG_PROPERTY;
    }
    
     getAttributeCache()->setOutputAttributeValue("MODE",(void*)&mode_v,sizeof(mode_v));
     getAttributeCache()->setOutputAttributeValue("SAMPLES",(void*)&samples_v,sizeof(samples_v));
     getAttributeCache()->setOutputAttributeValue("ACQUISITION",(void*)&acquire_v,sizeof(acquire_v));
     getAttributeCache()->setOutputDomainAsChanged();

}

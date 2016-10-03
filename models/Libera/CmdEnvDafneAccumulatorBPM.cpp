/* 
 * File:   CmdEnvDafneAccumulatorBPM.cpp
 * Author: michelo
 * 
 * Created on October 15, 2015, 10:21 AM
 */

#include "CmdEnvDafneAccumulatorBPM.h"
#include "LiberaData.h"
using namespace ::driver::daq::libera;
using namespace ::driver::misc;
BATCH_COMMAND_OPEN_DESCRIPTION_ALIAS(driver::daq::libera::,CmdEnvDafneAccumulatorBPM,"env","env command","62882f3e-37db-11e5-985f-334fcd6dff22")

BATCH_COMMAND_CLOSE_DESCRIPTION()

CmdEnvDafneAccumulatorBPM::CmdEnvDafneAccumulatorBPM() {
    
}



CmdEnvDafneAccumulatorBPM::~CmdEnvDafneAccumulatorBPM() {
}

uint8_t CmdEnvDafneAccumulatorBPM::implementedHandler(){
       return chaos::common::batch_command::HandlerType::HT_Set  | chaos::common::batch_command::HandlerType::HT_Acquisition;

}
void  CmdEnvDafneAccumulatorBPM::setHandler(c_data::CDataWrapper *data){
  ::driver::misc::CmdSync::setHandler(data);
    va=driver->getRemoteVariables("VA");
    vb=driver->getRemoteVariables("VB");
    vc=driver->getRemoteVariables("VC");;
    vd=driver->getRemoteVariables("VD");;
      
    dd=driver->getRemoteVariables("DD");
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
}

void CmdEnvDafneAccumulatorBPM::acquireHandler() {
    uint64_t acquire_v,mt_v;
    int32_t mode_v,samples_v;
    mode_v= *mode[0];
    samples_v = *samples[0];
    acquire_v = *acquire[0];
    int cntt=0;
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
                getAttributeCache()->setOutputAttributeNewSize(cntt+2,elems*sizeof(double));
                getAttributeCache()->setOutputAttributeNewSize(cntt+3,elems*sizeof(double));
                for(int c=0;c<elems;c++){
                    mm= bpm_voltage_to_mm((cnt>3)?1:0,buf[c].Va,buf[c].Vb,buf[c].Vc,buf[c].Vd);
                    x[c]=mm.x;
                    y[c]=mm.y;
                }
               getAttributeCache()->setOutputAttributeValue(cntt+2,(void*)x,elems*sizeof(double));
               getAttributeCache()->setOutputAttributeValue(cntt+3,(void*)y,elems*sizeof(double));
            }
            
    }
    
     getAttributeCache()->setOutputAttributeValue("MODE",(void*)&mode_v,sizeof(mode_v));
     getAttributeCache()->setOutputAttributeValue("SAMPLES",(void*)&samples_v,sizeof(samples_v));
     getAttributeCache()->setOutputAttributeValue("ACQUISITION",(void*)&acquire_v,sizeof(acquire_v));
     getAttributeCache()->setOutputDomainAsChanged();

}

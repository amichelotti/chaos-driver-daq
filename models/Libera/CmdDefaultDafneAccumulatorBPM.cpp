/* 
 * File:   CmdDefaultDafneAccumulatorBPM.cpp
 * Author: michelo
 * 
 * Created on October 15, 2015, 10:21 AM
 */

#include "CmdDefaultDafneAccumulatorBPM.h"
#include "LiberaData.h"
using namespace ::driver::daq::libera;
using namespace ::driver::misc;
BATCH_COMMAND_OPEN_DESCRIPTION_ALIAS(driver::daq::libera::,CmdDefaultDafneAccumulatorBPM,"default","devault command","62882f3e-36db-11e5-985f-334fcd6dff22")

BATCH_COMMAND_CLOSE_DESCRIPTION()

CmdDefaultDafneAccumulatorBPM::CmdDefaultDafneAccumulatorBPM() {
    
}


CmdDefaultDafneAccumulatorBPM::~CmdDefaultDafneAccumulatorBPM() {
}

uint8_t CmdDefaultDafneAccumulatorBPM::implementedHandler(){
       return chaos::common::batch_command::HandlerType::HT_Set  | chaos::common::batch_command::HandlerType::HT_Acquisition;

}
void  CmdDefaultDafneAccumulatorBPM::setHandler(c_data::CDataWrapper *data){
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
    BC_NORMAL_RUNNING_PROPERTY;
 rattrs= driver->getRemoteVariables("X_ACQ");
        std::vector<ChaosDatasetAttribute*> rattrs_y=driver->getRemoteVariables("Y_ACQ");
        std::vector<ChaosDatasetAttribute*> rattrs_xx=driver->getRemoteVariables("X");
        std::vector<ChaosDatasetAttribute*> rattrs_yy=driver->getRemoteVariables("Y");
        rattrs.insert(rattrs.end(),rattrs_y.begin(),rattrs_y.end());
        rattrs.insert(rattrs.end(),rattrs_xx.begin(),rattrs_xx.end());
        rattrs.insert(rattrs.end(),rattrs_yy.begin(),rattrs_yy.end());
}

void CmdDefaultDafneAccumulatorBPM::acquireHandler() {
    uint64_t acquire_v,mt_v;
    int32_t mode_v,samples_v;
    mode_v= *mode[0];
    samples_v = *samples[0];
    acquire_v = *acquire[0];
    
    try {
    mode_v= *mode[0];
    samples_v = *samples[0];
    acquire_v = *acquire[0];
    int cntt=0;
    
    
    int cnt=0;
     ATTRDBG_<<"current mode:"<<mode_v;
    for (std::vector<ChaosDatasetAttribute*>::iterator i=rattrs.begin();i!=rattrs.end();i++){
        
        if((*i)->getDir()==chaos::DataType::Output){
            uint32_t size;
            void*ptr=(*i)->get(&size);
            if((*i)->getType()==chaos::DataType::TYPE_BYTEARRAY){
                getAttributeCache()->setOutputAttributeNewSize(cnt,size);
            }
          
        
            getAttributeCache()->setOutputAttributeValue(cnt,ptr,size);
            cnt++;
        } 
    }
       } catch(chaos::CException e){
        ATTRDBG_<<"%% WARNING "<<e.errorMessage;
    }
 
    /*int cntt=0;
    
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
    */
     getAttributeCache()->setOutputAttributeValue("MODE",(void*)&mode_v,sizeof(mode_v));
     getAttributeCache()->setOutputAttributeValue("SAMPLES",(void*)&samples_v,sizeof(samples_v));
     getAttributeCache()->setOutputAttributeValue("ACQUISITION",(void*)&acquire_v,sizeof(acquire_v));
     getAttributeCache()->setOutputDomainAsChanged();
     

}

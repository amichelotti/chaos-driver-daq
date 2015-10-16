/* 
 * File:   CmdAcquireDafneAccumulatorBPM.cpp
 * Author: michelo
 * 
 * Created on October 15, 2015, 10:21 AM
 */

#include "CmdAcquireDafneAccumulatorBPM.h"
#include "LiberaData.h"
using namespace driver::daq::libera;
using namespace driver::misc;
struct bpmpos {
    double x;
    double y;
};

static bpmpos bpm_voltage_to_mm(uint32_t type,int32_t va,int32_t vb,int32_t vc,int32_t vd){
    bpmpos pos;
    double x=0,y=0;
    if((va +vb +vc+vd)==0)return pos;
    double U= ((double)(vb +vd -va -vc))/(va +vb +vc+vd);
    double V= ((double)(va +vb -vc -vd))/(va +vb +vc+vd);
    double a[2][6]={{28.5574,-0.046125,5.43125e-5,0.0172085,-1.15991e-5,1.94837e-7},{9.8435,-0.022408,0.034859,-1.4584e-6,-9.9279e-6}};
    double b[2][6]={{28.5574,-0.0172085,1.94837e-7,-0.046125,-1.15991e-5,5.43125e-5},{32.0137,0.0432143,0.000222447,-0.000318269,0.00167884}};
    if(type>1){
        return pos;
    }
    for(int cnt=0;cnt<7;cnt++){
        x = a[type][0] * U + a[type][1] * pow(y,2)*U +  a[type][2]*pow(y,4)*U + a[type][3] *pow(x,2)*U +a[type][4]*pow(x,2)*pow(y,2)*U+a[type][5]*pow(x,4)*U;
        y = b[type][1] * V + b[type][1] * pow(y,2)*V +  b[type][2]*pow(y,4)*V + b[type][3] *pow(x,2)*V +b[type][4]*pow(x,2)*pow(y,2)*V+b[type][5]*pow(x,4)*V;
    }
    
    /*MATLAB*/
    /*Xs=0;
Ys=0;

for i=1:7

x=a(1)*U+a(2)*Ys^2*U+a(3)*Ys^4*U+a(4)*Xs^2*U+a(5)*Xs^2*Ys^2*U+a(6)*Xs^4*U;
y=b(1)*V+b(2)*Ys^2*V+b(3)*Ys^4*V+b(4)*Xs^2*V+b(5)*Xs^2*Ys^2*V+b(6)*Xs^4*V;

Xs=x;
Ys=y;

end*/
    

    pos.x=x;
    pos.y=y;
    return pos;
}

CmdAcquireDafneAccumulatorBPM::CmdAcquireDafneAccumulatorBPM() {
    
}



CmdAcquireDafneAccumulatorBPM::~CmdAcquireDafneAccumulatorBPM() {
}

uint8_t CmdAcquireDafneAccumulatorBPM::implementedHandler(){
       return chaos::common::batch_command::HandlerType::HT_Set  | chaos::common::batch_command::HandlerType::HT_Acquisition;

}
void  CmdAcquireDafneAccumulatorBPM::setHandler(c_data::CDataWrapper *data){
    driver::misc::CmdSync::setHandler(data);
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

void CmdAcquireDafneAccumulatorBPM::acquireHandler() {
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
     getAttributeCache()->setOutputAttributeValue("ACQUIRE",(void*)&acquire_v,sizeof(acquire_v));
     getAttributeCache()->setOutputDomainAsChanged();

}

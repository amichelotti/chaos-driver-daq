/* 
 * File:   ChaosController.cpp
 * Author: michelo
 * 
 * Created on September 2, 2015, 5:29 PM
 */

#include "ChaosControllerLibera.h"
#include "LiberaData.h"

void ChaosControllerLibera::setCommandBlocking(bool t){
    blocking = t;
}
    
int ChaosControllerLibera::acquire_disable(){
    command_t c = prepareCommand("acquire");
    c->addParameter("enable",0);
    return executeCmd(c,blocking);
}

int ChaosControllerLibera::acquire_dd(int32_t samples,int32_t loop,uint64_t wait_for,bool triggered){
    int32_t mode=(triggered)?LIBERA_IOP_MODE_TRIGGERED:0;
    
    mode |= LIBERA_IOP_MODE_DD ;
    command_t c = prepareCommand("acquire");
    c->addParameter("enable",1);
    c->addParameter("mode",mode);
    c->addParameter("samples",samples);
    c->addParameter("loops",loop);
    c->scheduler_steps_delay=100;
   
    return executeCmd(c,blocking,0,wait_for);
    
}
int ChaosControllerLibera::acquire_sa(int32_t samples,int32_t loop,uint64_t wait_for,bool triggered){
    int32_t mode=(triggered)?LIBERA_IOP_MODE_TRIGGERED:0;
    mode |= LIBERA_IOP_MODE_SA;
    command_t c = prepareCommand("acquire");
    c->addParameter("enable",1);
    c->addParameter("mode",mode);
    c->addParameter("samples",samples);
    c->addParameter("loops",loop);
     c->scheduler_steps_delay=100;
   
    return executeCmd(c,blocking,0,wait_for);
   
}
int ChaosControllerLibera::acquire_adc_cw(int32_t samples,int32_t loop,uint64_t wait_for,bool triggered){
    int32_t mode=(triggered)?LIBERA_IOP_MODE_TRIGGERED:0;
    mode |= LIBERA_IOP_MODE_CONTINUOUS;
    command_t c = prepareCommand("acquire");
    c->addParameter("enable",1);
    c->addParameter("mode",mode);
    c->addParameter("samples",samples);
    c->addParameter("loops",loop);

    return executeCmd(c,blocking,0,wait_for);
   
}

int ChaosControllerLibera::acquire_adc_sp(int32_t samples,int32_t loop,uint64_t wait_for,bool triggered){
    int32_t mode=(triggered)?LIBERA_IOP_MODE_TRIGGERED:0;
    mode |= LIBERA_IOP_MODE_SINGLEPASS;
    command_t c = prepareCommand("acquire");
    c->addParameter("enable",1);
    c->addParameter("mode",mode);
    c->addParameter("samples",samples);
    c->addParameter("loops",loop);

    return executeCmd(c,blocking,0,wait_for);
   
}
int ChaosControllerLibera::acquire_avg(int32_t samples,int32_t loop,uint64_t wait_for,bool triggered){
    int32_t mode=(triggered)?LIBERA_IOP_MODE_TRIGGERED:0;
    mode |= LIBERA_IOP_MODE_AVG;
    command_t c = prepareCommand("acquire");
    c->addParameter("enable",1);
    c->addParameter("mode",mode);
    c->addParameter("samples",samples);
    c->addParameter("loops",loop);

    return executeCmd(c,blocking,0,wait_for);
}
int ChaosControllerLibera::setDate(std::string dat){
    command_t c = prepareCommand("time");
    c->addParameter("time",dat);
    return executeCmd(c,blocking);
}
int ChaosControllerLibera::setEnviron(std::string param,int32_t val){
    command_t c = prepareCommand("env");
    c->addParameter(param,val);
    return executeCmd(c,blocking);
}
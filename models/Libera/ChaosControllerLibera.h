/* 
 * File:   ChaosController.h
 * Author: michelo
 *
 * Created on September 2, 2015, 5:29 PM
 */

#ifndef ChaosControllerLibera_H
#define	ChaosControllerLibera_H
#include <map>
#include <string>
#include <chaos/ui_toolkit/ChaosUIToolkit.h>
#include <boost/shared_ptr.hpp>
#include <chaos/ui_toolkit/HighLevelApi/DeviceController.h>
#include <driver/misc/ChaosController.h>

#define CTRLAPP_ LAPP_ << "[ "<<__FUNCTION__<<" ] ["<<getPath()<<"] "
#define CTRLDBG_ LDBG_<< "[ "<<__FUNCTION__<<" ] ["<<getPath()<<"] "
#define CTRLERR_ LERR_ << "[ "<<__FUNCTION__<<" ] ["<<getPath()<<"] "


class ChaosControllerLibera:public ChaosController{
    
    private:
        bool blocking; 
    public:
    ChaosControllerLibera(const char* path,uint32_t timeo=5000):ChaosController( path,timeo){}

    void setCommandBlocking(bool t);
    
    int acquire_disable();
    int acquire_dd(int32_t samples,int32_t loop,bool triggered=0);
    int acquire_sa(int32_t samples,int32_t loop,bool triggered=0);
    int acquire_adc_cw(int32_t samples,int32_t loop,bool triggered=0);
    int acquire_adc_sp(int32_t samples,int32_t loop,bool triggered=0);

    int acquire_avg(int32_t samples,int32_t loop,bool triggered=0);
    int setDate(std::string dat);
    int setEnviron(std::string param,int32_t val);
    
};

#endif	/* ChaosController_H */


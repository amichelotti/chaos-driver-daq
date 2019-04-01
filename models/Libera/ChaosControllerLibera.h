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
#include <boost/shared_ptr.hpp>
#include <driver/misc/core/ChaosController.h>


namespace driver{
    
    namespace daq{
        namespace libera{
	  class ChaosControllerLibera:public ::driver::misc::ChaosController{
    
    private:
        bool blocking; 
    public:
        ChaosControllerLibera():ChaosController(){blocking =true;}
    ChaosControllerLibera(std::string path,uint32_t timeo=DEFAULT_TIMEOUT_FOR_CONTROLLER):ChaosController( path,timeo){blocking=true;}

    void setCommandBlocking(bool t);
    
    int acquire_disable();
    int acquire_dd(int32_t samples,int32_t loop,uint64_t wait_for=0,bool triggered=false);
    int acquire_sa(int32_t samples,int32_t loop,uint64_t wait_for=0,bool triggered=false);
    int acquire_adc_cw(int32_t samples,int32_t loop,uint64_t wait_for=0,bool triggered=false);
    int acquire_adc_sp(int32_t samples,int32_t loop,uint64_t wait_for=0,bool triggered=false);

    int acquire_avg(int32_t samples,int32_t loop,uint64_t wait_for=0,bool triggered=false);
    int setDate(std::string dat);
    int setEnviron(std::string param,int32_t val);
    
};
        }}}
#endif	/* ChaosController_H */


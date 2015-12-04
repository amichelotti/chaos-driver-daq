//
//  CmdLiberaAcquire.cpp
//  
//
//  Created by Andrea Michelotti 2015
//  Copyright (c) 2013 infn. All rights reserved.
//

#include "CmdLiberaAcquire.h"

#include <boost/format.hpp>


namespace c_data = chaos::common::data;
namespace chaos_batch = chaos::common::batch_command;

// return the implemented handler
/*
uint8_t own::CmdLiberaAcquire::implementedHandler() {
    return	AbstractPowerSupplyCommand::implementedHandler();
}
*/

/* MODES */
/*
#define LIBERA_IOP_MODE_DD 0x1 // data acquire on demand
#define LIBERA_IOP_MODE_SS 0x2 // streaming data acquire
#define LIBERA_IOP_MODE_ADC 0x4 // ADC data acquire
#define LIBERA_IOP_MODE_PM 0x8 // Post Mortem data acquire
#define LIBERA_IOP_MODE_AVG 0x10 // Average data acquire

#define LIBERA_IOP_MODE_TRIGGERED 0x100
#define LIBERA_IOP_MODE_DECIMATED 0x200
#define LIBERA_IOP_MODE_CONTINUOUS 0x400
#define LIBERA_IOP_MODE_SINGLEPASS 0x800
*/

// command syntax enable, mode, samples, loops
// {"acquire","enable:1","mode:<bit ored>","samples:XX","loops:YY","offset:HH","duration:SS"}
// mode: <> is required
// loops:<0 means loop forever

using namespace driver::daq::libera;
BATCH_COMMAND_OPEN_DESCRIPTION_ALIAS(,CmdLiberaAcquire,"acquire","acquire command","72882f3e-36db-11e6-985f-334fcd6dff22")
BATCH_COMMAND_ADD_INT32_PARAM("enable", "enable acquisition =1, disable =0",chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_MANDATORY)
BATCH_COMMAND_ADD_INT32_PARAM("mode", "acquisition modes, =1 SlowAcquisition, =2 Data on Demand, add 0x100 for DD triggering,3=Post Mortem,4=ADC",chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_MANDATORY)
BATCH_COMMAND_ADD_INT32_PARAM("samples", "in DataOnDemand number of samples",chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_OPTIONAL)
BATCH_COMMAND_ADD_INT32_PARAM("loops", "acquisition loops, -1 means continuos, to break launch a acquire command with enable=0",chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_OPTIONAL)

BATCH_COMMAND_CLOSE_DESCRIPTION()

driver::daq::libera::CmdLiberaAcquire::CmdLiberaAcquire():CmdLiberaDefault(){
}
driver::daq::libera::CmdLiberaAcquire::~CmdLiberaAcquire(){
}
void driver::daq::libera::CmdLiberaAcquire::setHandler(c_data::CDataWrapper *data) {
	CMDCUDBG_ << "Executing acquire set handler:"<<data->getJSONString();
        int tsamples=-1,toffset=-1,tmode=-1;
	int ret;
        mode =0;
        offset =0;
        loops=1;
        acquire_duration=0;
        wait_for_us=0;
        CmdLiberaDefault::setHandler(data);
	clearFeatures(chaos_batch::features::FeaturesFlagTypes::FF_SET_SCHEDULER_DELAY);
	setFeatures(chaos_batch::features::FeaturesFlagTypes::FF_SET_SCHEDULER_DELAY, (uint64_t)100000);
        	


        perr=getAttributeCache()->getRWPtr<int32_t>(DOMAIN_OUTPUT, "error");
        *perr=0;
        if((ret=driver->iop(LIBERA_IOP_CMD_STOP,0,0))!=0){
            *perr|=LIBERA_ERROR_STOP_ACQUIRE;
           
            getAttributeCache()->setOutputDomainAsChanged();
            
            BC_END_RUNNIG_PROPERTY;
            throw chaos::CException(ret, "Cannot stop acquire", __FUNCTION__);

        }
	//requested mode
	if(data->hasKey("enable")) {
            if(data->getInt32Value("enable")==0){
                loops=0;
             	CMDCUDBG_ << "Disable acquire";
                pmode=getAttributeCache()->getRWPtr<int32_t>(DOMAIN_OUTPUT, "MODE");
                *pmode=0;
                getAttributeCache()->setOutputDomainAsChanged();

                BC_END_RUNNIG_PROPERTY;
                return;
            }
	}
        
        if(data->hasKey("duration")) {
            acquire_duration=data->getInt32Value("duration");
            CMDCUDBG_ <<"setting acquire time :"<<acquire_duration << " sec";
	}
        
        if(!data->hasKey("mode")) {
            throw chaos::CException(1, "You have to specify a mode", __FUNCTION__);
        } else {
            tmode = data->getInt32Value("mode");
        }
        
        if(mode&LIBERA_IOP_MODE_SA){
            loops=-1;
        }
        if(data->hasKey("samples")) {
            tsamples = std::min(data->getInt32Value("samples"),64000); 
	}
        
         if(data->hasKey("offset")) {
            toffset = data->getInt32Value("offset");
         }
	
        if(data->hasKey("loops")) {
            loops = data->getInt32Value("loops");
         }	
        if(data->hasKey("wait_for")) {
            wait_for_us = data->getInt64Value("wait_for");
         }
         //getAttributeCache()->setOutputAttributeNewSize("SA", 0);
         //getAttributeCache()->setOutputAttributeNewSize("DD", 0);
        getAttributeCache()->setOutputAttributeNewSize("VA_ACQ", 0);
        getAttributeCache()->setOutputAttributeNewSize("VB_ACQ", 0);
        getAttributeCache()->setOutputAttributeNewSize("VC_ACQ", 0);
        getAttributeCache()->setOutputAttributeNewSize("VD_ACQ", 0);
        getAttributeCache()->setOutputAttributeNewSize("X_ACQ", 0);
        getAttributeCache()->setOutputAttributeNewSize("Y_ACQ", 0);
        
         getAttributeCache()->setOutputAttributeNewSize("ADC_CW", 0);
         getAttributeCache()->setOutputAttributeNewSize("ADC_SP", 0);

         if(tmode&LIBERA_IOP_MODE_DD){
             if(tsamples>0){
                //getAttributeCache()->setOutputAttributeNewSize("DD", tsamples*sizeof(libera_dd_t));
               // getAttributeCache()->setOutputAttributeNewSize("DD", tsamples*sizeof(libera_dd_t));
                getAttributeCache()->setOutputAttributeNewSize("VA_ACQ", tsamples*sizeof(int32_t));
                getAttributeCache()->setOutputAttributeNewSize("VB_ACQ", tsamples*sizeof(int32_t));
                getAttributeCache()->setOutputAttributeNewSize("VC_ACQ", tsamples*sizeof(int32_t));
                getAttributeCache()->setOutputAttributeNewSize("VD_ACQ", tsamples*sizeof(int32_t));
                getAttributeCache()->setOutputAttributeNewSize("X_ACQ", tsamples*sizeof(double));
                getAttributeCache()->setOutputAttributeNewSize("Y_ACQ", tsamples*sizeof(double));
                driver->iop(LIBERA_IOP_CMD_SET_SAMPLES,(void*)&tsamples,0);
                samples=tsamples;

             }
            } else if (tmode&LIBERA_IOP_MODE_SA){
                if(tsamples>0){
                   // getAttributeCache()->setOutputAttributeNewSize("SA", tsamples*sizeof(libera_sa_t));
                    driver->iop(LIBERA_IOP_CMD_SET_SAMPLES,(void*)&tsamples,0);
                    samples=tsamples;

                }
            } else if (tmode&LIBERA_IOP_MODE_CONTINUOUS){
                if(tsamples>0){
                    getAttributeCache()->setOutputAttributeNewSize("ADC_CW", tsamples*sizeof(libera_cw_t));
                    driver->iop(LIBERA_IOP_CMD_SET_SAMPLES,(void*)&tsamples,0);
                    samples=tsamples;

                }
            } else if (tmode&LIBERA_IOP_MODE_SINGLEPASS){
                if(tsamples>0){
                    getAttributeCache()->setOutputAttributeNewSize("ADC_SP", tsamples*sizeof(libera_sp_t));
                    driver->iop(LIBERA_IOP_CMD_SET_SAMPLES,(void*)&tsamples,0);
                    samples=tsamples;

                }
            } else if (tmode&LIBERA_IOP_MODE_AVG){
                  if(tsamples>0){
                        getAttributeCache()->setOutputAttributeNewSize("ADC_AVG", tsamples*sizeof(libera_avg_t));
                       driver->iop(LIBERA_IOP_CMD_SET_SAMPLES,(void*)&tsamples,0);
                       samples=tsamples;
               }
            } else {
              *perr|=LIBERA_ERROR_SWCONFIG;
              getAttributeCache()->setOutputDomainAsChanged();
              BC_END_RUNNIG_PROPERTY
              throw chaos::CException(1, "Unsupported mode", __FUNCTION__);

            }
        
        if(toffset>0){
           driver->iop(LIBERA_IOP_CMD_SET_OFFSET,(void*)&toffset,0);
           offset = toffset;
        }
       
        
        if((ret=driver->iop(LIBERA_IOP_CMD_ACQUIRE,(void*)&tmode,0))!=0){
            BC_END_RUNNIG_PROPERTY
            throw chaos::CException(ret, "Cannot start acquire", __FUNCTION__);

        }
        
         type=*getAttributeCache()->getROPtr<int32_t>(DOMAIN_INPUT, "POLYTYPE");
         mode = tmode;
         va = getAttributeCache()->getRWPtr<int32_t>(DOMAIN_OUTPUT, "VA");
         vb = getAttributeCache()->getRWPtr<int32_t>(DOMAIN_OUTPUT, "VB");
         vc = getAttributeCache()->getRWPtr<int32_t>(DOMAIN_OUTPUT, "VC");
         vd = getAttributeCache()->getRWPtr<int32_t>(DOMAIN_OUTPUT, "VD");
         x = getAttributeCache()->getRWPtr<int32_t>(DOMAIN_OUTPUT, "X");
         y = getAttributeCache()->getRWPtr<int32_t>(DOMAIN_OUTPUT, "Y");
         q = getAttributeCache()->getRWPtr<int32_t>(DOMAIN_OUTPUT, "Q");
         sum = getAttributeCache()->getRWPtr<int32_t>(DOMAIN_OUTPUT, "SUM");
         q1 = getAttributeCache()->getRWPtr<int32_t>(DOMAIN_OUTPUT, "Q1");
         q2 = getAttributeCache()->getRWPtr<int32_t>(DOMAIN_OUTPUT, "Q2");
         psamples=getAttributeCache()->getRWPtr<int32_t>(DOMAIN_OUTPUT, "SAMPLES");
         pmode=getAttributeCache()->getRWPtr<int32_t>(DOMAIN_OUTPUT, "MODE");
        mt=getAttributeCache()->getRWPtr<uint64_t>(DOMAIN_OUTPUT, "MT");
         st=getAttributeCache()->getRWPtr<uint64_t>(DOMAIN_OUTPUT, "ST");
        va_acq=getAttributeCache()->getRWPtr<int32_t>(DOMAIN_OUTPUT, "VA_ACQ");
        vb_acq=getAttributeCache()->getRWPtr<int32_t>(DOMAIN_OUTPUT, "VB_ACQ");
        vc_acq=getAttributeCache()->getRWPtr<int32_t>(DOMAIN_OUTPUT, "VC_ACQ");
        vd_acq=getAttributeCache()->getRWPtr<int32_t>(DOMAIN_OUTPUT, "VD_ACQ");
        x_acq=getAttributeCache()->getRWPtr<double>(DOMAIN_OUTPUT, "X_ACQ");
        y_acq=getAttributeCache()->getRWPtr<double>(DOMAIN_OUTPUT, "Y_ACQ");
         acquire_loops = getAttributeCache()->getRWPtr<int64_t>(DOMAIN_OUTPUT, "ACQUISITION");
         if(mode&LIBERA_IOP_MODE_PERMLOOP){
             loops=-1;
         } else {
            if(loops<0){
                mode|=LIBERA_IOP_MODE_PERMLOOP;
            }
         }
         *pmode=mode;
         *psamples=samples;
         *acquire_loops=0;
         getAttributeCache()->setOutputDomainAsChanged();
        CMDCU_<<" start acquiring mode:"<<mode<<" samples:"<<samples<<" offset:"<<offset<<" loops:"<<loops<<" WAIT COMMAND FOR:"<<wait_for_us;
         boost::posix_time::ptime start_test = boost::posix_time::microsec_clock::local_time();
         
        start_acquire=start_test.time_of_day().total_milliseconds();
        BC_NORMAL_RUNNIG_PROPERTY;
        usleep(wait_for_us);
}

void driver::daq::libera::CmdLiberaAcquire::acquireHandler() {
     boost::posix_time::ptime curr;
     int ret;
     libera_ts_t ts;
    
    if(acquire_duration !=0){
        curr= boost::posix_time::microsec_clock::local_time();
        if((curr.time_of_day().total_milliseconds() - start_acquire) > (acquire_duration*1000)){
            CMDCUDBG_ << "Acquiring time "<<acquire_duration << " expired";
            *pmode=0;
            getAttributeCache()->setOutputDomainAsChanged();
            BC_END_RUNNIG_PROPERTY;
            return;

        }
    }
    
    if(mode&LIBERA_IOP_MODE_DD){
        CMDCUDBG_ << "Acquiring DD";
        if(driver->iop(LIBERA_IOP_CMD_GET_TS,(void*)&ts,sizeof(ts))==0){
            if(mt)
                *mt = ts.mt;
            if(st)
                *st=((uint64_t)ts.st.tv_sec)*1000000ULL + ts.st.tv_nsec/1000;
            
            CMDCUDBG_<<"MT:"<<*mt<<" ST:"<<*st <<" TV_sec:"<<ts.st.tv_sec<<" TV_NSEC:"<<ts.st.tv_nsec;

     }
        libera_dd_t pnt[samples];//=(libera_dd_t*)getAttributeCache()->getRWPtr<int32_t>(DOMAIN_OUTPUT, "DD");
      /*  if(pnt==NULL){
            CMDCUERR_<<"cannot retrieve dataset \"DD\"";
            *pmode=0;
            *perr|=LIBERA_ERROR_ALLOCATE_DATASET;

            getAttributeCache()->setOutputDomainAsChanged();
            BC_END_RUNNIG_PROPERTY;
            return;
        }*/
        
        if((ret=driver->read((void*)pnt,0,samples*sizeof(libera_dd_t)))>=0){
            bpmpos mm;
            *va = pnt[0].Va;
            *vb = pnt[0].Vb;
            *vc = pnt[0].Vc;
            *vd = pnt[0].Vd;
            mm=bpm_voltage_to_mm(type,pnt[0].Va,pnt[0].Vb,pnt[0].Vc,pnt[0].Vd);
            *x  = mm.x;
            *y  = mm.y;
            *q  = pnt[0].Q;
            *sum  = pnt[0].Sum;
            *q1 = 0;
            *q2 = 0;
            for(int cnt=0;cnt<samples;cnt++){
                va_acq[cnt]=pnt[cnt].Va;
                vb_acq[cnt]=pnt[cnt].Vb;
                vc_acq[cnt]=pnt[cnt].Vc;
                vd_acq[cnt]=pnt[cnt].Vd;
                mm=bpm_voltage_to_mm(type,pnt[cnt].Va,pnt[cnt].Vb,pnt[cnt].Vc,pnt[cnt].Vd);
                x_acq[cnt]=mm.x;
                y_acq[cnt]=mm.y;
            }
             CMDCUDBG_ << "DD read [ret="<<std::dec<<ret<<"]:"<<pnt[0];
             (*acquire_loops)++;
        } else {
           *perr|=LIBERA_ERROR_READING;

            CMDCUERR_<<"Error reading DD ret:"<<ret<<", mode:"<<mode<<" samples:"<<samples;
        }
    } else if(mode&LIBERA_IOP_MODE_SA){
        libera_sa_t pnt;//=(libera_sa_t*)getAttributeCache()->getRWPtr<int32_t>(DOMAIN_OUTPUT, "SA");

        if((ret=driver->read((void*)&pnt,0,sizeof(libera_sa_t)))>=0){
            bpmpos mm;

            *va = pnt.Va;
            *vb = pnt.Vb;
            *vc = pnt.Vc;
            *vd = pnt.Vd;
            mm=bpm_voltage_to_mm(type,pnt.Va,pnt.Vb,pnt.Vc,pnt.Vd);
            *x  = mm.x;
            *y  = mm.y;
            *q  = pnt.Q;
            *sum  = pnt.Sum;
            *q1 = pnt.Cx;
            *q2 = pnt.Cy;
             (*acquire_loops)++;
             x_acq[0] = mm.x;
             y_acq[0] = mm.y;
            CMDCUDBG_ << "SA read:"<<pnt;

        } else {
             *perr|=LIBERA_ERROR_READING;

            CMDCUERR_<<"Error reading SA ret:"<<ret<<", mode:"<<mode<<" samples:"<<samples;
        }
    } else if(mode&LIBERA_IOP_MODE_CONTINUOUS){
         libera_cw_t*pnt=(libera_cw_t*)getAttributeCache()->getRWPtr<int32_t>(DOMAIN_OUTPUT, "ADC_CW");

         if(driver->read(pnt,0,samples*sizeof(libera_cw_t))>=0){
              (*acquire_loops)++;
              CMDCUDBG_ << "ADC CW read:"<<pnt[0];

        } else {
            *perr|=LIBERA_ERROR_READING;

            CMDCUERR_<<"Error reading ADC CONTINUOUS, mode:"<<mode<<" samples:"<<samples;
        }
    } else if(mode&LIBERA_IOP_MODE_SINGLEPASS){
        libera_sp_t*pnt = (libera_sp_t*)getAttributeCache()->getRWPtr<int32_t>(DOMAIN_OUTPUT, "ADC_SP");
         if(driver->read(pnt,0,samples*sizeof(libera_sp_t))>=0){
              (*acquire_loops)++;
              CMDCUDBG_ << "ADC SP read:"<<pnt[0];

        } else {
             *perr|=LIBERA_ERROR_READING;

            CMDCUERR_<<"Error reading ADC SINGLE PASS, mode:"<<mode<<" samples:"<<samples;
        }
    } else if(mode&LIBERA_IOP_MODE_AVG){
        libera_avg_t *pnt=(libera_avg_t *)getAttributeCache()->getRWPtr<int32_t>(DOMAIN_OUTPUT, "AVG");
         if(driver->read(pnt,0,samples*sizeof(libera_avg_t))>=0){
           (*acquire_loops)++;
            CMDCUDBG_ << "AVG read:"<<pnt[0];

        } else {
            *perr|=LIBERA_ERROR_READING;

            CMDCUERR_<<"Error reading Average, mode:"<<mode<<" samples:"<<samples;
        }
    }
        
    
    if((loops==0)|| (*pmode==0)){
        int ret;
        CMDCUDBG_ << "Acquiring loop ended after:"<<*acquire_loops<<" acquisitions.";
        if((ret=driver->iop(LIBERA_IOP_CMD_STOP,0,0))!=0){
             *perr|=LIBERA_ERROR_STOP_ACQUIRE;
        }
        *pmode=0;
        getAttributeCache()->setOutputDomainAsChanged();
        BC_END_RUNNIG_PROPERTY;
        return;
    } else if(loops>0){
        loops--;
    }
     
     if(*perr!=0){
       *pmode=0;
        if((ret=driver->iop(LIBERA_IOP_CMD_STOP,0,0))!=0){
             *perr|=LIBERA_ERROR_STOP_ACQUIRE;
        }
       getAttributeCache()->setOutputDomainAsChanged();
       BC_END_RUNNIG_PROPERTY;   
       throw chaos::CException(*perr, "Error Acquiring", __FUNCTION__);
     }
    CMDCUDBG_ << "End Acquiring loop:"<<*acquire_loops;
    getAttributeCache()->setOutputDomainAsChanged();

   
}
//void CmdLiberaAcquire::ccHandler() {
//	AbstractPowerSupplyCommand::ccHandler();
//	
//	BC_EXEC_RUNNIG_PROPERTY
//	CMDCU_ << "Check if we are gone";
//	switch(state_to_go) {
//		case 0://we need to go in stanby
//			if(*o_status_id == common::powersupply::POWER_SUPPLY_STATE_STANDBY) {
//				setWorkState(false);
//				//we are terminated the command
//				CMDCU_ << boost::str( boost::format("State reached %1% [%2%] we end command") % o_status % *o_status_id);
//				BC_END_RUNNIG_PROPERTY
//				return;
//			}
//			break;
//			
//		case 1://we need to go on operational
//			if(*o_status_id == common::powersupply::POWER_SUPPLY_STATE_STANDBY ||
//			   *o_status_id == common::powersupply::POWER_SUPPLY_STATE_ON) {
//				setWorkState(false);
//				//we are terminated the command
//				CMDCU_ << boost::str( boost::format("State reached %1% [%2%] we end command") % o_status % *o_status_id);
//				BC_END_RUNNIG_PROPERTY
//				return;
//			}
//			break;
//	}
//	
//	
//	if(*o_status_id == common::powersupply::POWER_SUPPLY_STATE_ALARM ||
//	   *o_status_id == common::powersupply::POWER_SUPPLY_STATE_ERROR ||
//	   *o_status_id == common::powersupply::POWER_SUPPLY_STATE_UKN ) {
//		setWorkState(false);
//		TROW_ERROR(1, boost::str( boost::format("Bad state got = %1% - [%2%]") % *o_status_id % o_status), std::string(__FUNCTION__))
//	}
//}
//
//bool own::CmdLiberaAcquire::timeoutHandler() {
//	//move the state machine on fault
//	setWorkState(false);
//	TROW_ERROR(1, "Command operation has gone on timeout", std::string(__FUNCTION__))
//	return true;
//}

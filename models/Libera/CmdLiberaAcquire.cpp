//
//  CmdLiberaAcquire.cpp
//  PowerSupply
//
//  Created by Claudio Bisegni on 06/11/13.
//  Copyright (c) 2013 infn. All rights reserved.
//

#include "CmdLiberaAcquire.h"

#include <boost/format.hpp>


#define CMDCU_ LAPP_ << "[CmdLiberaAcquire]"
#define CMDCUDBG_ LDBG_ <<"[CmdLiberaAcquire]"
#define CMDCUERR_ LERR_ <<"[CmdLiberaAcquire]"

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
// {"acquire","enable:1","mode:<bit ored>","samples:XX","loops:YY","offset:HH"}
// mode: <> is required
// loops:<0 means loop forever

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
        CmdLiberaDefault::setHandler(data);
        
        if((ret=driver->iop(LIBERA_IOP_CMD_STOP,0,0))!=0){
            throw chaos::CException(ret, "Cannot stop acquire", __FUNCTION__);

        }
	//requested mode
	if(data->hasKey("enable")) {
            if(data->getInt32Value("enable")==0){
                loops=0;
             	CMDCUDBG_ << "Disable acquire";

                BC_END_RUNNIG_PROPERTY;
                return;
            }
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
        
         if(tmode&LIBERA_IOP_MODE_DD){
             if(tsamples>0){
                getAttributeCache()->setOutputAttributeNewSize("DD", tsamples*sizeof(libera_dd_t));
                driver->iop(LIBERA_IOP_CMD_SET_SAMPLES,(void*)&tsamples,0);
                samples=tsamples;

             }
            } else if (mode&LIBERA_IOP_MODE_SA){
                if(tsamples>0){
                    getAttributeCache()->setOutputAttributeNewSize("SA", tsamples*sizeof(libera_sa_t));
                    driver->iop(LIBERA_IOP_CMD_SET_SAMPLES,(void*)&tsamples,0);
                    samples=tsamples;

                }
            } else if (mode&LIBERA_IOP_MODE_CONTINUOUS){
                if(tsamples>0){
                    getAttributeCache()->setOutputAttributeNewSize("ADC_CW", tsamples*sizeof(libera_cw_t));
                    driver->iop(LIBERA_IOP_CMD_SET_SAMPLES,(void*)&tsamples,0);
                    samples=tsamples;

                }
            } else if (mode&LIBERA_IOP_MODE_SINGLEPASS){
                if(tsamples>0){
                    getAttributeCache()->setOutputAttributeNewSize("ADC_SP", tsamples*sizeof(libera_sp_t));
                    driver->iop(LIBERA_IOP_CMD_SET_SAMPLES,(void*)&tsamples,0);
                    samples=tsamples;

                }
            } else if (mode&LIBERA_IOP_MODE_AVG){
                  if(tsamples>0){
                        getAttributeCache()->setOutputAttributeNewSize("ADC_AVG", tsamples*sizeof(libera_avg_t));
                       driver->iop(LIBERA_IOP_CMD_SET_SAMPLES,(void*)&tsamples,0);
                       samples=tsamples;
               }
            } else {
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
         acquire_loops = getAttributeCache()->getRWPtr<int64_t>(DOMAIN_OUTPUT, "ACQUISITION");
         *pmode=mode;
         *psamples=samples;
         *acquire_loops=0;
         getAttributeCache()->setOutputDomainAsChanged();
        CMDCU_<<" start acquiring mode:"<<mode<<" samples:"<<samples<<" offset:"<<offset<<" loops:"<<loops;
      BC_EXEC_RUNNIG_PROPERTY
}

void driver::daq::libera::CmdLiberaAcquire::acquireHandler() {
    if(mode&LIBERA_IOP_MODE_DD){
        CMDCUDBG_ << "Acquiring DD";
        libera_dd_t*pnt=(libera_dd_t*)getAttributeCache()->getRWPtr<int32_t>(DOMAIN_OUTPUT, "DD");
        if(pnt==NULL){
            CMDCUERR_<<"cannot retrieve dataset \"DD\"";
            return;
        }
        if(driver->read((void*)pnt,0,samples*sizeof(libera_dd_t))>0){
            
            *va = pnt[0].Va;
            *vb = pnt[0].Vb;
            *vc = pnt[0].Vc;
            *vd = pnt[0].Vd;
            *x  = pnt[0].X;
            *y  = pnt[0].Y;
            *q  = pnt[0].Q;
            *sum  = pnt[0].Sum;
            *q1 = 0;
            *q2 = 0;
             CMDCUDBG_ << "read:"<<pnt[0];
             (*acquire_loops)++;
            getAttributeCache()->setOutputDomainAsChanged();
        } else {
            CMDCUERR_<<"Error reading DD, mode:"<<mode<<" samples:"<<samples;
        }
    } else if(mode&LIBERA_IOP_MODE_SA){
        libera_sa_t*pnt=(libera_sa_t*)getAttributeCache()->getRWPtr<int32_t>(DOMAIN_OUTPUT, "SA");

        if(driver->read((void*)pnt,0,samples*sizeof(libera_sa_t))>0){
            *va = pnt[0].Va;
            *vb = pnt[0].Vb;
            *vc = pnt[0].Vc;
            *vd = pnt[0].Vd;
            *x  = pnt[0].X;
            *y  = pnt[0].Y;
            *q  = pnt[0].Q;
            *sum  = pnt[0].Sum;
            *q1 = pnt[0].Cx;
            *q2 = pnt[0].Cy;
            getAttributeCache()->setOutputDomainAsChanged();
        } else {
            CMDCUERR_<<"Error reading SA, mode:"<<mode<<" samples:"<<samples;
        }
    } else if(mode&LIBERA_IOP_MODE_CONTINUOUS){
         if(driver->read(getAttributeCache()->getRWPtr<int32_t>(DOMAIN_OUTPUT, "ADC_CW"),0,samples*sizeof(libera_cw_t))>0){
            getAttributeCache()->setOutputDomainAsChanged();
        } else {
            CMDCUERR_<<"Error reading ADC CONTINUOUS, mode:"<<mode<<" samples:"<<samples;
        }
    } else if(mode&LIBERA_IOP_MODE_SINGLEPASS){
         if(driver->read(getAttributeCache()->getRWPtr<int32_t>(DOMAIN_OUTPUT, "ADC_SP"),0,samples*sizeof(libera_sp_t))>0){
            getAttributeCache()->setOutputDomainAsChanged();
        } else {
            CMDCUERR_<<"Error reading ADC SINGLE PASS, mode:"<<mode<<" samples:"<<samples;
        }
    } else if(mode&LIBERA_IOP_MODE_AVG){
         if(driver->read(getAttributeCache()->getRWPtr<int32_t>(DOMAIN_OUTPUT, "AVG"),0,samples*sizeof(libera_avg_t))>0){
            getAttributeCache()->setOutputDomainAsChanged();
        } else {
            CMDCUERR_<<"Error reading Average, mode:"<<mode<<" samples:"<<samples;
        }
    }
        
    
    if(loops==0){
        int ret;
        CMDCUDBG_ << "Acquiring loop ended after:"<<*acquire_loops<<" acquisitions.";
        if((ret=driver->iop(LIBERA_IOP_CMD_STOP,0,0))!=0){
            throw chaos::CException(ret, "Cannot stop acquire", __FUNCTION__);

        }
        *pmode=0;
        getAttributeCache()->setOutputDomainAsChanged();
        BC_END_RUNNIG_PROPERTY;
        return;
    } else if(loops>0){
        loops--;
    }
    CMDCUDBG_ << "End Acquiring loop:"<<*acquire_loops;

   
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

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
using namespace chaos::cu::control_manager;

BATCH_COMMAND_OPEN_DESCRIPTION_ALIAS(driver::daq::libera::,CmdLiberaAcquire,"acquire","acquire command","72882f3e-36db-11e5-985f-334fcd6dff22")
BATCH_COMMAND_ADD_INT32_PARAM("enable", "enable acquisition =1, disable =0",chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_MANDATORY)
BATCH_COMMAND_ADD_INT32_PARAM("mode", "acquisition modes, =2 SlowAcquisition, =1 Data on Demand (buffer data), add 0x100 for DD triggering,3=Post Mortem,4=ADC",chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_MANDATORY)
BATCH_COMMAND_ADD_INT32_PARAM("samples", "in DataOnDemand number of samples",chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_OPTIONAL)
BATCH_COMMAND_ADD_INT32_PARAM("loops", "acquisition loops, -1 means continuos, to break launch a acquire command with enable=0",chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_OPTIONAL)

BATCH_COMMAND_CLOSE_DESCRIPTION()

driver::daq::libera::CmdLiberaAcquire::CmdLiberaAcquire():CmdLiberaDefault(),buffer(NULL){
}
driver::daq::libera::CmdLiberaAcquire::~CmdLiberaAcquire(){
	if(buffer){
		free(buffer);
	}

}
void driver::daq::libera::CmdLiberaAcquire::setHandler(c_data::CDataWrapper *data) {
	CMDCUDBG_ << "Executing acquire set handler:"<<data->getJSONString();
	int tsamples=-1,toffset=-1,tmode=-1;
	int ret;
	acquire_duration=0;
	wait_for_us=0;
	loop=-1;
	CmdLiberaDefault::setHandler(data);
	
    //clearFeatures(chaos_batch::features::FeaturesFlagTypes::FF_SET_SCHEDULER_DELAY);
    //setFeatures(chaos_batch::features::FeaturesFlagTypes::FF_SET_SCHEDULER_DELAY, (uint64_t)100000);

	setStateVariableSeverity(StateVariableTypeAlarmDEV,"mode_not_reached", chaos::common::alarm::MultiSeverityAlarmLevelClear);
	if((ret=driver->iop(LIBERA_IOP_CMD_STOP,0,0))!=0){
		metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelError,CHAOS_FORMAT("Error STOPPING ACQUIRE Acquire mode %1% samples %2%",%*imode %*isamples ));

    }

	/*if((ret=driver->iop(LIBERA_IOP_CMD_STOP,0,0))!=0){

		setStateVariableSeverity(StateVariableTypeAlarmDEV,"acquire", chaos::common::alarm::MultiSeverityAlarmLevelWarning);
		metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelError,"stop acquire command failed" );

		getAttributeCache()->setOutputDomainAsChanged();
		CMDCUERR_<<"Cannot stop acquire";

		BC_FAULT_RUNNING_PROPERTY;
		return;
	}*/
	//requested mode
	if(data->hasKey("enable")) {
		if(data->getInt32Value("enable")==0){
			loop=0;
			CMDCUDBG_ << "Disable acquire";
			*pmode=0;
			getAttributeCache()->setOutputDomainAsChanged();
			metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelInfo,"disabling acquire" );
			*idd=0;
			*isa=0;
			*odd=0;
			*osa=0;
			getAttributeCache()->setInputDomainAsChanged();

			BC_END_RUNNING_PROPERTY;
			return;
		}
	}

	if(data->hasKey("duration")) {
		acquire_duration=data->getInt32Value("duration");
		CMDCUDBG_ <<"setting acquire time :"<<acquire_duration << " sec";
	}

	if(!data->hasKey("mode")) {
		metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelWarning,"no mode was given" );

		BC_END_RUNNING_PROPERTY;
		return;
	} else {
		tmode = data->getInt32Value("mode");
	}

	if(tmode&LIBERA_IOP_MODE_SA){
		loop=-1;
	}
	if(data->hasKey("samples")) {
		tsamples = std::min(data->getInt32Value("samples"),64000);
		*isamples=tsamples;

	}

	if(data->hasKey("offset")) {
		*ioffset = data->getInt32Value("offset");
	}

	if(data->hasKey("loops")) {
		loop = data->getInt32Value("loops");
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
	getAttributeCache()->setOutputAttributeNewSize("SUM_ACQ", 0);
	getAttributeCache()->setOutputAttributeNewSize("ADC_CW", 0);
	getAttributeCache()->setOutputAttributeNewSize("ADC_SP", 0);

	if(tmode&LIBERA_IOP_MODE_DD){
		if(tsamples>0){
			int ret;
			//getAttributeCache()->setOutputAttributeNewSize("DD", tsamples*sizeof(libera_dd_t));
			// getAttributeCache()->setOutputAttributeNewSize("DD", tsamples*sizeof(libera_dd_t));
			getAttributeCache()->setOutputAttributeNewSize("VA_ACQ", tsamples*sizeof(int32_t));
			getAttributeCache()->setOutputAttributeNewSize("VB_ACQ", tsamples*sizeof(int32_t));
			getAttributeCache()->setOutputAttributeNewSize("VC_ACQ", tsamples*sizeof(int32_t));
			getAttributeCache()->setOutputAttributeNewSize("VD_ACQ", tsamples*sizeof(int32_t));
			getAttributeCache()->setOutputAttributeNewSize("SUM_ACQ", tsamples*sizeof(int32_t));

			getAttributeCache()->setOutputAttributeNewSize("X_ACQ", tsamples*sizeof(double));
			getAttributeCache()->setOutputAttributeNewSize("Y_ACQ", tsamples*sizeof(double));

			if( (ret=driver->iop(LIBERA_IOP_CMD_SET_SAMPLES,(void*)&tsamples,0))!=0){
				BC_FAULT_RUNNING_PROPERTY;
				setStateVariableSeverity(StateVariableTypeAlarmDEV,"mode_not_reached", chaos::common::alarm::MultiSeverityAlarmLevelHigh);
				metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelError,CHAOS_FORMAT("cannot set samples acquire mode %1% samples %2%",%tmode %tsamples));
				return;
			}
			*isamples=tsamples;

		}
	} else if (tmode&LIBERA_IOP_MODE_SA){
		if(tsamples>0){
			int ret;
			// getAttributeCache()->setOutputAttributeNewSize("SA", tsamples*sizeof(libera_sa_t));
			if((ret=driver->iop(LIBERA_IOP_CMD_SET_SAMPLES,(void*)&tsamples,0))!=0){
				BC_FAULT_RUNNING_PROPERTY;
				setStateVariableSeverity(StateVariableTypeAlarmDEV,"mode_not_reached", chaos::common::alarm::MultiSeverityAlarmLevelHigh);
				metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelError,CHAOS_FORMAT("cannot set samples acquire mode %1% samples %2%",%tmode %tsamples));
				return;

			}
			*isamples=tsamples;

		}
	} else if (tmode&LIBERA_IOP_MODE_CONTINUOUS){
		if(tsamples>0){
			getAttributeCache()->setOutputAttributeNewSize("ADC_CW", tsamples*sizeof(libera_cw_t));
			driver->iop(LIBERA_IOP_CMD_SET_SAMPLES,(void*)&tsamples,0);
			*isamples=tsamples;

		}
	} else if (tmode&LIBERA_IOP_MODE_SINGLEPASS){
		if(tsamples>0){
			getAttributeCache()->setOutputAttributeNewSize("ADC_SP", tsamples*sizeof(libera_sp_t));
			driver->iop(LIBERA_IOP_CMD_SET_SAMPLES,(void*)&tsamples,0);
			*isamples=tsamples;

		}
	} else if (tmode&LIBERA_IOP_MODE_AVG){
		if(tsamples>0){
			getAttributeCache()->setOutputAttributeNewSize("ADC_AVG", tsamples*sizeof(libera_avg_t));
			driver->iop(LIBERA_IOP_CMD_SET_SAMPLES,(void*)&tsamples,0);
			*isamples=tsamples;
		}
	} else {

		getAttributeCache()->setOutputDomainAsChanged();
		BC_END_RUNNING_PROPERTY
		setStateVariableSeverity(StateVariableTypeAlarmDEV,"mode_not_reached", chaos::common::alarm::MultiSeverityAlarmLevelHigh);
		metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelError,CHAOS_FORMAT("Unsupported acquire mode %1% samples %2%",%tmode %tsamples));
		//throw chaos::CException(1, "Unsupported mode", __FUNCTION__);

	}

	if(toffset>0){
		driver->iop(LIBERA_IOP_CMD_SET_OFFSET,(void*)&toffset,0);
		*ioffset = toffset;
	}

	*imode=tmode;
	if((ret=driver->iop(LIBERA_IOP_CMD_ACQUIRE,(void*)&tmode,0))!=0){
		BC_FAULT_RUNNING_PROPERTY;
		setStateVariableSeverity(StateVariableTypeAlarmDEV,"mode_not_reached", chaos::common::alarm::MultiSeverityAlarmLevelHigh);

		metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelError,CHAOS_FORMAT("cannot start acquire mode %1% samples %2%",%tmode %tsamples));

		//CMDCUERR_<<"cannot start acquire command, mode "<<tmode<<" samples:"<<tsamples<< " ret:"<<ret;
		//throw chaos::CException(ret, "Cannot start acquire", __FUNCTION__);
		return;

	}


	if(tmode&LIBERA_IOP_MODE_PERMLOOP){
		loop=-1;
	} else {
		if(loop<0){
			tmode|=LIBERA_IOP_MODE_PERMLOOP;
		}
	}
	*pmode=tmode;
	*acquire_loops=0;
	metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelInfo,CHAOS_FORMAT("start acquire mode %1% samples %2%",%*pmode %*isamples ));

	boost::posix_time::ptime start_test = boost::posix_time::microsec_clock::local_time();

	start_acquire=start_test.time_of_day().total_milliseconds();

	*odd=(tmode&LIBERA_IOP_MODE_DD)?true:false;
	*osa=(tmode&LIBERA_IOP_MODE_SA)?true:false;
	*idd=*odd;
	*isa=*osa;

	*itrigger=(tmode&LIBERA_IOP_MODE_TRIGGERED)?true:false;
	if(*itrigger){
		// if triggered free running
		 clearFeatures(chaos_batch::features::FeaturesFlagTypes::FF_SET_SCHEDULER_DELAY);
    	setFeatures(chaos_batch::features::FeaturesFlagTypes::FF_SET_SCHEDULER_DELAY, (uint64_t)1);
	}
	setStateVariableSeverity(StateVariableTypeAlarmDEV,"trigger_timeout", chaos::common::alarm::MultiSeverityAlarmLevelClear);
	setStateVariableSeverity(StateVariableTypeAlarmDEV,"acquire_error", chaos::common::alarm::MultiSeverityAlarmLevelClear);
	getAttributeCache()->setInputDomainAsChanged();
	samples=*isamples;
	/*if(driver->iop(LIBERA_IOP_CMD_GETENV,status,MAX_STRING)!=0){
            CMDCUERR_<<" Cannot retrive STATUS";
    } */
	va_acq=getAttributeCache()->getRWPtr<int32_t>(DOMAIN_OUTPUT, "VA_ACQ");
    vb_acq=getAttributeCache()->getRWPtr<int32_t>(DOMAIN_OUTPUT, "VB_ACQ");
    vc_acq=getAttributeCache()->getRWPtr<int32_t>(DOMAIN_OUTPUT, "VC_ACQ");
    vd_acq=getAttributeCache()->getRWPtr<int32_t>(DOMAIN_OUTPUT, "VD_ACQ");
    sum_acq=getAttributeCache()->getRWPtr<int32_t>(DOMAIN_OUTPUT, "SUM_ACQ");
	x_acq=getAttributeCache()->getRWPtr<double>(DOMAIN_OUTPUT, "X_ACQ");
    y_acq=getAttributeCache()->getRWPtr<double>(DOMAIN_OUTPUT, "Y_ACQ");
	for(int cnt=0;cnt<samples;cnt++){
		va_acq[cnt]=0;
		vb_acq[cnt]=0;
		vc_acq[cnt]=0;
		vd_acq[cnt]=0;
		sum_acq[cnt]=0;
		x_acq[cnt]=0;
		y_acq[cnt]=0;
	}
	getAttributeCache()->setOutputDomainAsChanged();
	buffer=(libera_dd_t*)malloc(samples*sizeof(libera_dd_t));
	if(buffer==NULL){
		throw chaos::CException(ret, "Cannot start acquire bacause lack of resources", __FUNCTION__);

	}
	BC_NORMAL_RUNNING_PROPERTY;
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
			BC_END_RUNNING_PROPERTY;
			return;

		}
	}

	if((*imode)&LIBERA_IOP_MODE_DD){
		CMDCUDBG_ << "Acquiring DD samples:"<<samples;
		/*if(driver->iop(LIBERA_IOP_CMD_GET_TS,(void*)&ts,sizeof(ts))==0){
			if(mt)
				*mt = ts.mt;
			if(st)
				*st=((uint64_t)ts.st.tv_sec)*1000000ULL + ts.st.tv_nsec/1000;

			CMDCUDBG_<<"MT:"<<*mt<<" ST:"<<*st <<" TV_sec:"<<ts.st.tv_sec<<" TV_NSEC:"<<ts.st.tv_nsec;

		}*/
	//	libera_dd_t buffer[samples];//=(libera_dd_t*)getAttributeCache()->getRWPtr<int32_t>(DOMAIN_OUTPUT, "DD");
		/*  if(pnt==NULL){
            CMDCUERR_<<"cannot retrieve dataset \"DD\"";
		 *pmode=0;


            getAttributeCache()->setOutputDomainAsChanged();
            BC_END_RUNNING_PROPERTY;
            return;
        }*/

		if((ret=driver->read((void*)buffer,0,samples*sizeof(libera_dd_t)))>=0){
			*va = buffer[0].Va;
			*vb = buffer[0].Vb;
			*vc = buffer[0].Vc;
			*vd = buffer[0].Vd;
			if(calc_poly){
				bpmpos mm;

					mm=  bpm_voltage_to_mm(u,v,buffer[0].Va,buffer[0].Vb,buffer[0].Vc,buffer[0].Vd);
					*x  = mm.x;
					*y  =mm.y;
			} else {
				*x  = buffer[0].X;
				*y  = buffer[0].Y;
			}
			*q  = buffer[0].Q;
			*sum  = buffer[0].Sum;
			*q1 = 0;
			*q2 = 0;
			setStateVariableSeverity(StateVariableTypeAlarmDEV,"trigger_timeout", chaos::common::alarm::MultiSeverityAlarmLevelClear);
			setStateVariableSeverity(StateVariableTypeAlarmDEV,"acquire_error", chaos::common::alarm::MultiSeverityAlarmLevelClear);
			for(int cnt=0;cnt<samples;cnt++){
				va_acq[cnt]=buffer[cnt].Va;
				vb_acq[cnt]=buffer[cnt].Vb;
				vc_acq[cnt]=buffer[cnt].Vc;
				vd_acq[cnt]=buffer[cnt].Vd;
				sum_acq[cnt]=buffer[cnt].Sum;
				if(calc_poly){
				//mm=bpm_voltage_to_mm(type,buffer[cnt].Va,buffer[cnt].Vb,buffer[cnt].Vc,buffer[cnt].Vd);
					bpmpos mm=  bpm_voltage_to_mm(u,v,buffer[cnt].Va,buffer[cnt].Vb,buffer[cnt].Vc,buffer[cnt].Vd);
					x_acq[cnt]=mm.x;
					y_acq[cnt]=mm.y;
				} else {
					x_acq[cnt]=buffer[cnt].X;
					y_acq[cnt]=buffer[cnt].Y;
				}
			}
			CMDCUDBG_ << "["<<(*acquire_loops)<<"] DD read "<<samples<<" [ret="<<std::dec<<ret<<"]:"<<buffer[0];

			(*acquire_loops)++;
		

		} else {
		/*	if(driver->iop(LIBERA_IOP_CMD_GETENV,status,MAX_STRING)!=0){
            CMDCUERR_<<" Cannot retrive STATUS";
    		} */
			if(ret==TRIGGER_TIMEOUT_ERROR){
				setStateVariableSeverity(StateVariableTypeAlarmDEV,"trigger_timeout", chaos::common::alarm::MultiSeverityAlarmLevelWarning);
			}else { 
				metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelError,CHAOS_FORMAT("DD Acquire mode %1% samples %2%",%*imode %*isamples ));

				setStateVariableSeverity(StateVariableTypeAlarmDEV,"acquire_error", chaos::common::alarm::MultiSeverityAlarmLevelWarning);
			}
		}
	} else if((*imode)&LIBERA_IOP_MODE_SA){
		libera_sa_t pnt;//=(libera_sa_t*)getAttributeCache()->getRWPtr<int32_t>(DOMAIN_OUTPUT, "SA");

		if((ret=driver->read((void*)&pnt,0,sizeof(libera_sa_t)))>=0){
			bpmpos mm;

			*va = pnt.Va;
			*vb = pnt.Vb;
			*vc = pnt.Vc;
			*vd = pnt.Vd;
			mm=bpm_voltage_to_mm(u,v,pnt.Va,pnt.Vb,pnt.Vc,pnt.Vd);
			*x  = mm.x;
			*y  = mm.y;
			*q  = pnt.Q;
			*sum  = pnt.Va + pnt.Vb + pnt.Vc + pnt.Vd;//pnt.Sum;
			*q1 = pnt.Cx;
			*q2 = pnt.Cy;
			(*acquire_loops)++;
			x_acq[0] = mm.x;
			y_acq[0] = mm.y;
			CMDCUDBG_ << "SA read:"<<pnt;

		} else {
			metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelError,CHAOS_FORMAT("SA Acquire mode %1% samples %2%",%*imode %*isamples ));
			setStateVariableSeverity(StateVariableTypeAlarmDEV,"acquire_error", chaos::common::alarm::MultiSeverityAlarmLevelWarning);

		}
	} else if((*imode)&LIBERA_IOP_MODE_CONTINUOUS){
		libera_cw_t*pnt=(libera_cw_t*)getAttributeCache()->getRWPtr<int32_t>(DOMAIN_OUTPUT, "ADC_CW");

		if(driver->read(pnt,0,samples*sizeof(libera_cw_t))>=0){
			(*acquire_loops)++;
			CMDCUDBG_ << "ADC CW read:"<<buffer[0];

		} else {
			metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelError,CHAOS_FORMAT("CW Acquire mode %1% samples %2%",%*imode %*isamples ));
		}
	} else if((*imode)&LIBERA_IOP_MODE_SINGLEPASS){
		libera_sp_t*pnt = (libera_sp_t*)getAttributeCache()->getRWPtr<int32_t>(DOMAIN_OUTPUT, "ADC_SP");
		if(driver->read(pnt,0,samples*sizeof(libera_sp_t))>=0){
			(*acquire_loops)++;
			CMDCUDBG_ << "ADC SP read:"<<buffer[0];

		} else {
			metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelError,CHAOS_FORMAT("SP Acquire mode %1% samples %2%",%*imode %*isamples ));
			setStateVariableSeverity(StateVariableTypeAlarmDEV,"acquire_error", chaos::common::alarm::MultiSeverityAlarmLevelWarning);

		}
	} else if((*imode)&LIBERA_IOP_MODE_AVG){
		libera_avg_t *pnt=(libera_avg_t *)getAttributeCache()->getRWPtr<int32_t>(DOMAIN_OUTPUT, "AVG");
		if(driver->read(pnt,0,samples*sizeof(libera_avg_t))>=0){
			(*acquire_loops)++;
			CMDCUDBG_ << "AVG read:"<<buffer[0];

		} else {
			metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelError,CHAOS_FORMAT("AVG Acquire mode %1% samples %2%",%*imode %*isamples ));
			setStateVariableSeverity(StateVariableTypeAlarmDEV,"acquire_error", chaos::common::alarm::MultiSeverityAlarmLevelWarning);
		}
	}


	if((loop==0)|| (*imode==0)){
		int ret;
		CMDCUDBG_ << "Acquiring loop ended after:"<<*acquire_loops<<" acquisitions.";
		metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelInfo,CHAOS_FORMAT("Acquiring mode %1% loop ended after %1% samples %2%",%*imode %*acquire_loops ));

		if((ret=driver->iop(LIBERA_IOP_CMD_STOP,0,0))!=0){
			//             *perr|=LIBERA_ERROR_STOP_ACQUIRE;
		}
		*pmode=0;
		getAttributeCache()->setOutputDomainAsChanged();

		BC_END_RUNNING_PROPERTY;
		return;
	} else if(loop>0){
		loop--;
	}


	CMDCUDBG_ << "End Acquiring loop:"<<*acquire_loops;
	getAttributeCache()->setOutputDomainAsChanged();


}
//void CmdLiberaAcquire::ccHandler() {
//	AbstractPowerSupplyCommand::ccHandler();
//	
//	BC_EXEC_RUNNING_PROPERTY
//	CMDCU_ << "Check if we are gone";
//	switch(state_to_go) {
//		case 0://we need to go in stanby
//			if(*o_status_id == common::powersupply::POWER_SUPPLY_STATE_STANDBY) {
//				setWorkState(false);
//				//we are terminated the command
//				CMDCU_ << boost::str( boost::format("State reached %1% [%2%] we end command") % o_status % *o_status_id);
//				BC_END_RUNNING_PROPERTY
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
//				BC_END_RUNNING_PROPERTY
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

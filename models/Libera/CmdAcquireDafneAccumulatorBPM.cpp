/* 
 * File:   CmdAcquireDafneAccumulatorBPM.cpp
 * Author: michelo
 * 
 * Created on October 15, 2015, 10:21 AM
 */

#include "LiberaData.h"
#include <boost/format.hpp>
#include <chaos/cu_toolkit/control_manager/slow_command/SlowCommand.h>
#include <driver/daq/models/Libera/CmdAcquireDafneAccumulatorBPM.h>
#define RECOVER_PERIOD 100
using namespace chaos::common::data;
using namespace driver::daq::libera;
using namespace ::driver::misc;

BATCH_COMMAND_OPEN_DESCRIPTION_ALIAS(driver::daq::libera::,CmdAcquireDafneAccumulatorBPM,"acquire","acquire command","72882f3e-36db-11e5-985f-334fcd6dff22")
BATCH_COMMAND_ADD_INT32_PARAM("enable", "enable acquisition =1, disable =0",chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_MANDATORY)
BATCH_COMMAND_ADD_INT32_PARAM("mode", "acquisition modes, =1 SlowAcquisition, =2 Data on Demand, add 0x100 for DD triggering",chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_MANDATORY)
BATCH_COMMAND_ADD_INT32_PARAM("samples", "in DataOnDemand number of samples",chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_OPTIONAL)
BATCH_COMMAND_ADD_INT32_PARAM("loops", "acquisition loops, -1 means continuos, to break launch a acquire command with enable=0",chaos::common::batch_command::BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_OPTIONAL)

BATCH_COMMAND_CLOSE_DESCRIPTION()

CmdAcquireDafneAccumulatorBPM::CmdAcquireDafneAccumulatorBPM() {
	dafne_status = new ChaosDatasetAttribute("DAFNE/STATUS/dafne_status");
	linac_mode = new ChaosDatasetAttribute("DAFNE/STATUS/linac_mode");



}



CmdAcquireDafneAccumulatorBPM::~CmdAcquireDafneAccumulatorBPM() {
	CUDBG<<"removing dafne status:"<<dafne_status;

	delete dafne_status;
	delete linac_mode;

}

uint8_t CmdAcquireDafneAccumulatorBPM::implementedHandler(){
	return chaos::common::batch_command::HandlerType::HT_Set  | chaos::common::batch_command::HandlerType::HT_Acquisition;

}
void  CmdAcquireDafneAccumulatorBPM::setHandler(c_data::CDataWrapper *data){
	::driver::misc::CmdSync::setHandler(data);
    setBusyFlag(false);

	tomode=0;
	int32_t samples_v;
	int cnt;
	recover=RECOVER_PERIOD;
	last_command=data;
	setBusyFlag(true);
	if(data->hasKey("enable")) {
		if(data->getInt32Value("enable")==0){
			tomode = 0;
			mode_sync.setTimeout(10000000);
			CTRLDBG_<<" WAITING for exiting acquire";

			if(mode_sync.sync(tomode)==0){

				CTRLERR_<<" cannot synchronize pool to:"<<tomode;
                metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelError,"cannot disable acquisition of all BPM pool" );

			}
			BC_FAULT_RUNNING_PROPERTY;
			return;
		}
	}
	mode=driver->getRemoteVariables("MODE");
	elem_size=mode.size();
	for(int cnt=0;cnt<elem_size;cnt++){
		mode_sync.add(mode[cnt]);
	}
	if(data->hasKey("mode")) {
		tomode=data->getInt32Value("mode");
		if(data->hasKey("loops")){
			if(data->getInt32Value("loops")<0){
				tomode|=LIBERA_IOP_MODE_PERMLOOP;
			}
		}
		CTRLDBG_<<" Going into mode:"<<tomode;
		if(tomode==0){
			mode_sync.setTimeout(10000000);
			CUDBG<<" WAITING for exiting acquire";

			if(mode_sync.sync(tomode)==0){
				CUERR<<" cannot synchronize pool to:"<<tomode;
                metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelError,"cannot stop acquisition of all BPM pool" );

                BC_FAULT_RUNNING_PROPERTY;
			} else {
				BC_END_RUNNING_PROPERTY;
			}
			return;
		}
	}
	va=driver->getRemoteVariables("VA");
	vb=driver->getRemoteVariables("VB");
	vc=driver->getRemoteVariables("VC");
	vd=driver->getRemoteVariables("VD");
	va_acq=driver->getRemoteVariables("VA_ACQ");
	vb_acq=driver->getRemoteVariables("VB_ACQ");
	vc_acq=driver->getRemoteVariables("VC_ACQ");;
	vd_acq=driver->getRemoteVariables("VD_ACQ");;
	x_acq=driver->getRemoteVariables("X_ACQ");
	y_acq=driver->getRemoteVariables("Y_ACQ");
	x=driver->getRemoteVariables("X");
	y=driver->getRemoteVariables("Y");
	acquire=driver->getRemoteVariables("ACQUISITION");
	samples=driver->getRemoteVariables("SAMPLES");
	poly_type=driver->getRemoteVariables("config");
	sum_acq = driver->getRemoteVariables("SUM");

	if((va.size()==vb.size())&&(vb.size()==vc.size())&&(vc.size()==vd.size())&&(vd.size()==mode.size())&&(mode.size()==acquire.size())&&(acquire.size()==samples.size())&&(va_acq.size()==samples.size())&&(vb_acq.size()==samples.size())&&(vc_acq.size()==samples.size())&&(vd_acq.size()==samples.size())&&(poly_type.size()==samples.size())){
		CUDBG<<" Array BPM size:"<<va.size();
		elem_size = va.size();
	} else {
		CUERR<<"Different array size, check driver input parameters: VA:"<<va.size()<<" VB:"<<vb.size()<<" VC:"<<vc.size()<<" VD:"<<vd.size()<<" MODE:"<<mode.size()<<" acquire:"<<acquire.size()<<" samples:"<<samples.size();
        metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelError,"Different array size, check driver input parameters" );
        BC_FAULT_RUNNING_PROPERTY;

        return;

	}



	CTRLDBG_<<" WAITING for mode:"<<tomode;
	mode_sync.setTimeout(10000000);

	if(mode_sync.sync(tomode)<0){
		CTRLERR_<<" cannot synchronize pool to:"<<tomode;
		BC_FAULT_RUNNING_PROPERTY;
		return;
	}
	mode_sync.setUpdateMode(driver::misc::ChaosDatasetAttribute::NOTBEFORE,10000);

	CUDBG<<" EXITING from waiting mode:"<<tomode;
	rattrs= driver->getRemoteVariables("X_ACQ");
	std::vector<ChaosDatasetAttribute*> rattrs_y=driver->getRemoteVariables("Y_ACQ");
	std::vector<ChaosDatasetAttribute*> rattrs_xx=driver->getRemoteVariables("X");
	std::vector<ChaosDatasetAttribute*> rattrs_yy=driver->getRemoteVariables("Y");
	std::vector<ChaosDatasetAttribute*> rattrs_sum=driver->getRemoteVariables("SUM");

	rattrs.insert(rattrs.end(),rattrs_y.begin(),rattrs_y.end());
	rattrs.insert(rattrs.end(),rattrs_xx.begin(),rattrs_xx.end());
	rattrs.insert(rattrs.end(),rattrs_yy.begin(),rattrs_yy.end());
	rattrs.insert(rattrs.end(),rattrs_sum.begin(),rattrs_sum.end());

	// CTRLDBG_<<"dafne status:"<<(int32_t)*dafne_status;

	rattrs.push_back(dafne_status);
	rattrs.push_back(linac_mode);

	for(cnt=0;cnt<elem_size;cnt++){
		samples_v=*samples[cnt];
		//10ms
		va[cnt]->setUpdateMode(driver::misc::ChaosDatasetAttribute::NOTBEFORE,10000);
		vb[cnt]->setUpdateMode(driver::misc::ChaosDatasetAttribute::NOTBEFORE,10000);
		vc[cnt]->setUpdateMode(driver::misc::ChaosDatasetAttribute::NOTBEFORE,10000);
		vd[cnt]->setUpdateMode(driver::misc::ChaosDatasetAttribute::NOTBEFORE,10000);
		va_acq[cnt]->setUpdateMode(driver::misc::ChaosDatasetAttribute::NOTBEFORE,10000);
		vb_acq[cnt]->setUpdateMode(driver::misc::ChaosDatasetAttribute::NOTBEFORE,10000);
		vc_acq[cnt]->setUpdateMode(driver::misc::ChaosDatasetAttribute::NOTBEFORE,10000);
		vd_acq[cnt]->setUpdateMode(driver::misc::ChaosDatasetAttribute::NOTBEFORE,10000);
		acquire[cnt]->setUpdateMode(driver::misc::ChaosDatasetAttribute::NOTBEFORE,10000);
		samples[cnt]->setUpdateMode(driver::misc::ChaosDatasetAttribute::NOTBEFORE,10000);
		poly_type[cnt]->setUpdateMode(driver::misc::ChaosDatasetAttribute::NOTBEFORE,1000000);
		sum_acq[cnt]->setUpdateMode(driver::misc::ChaosDatasetAttribute::NOTBEFORE,1000000);

		x_acq[cnt]->setUpdateMode(driver::misc::ChaosDatasetAttribute::DONTUPDATE,0);
		y_acq[cnt]->setUpdateMode(driver::misc::ChaosDatasetAttribute::DONTUPDATE,0);
		x_acq[cnt]->resize(samples_v*sizeof(double));
		y_acq[cnt]->resize(samples_v*sizeof(double));
		x[cnt]->setUpdateMode(driver::misc::ChaosDatasetAttribute::DONTUPDATE,0);
		y[cnt]->setUpdateMode(driver::misc::ChaosDatasetAttribute::DONTUPDATE,0);
		chaos::common::data::CDataWrapper config;

		const char*tmp;
		tmp=(const char*)(poly_type[cnt]->get(NULL));
		if(tmp==NULL){
			CTRLERR_<<" ERROR getting config:"<<poly_type[cnt]->getPath();
			BC_FAULT_RUNNING_PROPERTY;
			return;
		}
		CTRLDBG_<<"config ["<<poly_type[cnt]->getPath()<<"]="<<tmp;
		//CDataWrapper *config=static_cast<CDataWrapper*>(tmp);

		config.setSerializedJsonData(tmp);
		coeff_u[cnt][0] =1;
		coeff_v[cnt][0] =1;
		for(int cntt=1;cntt<6;cntt++){
			coeff_u[cnt][cntt]=coeff_v[cnt][cntt]=0;

		}
		if(config.hasKey("coeff_u")&&config.isVector("coeff_u")){
			CMultiTypeDataArrayWrapper* p = config.getVectorValue("coeff_u");
			for(int cntt=0;cntt<p->size();cntt++){
				if(cntt<6){
					coeff_u[cnt][cntt] = p->getDoubleElementAtIndex(cntt);
					CTRLDBG_<<poly_type[cnt]->getPath()<< " COEFF u["<<cntt<<"]="<<coeff_u[cnt][cntt];

				}
			}
		}

		if(config.hasKey("coeff_v")&&config.isVector("coeff_v")){
			CMultiTypeDataArrayWrapper* p = config.getVectorValue("coeff_v");
			for(int cntt=0;cntt<p->size();cntt++){
				if(cntt<6){
					coeff_v[cnt][cntt] = p->getDoubleElementAtIndex(cntt);
					CTRLDBG_<<poly_type[cnt]->getPath()<< " COEFF v["<<cntt<<"]="<<coeff_v[cnt][cntt];

				}
			}
		}
	}

	cnt=0;
	for (std::vector<ChaosDatasetAttribute*>::iterator i=rattrs.begin();i!=rattrs.end();i++){

		if((*i)->getDir()==chaos::DataType::Output){
			uint32_t size;
			//void*ptr=(*i)->get(&size);
			if((*i)->getType()==chaos::DataType::TYPE_BYTEARRAY){
				getAttributeCache()->setOutputAttributeNewSize(cnt,size);
			}
			//    CTRLDBG_<<"setting "<<cnt<<" "<<(*i)->getPath()<<" size:"<<size;
			//getAttributeCache()->setOutputAttributeValue(cnt,ptr,size);
			cnt++;

		}

	}
    metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelInfo,CHAOS_FORMAT("starting BPM poll acquire of %1% BPM(s)", %elem_size ));


	//getAttributeCache()->setOutputAttributeNewSize("X_ACQ", samples_v*sizeof(double));
	//     getAttributeCache()->setOutputAttributeNewSize("Y_ACQ", samples_v*sizeof(double));
}


void CmdAcquireDafneAccumulatorBPM::acquireHandler() {
	uint64_t acquire_v,mt_v;
	int32_t mode_v,samples_v;
	try {

		int cntt;
		int cnt;


		for(cnt=0;cnt<elem_size;cnt++){
			bpmpos mm;
			int32_t a,b,c,d;
			uint32_t size;
			int poly;
			double dx,dy;
			mode_v= *mode[cnt];
			/* if((mode_v != tomode)&&(recover--==0)){

	    CTRLDBG_<<" Reapply command because \""<<rattrs[cnt]->getPath()<<"\" has mode ="<<mode_v;
	    ::driver::misc::CmdSync::setHandler(last_command);
	    sleep(1);

	  }*/
			if(mode_v !=tomode){
				CUDBG<<"["<<cnt<<"] not any more synchronized to:"<<tomode<<" now:"<<tomode<<" exiting acquire";
			    metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelWarning,CHAOS_FORMAT(" BPM %1% not anymore synchronized to mode '%2%' exiting acquire", %cnt %tomode ));

				BC_END_RUNNING_PROPERTY;
				return;
			}

			samples_v = *samples[cnt];
			acquire_v = *acquire[cnt];
			a= *va[cnt];
			b= *vb[cnt];
			c= *vc[cnt];
			d= *vd[cnt];
			poly=*poly_type[cnt];
			mm=bpm_voltage_to_mm(coeff_u[cnt],coeff_v[cnt],a,b,c,d);
			dx=mm.x;
			dy=mm.y;
			*x[cnt]=dx;
			*y[cnt]=dy;
			if(mode_v&LIBERA_IOP_MODE_DD){
				// double vx_acq[samples_v];
				//  double vy_acq[samples_v];

				int32_t *vva_acq=(int32_t*) *va_acq[cnt];
				int32_t *vvb_acq=(int32_t*) *vb_acq[cnt];
				int32_t *vvc_acq=(int32_t*) *vc_acq[cnt];
				int32_t *vvd_acq=(int32_t*) *vd_acq[cnt];
				double *vx_acq=(double*) *x_acq[cnt];
				double *vy_acq=(double*) *y_acq[cnt];
				for(cntt=0;cntt<samples_v;cntt++){
					mm=bpm_voltage_to_mm(coeff_u[cnt],coeff_v[cnt],vva_acq[cntt],vvb_acq[cntt],vvc_acq[cntt],vvd_acq[cntt]);
					dx=mm.x;
					dy=mm.y;
					vx_acq[cntt]=dx;
					vy_acq[cntt]=dy;
				}

			}

			CTRLDBG_<<"BPM ["<<cnt<<"] type:"<<poly<<" " <<mode[cnt]->getPath()<<"["<<mode[cnt]->getInfo().getTimeStamp()<<"] mode:"<<mode_v<<" samples:"<<samples_v<<" "<<"acquire:"<<acquire_v<<": ("<<mm.x<<" mm, "<<mm.y<<" mm) Voltages:"<<a <<" "<<b <<" "<<c<<" "<<d;
		}

		cnt=0;
		for (std::vector<ChaosDatasetAttribute*>::iterator i=rattrs.begin();i!=rattrs.end();i++){

			if((*i)->getDir()==chaos::DataType::Output){
				uint32_t size;
				void*ptr=(*i)->get(&size);
				/*  if((*i)->getType()==chaos::DataType::TYPE_BYTEARRAY){
                getAttributeCache()->setOutputAttributeNewSize(cnt,size);
            }*/
				//    CTRLDBG_<<"setting "<<cnt<<" "<<(*i)->getPath()<<" size:"<<size;
				getAttributeCache()->setOutputAttributeValue(cnt,ptr,size);
				cnt++;

			}

		}
	} catch(chaos::CException e){
	    metadataLogging(chaos::common::metadata_logging::StandardLoggingChannel::LogLevelWarning,CHAOS_FORMAT("an exception occurred %1%", %e.errorMessage));

		CUDBG<<"%% WARNING "<<e.errorMessage;
	}

	/*if(mode_v==0){
        ATTRDBG_<<"exiting from acquire, by mode =0";
        BC_END_RUNNING_PROPERTY;
      }*/

	getAttributeCache()->setOutputAttributeValue("MODE",(void*)&mode_v,sizeof(mode_v));
	getAttributeCache()->setOutputAttributeValue("SAMPLES",(void*)&samples_v,sizeof(samples_v));
	getAttributeCache()->setOutputAttributeValue("ACQUISITION",(void*)&acquire_v,sizeof(acquire_v));
	getAttributeCache()->setOutputDomainAsChanged();

}

/*
 *	CmdLiberaDefault.cpp
 *	!CHAOS
 *	Created by Andrea Michelotti
 *
 *    	Copyright 2013 INFN, National Institute of Nuclear Physics
 *
 *    	Licensed under the Apache License, Version 2.0 (the "License");
 *    	you may not use this file except in compliance with the License.
 *    	You may obtain a copy of the License at
 *
 *    	http://www.apache.org/licenses/LICENSE-2.0
 *
 *    	Unless required by applicable law or agreed to in writing, software
 *    	distributed under the License is distributed on an "AS IS" BASIS,
 *    	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    	See the License for the specific language governing permissions and
 *    	limitations under the License.
 */

#include <string.h>
#include "CmdLiberaDefault.h"



namespace chaos_batch = chaos::common::batch_command;
using namespace chaos::common::data;
using namespace chaos::common::batch_command;
using namespace chaos::cu::control_manager::slow_command;
using namespace driver::daq::libera;
CmdLiberaDefault::CmdLiberaDefault() {
  CMDCUDBG_<< "Created command default:"<<driverAccessorsErogator;
  driver =NULL;
  mt= NULL;
  st=NULL;
}
void CmdLiberaDefault::endHandler() {
	CMDCUDBG_<<"Close Command:'"<<this->getAlias()<<"'";
	setBusyFlag(false);
	 getAttributeCache()->setOutputDomainAsChanged();
}
CmdLiberaDefault::~CmdLiberaDefault() {
    if(driver){
        delete driver;
        driver = NULL;
    }

}

    // return the implemented handler
uint8_t CmdLiberaDefault::implementedHandler() {
        //add to default hadnler the acquisition one
    return chaos_batch::HandlerType::HT_Set  | HandlerType::HT_Acquisition;

}

    // Start the command execution
void CmdLiberaDefault::setHandler(c_data::CDataWrapper *data) {
    setBusyFlag(false);;
    getAttributeCache()->setOutputDomainAsChanged();

	setFeatures(features::FeaturesFlagTypes::FF_SET_SCHEDULER_DELAY, (uint64_t)1000000);
	chaos::cu::driver_manager::driver::DriverAccessor * accessor=driverAccessorsErogator->getAccessoInstanceByIndex(0);
  	CMDCUDBG_<< "Created accessor:"<<accessor;
	if(accessor==NULL){
		throw chaos::CException(-1, "Cannot retrieve the requested driver", __FUNCTION__);
	}
	driver = new chaos::cu::driver_manager::driver::BasicIODriverInterface(accessor);

	if(driver==NULL){
		throw chaos::CException(-2, "Cannot allocate driver resources", __FUNCTION__);
	}
	CMDCUDBG_<< "retrived BasicIODriver:"<<driver;

        idd=getAttributeCache()->getRWPtr<bool>(DOMAIN_INPUT, "DD");
        isa=getAttributeCache()->getRWPtr<bool>(DOMAIN_INPUT, "SA");
        itrigger=getAttributeCache()->getRWPtr<bool>(DOMAIN_INPUT, "TRIGGER");
        imode = getAttributeCache()->getRWPtr<int32_t>(DOMAIN_INPUT, "MODE");
        isamples=getAttributeCache()->getRWPtr<int32_t>(DOMAIN_INPUT, "SAMPLES");
        

        ioffset=getAttributeCache()->getRWPtr<int32_t>(DOMAIN_INPUT, "OFFSET");

        config=getAttributeCache()->getRWPtr<CDataWrapper>(DOMAIN_INPUT, "config");
        u[0]=1;
        v[0]=1;
        if(config){
        	if(config->hasKey("coeff_u")&&config->isVector("coeff_u")){
        		CMultiTypeDataArrayWrapper* p = config->getVectorValue("coeff_u");
        		for(int cnt=0;cnt<p->size();cnt++){
        			if(cnt<6){
        				u[cnt] = p->getDoubleElementAtIndex(cnt);
        				CMDCUDBG_<< "COEFF u["<<cnt<<"]="<<u[cnt];

        			}
        		}
        	}

        	if(config->hasKey("coeff_v")&&config->isVector("coeff_v")){
        	        		CMultiTypeDataArrayWrapper* p = config->getVectorValue("coeff_v");
        	        		for(int cnt=0;cnt<p->size();cnt++){
        	        			if(cnt<6){
        	        				v[cnt] = p->getDoubleElementAtIndex(cnt);
        	        				CMDCUDBG_<< "v["<<cnt<<"]="<<v[cnt];

        	        			}
        	        		}
        	        	}

        } else {
			for(int cnt=1;cnt<6;cnt++){
				u[cnt]=v[cnt]=0;

			}
        }
        odd=getAttributeCache()->getRWPtr<bool>(DOMAIN_OUTPUT, "DD");
               osa=getAttributeCache()->getRWPtr<bool>(DOMAIN_OUTPUT, "SA");
	 mt=getAttributeCache()->getRWPtr<uint64_t>(DOMAIN_OUTPUT, "MT");
         st=getAttributeCache()->getRWPtr<uint64_t>(DOMAIN_OUTPUT, "ST");
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
         pmode=getAttributeCache()->getRWPtr<int32_t>(DOMAIN_OUTPUT, "MODE");
        mt=getAttributeCache()->getRWPtr<uint64_t>(DOMAIN_OUTPUT, "MT");
         st=getAttributeCache()->getRWPtr<uint64_t>(DOMAIN_OUTPUT, "ST");
        va_acq=getAttributeCache()->getRWPtr<int32_t>(DOMAIN_OUTPUT, "VA_ACQ");
        vb_acq=getAttributeCache()->getRWPtr<int32_t>(DOMAIN_OUTPUT, "VB_ACQ");
        vc_acq=getAttributeCache()->getRWPtr<int32_t>(DOMAIN_OUTPUT, "VC_ACQ");
        vd_acq=getAttributeCache()->getRWPtr<int32_t>(DOMAIN_OUTPUT, "VD_ACQ");
        sum_acq=getAttributeCache()->getRWPtr<int32_t>(DOMAIN_OUTPUT, "SUM_ACQ");

        x_acq=getAttributeCache()->getRWPtr<double>(DOMAIN_OUTPUT, "X_ACQ");
        y_acq=getAttributeCache()->getRWPtr<double>(DOMAIN_OUTPUT, "Y_ACQ");
         acquire_loops = getAttributeCache()->getRWPtr<int64_t>(DOMAIN_OUTPUT, "ACQUISITION");
	BC_NORMAL_RUNNING_PROPERTY

}

    // Aquire the necessary data for the command
/*!
 The acquire handler has the purpose to get all necessary data need the by CC handler.
 \return the mask for the runnign state
 */
void CmdLiberaDefault::acquireHandler() {
        libera_ts_t ts;
	CMDCUDBG_ << "Default Acquiring libera status";
	char * status= getAttributeCache()->getRWPtr<char>(DOMAIN_OUTPUT, "STATUS");
	if(driver->iop(LIBERA_IOP_CMD_GETENV,status,MAX_STRING)==0){
            CMDCUDBG_<<"STATUS:"<<status;
        }

	/*        
		  if(driver->iop(LIBERA_IOP_CMD_GET_TS,(void*)&ts,sizeof(ts))==0){

		  CMDCUDBG_<<"MT:"<<ts.mt<<" ST:"<<ts.st.tv_sec;
		  if(mt)
		  *mt = ts.mt;
		  if(st)
		  *st=ts.st.tv_sec*1000000 + ts.st.tv_nsec/1000;
		  }
	*/
        getAttributeCache()->setOutputDomainAsChanged();
	//force output dataset as changed
}

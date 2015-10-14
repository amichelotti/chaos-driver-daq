/*
 *	SCDafneAccumulatorBPMSync.h
 *	!CHAOS
 *	Created by Andrea Michelotti
 *      Collects and align a given series of Libera BPMs
 *    	Copyright 2015 INFN, National Institute of Nuclear Physics
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
#ifndef ChaosRTControlUnit_SCDafneAccumulatorBPMSync_h
#define ChaosRTControlUnit_SCDafneAccumulatorBPMSync_h

#include <chaos/cu_toolkit/ControlManager/RTAbstractControlUnit.h>
#include <driver/misc/ChaosControllerGroup.h>
#include <driver/misc/ChaosDatasetAttribute.h>
#include <driver/misc/ChaosDatasetAttributeSyncronizer.h>
#include <driver/daq/models/Libera/ChaosControllerLibera.h>
#include <driver/misc/SCDataSync.h>
   
    namespace driver {
        
        namespace daq {
class SCDafneAccumulatorBPMSync : public SCDataSync {
	PUBLISHABLE_CONTROL_UNIT_INTERFACE(SCDafneAccumulatorBPMSync)
public:
    /*!
     Construct a new CU with full constructor
     */
    SCDafneAccumulatorBPMSync(const std::string& _control_unit_id, const std::string& _control_unit_param, const ControlUnitDriverList& _control_unit_drivers);
    /*!
     Destructor a new CU
     */
    ~SCDafneAccumulatorBPMSync();

protected:


    /*!
		Define the Control Unit Dataset and Actions
		*/
		void unitDefineActionAndDataset()throw(chaos::CException);

 
};
        }}
#endif

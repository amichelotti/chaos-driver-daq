/*
 *	RTBTFdaqCU.h
 *	!CHAOS
 *	Created by Andrea Michelotti 8/1/2016
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
#ifndef __RTBTFdaqCU__
#define __RTBTFdaqCU__

#include <chaos/cu_toolkit/control_manager/RTAbstractControlUnit.h>
#include <common/vme/caen/caen965_drv.h>
#include <common/vme/caen/caen792_drv.h>
#include <common/vme/sis/sis3800_drv.h>
#include <common/vme/caen/caen513_drv.h>


namespace driver {
	namespace daq {
            namespace btf {
		class RTBTFdaqCU : public chaos::cu::control_manager::RTAbstractControlUnit {
			PUBLISHABLE_CONTROL_UNIT_INTERFACE(RTBTFdaqCU);

		protected:
                    
                    const uint32_t *sis3800_addr,*caen965_addr,*caen792_addr,*caen513_addr;
                    caen965_handle_t caen965_handle;
                    caen792_handle_t caen792_handle;
                    sis3800_handle_t sis3800_handle;
                    caen513_handle_t caen513_handle;
                    uint32_t counter,counter_old,counter_middle; 
                    uint64_t tot_lost,loop;
                    uint32_t*qdchi,*qdclow,*qdc792,*counters;
                    uint64_t* trigger_lost,*acquisition,*triggers;
                    
			/*
			 Define the Control Unit Dataset and Actions
			 */
			void unitDefineActionAndDataset()throw(chaos::CException);

			void unitDefineCustomAttribute();
			
			/*(Optional)
			 Initialize the Control Unit and all driver, with received param from MetadataServer
			 */
			void unitInit() throw(chaos::CException);
			/*(Optional)
			 Execute the work, this is called with a determinated delay, it must be as fast as possible
			 */
			void unitStart() throw(chaos::CException);
			void unitRun() throw(chaos::CException);
			/*(Optional)
			 The Control Unit will be stopped
			 */
			void unitStop() throw(chaos::CException);
			/*(Optional)
			 The Control Unit will be deinitialized and disposed
			 */
			void unitDeinit() throw(chaos::CException);
			
		public:
			/*
			 Construct a new CU with an identifier
			 */
			RTBTFdaqCU(const std::string& _control_unit_id,
									 const std::string& _control_unit_param,
									 const ControlUnitDriverList& _control_unit_drivers);
			
			/*
			 Base destructor
			 */
			~RTBTFdaqCU();
		};
	}
   }
}

#endif 

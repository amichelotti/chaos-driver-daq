/*
 *	SCLiberaCU.h
 *	!CHAOS
 *	Created by Andrea Michelotti 15/5/2015
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
#ifndef __PowerSupply__SCLiberaCU__
#define __PowerSupply__SCLiberaCU__

#include <chaos/cu_toolkit/ControlManager/SCAbstractControlUnit.h>
#include <chaos/cu_toolkit/driver_manager/driver/BasicIODriverInterface.h>


namespace driver {
	namespace daq {
            namespace libera {
		class SCLiberaCU : public chaos::cu::control_manager::SCAbstractControlUnit {
			PUBLISHABLE_CONTROL_UNIT_INTERFACE(SCLiberaCU)

			
			chaos::cu::driver_manager::driver::BasicIODriverInterface *driver;

		protected:
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
			SCLiberaCU(const std::string& _control_unit_id,
									 const std::string& _control_unit_param,
									 const ControlUnitDriverList& _control_unit_drivers);
			
			/*
			 Base destructor
			 */
			~SCLiberaCU();
		};
	}
   }
}

#endif 

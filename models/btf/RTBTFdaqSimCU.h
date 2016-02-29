/*
 *	RTBTFdaqSimCU.h
 *	!CHAOS
 *	Created by Andrea Michelotti 1/2/2016
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
#ifndef __RTBTFdaqSimCU__
#define __RTBTFdaqSimCU__

#include <chaos/cu_toolkit/control_manager/RTAbstractControlUnit.h>
#include "RTBTFdaqCU.h"
#define REFRESH_RATE 20000 //20 ms

namespace driver {
	namespace daq {
            namespace btf {
		class RTBTFdaqSimCU : public RTBTFdaqCU {
			PUBLISHABLE_CONTROL_UNIT_INTERFACE(RTBTFdaqSimCU);

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
			RTBTFdaqSimCU(const std::string& _control_unit_id,
									 const std::string& _control_unit_param,
									 const ControlUnitDriverList& _control_unit_drivers);
			
			/*
			 Base destructor
			 */
			~RTBTFdaqSimCU();
		};
	}
   }
}

#endif 
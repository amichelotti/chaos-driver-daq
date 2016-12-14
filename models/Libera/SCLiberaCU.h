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
#ifndef __SCLiberaCU__
#define __SCLiberaCU__

#include <chaos/cu_toolkit/control_manager/SCAbstractControlUnit.h>
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
                        
                        
                         int32_t *pmode,*perr,*imode,*isamples,*ioffset;
                         bool *itrigger;
			
		public:
			/*
			 Construct a new CU with an identifier
			 */
			SCLiberaCU(const std::string& _control_unit_id,
									 const std::string& _control_unit_param,
									 const ControlUnitDriverList& _control_unit_drivers);
			
                        
                        bool setDD(const std::string &name, bool value, uint32_t size);
                        bool setSA(const std::string &name, bool value, uint32_t size);
                        bool setADC(const std::string &name, bool value, uint32_t size);
                        
                        bool setTrigger(const std::string &name, bool value, uint32_t size);
                        bool setMode(const std::string &name, int32_t value, uint32_t size);
                        bool sendAcquire(int32_t mode, bool enable,int32_t loops, int32_t samples,int32_t offset,bool sync=true);


			/*
			 Base destructor
			 */
			~SCLiberaCU();
		};
	}
   }
}

#endif 

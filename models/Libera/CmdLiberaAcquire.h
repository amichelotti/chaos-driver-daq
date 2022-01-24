/*
 *	CmdLiberaAcquire.h
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

#ifndef __CmdLiberaAcquire__
#define __CmdLiberaAcquire__

#include "CmdLiberaDefault.h"

namespace c_data = chaos::common::data;
namespace ccc_slow_command = chaos::cu::control_manager::slow_command;

namespace driver {
	namespace daq {
            namespace libera {
		
                   DEFINE_BATCH_COMMAND_CLASS(CmdLiberaAcquire,CmdLiberaDefault) {
    
                   
                    int acquire_duration;
                    uint64_t start_acquire,wait_for_us,loop,samples;
					libera_dd_t* buffer;
					libera_data_handle_t dd_handle;
		protected:
			//implemented handler
		    //			uint8_t implementedHandler();
			
			// Set handler
			void setHandler(c_data::CDataWrapper *data);
			
			//Correlation and commit phase
			void acquireHandler();
			
			//manage the timeout
			//			bool timeoutHandler();
		public:
			CmdLiberaAcquire();

			~CmdLiberaAcquire();
		};
	}
      }
}

#endif 

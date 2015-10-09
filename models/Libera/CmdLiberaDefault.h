/*
 *	CmdLiberaDefault.h
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

#ifndef __CmdLiberaDefault__
#define __CmdLiberaDefault__

#include <chaos/cu_toolkit/ControlManager/slow_command/SlowCommand.h>
#include <chaos/cu_toolkit/driver_manager/driver/BasicIODriverInterface.h>
#include "LiberaData.h"

#define CMDCU_ LAPP_ << "["<<__FUNCTION__<<"]"

#define CMDCUDBG_ LDBG_ << "[- "<<__FUNCTION__<<" -]"
#define CMDCUERR_ LERR_ << "["<<__FUNCTION__<<"]"

namespace c_data = chaos::common::data;
namespace ccc_slow_command = chaos::cu::control_manager::slow_command;
#define MAX_STRING 1024
#define MAX_SAMPLES 64*1024

namespace driver {
	namespace daq {
            namespace libera {
		class CmdLiberaDefault : public ccc_slow_command::SlowCommand {
                   
                protected:
                     const uint32_t	*i_command_timeout;
                     uint64_t     *mt; // machine time
                      uint64_t     *st; // system time
                     
                    chaos::cu::driver_manager::driver::BasicIODriverInterface *driver;
                
			// return the implemented handler
			uint8_t implementedHandler();
			
			// Start the command execution
			void setHandler(c_data::CDataWrapper *data);
			
			// Aquire the necessary data for the command
			/*!
			 The acquire handler has the purpose to get all necessary data need the by CC handler.
			 \return the mask for the runnign state
			 */
			void acquireHandler();
		public:
			CmdLiberaDefault();
			~CmdLiberaDefault();
		};
		
	}
        }
}


#endif 

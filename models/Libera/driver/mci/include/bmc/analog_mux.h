/*
 * Copyright (c) 2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: analog_mux.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef ANALOG_MUX_H_
#define ANALOG_MUX_H_

#include <vector>
#include "iapp/libera_board.h"
#include "iapp/libera_fpga_object.h"

typedef struct _tAnalogMux {
    uint32_t  muxInputSel;
} tAnalogMux;

/*-----------------------------------------------------------------------------*/
namespace bmc {
    /* class declaration */
    class AnalogMux
    {
        public:
            AnalogMux(FPGAMem_t         *a_reg,
                      const std::string &a_name,
                      const uint32_t a_numOfInputs);
            virtual ~AnalogMux();

            bool SetInput(uint32_t a_input) const;
            uint32_t GetInput(void) const;

        protected:
        private:
            volatile tAnalogMux  *m_reg;
            const uint32_t m_numOfInputs;
    };
}

#endif /* ANALOG_MUX_H_ */

/*
 * Copyright (c) 2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: i2c_bus_controller_fpga_amba.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef I2C_BUS_CONTROLLER_FPGA_AMBA_H_
#define I2C_BUS_CONTROLLER_FPGA_AMBA_H_

#include "i2c_bus_controller.h"

const uint32_t c_dataRegMask = 0x000000ff;
const uint32_t c_ctrlRegMask = 0xff000000;

namespace bmc {

    struct tI2cBusController {
        /* offset + 0x00 */ uint32_t data;
                            uint32_t reserved_1;
        /* offset + 0x08 */ uint32_t ctrl;
                            uint32_t reserved_2;
        /* offset + 0x10 */ uint32_t prsVal;     // speed prescaler
                            uint32_t reserved_3;
        /* offset + 0x18 */ uint32_t busSel;
                            uint32_t reserved_4;
        /* offset + 0x20 */ uint32_t output;
                            uint32_t reserved_5;
        /* offset + 0x28 */ uint32_t inputReg;
                            uint32_t reserved_6;
    };

    /*----------------------------------------------------------------*/

    class  I2cBusControllerFpgaAmba : public I2cBusController {

    public:
        I2cBusControllerFpgaAmba(const std::string          &a_name,
                                 uint32_t                    a_capacity,
                                 volatile tI2cBusController *a_reg);

        static size_t  Size() { return sizeof(tI2cBusController); }

    protected:
        virtual bool  IsIdle();
        virtual bool  IsAcknowledged();

        virtual void  OnSelectBus(uint8_t a_bus_address);
        virtual void  OnSetSpeed(uint32_t a_speed);
        virtual void  OnWriteCtrl(uint32_t a_ctrl);
        virtual void  OnWriteData(uint8_t a_data);
        virtual void  OnReadData(uint8_t &a_data);

        virtual bool  OnSetOutput(uint32_t  a_val);
        virtual bool  OnGetOutput(uint32_t &a_val);

    private:
        /* interface to FPGA PCI memory */
        volatile tI2cBusController  *m_reg;
    };
}
#endif /* I2C_BUS_CONTROLLER_FPGA_AMBA_H_ */

/*
 * Copyright (c) 2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: i2c_bus_controller_fpga_spiamba.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef I2C_BUS_CONTROLLER_FPGA_SPIAMBA_H_
#define I2C_BUS_CONTROLLER_FPGA_SPIAMBA_H_

#include "board.h"
#include "i2c_bus_controller.h"

const uint32_t c_dataRegMask = 0x000000ff;
const uint32_t c_ctrlRegMask = 0xff000000;

/*----------------------------------------------------------------------------*/
namespace bmc {

    class  I2cBusControllerFpgaSPIAmba : public I2cBusController {
    public:
        /* fpga i2c channel offsets into base address. */
        enum i2c_OffsetEnum {
            data      = 0,
            ctrlReg   = 0x8,
            prsVal    = 0x10,
            busSel    = 0x18,
            outputReg = 0x20,
            inputReg  = 0x28
        };

        I2cBusControllerFpgaSPIAmba(const std::string &a_name,
                                    uint32_t           a_capacity,
                                    const Board*       a_board,
                                    const size_t       a_i2c_ambabase);

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
        const Board* m_board;
        const size_t m_i2cbase;
    };
}
#endif /* I2C_BUS_CONTROLLER_FPGA_SPIAMBA_H_ */

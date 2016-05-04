/*
 * Copyright (c) 2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: i2c_fpga_spiamba.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef I2C_FPGA_SPIAMBA_H_
#define I2C_FPGA_SPIAMBA_H_

#include "board.h"
#include "i2c.h"

const uint32_t c_dataRegMask = 0x000000ff;
const uint32_t c_ctrlRegMask = 0xff000000;

/*----------------------------------------------------------------------------*/
namespace bmc {

    class  I2CFpgaSPIAmba : public I2C {
    public:
        // fpga i2c channel offsets into base address.
        enum i2c_OffsetEnum {
            data = 0, ctrlReg = 0x8, prsVal = 0x10, slaveSel = 0x18,
            outputReg = 0x20, inputReg = 0x28
        };

        I2CFpgaSPIAmba(const Board* a_board, const uint8_t a_i2cDevAddr,
            const uint8_t a_i2cSlvSel = 0, const size_t a_i2c_ambabase = 0xB00000,
            const uint32_t a_i2cFreq = 100);

        virtual bool WaitEndOfDataTransfer();
        virtual bool WaitEndOfDataTransferAndCheckAck();
        virtual void MasterStopTransfer();

    protected:
        virtual void SetSpeed(uint32_t a_speed);
        virtual void SlaveSelect(uint8_t a_slvSelect);

        virtual void WriteData(uint8_t a_data);
        virtual void ReadData(uint8_t &a_data);
        virtual void WriteCtrl(uint32_t a_ctrl);

    private:
        const Board* m_board;
        const size_t m_i2cbase;
    };
}
#endif /* I2C_FPGA_SPIAMBA_H_ */

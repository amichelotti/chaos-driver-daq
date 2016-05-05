/*
 * Copyright (c) 2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: i2c_fpga_amba.h 11980 2011-06-15 10:36:13Z matej.kenda $
 */

#ifndef I2C_FPGA_AMBA_H_
#define I2C_FPGA_AMBA_H_

#include "i2c.h"

/* fpga i2c channel memory map */
typedef struct _tI2cChannel {
/* offset + 0x00 */ struct {
                        uint64_t reserved : 56;
                        uint64_t rtx : 8;
                    } data;
/* offset + 0x08 */ uint64_t ctrl;
/* offset + 0x10 */ struct {
                        uint64_t reserved : 47;
                        uint64_t prsVal   : 16;
                    } speed;
/* offset + 0x18 */ struct {
                        uint64_t reserved : 63;
                        uint64_t slave    :  1;
                    } slaveSel;
} tI2cChannel;


namespace bmc {

    class  I2CFpgaAmba : public I2C {

    public:

        I2CFpgaAmba(volatile tI2cChannel *a_reg,
                    const uint8_t         a_i2cDevAddr,
                    const uint8_t         a_i2cSlvSel,
                    const uint32_t        a_i2cFreq);

        virtual bool WaitEndOfDataTransfer();
        virtual bool WaitEndOfDataTransferAndCheckAck();
        virtual void MasterStopTransfer();

        static size_t  Size() { return sizeof(tI2cChannel); }

    protected:
        virtual void SetSpeed(uint32_t a_speed);
        virtual void SlaveSelect(uint8_t a_slvSelect);

        virtual void WriteData(uint8_t a_data);
        virtual void ReadData(uint8_t &a_data);
        virtual void WriteCtrl(uint64_t a_ctrl);

    private:
        /* interface to FPGA PCI memory */
        volatile tI2cChannel  *m_reg;
    };
}
#endif /* I2C_FPGA_AMBA_H_ */

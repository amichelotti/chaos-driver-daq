/*
 * Copyright (c) 2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: i2c.h 11980 2011-06-15 10:36:13Z matej.kenda $
 */

#ifndef I2C_H_
#define I2C_H_

#include <memory>

const int32_t c_i2cBusyFlagCheckMaxCnt(10);
const int32_t c_ambaFreqMhz(125);

/*-----------------------------------------------------------------------------*/

/* fpga i2c channel ctrl reg. flags */
#define CTRL_REG_START_SEQ              (1<<31)
#define CTRL_REG_STOP_SEQ               (1<<30)
#define CTRL_REG_DATE_READ_WRITE        (1<<29)
#define CTRL_REG_MASTER_WRITE_ACK_FLAG  (1<<28)
#define CTRL_REG_STATUS_FLAG_BUSY       (1<<27)
#define CTRL_REG_STATUS_FLAG_ACK        (1<<26)

/* i2c protocol flags */
#define I2C_WRITE_READ_BIT       0x1

typedef enum _tI2cCmd_e {
    eI2cWrite = 0x0,
    eI2cRead  = 0x1
} tI2cCmd_e;


/*-----------------------------------------------------------------------------*/

namespace bmc {

    class I2C
    {
    public:
        void Init();

        void MasterAddrSlaveDevice(tI2cCmd_e a_cmd);
        void MasterWrite(uint8_t * a_dataPtr, uint8_t a_length);
        void MasterRead(uint8_t * a_dataPtr, uint8_t a_length);

        virtual bool WaitEndOfDataTransfer() = 0;
        virtual bool WaitEndOfDataTransferAndCheckAck() = 0;
        virtual void MasterStopTransfer() = 0;

        I2C(const uint8_t         a_i2cDevAddr,
            const uint8_t         a_i2cSlvSel,
            const uint32_t        a_i2cFreq);

        virtual ~I2C();

    protected:

        /*-----------------------------------------------------------------------------*/
        /* Set speed                                                                   */
        virtual void SetSpeed(uint32_t a_speed) = 0;
        /*-----------------------------------------------------------------------------*/
        /* Select slave                                                                */
        virtual void SlaveSelect(uint8_t a_slvSelect) = 0;
        /*-----------------------------------------------------------------------------*/
        /* Write data to data.rtx register                                             */
        virtual void WriteData(uint8_t a_data) = 0;
        /*-----------------------------------------------------------------------------*/
        /* Read data from data.rtx register                                            */
        virtual void ReadData(uint8_t &a_data) = 0;
        /*-----------------------------------------------------------------------------*/
        /* Write data to data.rtx register                                             */
        virtual void WriteCtrl(uint64_t a_ctrl) = 0;

    protected:
        uint8_t                m_i2cDevAddr;
        uint8_t                m_i2cSlvSel;
        uint32_t               m_freqKhz;
    };

    typedef std::shared_ptr<I2C> I2CPtr;

} //namespace

#endif /* I2C_H_ */

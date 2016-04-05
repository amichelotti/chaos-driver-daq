/*
 * Copyright (c) 2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: i2c_bus_controller.h 19990 2014-01-07 09:35:55Z damijan.skvarc $
 */

#ifndef I2C_BUS_CONTROLLER_H_
#define I2C_BUS_CONTROLLER_H_

#include "bus_controller.h"

/*-----------------------------------------------------------------------------*/

/* fpga i2c channel ctrl reg. flags */
const uint32_t c_ctrlRegStartSeq                = (1<<31);
const uint32_t c_ctrlRegStopSeq                 = (1<<30);
const uint32_t c_ctrlRegDateReadWrite           = (1<<29);
const uint32_t c_ctrlRegMasterWriteNotAckFlag   = (1<<28);
const uint32_t c_ctrlRegStatusFlagBusy          = (1<<27);
const uint32_t c_ctrlRegStatusFlagAck           = (1<<26);

/* i2c protocol flags */
const uint32_t  c_i2cWriteReadBit               = 0x1;

/*-----------------------------------------------------------------------------*/

namespace bmc {

    class I2cBusController : public BusController
    {
    public:
        I2cBusController(const std::string &a_name, uint32_t a_capacity);
        virtual ~I2cBusController();

    protected:
        /* implementation of pure virtual functions from bmc::BusController class */
        virtual void  Write(uint8_t a_reg, uint64_t a_data);
        virtual void  Read(uint8_t a_reg, uint64_t &a_data);
        virtual void  SetBusParams(const bmc::BusParamsPtr &a_params);
        virtual void  SelectDevice(const bmc::BusDevice &a_device, bmc::tDevSelectMode_e a_select);
        virtual bool  ToggleChipSelect() { return false; }

        /* the remaining pure virtual function IsIdle() need to be implemented in derived classes */

        /* declare additional pure virtual functions, to be implemented in derived classes */
        virtual void  OnSetSpeed(uint32_t a_speed)       = 0;
        virtual void  OnSelectBus(uint8_t a_bus_address) = 0;
        virtual void  OnWriteCtrl(uint32_t a_ctrl)       = 0;
        virtual void  OnWriteData(uint8_t a_data)        = 0;
        virtual void  OnReadData(uint8_t &a_data)        = 0;

    protected:
        uint8_t       m_dev_address;   /* device address, defined from SetBusParams() callback */
        uint8_t       m_bus_address;   /* bus address, defined from SetBusParams() callback    */
    };

} //namespace

#endif /* I2C_BUS_CONTROLLER_H_ */

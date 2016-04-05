/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * Silabs Si5338 i2c device - iapp.
 *
 * $Id: i2c_SI5338.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef I2C_SI5338_H
#define I2C_SI5338_H

#include "iapp/libera_i2c_bus_device.h"
#include "iapp/libera_i2c_bus_controller.h"

#include "bmc/i2c_device_SI5338.h"

namespace iapp {

    class LiberaBoard;

    class SI5338 : public LiberaI2cBusDevice
    {
    public:
        SI5338(LiberaI2cBusController  &a_parent,
              const std::string       &a_name,
              bmc::BusDeviceId         a_id,
              uint8_t                  a_device_address,
              uint8_t                  a_bus_address);
        virtual ~SI5338();

        bmc::Si5338&   Device() { return m_device; }

    protected:
        virtual void  OnRegistryAdd(ireg::TreeNodePtr &parent);
        virtual void  OnPostConfig();

    private:
        bmc::Si5338       &m_device;

        /* registry nodes */

        /* device state */
        ireg::TreeNodePtr  m_deviceState;

        /* input selector & dividers */
        ireg::TreeNodePtr  m_inputClkSelNode;
        ireg::TreeNodePtr  m_inputDivP1Node;
        ireg::TreeNodePtr  m_inputDivP2Node;

        /* pll */
        ireg::TreeNodePtr  m_PllKphi;
        ireg::TreeNodePtr  m_VcoGain;
        ireg::TreeNodePtr  m_RSel;
        ireg::TreeNodePtr  m_BwSel;
        ireg::TreeNodePtr  m_MCal;

        /* multisynth */
        ireg::TreeNodePtr  m_divMsP1Node;
        ireg::TreeNodePtr  m_divMsP2Node;
        ireg::TreeNodePtr  m_divMsP3Node;

        /* output dividers */
        ireg::TreeNodePtr  m_divRxNode;

        /* input clk & pll status nodes */
        ireg::TreeNodePtr  m_clkIn1ClkStatNode;
        ireg::TreeNodePtr  m_clkIn2ClkStatNode;
        ireg::TreeNodePtr  m_clkPllStatNode;

    };

} //namespace

#endif // I2C_SI5338_H

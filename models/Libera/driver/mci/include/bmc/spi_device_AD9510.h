/*
 * Copyright (c) 2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: spi_device_AD9510.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef SPI_DEVICE_AD9510_H_
#define SPI_DEVICE_AD9510_H_

#include "spi_device.h"

#define   REG_SPI_CONTROL       0x00U    /* Serial Control Port Configuration */
#define   REG_DIVIDER_A         0x04U    /* A counter/divider                 */
#define   REG_DIVIDER_B_MSB     0x05U    /* B counter/divider MSB             */
#define   REG_DIVIDER_B_LSB     0x06U    /* B counter/divider LSB             */
#define   REG_LOR               0x07U    /* Loss of reference                 */
#define   REG_MUX               0x08U    /* MUX, CP & PFD control             */
#define   REG_CNTRESET          0x09U    /* Counters reset & CP current       */
#define   REG_POWER_PRESCALER   0x0aU    /* Power modes & prescaler           */
#define   REG_DIVIDER_R_MSB     0x0bU    /* R counter/divider MSB             */
#define   REG_DIVIDER_R_LSB     0x0cU    /* R counter/divider LSB             */
#define   REG_DLD               0x0dU    /* Digital Lock Detect               */

#define   REG_DELAY_CONTROL_5   0x34U    /* delay control @OUT5 pin           */
#define   REG_RAMP_5            0x35U    /* ramp current/capacitor @OUT5 pin  */
#define   REG_DELAY_ADJUST_5    0x36U    /* delay fine adjust @OUT5 pin       */
#define   REG_DELAY_CONTROL_6   0x38U    /* delay control @OUT6 pin           */
#define   REG_RAMP_6            0x39U    /* ramp current/capacitor @OUT6 pin  */
#define   REG_DELAY_ADJUST_6    0x3AU    /* delay fine adjust @OUT6 pin       */

#define   REG_LVPECL_0          0x3CU    /* LVPCELK driver     @OUT0 pin      */
#define   REG_LVPECL_1          0x3DU    /*                    @OUT1 pin      */
#define   REG_LVPECL_2          0x3EU    /*                    @OUT2 pin      */
#define   REG_LVPECL_3          0x3FU    /*                    @OUT3 pin      */

#define   REG_LVDSCMOS_4        0x40U    /* LVDS/CMOS driver   @OUT4 pin      */
#define   REG_LVDSCMOS_5        0x41U    /*                    @OUT5 pin      */
#define   REG_LVDSCMOS_6        0x42U    /*                    @OUT6 pin      */
#define   REG_LVDSCMOS_7        0x43U    /*                    @OUT7 pin      */

#define   REG_CLK_CONTROL       0x45U    /* Clock control                     */

#define   REG_OUT0_DUTY         0x48U    /* OUT0 Duty cycle control           */
#define   REG_OUT1_DUTY         0x4aU    /* OUT1 Duty cycle control           */
#define   REG_OUT2_DUTY         0x4cU    /* OUT2 Duty cycle control           */
#define   REG_OUT3_DUTY         0x4eU    /* OUT3 Duty cycle control           */
#define   REG_OUT4_DUTY         0x50U    /* OUT4 Duty cycle control           */
#define   REG_OUT5_DUTY         0x52U    /* OUT5 Duty cycle control           */
#define   REG_OUT6_DUTY         0x54U    /* OUT6 Duty cycle control           */
#define   REG_OUT7_DUTY         0x56U    /* OUT7 Duty cycle control           */

#define   REG_OUT0_BYPASS       0x49U    /* OUT0 Bypass & phase control       */
#define   REG_OUT1_BYPASS       0x4bU    /* OUT0 Bypass & phase control       */
#define   REG_OUT2_BYPASS       0x4dU    /* OUT0 Bypass & phase control       */
#define   REG_OUT3_BYPASS       0x4fU    /* OUT0 Bypass & phase control       */
#define   REG_OUT4_BYPASS       0x51U    /* OUT0 Bypass & phase control       */
#define   REG_OUT5_BYPASS       0x53U    /* OUT0 Bypass & phase control       */
#define   REG_OUT6_BYPASS       0x55U    /* OUT0 Bypass & phase control       */
#define   REG_OUT7_BYPASS       0x57U    /* OUT0 Bypass & phase control       */

#define   REG_FUNCTION          0x58U    /* FUNCTION pin select & SYNC        */
#define   REG_UPDATE_REGS       0x5aU    /* Chip double buffer flip           */


/*-----------------------------------------------------------------------------*/

namespace bmc {

    /* class declaration */
    class AD9510 : public SpiDevice
    {
    public:

        enum tDivider {
            eDividerA,
            eDividerB,
            eDividerR
        };

        enum tPllPowerDown {
            eNormal             = 0,
            eAsynchronous       = 1,
            eNormalAlternative  = 2,
            eSynchronous        = 3
        };

        struct ConfigurationItem {
            uint8_t addr;
            uint8_t value;
        };

        typedef std::function<bool()>  HardResetFnc;
        typedef std::function<bool()>  PostInitFnc;

        AD9510(BusController &a_controller, const std::string &a_name, BusDeviceId a_id, const ConfigurationItem *a_conf);
        virtual ~AD9510();

        bool     SetDivider(const enum tDivider& a_divider, const uint32_t& a_val);
        bool     GetDivider(const enum tDivider& a_divider, uint32_t& a_val);

        void     SetRegister(const uint32_t a_reg, const uint32_t& a_val);
        void     GetRegister(const uint32_t a_reg, uint32_t& a_val);
        void     Flush();

        void     SetPllPowerDown(tPllPowerDown a_mode);
        void     GetPllPowerDown(tPllPowerDown &a_mode);

        void     SetSpiControl(bool  a_SdoInactive, bool  a_LsbFirst, bool  a_softReset, bool  a_16bit);
        void     GetSpiControl(bool &a_SdoInactive, bool &a_LsbFirst, bool &a_softReset, bool &a_16bit);

        void     ReloadConfiguration();
        bool     AssureCommunication();
        bool     Reset();

        void     SetHardResetFnc(HardResetFnc a_fnc);
        void     SetPostInitFnc(PostInitFnc a_fnc);

    protected:

    private:
        const ConfigurationItem  *m_conf;
        HardResetFnc              m_hardResetFnc;
        PostInitFnc               m_postInitFnc;
    };
}

#endif /* SPI_DEVICE_AD9510_H_ */

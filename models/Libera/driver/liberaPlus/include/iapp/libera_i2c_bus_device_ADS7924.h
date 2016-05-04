/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: libera_i2c_bus_device_ADS7924.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef LIBERA_I2C_BUS_DEVICE_ADS7924_H
#define LIBERA_I2C_BUS_DEVICE_ADS7924_H

#include "bmc/i2c_device_ADS7924.h"

#include "declarations.h"
#include "libera_i2c_bus_device.h"
#include "libera_i2c_bus_controller.h"

namespace iapp {

    class LiberaBoard;

    /*-----------------------------------------------------------------------*/
    /* internal class,  used to reduce the code handling registry interface  */

    class ADS7924Channel
    {
    public:
        ADS7924Channel(bmc::ADS7924 &a_device, bmc::ADS7924::tChannel a_channel) :
            m_device(a_device),
            m_channel(a_channel) {}
        ~ADS7924Channel() {}

        void DefineRegistry(ireg::TreeNodePtr a_parent);

    private:
        bool GetData(uint32_t& a_val) const;

        bool GetLowerLimit(uint32_t& a_val) const;
        bool SetLowerLimit(const uint32_t& a_val);

        bool GetUpperLimit(uint32_t& a_val) const;
        bool SetUpperLimit(const uint32_t& a_val);

        bool GetAlarmEnabled(bool &a_val) const;
        bool SetAlarmEnabled(const bool &a_val);

        bool GetAlarmStatus(bool &a_val) const;

        bmc::ADS7924           &m_device;
        bmc::ADS7924::tChannel  m_channel;
    };

    /*-----------------------------------------------------------------------*/
    /* main class for exposing registry interface to ADS7924 i2c device      */

    class ADS7924 : public LiberaI2cBusDevice
    {
    public:
        enum tSleepTime {  /* controls the amount of time to sleep */
            eSleep625us     = 625,
            eSleep1250us    = 1250,
            eSleep2500us    = 2500,
            eSleep5ms       = 5000,
            eSleep10ms      = 10000,
            eSleep20ms      = 20000,
            eSleep40ms      = 40000,
            eSleep80ms      = 80000,
            eSleep160ms     = 160000,
            eSleep320ms     = 320000,
            eSleep640ms     = 640000,
            eSleep1280ms    = 1280000,
            eSleep2560ms    = 2560000
        };

        static const size_t c_num_of_sleep_times;

        enum tCalibrationChannel {  /* controls which channel is output on MUXOUT pin  */
            eCalibCh0  = bmc::ADS7924::eCh0,
            eCalibCh1  = bmc::ADS7924::eCh1,
            eCalibCh2  = bmc::ADS7924::eCh2,
            eCalibCh3  = bmc::ADS7924::eCh3,
            eCalibGnd
        };

        ADS7924(LiberaI2cBusController  &a_parent,
                const std::string       &a_name,
                bmc::BusDeviceId         a_id,
                uint8_t                  a_device_address,
                uint8_t                  a_bus_address);
        virtual ~ADS7924();

        inline bool GetMode(bmc::ADS7924::tMode& a_val) const                     { return _GetMode(a_val); }
        inline bool GetSleepTime(tSleepTime& a_val) const                         { return _GetSleepTime(a_val); }
        inline bool GetAcquireTime(bmc::ADS7924::tAcquireTime& a_val) const       { return _GetAcquireTime(a_val); }
        inline bool GetConversionMode(bmc::ADS7924::tConversionMode& a_val) const { return _GetConversionMode(a_val); }
        inline bool GetCalibrationChannel(tCalibrationChannel& a_val) const       { return _GetCalibrationChannel(a_val); }
        inline bool GetLastAcquiredChannel(bmc::ADS7924::tChannel& a_val) const   { return _GetLastAcquiredChannel(a_val); }
        inline bool GetPwrPinPolarity(bmc::ADS7924::tPwrPinPolarity& a_val) const { return _GetPwrPinPolarity(a_val); }
        inline bool GetPwrPinEnable(bool& a_val) const                            { return _GetPwrPinEnable(a_val); }
        inline bool GetPwrUpTime(bmc::ADS7924::tPwrUpTime& a_val) const           { return _GetPwrUpTime(a_val); }

        void SetMode(const bmc::ADS7924::tMode& a_val);
        void SetSleepTime(const tSleepTime& a_val);
        void SetAcquireTime(const bmc::ADS7924::tAcquireTime& a_val);
        void SetConversionMode(const bmc::ADS7924::tConversionMode& a_val);
        void SetCalibrationChannel(const tCalibrationChannel& a_val);
        void SetPwrPinPolarity(const bmc::ADS7924::tPwrPinPolarity& a_val);
        void SetPwrPinEnable(const bool& a_val);
        void SetPwrUpTime(const bmc::ADS7924::tPwrUpTime& a_val);

        bool Reset();
        bmc::ADS7924& Device() { return m_device; }

    protected:
        virtual void  OnRegistryAdd(ireg::TreeNodePtr &parent);

    private:
        bool _GetLastAcquiredChannel(bmc::ADS7924::tChannel& a_val) const;

        bool _GetMode(bmc::ADS7924::tMode& a_val) const;
        bool _SetMode(const bmc::ADS7924::tMode& a_val);

        bool _GetSleepTime(tSleepTime& a_val) const;
        bool _SetSleepTime(const tSleepTime& a_val);

        bool _GetAcquireTime(bmc::ADS7924::tAcquireTime& a_val) const;
        bool _SetAcquireTime(const bmc::ADS7924::tAcquireTime& a_val);

        bool _GetConversionMode(bmc::ADS7924::tConversionMode& a_val) const;
        bool _SetConversionMode(const bmc::ADS7924::tConversionMode& a_val);

        bool _GetCalibrationChannel(tCalibrationChannel& a_val) const;
        bool _SetCalibrationChannel(const tCalibrationChannel& a_val);

        bool _GetPwrPinPolarity(bmc::ADS7924::tPwrPinPolarity& a_val) const;
        bool _SetPwrPinPolarity(const bmc::ADS7924::tPwrPinPolarity& a_val);
        bool _GetPwrPinEnable(bool& a_val) const;
        bool _SetPwrPinEnable(const bool& a_val);
        bool _GetPwrUpTime(bmc::ADS7924::tPwrUpTime& a_val) const;
        bool _SetPwrUpTime(const bmc::ADS7924::tPwrUpTime& a_val);

        bool _GetIntAlarmCounter(bmc::ADS7924::tAlarmCounter& a_val) const;
        bool _SetIntAlarmCounter(const  bmc::ADS7924::tAlarmCounter& a_val);
        bool _GetIntPinConfig(bmc::ADS7924::tIntPinConfig& a_val) const;
        bool _SetIntPinConfig(const  bmc::ADS7924::tIntPinConfig& a_val);
        bool _GetIntPolarity(bmc::ADS7924::tIntPolarity& a_val) const;
        bool _SetIntPolarity(const  bmc::ADS7924::tIntPolarity& a_val);
        bool _GetIntTrigMode(bmc::ADS7924::tIntTriggeringMode& a_val) const;
        bool _SetIntTrigMode(const  bmc::ADS7924::tIntTriggeringMode& a_val);

        bool _GetDeviceId(uint32_t& a_val) const;

        bmc::ADS7924       &m_device;
        ADS7924Channel      m_ch0;
        ADS7924Channel      m_ch1;
        ADS7924Channel      m_ch2;
        ADS7924Channel      m_ch3;

        ireg::TreeNodePtr   m_device_id_node;
        ireg::TreeNodePtr   m_mode_node;
        ireg::TreeNodePtr   m_acqu_node;
        ireg::TreeNodePtr   m_calib_channel_node;
        ireg::TreeNodePtr   m_sleep_time_node;
        ireg::TreeNodePtr   m_conv_mode_node;
        ireg::TreeNodePtr   m_acquire_time_node;

        ireg::TreeNodePtr   m_pwr_pin_polarity_node;
        ireg::TreeNodePtr   m_pwr_pin_enable_node;
        ireg::TreeNodePtr   m_pwr_up_time_node;

        ireg::TreeNodePtr   m_int_alarm_counter_node;
        ireg::TreeNodePtr   m_int_mode_node;
        ireg::TreeNodePtr   m_int_polarity_node;
        ireg::TreeNodePtr   m_int_trig_mode_node;

        tCalibrationChannel m_calib_channel;
    };

} //namespace


#endif // LIBERA_I2C_BUS_DEVICE_ADS7924_H

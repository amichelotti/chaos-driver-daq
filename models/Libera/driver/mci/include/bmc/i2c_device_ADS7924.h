/*
 * Copyright (c) 2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: i2c_device_ADS7924.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef I2C_DEVICE_ADS7924_H
#define I2C_DEVICE_ADS7924_H

#include "i2c_device.h"

/*-----------------------------------------------------------------------------*/

namespace bmc {


    class ADS7924 : public I2cDevice
    {
    public:
        enum tMode {
            eIdle            = 0x00,  /* idle mode (default)                 */
            eAwake           = 0x20,  /* awake mode                          */
            eManualSingle    = 0x30,  /* manual single mode                  */
            eManualScan      = 0x32,  /* manual scan mode                    */
            eAutoSingle      = 0x31,  /* auto single mode                    */
            eAutoScan        = 0x33,  /* auto scan mode                      */
            eAutoSingleSleep = 0x39,  /* auto single with sleep mode         */
            eAutoScanSleep   = 0x3B,  /* auto scan with sleep mode           */
            eAutoBurstSleep  = 0x3F   /* auto burst with sleep               */
        };

        enum tChannel {
            eCh0             = 0x0,  /* channel 0  */
            eCh1             = 0x1,  /* channel 1  */
            eCh2             = 0x2,  /* channel 2  */
            eCh3             = 0x3   /* channel 3  */
        };

        enum tChannelFlag {   /* used for setting and checking alarm status */
            eChFlag0             = 0x1,
            eChFlag1             = 0x2,
            eChFlag2             = 0x4,
            eChFlag3             = 0x8
        };

        enum tAlarmCounter {   /* used for setting and checking alarm status */
            eAlarmEvery   = 0x0,  /* Every conversion generates an alarm */
            eAlarmCount1  = 0x1,  /* Exceeding the threshold limit 1 time generates an alarm condition */
            eAlarmCount2  = 0x2,  /* Exceeding the threshold limit 2 time generates an alarm condition */
            eAlarmCount3  = 0x3,  /* Exceeding the threshold limit 3 time generates an alarm condition */
            eAlarmCount4  = 0x4,  /* Exceeding the threshold limit 4 time generates an alarm condition */
            eAlarmCount5  = 0x5,  /* Exceeding the threshold limit 5 time generates an alarm condition */
            eAlarmCount6  = 0x6,  /* Exceeding the threshold limit 6 time generates an alarm condition */
            eAlarmCount7  = 0x7   /* Exceeding the threshold limit 7 time generates an alarm condition */
        };

        enum tIntPinConfig {   /* used to handle   INT pin configuration and     Conversion Control Event      */
            eIntAlarmEventAlarm           = 0x0, /* Alarm                         Alarm                         */
            eIntBusyEventAlarm            = 0x1, /* Busy                          Alarm                         */
            eIntSingleConvEventSingleConv = 0x2, /* one conversion completed      one conversion complete       */
            eIntBusyEventSingleConv       = 0x3, /* Busy                          one conversion complete       */
            eIntOff                       = 0x4, /* unused                                                      */
            eIntUnused                    = 0x5, /* unused                                                      */
            eIntAllConvEventAllConv       = 0x6, /* all four conversions complete all four conversions complete */
            eIntBusyEventAllConv          = 0x7  /* Busy                          all four conversions complete */
        };

        enum tIntPolarity {
            eIntActiveLow   = 0x0,
            eIntActiveHigh  = 0x1
        };

        enum tIntTriggeringMode {
            eIntLevel       = 0x0,
            eIntEdge        = 0x1
        };

        enum tSleepTime {  /* controls the amount of time to sleep */
            eSleep2_5ms     = 0x0,
            eSleep5ms       = 0x1,
            eSleep10ms      = 0x2,
            eSleep20ms      = 0x3,
            eSleep40ms      = 0x4,
            eSleep80ms      = 0x5,
            eSleep160ms     = 0x6,
            eSleep320ms     = 0x7
        };

        enum tConversionMode { /* determines the conversion status after a conversion control event        */
            eConversionContinuous = 0x0,  /* Conversions continue, independent of the control event status */
            eConversionSingle     = 0x1   /* Conversions are stopped as soon as a control event occurs;    */
                                          /* the event must be cleared to resume conversions               */
        };

        enum tAcquireTime {  /* controls the amount of time to acquire the signal before a conversion */
            eAcqTime6us     = 0x0,
            eAcqTime8us     = 0x1,
            eAcqTime10us    = 0x2,
            eAcqTime12us    = 0x3,
            eAcqTime14us    = 0x4,
            eAcqTime16us    = 0x5,
            eAcqTime18us    = 0x6,
            eAcqTime20us    = 0x7,
            eAcqTime22us    = 0x8,
            eAcqTime24us    = 0x9,
            eAcqTime26us    = 0xA,
            eAcqTime28us    = 0xB,
            eAcqTime30us    = 0xC,
            eAcqTime32us    = 0xD,
            eAcqTime34us    = 0xE,
            eAcqTime36us    = 0xF
        };

        enum tPwrMuxoutMode {  /* controls the output signal on MUXOUT pin */
            eMuxOutChannel  = 0x0,  /* signal from multiplexer is directed to MUXOUT pin */
            eMuxOutGnd      = 0x1   /* MUXOUT pin is grPCA9532ounded                            */
        };

        enum tPwrPinPolarity {  /* controls the polarity of PWRCON pin on power up  */
            ePwrPinActiveLow   = 0x0,  /* default */
            ePwrPinActiveHigh  = 0x1
        };

        enum tPwrPinMode {  /* controls the polarity of PWRCON pin on power up  */
            ePwrPinDisabled  = 0x0,  /* default */
            ePwrPinEnabled  = 0x1
        };

        enum tPwrUpTime {  /* controls the  power-up time  */
            ePwrUpTime0us    = 0x0,
            ePwrUpTime2us    = 0x1,
            ePwrUpTime4us    = 0x2,
            ePwrUpTime6us    = 0x3,
            ePwrUpTime8us    = 0x4,
            ePwrUpTime10us   = 0x5,
            ePwrUpTime12us   = 0x6,
            ePwrUpTime14us   = 0x7,
            ePwrUpTime16us   = 0x8,
            ePwrUpTime18us   = 0x9,
            ePwrUpTime20us   = 0xA,
            ePwrUpTime22us   = 0xB,
            ePwrUpTime24us   = 0xC,
            ePwrUpTime26us   = 0xD,
            ePwrUpTime28us   = 0xE,
            ePwrUpTime30us   = 0xF
        };

        ADS7924(BusController &a_controller,
                std::string    a_name,
                BusDeviceId    a_id,
                uint8_t        a_device_address,
                uint8_t        a_bus_address);

        virtual ~ADS7924();

        tMode     GetMode() const;
        void      SetMode(tMode a_val);

        void      GetAlarmStatus(uint8_t &a_curr, uint8_t &a_startup) const;
        uint8_t   GetAlarmEnabled() const;
        void      SetAlarmEnabled(uint8_t a_flags);

        void      GetAlarmStatus(tChannel a_channel, bool &a_curr, bool &a_startup) const;
        bool      GetAlarmEnabled(tChannel a_channel) const;
        void      SetAlarmEnabled(tChannel a_channel, const bool &a_val);

        tChannel  GetLastAcquiredChannel() const;
        void      SetChannelOnMuxOutput(tChannel a_val);
        uint16_t  GetConversionData(tChannel a_channel);

        uint8_t   GetLowerLimit(tChannel a_channel);
        void      SetLowerLimit(tChannel a_channel, uint8_t a_val);

        uint8_t   GetUpperLimit(tChannel a_channel);
        void      SetUpperLimit(tChannel a_channel, uint8_t a_val);


        void      GetInterruptConfiguration(tAlarmCounter      &a_alarm_counter,
                                            tIntPinConfig      &a_pin_config,
                                            tIntPolarity       &a_polarity,
                                            tIntTriggeringMode &a_trig_mode) const;

        void      SetInterruptConfiguration(tAlarmCounter      a_alarm_counter,
                                            tIntPinConfig      a_pin_config,
                                            tIntPolarity       a_polarity,
                                            tIntTriggeringMode a_trig_mode);

        void      GetSleepConfiguration(tSleepTime       &a_sleep_time,
                                        bool             &a_four_times_divider,
                                        bool             &a_eight_times_multiplier,
                                        tConversionMode  &a_conv_mode) const;

        void      SetSleepConfiguration(tSleepTime       a_sleep_time,
                                        bool             a_four_times_divider,
                                        bool             a_eight_times_multiplier,
                                        tConversionMode  a_conv_mode);
        size_t    GetSleepTime() const;

        void      GetAcquireConfiguration(tAcquireTime   &a_sleep_time) const;
        void      SetAcquireConfiguration(tAcquireTime   a_sleep_time);

        void      GetPwrUpConfiguration(tPwrMuxoutMode    &a_calib_signal,
                                        tPwrPinPolarity   &a_pin_polarity,
                                        tPwrPinMode       &a_pin_mode,
                                        tPwrUpTime        &a_pwr_up_time) const;

        void      SetPwrUpConfiguration(tPwrMuxoutMode    a_calib_signal,
                                        tPwrPinPolarity   a_pin_polarity,
                                        tPwrPinMode       a_pin_mode,
                                        tPwrUpTime        a_pwr_up_time);

        uint8_t   GetDeviceId();
        void      Reset();


    protected:

    private:
        uint8_t       GetLowerLimitRegister(tChannel a_channel);
        uint8_t       GetUpperLimitRegister(tChannel a_channel);
        tChannelFlag  GetChannelFlag(tChannel a_channel) const { return static_cast<tChannelFlag>(0x1 << a_channel); }

        uint8_t   m_alarm_enabled;
    };
}

#endif // I2C_DEVICE_ADS7924_H

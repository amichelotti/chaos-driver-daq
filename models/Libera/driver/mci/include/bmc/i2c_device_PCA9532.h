/*
 * Copyright (c) 2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: i2c_device_PCA9532.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef I2C_DEVICE_PCA9532_H
#define I2C_DEVICE_PCA9532_H

#include "i2c_device.h"

/*-----------------------------------------------------------------------------*/

namespace bmc {


    class PCA9532 : public I2cDevice
    {
    public:
        static const uint32_t c_num_of_pins;

        /* see data sheet for clock frequency */
        enum tRegister {
           eInput0     = 0U,   /* input register 0        */
           eInput1     = 1U,   /* input register 1        */
           ePrescaler0 = 2U,   /* frequency prescaler 0   */
           ePwm0       = 3U,   /* PWM register 0          */
           ePrescaler1 = 4U,   /* frequency prescaler 0   */
           ePwm1       = 5U,   /* PWM register 1          */
           eLS0        = 6U,   /* LED0  to LED3 selector  */
           eLS1        = 7U,   /* LED4  to LED7 selector  */
           eLS2        = 8U,   /* LED8  to LED11 selector */
           eLS3        = 9U    /* LED12 to LED15 selector */
        };

        enum tMode {
            eOff    = 0,   /* output is set high-impedance (LED off; default) */
            eOn     = 1,   /* output is set LOW (LED on) */
            eBlink0 = 2,   /* output blinks at PWM0 rate */
            eBlink1 = 3    /* output blinks at PWM1 rate */
        };

        PCA9532(BusController &a_controller,
                std::string    a_name,
                BusDeviceId    a_id,
                uint8_t        a_device_address,
                uint8_t        a_bus_address);

        virtual ~PCA9532();

        uint8_t GetState0()  const           {  return Get(eInput0); }
        uint8_t GetState1()  const           {  return Get(eInput1); }

        uint8_t GetPrescaler0() const        {  return Get(ePrescaler0); }
        void    SetPrescaler0(uint8_t a_val) {  Set(ePrescaler0,a_val);  }

        uint8_t GetPrescaler1() const        {  return Get(ePrescaler1); };
        void    SetPrescaler1(uint8_t a_val) {  Set(ePrescaler1,a_val);  }

        uint8_t GetPWM0() const              {  return Get(ePwm0); }
        void    SetPWM0(uint8_t a_val)       {  Set(ePwm0,a_val);  }

        uint8_t GetPWM1() const              {  return Get(ePwm1); }
        void    SetPWM1(uint8_t a_val)       {  Set(ePwm1,a_val);  }

        void    GetModes(tRegister a_register, tMode &a_mode3, tMode &a_mode2, tMode &a_mode1, tMode &a_mode0) const;
        void    SetModes(tRegister a_register, tMode  a_mode3, tMode  a_mode2, tMode  a_mode1, tMode  a_mode0);

        size_t  GetOperationModes(tMode *a_values, const size_t a_pos, const size_t a_size) const;
        bool    SetOperationModes(const tMode* a_values, const size_t a_pos, const size_t a_size);

    protected:

    private:
    };
}

#endif // I2C_DEVICE_PCA9532_H

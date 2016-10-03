/*
 * Copyright (c) 2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: i2c_device_SI57x.h 14606 2011-12-21 12:43:41Z matej.kenda $
 */


/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * Programmable Interrupt Controller registers (PIC)
 *
 * $Id $
 */

#ifndef SI57X_H
#define SI57X_H

#include "i2c_device.h"

/*-----------------------------------------------------------------------------*/

/* I2C Si57x chip address. */
#define VCXO_I2C_ADDRESS        0x55

/* output freq */
#define MAX_OUTPUT_FREQ_HZ 945000000
#define MIN_OUTPUT_FREQ_HZ  10000000

/* VCXO start-up output frequency after power-up is set to 100,00 MHz. */
#define STARTUP_FREQUENCY       100000000.0

/* DCO freq range */
#define LOW_DCO_FREQUENCY       (4.85 * 1e9)
#define HIGH_DCO_FREQUENCY      (5.67 * 1e9)

/* Si57x reg. addresses */
#define SI57X_REG_7     7U
#define SI57X_REG_8     8U
#define SI57X_REG_9     9U
#define SI57X_REG_10   10U
#define SI57X_REG_11   11U
#define SI57X_REG_12   12U
#define SI57X_REG_135 135U
#define SI57X_REG_137 137U


#define REG135_NEWFREQ    (1<<6)
#define REG135_FREEZE_M   (1<<5)
#define REG135_RECALL     (1<<0)

#define REG137_FREEZE_DCO (1<<4)

/*-----------------------------------------------------------------------------*/

namespace bmc {

    class Si57x : public I2cDevice
    {
    public:
        Si57x(BusController  &a_controller,
                std::string   a_name,
                BusDeviceId   a_id,
                uint8_t       a_device_address,
                uint8_t       a_bus_address);
        virtual ~Si57x();

        /* VCXO divider parameters: hs_div, n1 and RFREQ. */
        struct dividerData {
            int hsdiv_reg;
            int n1div_reg;
            int hsdiv;
            int n1div;
            unsigned long long lrfreq;
            double dfdco;
        };

        static const uint8_t c_i2cAddr;

        /* vcxo */
        bool CheckVcxoPresence(void);
        bool SetVcxoFreq(const uint32_t& a_val);
        bool GetVcxoFreq(uint32_t& a_val) const;
        uint64_t GetVcxoConfigParams(void) const;

    protected:
        virtual void OnEnabled();
        virtual void OnDisabled();

        virtual void OnDeviceDisable() {};
        virtual void OnDeviceEnable() {};

    private:
        uint32_t m_newFreqHz;
        uint64_t m_silabsCfg;

        /* Si57x ctrl. */
        bool RecallNvmVcxo(void);

        bool ReadRfreq(struct dividerData *a_data);
        bool CalcNewRfreq (unsigned long long a_lfrequency,
                struct dividerData *a_oldDiv, struct dividerData *a_new_div);
        bool WriteRfreq(struct dividerData *a_data);
    };
}

#endif // GDX_SI57X_H

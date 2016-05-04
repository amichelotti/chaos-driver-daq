/*
 * Copyright (c) 2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: spi_device_LMH6518.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef SPI_DEVICE_LMH_6518_H_
#define SPI_DEVICE_LMH_6518_H_

#include "spi_device.h"

/*-----------------------------------------------------------------------------*/

namespace bmc {

    /* class declaration */
    class LMH6518 : public SpiDevice
    {
    public:

        enum tFilterBandwidth {
            eFiltFull  = 0,
            eFilt20    = 1,
            eFilt100   = 2,
            eFilt200   = 3,
            eFilt350   = 4,
            eFilt650   = 5,
            eFilt750   = 6
        };

        enum tAttenuator {
            eAtt0   = 0,
            eAtt2   = 1,
            eAtt4   = 2,
            eAtt6   = 3,
            eAtt8   = 4,
            eAtt10  = 5,
            eAtt12  = 6,
            eAtt14  = 7,
            eAtt16  = 8,
            eAtt18  = 9,
            eAtt20  = 10
        };

        LMH6518(BusController &a_controller, const std::string &a_name, BusDeviceId a_id, bool a_write_only);
        virtual ~LMH6518();

        bool    Get(bool &a_full_power, tFilterBandwidth &a_filter, bool &a_hi_gain, tAttenuator &a_att);
        bool    Set(bool  a_full_power, tFilterBandwidth  a_filter, bool  a_hi_gain, tAttenuator  a_att);

        bool    GetRaw(uint16_t &a_val);
        bool    SetRaw(uint16_t a_val);

        bool    GetFullPower(bool &a_val);
        bool    SetFullPower(bool a_val);

        bool    GetHiGain(bool &a_val);
        bool    SetHiGain(bool a_val);

        bool    GetFilterBandWidth(tFilterBandwidth &a_val);
        bool    SetFilterBandWidth(const tFilterBandwidth &a_val);

        bool    GetAttenuation(tAttenuator &a_val);
        bool    SetAttenuation(const tAttenuator &a_val);

    protected:

    private:
        bool     m_write_only;  /* in certain application SPI bus can be used just for writing */
        uint16_t m_raw_data;    /* cached value, used on "write_only" SPI bus implementations  */
    };
}


#endif /* SPI_DEVICE_LMH_6518_H_ */

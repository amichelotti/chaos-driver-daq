/*
 * Copyright (c) 2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: spi_device.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef SPI_DEVICE_H_
#define SPI_DEVICE_H_

#include <thread>
#include <bits/shared_ptr.h>

#include "bus_device.h"
#include "bus_controller.h"
#include "spi_bus_params.h"

/*-----------------------------------------------------------------------------*/

namespace bmc {

    /* forward delarations */
    class SpiDevice;

    /* our data types */
    typedef std::shared_ptr<SpiDevice> SpiDevicePtr;

    /* class declaration */
    class SpiDevice : public BusDevice
    {
    public:
        /* prevent copy and assign operators */
        SpiDevice(const SpiDevice& a_other) = delete;
        SpiDevice& operator = (const SpiDevice& a_other) = delete;

        /* allow shared pointer classes to access our private destructor */
        friend class std::__shared_count<>;
        friend class std::_Sp_counted_ptr<SpiDevice*,std::__default_lock_policy>;

        /* shortcuts, since the registry adress is not used */
        void  Write(const uint64_t &a_val) { BusDevice::Write(0, a_val); }
        void  Read(uint64_t &a_val)        { BusDevice::Read(0, a_val);  }
        void  Transfer(const uint64_t *a_wBuf, uint32_t a_wLen,
                       uint64_t *a_rBuf, uint32_t a_rLen)
                       { BusDevice::Transfer(0, a_wBuf, a_wLen, a_rBuf, a_rLen);  }
    protected:
        SpiBusParams& GetSpiBusParams() { return dynamic_cast<bmc::SpiBusParams&>(*(GetBusParams().get())); }
        SpiDevice(BusController &a_controller, const std::string &a_name, BusDeviceId a_id);
        virtual ~SpiDevice();

    private:
    };

} //namespace


#endif /* SPI_DEVICE_H_ */

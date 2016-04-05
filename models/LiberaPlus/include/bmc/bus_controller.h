/*
 * Copyright (c) 2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: bus_controller.h 19990 2014-01-07 09:35:55Z damijan.skvarc $
 */

#ifndef BUS_CONTROLLER_H_
#define BUS_CONTROLLER_H_

#include <map>

#include <iostream>
#include "istd/trace.h"
#include "bus_device.h"

/*-----------------------------------------------------------------------------*/
namespace bmc
{
    const BusDeviceId c_undefined_device(-1);
    const uint32_t    c_prescaler_factor(125000); /* refer to fpga amba clock  */

    /* forward class declarations */
    class BusController;
    class BusDevice;

    /* our types */
    typedef std::map<BusDeviceId, BusDevicePtr>             BusDevices;
    typedef std::function<void(BusDevice&,uint32_t,void*)>  ForEachDeviceFnc;
    typedef std::function<void(BusDevice&)>                 OnAddDeviceFnc;
    typedef std::shared_ptr<BusController>                  BusControllerPtr;

    typedef enum _tDevSelectMode_e {
        eDevSelRead,
        eDevSelWrite,
        eDevSelDisabled
    } tDevSelectMode_e;

    /* forward declarations of factory function for creating bus controller instances  */
    template<typename CNTRL_TYPE, typename ...ARGS>
        BusControllerPtr Create(
            const std::string   &a_name,
            uint32_t            a_capacity,
            ARGS&&              ...a_args);

    /* forward declarations of factory function for creating bus device instances  */
    template<typename DEV_TYPE, typename ...ARGS>
        BusDevicePtr Create(
            BusControllerPtr    a_bus_controller,
            const std::string   &a_name,
            BusDeviceId         a_id,
            ARGS&&              ...a_args);


    /*-------------------------------------------------------------------------- */
    /* friend class to assure proper locking over multiple read/write operations */
    /* on specified bus controller                                               */

    class BusControllerLocker
    {
    public:
        BusControllerLocker(BusController &a_controller);
        virtual ~BusControllerLocker();
    private:
        BusController&                          m_controller;
        std::unique_lock<std::recursive_mutex>  m_lock;
    };


    /*---------------------------------------------------------------------------*/
    /* BusController class definition                                            */

    class BusController : public std::enable_shared_from_this<BusController>
    {
    public:
        void              Transfer(const BusDevice &a_device, uint8_t a_reg,
                                   const uint64_t* a_wBuff, uint32_t a_wLength,
                                   uint64_t* a_rBuff, uint32_t a_rLength);
        void              Write(const BusDevice &a_device, uint8_t a_reg, const uint64_t * a_buff, uint32_t a_length);
        void              Read(const BusDevice &a_device, uint8_t a_reg, uint64_t * a_buff, uint32_t a_length);
        uint32_t          NumOfDevices()                         { return m_devices.size(); }
        void              SetTimeout(uint32_t a_timeout)         { m_timeout = a_timeout; }
        uint32_t          GetTimeout()                           { return m_timeout;  }
        uint32_t          GetCapacity()                          { return m_capacity; }
        std::string&      GetName()                              { return m_name;     }
        BusControllerPtr  SharedPtr()                            { return this->shared_from_this(); }

        void              SetOnAddDeviceFnc(OnAddDeviceFnc a_fnc)  { m_on_add_dev_fnc = a_fnc; }
        uint32_t          ForEachDevice(ForEachDeviceFnc a_fnc, const size_t a_from=0, const size_t a_count=-1, void* a_data=NULL);
        uint32_t          CalcPrescaleFactor(uint32_t a_freq_khz);

        bool              SetOutput(uint32_t  a_val);
        bool              GetOutput(uint32_t &a_val);

    protected:
        BusController(const std::string &a_name, uint32_t a_capacity = 1,  bool a_writeBeforeRead = false);
        virtual ~BusController();

        friend class BusControllerLocker;

        /* allow Create() method to call AddDevice() function */
        template<typename DEV_TYPE, typename ...ARGS>
            friend BusDevicePtr Create(
                    BusControllerPtr    a_bus_controller,
                    const std::string   &a_name,
                    BusDeviceId         a_id,
                    ARGS&&              ...a_args);

        /* allow Create() method to instantiate BusController */
        template<typename CNTRL_TYPE, typename ...ARGS>
            friend BusControllerPtr Create(
                const std::string   &a_name,
                uint32_t            a_capacity,
                ARGS&&              ...a_args);

        /* publish WaitUntilDone function to derived classes */
        bool            WaitUntilDone(bool a_wait_ack);

        /* event functions */
        virtual void    OnAddDevice(const BusDevice &a_device) {}
        virtual bool    OnSetOutput(uint32_t  a_val) { return false; }
        virtual bool    OnGetOutput(uint32_t &a_val) { return false; }

        /* interface functions */
        virtual void    Write(uint8_t a_reg, uint64_t a_data)  = 0;
        virtual void    Read(uint8_t a_reg, uint64_t &a_data)  = 0;
        virtual void    SetBusParams(const BusParamsPtr &a_params) = 0;
        virtual void    SelectDevice(const BusDevice &a_device, tDevSelectMode_e a_select) = 0;
        virtual bool    IsIdle() = 0;
        virtual bool    IsAcknowledged() = 0;
        virtual bool    ToggleChipSelect() = 0;

    private:
        void            Shutdown();
        void            AddDevice(BusDevicePtr a_device);
        void            AdjustBusParams(const BusDevice &a_device);
        void            WaitForIdle();

        std::string     m_name;             /* bus controller name                            */
        uint32_t        m_capacity;         /* max number of devices on the bus               */
        uint32_t        m_prescaler_factor; /* factor, used for mapping clock frequency,      */
                                            /* expressed in Khz into prescaler value, which   */
                                            /* is used to directly adapt clock on HW          */
        std::recursive_mutex m_mutex;       /* control access between devices                 */
        BusDevices      m_devices;          /* list of devices on the same bus                */
        BusDeviceId     m_active_id;        /* id of device, whose characteristic are         */
                                            /* applied to communication parameters            */
        uint32_t        m_timeout;          /* ms timeout while waiting for idle state        */
                                            /* value 0 indicates timeout is not observed      */
        bool            m_writeBeforeRead;  /*                                                */
        OnAddDeviceFnc  m_on_add_dev_fnc;   /* callback function, called when device is added */
    };


    /*-----------------------------------------------------------------------------*/
    /* factory function for instantiation of BusController class instances         */

    template<typename CNTRL_TYPE, typename ...ARGS>
    BusControllerPtr Create(const std::string &a_name, uint32_t a_capacity, ARGS&& ...a_args)
    {
        istd_FTRC();

        CNTRL_TYPE* obj = new CNTRL_TYPE(a_name, a_capacity, std::forward<ARGS>(a_args)...);
        try {
            BusControllerPtr p(obj);
            return p;
        }
        catch (...) {
            throw;
        }
    }


    /*-----------------------------------------------------------------------------*/
    /* factory function for instantiation of BusDevice class instances             */

    template<typename DEV_TYPE, typename ...ARGS>
    BusDevicePtr Create(BusControllerPtr a_bus_controller, const std::string &a_name, BusDeviceId a_id, ARGS&& ...a_args)
    {
        istd_FTRC();
        BusController& busController = *(a_bus_controller.get());
        BusDevicePtr obj(new DEV_TYPE(busController, a_name, a_id, std::forward<ARGS>(a_args)...) );
        busController.AddDevice(obj);
        return obj;
    }


} //namespace


#endif /* BUS_CONTROLLER_H_ */

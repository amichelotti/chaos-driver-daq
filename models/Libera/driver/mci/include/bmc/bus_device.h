/*
 * Copyright (c) 2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: bus_device.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef BUS_DEVICE_H_
#define BUS_DEVICE_H_

#include <thread>
#include <bits/shared_ptr.h>

#include "bus_params.h"

/*-----------------------------------------------------------------------------*/
namespace bmc {

    /* forward declarations */
    class BusController;
    class BusDevice;
    class BusParams;

    /* our data types */
    typedef std::shared_ptr<BusDevice>      BusDevicePtr;
    typedef uint8_t                         BusDeviceId;
    typedef std::shared_ptr<BusController>  BusControllerPtr;

    enum BusDeviceState_e {
        eDisabled,
        eEnabled
    };

    typedef std::function<void(BusDevice&)>  OnSetStateFnc;

    /* forward declarations of factory function for creating bus device instances  */
    template<typename DEV_TYPE, typename ...ARGS>
        BusDevicePtr Create(
            BusControllerPtr    a_bus_controller,
            const std::string   &a_name,
            BusDeviceId         a_id,
            ARGS&&              ...a_args);

    /* class declaration */
    class BusDevice : public std::enable_shared_from_this<BusDevice>
    {
    public:
        void               Transfer(uint32_t a_reg,
                                    const uint64_t *a_wBuf, uint32_t a_wLen,
                                    uint64_t *a_rBuf, uint32_t a_rLen);
        void               Write(uint32_t a_reg, const uint64_t *a_buf, uint32_t a_len);
        void               Read(uint32_t a_reg, uint64_t *a_buf, uint32_t a_len) const;
        void               Write(uint32_t a_reg, const uint64_t &a_val);
        void               Read(uint32_t a_reg, uint64_t &a_val) const;

        void               SetDeviceState(BusDeviceState_e a_state);
        BusDeviceId        GetId() const                           { return m_id;                     }
        const std::string  GetName() const                         { return m_name;                   }
        const BusParamsPtr GetBusParams() const                    { return m_bus_params;             }
        void               SetBusParams(BusParamsPtr a_bus_params) { m_bus_params = a_bus_params;     }
        BusDevicePtr       SharedPtr()                             { return this->shared_from_this(); }
        void               SetOnSetStateFnc(OnSetStateFnc a_fnc)   { m_on_set_state_fnc = a_fnc;      }

        /* prevent copy and assign operators */
        BusDevice(const BusDevice& a_other) = delete;
        BusDevice& operator = (const BusDevice& a_other) = delete;

    protected:
        template<typename OBJ_TYPE, typename T, typename ...ARGS>
            friend BusDevicePtr Create(
                    BusControllerPtr    a_bus_controller,
                    const std::string   &a_name,
                    BusDeviceId         a_id,
                    ARGS&&              ...a_args);

        BusDevice(BusController &a_controller, const std::string &a_name, BusDeviceId a_id);
        virtual ~BusDevice();

        BusController&     Controller() { return m_controller; }

        /* expose event functions */
        virtual void OnDisabled() {};
        virtual void OnEnabled()  {};
        virtual void OnShutdown() {};

    private:
        friend class       BusController;

        void               Shutdown();
        void               Disable();
        void               Enable();


        BusController&     m_controller;
        std::string        m_name;
        BusDeviceId        m_id;
        BusParamsPtr       m_bus_params;
        BusDeviceState_e   m_state;
        OnSetStateFnc      m_on_set_state_fnc;
    };


    template<typename OBJ_TYPE, typename T, typename ...ARGS>
        BusParamsPtr SetBusParam(T &a_bus_device, ARGS&& ...a_args)
        {
            BusDevice& busDevice = dynamic_cast<BusDevice&>(a_bus_device);

            OBJ_TYPE* obj = new OBJ_TYPE(std::forward<ARGS>(a_args)...);

            try {
                BusParamsPtr p(obj);
                busDevice.SetBusParams(p);
                return p;
            }
            catch (...) {
                delete obj;
                throw;
            }
        }

} //namespace


#endif /* BUS_DEVICE_H_ */

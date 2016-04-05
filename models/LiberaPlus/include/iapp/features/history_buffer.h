/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * Interface to FPGA HistoryBuffer module.
 *
 * This module is used to control access to RAM component into which data is stored.
 *
 * Usually only signal data is stored into RAM component, where RAM component can be
 * split into blocks, being capable for storing several signals. In most application
 * only one signal is stored into RAM.
 *
 * The most common operation over RAM component is to start/stop storing data into RAM.
 * Each signal, which is stored into RAM, has its own control register (called "mode"
 * register), which is used to start/stop signal acquisition. In order to stop a signal,
 * eForceDisable value must be put into control register. On the other side, a signal
 * can be started in two different way. eStartImmediatelly specifies signal acquisition
 * should start immediately, while eStartOnEvent specifies signal acquisition should start
 * when appropriate event is generated by EventGenerationModule. This event is usually
 * fired from final application.
 *
 * Different applications might have different structure of HistoryBuffer module. A common
 * resource to all different applications of histry_buffer modules is the "mode" register
 * whose address is specified as an argument to the class constructor. Since there might
 * exist several signals, the contructor is implemented as template function, allowing to
 * specified more control addresses. Actually, each signal configuration item is constituted
 * from the address of mode register and start mode (eStartImmediatelly, eStartOnEvent)
 *
 * $Id: history_buffer.h 17673 2012-09-12 10:51:01Z damijan.skvarc $
 */

#ifndef __IAPP_HISTORY_BUFFER_H
#define __IAPP_HISTORY_BUFFER_H

#include "iapp/libera_fpga_object.h"
#include "iapp/libera_board.h"


/*-----------------------------------------------------------------------------*/

namespace ifeature {

class HistoryBuffer : public iapp::LiberaFeature
{
public:
    enum flags {
        eStartImmediatelly = 1,
        eForceDisable      = 2,
        eStartOnEvent      = 4,
        eStopOnEvent       = 8
    };

    struct tControl {
        volatile uint64_t *address;
        flags              enable_mode;
        tControl(volatile uint64_t *a_address, flags a_flags);
        tControl(const tControl& a_other);
        bool IsEnabled();
    };

    template <typename First, typename... Rest>
    HistoryBuffer(iapp::LiberaBoard &a_board, const std::string &a_name,
                  First first, Rest... rest) :
        iapp::LiberaFeature(a_board, a_name)
    {
        SubscribeControl(first, rest...);
    }

    virtual void  ForceEnable();
    virtual void  AssureEnable();
    virtual void  Disable();
    virtual bool  IsEnabled();

protected:
    virtual void OnPostConfig();

private:
    template<typename First, typename... Rest>
    void SubscribeControl(const First& first, const Rest&... rest)
    {
        m_controls.push_back(first);
        SubscribeControl(rest...);
    }
    void SubscribeControl() {}

private:
    std::mutex             m_mutex;           /* control access over m_mode_regs */
    std::vector<tControl>  m_controls;        /* list of mode registers          */
};

}

#endif // __IAPP_HISTORY_BUFFER_H

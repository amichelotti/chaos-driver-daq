/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: tim_board.h 16165 2012-05-21 09:12:40Z damijan.skvarc $
 */

#ifndef TIM_BOARD_H
#define TIM_BOARD_H

#include "bmc/uri.h"
#include "ireg/declarations.h"
#include "ireg/notification_client.h"
#include "isig/libera_event_stream.h"
#include "iapp/libera_board.h"
#include "iapp/features/tim_trigger_generation.h"
#include "iapp/features/local_timing.h"
#include "iapp/features/events.h"
#include "tim_id_specific.h"

namespace iapp {
    class LiberaSpiBusController;
    class DAC8552;
    class CDCE72010;
}

/*-----------------------------------------------------------------------------*/
/* Tim board                                                                   */

class McPll;

namespace iboard {

class TimBoard : public iapp::LiberaBoard
{
public:
    enum mc_oscEnum { vcxoUndef = 0, vcxoA = 1, vcxoB = 2 };
    enum sc_srcEnum { scUndef = 0, scInternal = 1, scExternal };
    enum mc_srcEnum { mcUndef = 0, mcExternal = 1, mcDivout };

    static const ifeature::Events::EventDescription c_evtDesc;

    TimBoard(iapp::LiberaApplication &a_application, const bmc::Board* a_board,
            const ifeature::Events::EventDescription                    &a_evtDesc       = c_evtDesc,
            const ifeature::TimTriggerGeneration::TriggerConfigurations &a_trigConfig    = ifeature::TimTriggerGeneration::c_trigConfigurations,
            const ifeature::TimTriggerGeneration::RegistryConfiguration *a_propConfig    = NULL,
            const bool                                                   a_hasTrigSwitch = false);

    virtual ~TimBoard();

    ifeature::TimTriggerGeneration* GetTriggerGeneration() { return m_trigGeneration; };
    const TimIdSpecific&            GetIdSpecific()        { return *m_tim_id; };
    isig::LiberaEventStream*        GetEventStream()       { return m_eventStream; };
    iapp::DAC8552*                  GetDac()               { return m_DAC8552; };
    McPll*                          GetMcPll()             { return m_mc_pll; };

    void   CreateTrigEvent();
    void   SetTrigSourceExt();
    void   SetTrigSourceOff();

    double GetADCFrequency() const;

    void   SetTrigDebouncer(const uint32_t a_val);
    void   GetTrigDebouncer(uint32_t &a_val);

    void   SetTrigPrescaler(const uint32_t a_val);
    void   GetTrigPrescaler(uint32_t &a_val);

    bool   GetMCCfg(mc_oscEnum &a_val) const;
    bool   GetMCSrc(mc_srcEnum &a_val) const;
    bool   GetSCSrc(sc_srcEnum &a_val) const;

    ireg::TreeNodePtr SignalsNode() { return m_signalsNode; }

protected:
    virtual void OnRegistryAdd(ireg::TreeNodePtr &parent);
    virtual void OnPostInit();
    virtual void OnPostConfig();
    virtual void OnShutdown();
    virtual bool OnQueryChild(const LiberaObject &a_child);

private:
    void        ResetTriggerEventCounter();
    std::string GetDeviceName();
    uint16_t    FpgaSpiRead(uint16_t a_reg) const;
    void        FpgaSpiWrite(uint16_t a_reg, uint16_t a_value) const;

    /* LiberaFpgaObject */
    TimIdSpecific                  *m_tim_id;
    ifeature::TimTriggerGeneration *m_trigGeneration;
    ifeature::LocalTiming          *m_ltim;
    ifeature::Events               *m_events;
    McPll                          *m_mc_pll;

    iapp::LiberaSpiBusController   *m_spi_clk_ctrl;
    iapp::LiberaSpiBusController   *m_spi_dac_ctrl;

    /* device, attached on spi clock controller, used for pll control loop*/
    iapp::DAC8552                  *m_DAC8552;
    iapp::CDCE72010                *m_CDCE72010;

    isig::LiberaEventStream*        m_eventStream;

    /* registry nodes */
    ireg::TreeNodePtr               m_signalsNode;
};

} // iboard namespace

#endif // TIM_BOARD_H

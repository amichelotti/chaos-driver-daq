/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: evrx_board.h 16165 2012-05-21 09:12:40Z matej.kenda $
 */

#ifndef EVRX_BOARD_H
#define EVRX_BOARD_H

#include "bmc/uri.h"
#include "ireg/declarations.h"
#include "ireg/notification_client.h"
#include "isig/libera_event_stream.h"
#include "iapp/libera_board.h"
#include "iapp/libera_spi_bus_controller.h"
#include "iapp/libera_spi_bus_device_DAC8552.h"
#include "iapp/features/tim_trigger_generation.h"
#include "iapp/features/local_timing.h"
#include "iapp/boards/tim/tim_board.h"

#include "iapp/libera_i2c_bus_controller.h"
#include "iapp/libera_i2c_bus_device_SI57x.h"

#include "multi_gigabit_transciever_node.h"

/*-----------------------------------------------------------------------------*/
/* EVRX monitoring stream                                                      */

struct EvrxStruct {
    uint64_t code;      ///< Unique event receiver code (16-bits)
    uint64_t timestamp; ///< Time stamp of emitted event.
};

typedef isig::SignalTraits<int64_t, EvrxStruct> EvrxTraits;
typedef isig::LiberaStream<EvrxTraits>          EvrxStream;


namespace iboard {

/*-----------------------------------------------------------------------------*/
/* EVRX Timing board                                                           */

class EvrxBoard : public TimBoard
{
public:
    EvrxBoard(iapp::LiberaApplication &a_application, const bmc::Board* a_board,
            const ifeature::Events::EventDescription                    &a_evtDesc     =  iboard::TimBoard::c_evtDesc,
            const ifeature::TimTriggerGeneration::TriggerConfigurations &a_trigConfig  =  ifeature::TimTriggerGeneration::c_trigConfigurations,
            const ifeature::TimTriggerGeneration::RegistryConfiguration *a_propConfig  =  NULL);
    ~EvrxBoard();

    bool SetSFPFreq(const uint32_t& a_val);
    bool SetBackplaneT0Switch(const BackplaneLineT0_e& a_val);

protected:
    virtual void OnRegistryAdd(ireg::TreeNodePtr &parent);
    virtual void OnPostConfig();
    virtual void OnShutdown();

private:

    // Note: This i2c controller should be moved to TimBoard
    // if the same controller will be used for both VCXO devices.
    iapp::LiberaI2cBusController  *m_i2c_ctrl;
    iapp::SI57x                   *m_SI57x;
    uint32_t                       m_sfp_freq;           /* SFP frequency    */

    MultiGigabitTranscieverNode   *m_mgt;
    EvrxStream*  m_evrxStream;
};

} // namespace

#endif // EVRX_BOARD_H

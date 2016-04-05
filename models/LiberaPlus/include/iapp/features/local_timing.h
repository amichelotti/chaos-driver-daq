/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * Programmable Interrupt Controller registers (PIC)
 *
 * $Id: local_timing.h 14766 2012-01-23 13:45:58Z matej.kenda $
 */

#ifndef LOCAL_TIMING_H
#define LOCAL_TIMING_H

#include "iapp/libera_feature.h"
#include "iapp/libera_board.h"
#include "isig/libera_event_stream.h"



namespace ifeature {


class LocalTiming : public iapp::LiberaFeature
{
public:
    LocalTiming(
        iapp::LiberaBoard       &a_board,
        const std::string       &a_name,
        isig::LiberaEventStream *a_stream);

    virtual ~LocalTiming();

    void SetDefault();

    void DisableEvent(isig::EventIdType a_eventId);
    void EnableEvent(isig::EventIdType a_eventId);
    void IncrementLMT(uint64_t a_offsetValue);

protected:
    virtual void OnRegistryAdd(ireg::TreeNodePtr &parent);

private:
    isig::LiberaEventClient m_evtClient;
};


} //ifeature namespace

#endif // LOCAL_TIMING_H

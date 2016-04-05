/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * LocalTimingSync class extends common LocalTiming class with synchronization feature
 *
 * $Id: local_timing_sync.h 14766 2012-01-23 13:45:58Z damijan.skvarc $
 */

#ifndef LOCAL_TIMING_SYNC_H
#define LOCAL_TIMING_SYNC_H

#include "iapp/libera_object.h"
#include "iapp/libera_board.h"
#include "isig/libera_event_stream.h"


#include "local_timing.h"
#include "history_buffer.h"


namespace ifeature {


class LocalTimingSync : public LocalTiming
{
public:
    static const uint64_t c_unsynchronizedLMT;
    static const uint64_t c_min_syncLMT;
    static const uint64_t c_max_syncLMT;

    LocalTimingSync(
        iapp::LiberaBoard        &a_board,
        const std::string        &a_name,
        isig::LiberaEventStream  *a_stream,
        HistoryBuffer            *a_hb);

    void               AnnounceSync(uint64_t a_val);
    bool               IsUnsynchronized();
    void               SetSynchronized();
    std::uint64_t      GetNewLMT();
    void               Synchronize(isig::EventIdType a_id, uint64_t a_timestamp);
    iapp::LiberaBoard &Board() { return m_board; }

protected:

    /* libera base callback functions */
    virtual void       OnRegistryAdd(ireg::TreeNodePtr &a_parent);

    /* callback functions for derived classes to be informed about time stamp adjustments */
    virtual void       OnAnnounced()               {};  /* timestamp change was announced, waiting for trigger      */
    virtual void       OnStartAdjustment()         {};  /* timestamp adjustment are going to be applied             */
    virtual void       OnStopAdjustment(bool a_ok) {};  /* timestamp adjustment stops successfully or with failure  */

private:
    /* registry implementation functions */
    bool              _SetSynchronizeLMT(const std::uint64_t& a_val);
    bool              _GetSynchronizeLMT(std::uint64_t& a_val) const;

private:
    iapp::LiberaBoard         &m_board;
    uint64_t                   m_syncLMT;       /* lmt, which is going to be applied on synchronized event */
    uint64_t                   m_newLMT;
    ireg::TreeNodePtr          m_syncLmtNode;   /* registry node, pointing to m_syncLMT value */
    HistoryBuffer             *m_hb;
    isig::LiberaEventStream   *m_eventStream;
};


} //ifeature namespace

#endif // LOCAL_TIMING_SYNC_H

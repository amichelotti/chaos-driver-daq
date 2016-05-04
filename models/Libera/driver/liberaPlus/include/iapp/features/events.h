/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: events.h 14610 2011-12-21 13:25:23Z damijan.skvarc $
 */


#ifndef EVENTS_H_
#define EVENTS_H_

#include <thread>
#include <map>
#include <condition_variable>
#include <mutex>

#include "iapp/libera_feature.h"
#include "ireg/reg_node.h"
#include "ireg/reg_fnc_node.h"
#include "isig/libera_event_stream.h"


namespace ifeature {
/*----------------------------------------------------------------------------*/
class Events : public iapp::LiberaFeature
{
public:
    typedef std::function<void(const isig::EventIdType a_id, uint64_t a_timestamp, uint64_t a_count)>  EventFnc;

    struct Description {
        isig::EventIdType id;
        const char*       name;
    };

    struct Data {
        uint64_t          timestamp;
        uint64_t          count;
        ireg::TreeNodePtr timestampNode;
        ireg::TreeNodePtr countNode;
    };

    typedef std::map<isig::EventIdType, Data>   EventData;
    typedef std::vector<Description>            EventDescription;

    explicit Events(iapp::LiberaObject       &a_parent,
                    const std::string        &a_name,
                    isig::LiberaEventStream  *a_stream,
                    const EventDescription   &a_description,
                    EventFnc                  a_fnc  = ireg::nilFunc);

    virtual ~Events();

    void operator()();
    void GetLastLMT(isig::EventIdType &a_type, uint64_t &a_time);
    bool GetCurrentLMT(uint64_t& a_time);

protected:
    virtual void   OnRegistryAdd(ireg::TreeNodePtr &parent);
    virtual void   OnShutdown();
    virtual void   OnPostConfig();

private:
    void UpdateData(const isig::LiberaEventStream::Atom& a_event);

    isig::LiberaEventStream   *m_stream;
    std::thread                m_thread;
    std::mutex                 m_mutex;
    EventData                  m_data;
    EventDescription           m_description;
    EventFnc                   m_fnc;
    std::condition_variable    m_condition;
};

}  /* nasmespace */

#endif /* EVENTS_H_ */

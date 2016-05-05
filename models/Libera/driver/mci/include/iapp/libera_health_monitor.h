/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * Health Stream monitor
 *
 * $Id: libera_health_monitor.h 19990 2014-01-07 09:35:55Z damijan.skvarc $
 */

#ifndef _LIBERA_HEALTH_MONITOR_H
#define _LIBERA_HEALTH_MONITOR_H

#include <thread>

#include "bmc/board.h"
#include "iapp/libera_feature.h"
#include "iapp/identification_block.h"
#include "isig/libera_health_stream.h"

/*-----------------------------------------------------------------------------*/
/* class declaration                                                           */

namespace iapp {

    class LiberaHealthMonitor: public iapp::LiberaFeature
    {
    public:
        typedef std::function<void(const uint64_t)>  HealthFnc;

        LiberaHealthMonitor(iapp::LiberaObject&        a_parent,
                            const std::string&         a_name,
                            isig::LiberaHealthStream  *a_stream,
                            uint32_t                   a_mask,
                            HealthFnc                  a_fnc,
                            IdentificationBlock       *a_id_block,
                            bool                       a_enabled=false);

        virtual ~LiberaHealthMonitor();

        void          SetEnabled(bool a_val);

    protected:
        /* libera-base event functions */
        virtual void  OnRegistryAdd(ireg::TreeNodePtr &parent);
        virtual void  OnShutdown();
        virtual void  OnStartRunning();

    private:
        /* thread function */
        void          MonitorFunction();

        /* registry implementation functions */
        bool          _SetEnabled(const bool& a_val);
        bool          _GetEnabled(bool& a_val) const;

        /* private members */
        isig::LiberaHealthStream *m_stream;       /* monitored health stream                 */
        const uint32_t            m_mask;         /* mask bits                               */
        HealthFnc                 m_fnc;          /* callback function                       */
        IdentificationBlock*      m_id_block;     /* used for enabling/disabling health mask */
        bool                      m_enabled;      /* self explaining                         */
        std::thread               m_thread;       /* thread, running the ThreadFunction      */
        uint64_t                  m_counter;      /* counter of of last received health atom */
        uint64_t                  m_status;       /* status of of last received health atom  */

        /* registry interface */
        ireg::TreeNodePtr         m_enable_node;
        ireg::TreeNodePtr         m_status_node;
        ireg::TreeNodePtr         m_counter_node;
    };

}


#endif // _SPECTRA_HEALTH_MONITOR_H

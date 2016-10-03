/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: cpu_stats.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */


#ifndef BMC_CPU_STATS_H_
#define BMC_CPU_STATS_H_

#include <thread>
#include <mutex>

namespace bmc {

    /**
    * @class CpuStats
    *
    * @brief  Singleton class provides information about current CPU usage.
    */
    class CpuStats {

    public:

       /**
        * Initializes the object and starts the thread
        * if it is not started yet.
        */
       static CpuStats &Instance();

       /**
        * Return statistics of CPU in different states.
        */
       void GetValues(float &a_idle, float &a_user, float &a_kernel,
                      float &a_ioWait, float &a_swap, float &a_nice);

       /**
        * Main thread loop is sampling CPU percents every second.
        */
       void operator()();

    private:
        CpuStats();                           // Constructor is private
        ~CpuStats();
        CpuStats(const CpuStats &);           // copy constructor is private
        CpuStats& operator=(const CpuStats&); // assignment operator is private

        void Update();

        std::thread m_t;
        std::mutex  m_m;

        bool m_stop;
        bool m_statGrabInitialized;

        float m_user;
        float m_kernel;
        float m_idle;
        float m_ioWait;
        float m_swap;
        float m_nice;
        bool  m_valueSet;
    };
}

#endif /* BMC_CPU_STATS_H_ */

/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: stop_watch.h 8621 2010-09-14 05:54:19Z matej.kenda $
 */


#ifndef ISTD_STOP_WATCH_H
#define ISTD_STOP_WATCH_H

#include "istd/time.h"

namespace istd {

    /**
     * @addtogroup istd_time
     * @{
     */

    /**
    StopWatch is designed to measure time and calculate statistics about elapsed
    time. General use case is:
    @verbatim
    istd::StopWatch w;
    w.Start();
    // Do some processing
    // Use NextLoop() if several executions of a loop are measured
    // NextLoop() updates the statistics.
    w.Stop();
    // Use statistics: Elapsed(), Min(), Max(), Avg(), ...
    @endverbatim
    */
    class StopWatch
    {
        public:
            StopWatch();
            ~StopWatch() {};

            void Start();

            /** Returns elapsed time of the completed loop.
            If the stop watch is not running yet, then it is started and returns
            epoch time (0).
            */
            const Time& NextLoop();
            void Stop();

            const Time& Elapsed() const
                { return m_elapsed; };

            const Time& Min() const
                { return m_min; };

            const Time& Max() const
                { return m_max; };

            const Time& Avg() const
                { return m_avg; };

            unsigned long long Loops() const
                { return m_loops; };

        private:
            Time    m_start;
            Time    m_elapsed;
            Time    m_loopStart;
            Time    m_loopElapsed;
            bool    m_running;

            unsigned long long  m_loops;
            Time    m_min;
            Time    m_max;
            Time    m_avg;
    };
    /**@}*/

} // namespace

#endif // ISTD_STOP_WATCH_H

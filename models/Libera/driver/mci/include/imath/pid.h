/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: pid.h 8621 2010-09-14 05:54:19Z matej.kenda $
 */

#ifndef PID_H
#define PID_H

#include "imath/filter.h"

namespace imath {

    typedef struct {
        double low;
        double high;
    } limit_t;

    /**
     * @addtogroup controls
     * @{
     */

    /**
     * PID Filter/Controller
     */
    class PIDFilter : public Filter
    {
    public:
        PIDFilter(double a_Ts);
        virtual ~PIDFilter();

        void SetGains(double a_Kp, double a_Ki, double a_Kd, double a_Td = 0.0);
        void GetGains(double& a_Kp, double& a_Ki,
                      double& a_Kd, double& a_Td) const;
        void SetClip(double a_low, double a_high);
        void GetClip(double& a_low, double& a_high) const;
        void SetHysteresis(double a_hyst);
        double GetHysteresis() const;
        void SetDeadZone(double a_dead);
        double GetDeadZone() const;

        double Step(double a_u);

    private:
        double m_Kp;
        double m_Ki;
        double m_Kd;
        double m_Td;
        double m_Ts;

        limit_t m_clip;
        double m_hyst;
        double m_dead;
        double m_last;

        mutable omni_mutex m_gains_x;
    };
    /**@}*/

} // namespace

#endif // PID_H

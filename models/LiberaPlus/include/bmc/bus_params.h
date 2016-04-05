/*
 * Copyright (c) 2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: bus_params.h 19990 2014-01-07 09:35:55Z damijan.skvarc $
 */

#ifndef BUS_PARAMS_H_
#define BUS_PARAMS_H_

#include <thread>
#include <sstream>
#include <bits/shared_ptr.h>

/*-----------------------------------------------------------------------------*/

namespace bmc {

    class BusParams;
    typedef std::shared_ptr<BusParams> BusParamsPtr;

    class BusParams : public std::enable_shared_from_this<BusParams>
    {
    public:
       BusParams(uint32_t a_freq_khz, bool a_pollingMode, bool a_toggleChipSelect = false);
       virtual ~BusParams();
       inline BusParamsPtr SharedPtr()                { return this->shared_from_this(); }
       uint32_t            GetFreqKhz() const         { return m_freq_khz; }
       void                SetFreqKhz(uint32_t a_val) { m_freq_khz = a_val; }
       bool                GetPollingMode() const     { return m_pollingMode; }
       void                SetPollingMode(bool a_val) { m_pollingMode = a_val; }

       bool                GetModified()              { return m_paramsModified; }
       void                SetModified(bool a_val)    { m_paramsModified = a_val; }
       bool                ToggleCS() const           { return m_toggleChipSelect; }

       const std::string ToString() const
       {
           std::ostringstream sstr;
           sstr << "Freq_khz: "     << m_freq_khz << std::endl <<
                   "Polling mode: " << (m_pollingMode ? "enabled" : "disabled") << std::endl;
           return sstr.str();
       }

    private:
       uint32_t m_freq_khz;             /* clock speed, specified in khz frequency           */
       bool     m_pollingMode;
       bool     m_toggleChipSelect;     /* toggle chip select between write & read operation */
       bool     m_paramsModified;       /* par. were modified */
    };
}


#endif /* BUS_PARAMS_H_ */

/*
 * Copyright (c) 2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: spi_bus_params.h 19990 2014-01-07 09:35:55Z damijan.skvarc $
 */

#ifndef SPI_BUS_PARAMS_H_
#define SPI_BUS_PARAMS_H_

#include <thread>
#include <sstream>

#include "bus_params.h"

/*-----------------------------------------------------------------------------*/

namespace bmc {

    /* default values for spi bus parameters */
    const bool       c_lsb_first(false);
    const uint16_t   c_transfer_len(4);
    const bool       c_clock_sample_edge(false);
    const bool       c_clock_transmit_edge(false);
    const bool       c_clock_polarity(false);
    const uint32_t   c_clock_freq_Khz(125000);

    class SpiBusParams : public BusParams
    {
    public:
        SpiBusParams();
        SpiBusParams(bool     a_lsb_first,
                     uint16_t a_transfer_len,
                     bool     a_clock_sample_edge,
                     bool     a_clock_transmit_edge,
                     bool     a_clock_polarity,
                     bool     a_polling_mode,
                     uint16_t a_clock_freq_Khz,
                     bool     a_toggle_chip_select = false);
        virtual ~SpiBusParams();

        bool operator==(const struct SpiBusParams& a_other) const
        {
            return (this->m_lsb_first           == a_other.m_lsb_first) &&
                   (this->m_transfer_len        == a_other.m_transfer_len) &&
                   (this->m_clock_sample_edge   == a_other.m_clock_sample_edge) &&
                   (this->m_clock_transmit_edge == a_other.m_clock_transmit_edge) &&
                   (this->m_clock_polarity      == a_other.m_clock_polarity) &&
                   (this->GetPollingMode()      == a_other.GetPollingMode()) &&
                   (this->GetFreqKhz()          == a_other.GetFreqKhz());
        }

        const std::string  ToString() const
        {
            std::ostringstream sstr;
            sstr << "LSB first: "           << m_lsb_first << std::endl <<
                    "Transfer length: "     << m_transfer_len << std::endl <<
                    "Clock sample edge: "   << m_clock_sample_edge << std::endl <<
                    "Clock transmit edge: " << m_clock_transmit_edge << std::endl <<
                    "Clock polarity: "      << m_clock_polarity << std::endl <<
                    BusParams::ToString();
            return sstr.str();
        }

        bool     LsbFirst()          const { return m_lsb_first;           }
        uint16_t TransferLen()       const { return m_transfer_len;        }
        bool     ClockSampleEdge()   const { return m_clock_sample_edge;   }
        bool     ClockTransmitEdge() const { return m_clock_transmit_edge; }
        bool     ClockPolarity()     const { return m_clock_polarity;      }

        void     SetLsbFirst         (bool     a_val) { if(m_lsb_first!=a_val){SetModified(true);}           m_lsb_first=a_val;           }
        void     SetTransferLen      (uint16_t a_val) { if(m_transfer_len!=a_val){SetModified(true);}        m_transfer_len=a_val;        }
        void     SetClockSampleEdge  (bool     a_val) { if(m_clock_sample_edge!=a_val){SetModified(true);}   m_clock_sample_edge=a_val;   }
        void     SetClockTransmitEdge(bool     a_val) { if(m_clock_transmit_edge!=a_val){SetModified(true);} m_clock_transmit_edge=a_val; }
        void     SetClockPolarity    (bool     a_val) { if(m_clock_polarity!=a_val){SetModified(true);}      m_clock_polarity=a_val;      }

    private:
        bool     m_lsb_first;
        uint16_t m_transfer_len;
        bool     m_clock_sample_edge;
        bool     m_clock_transmit_edge;
        bool     m_clock_polarity;
    };

} //namespace


#endif /* SPI_BUS_PARAMS_H_ */

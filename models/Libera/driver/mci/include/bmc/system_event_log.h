/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: system_event_log.h 8621 2010-09-14 05:54:19Z matej.kenda $
 */

#ifndef BMC_SYSTEM_EVENT_LOG_H
#define BMC_SYSTEM_EVENT_LOG_H

// system headers
#include <string>
#include <vector>

// shared headers
#include "bmc/sel_reading.h"
#include "istd/time.h"

namespace bmc {

    class Board;

    typedef std::vector<bmc::SelReading> SelReading_vec;

	/**
	 * @addtogroup bmc_sel
	 * @{
	 */

    /** Allow the access to the System Event Logging */
    class SystemEventLog {
    public:
        void               UpdateInfo();
        void               Clear();

        SelReading_vec     ReadMany(int a_numOfSelEntries = -1, bool a_reversOrder = false);
        SelReading_vec     ReadMany(const std::vector<int>& a_selIds);
        SelReading         Read(int a_selId);

        uint16_t           GetNumOfEntries() const {return m_entriesNumber;}
        uint16_t           GetFreeSpace() const {return m_freeSpaceInBytes;}
        const istd::Time&  GetTimeLastAdd() const {return m_recentAdditionTimestamp;}
        const istd::Time&  GetTimeLastDel() const {return m_recentEraseTimestmap;}

    private:
        explicit SystemEventLog(bmc::Board& board);
        ~SystemEventLog();

        SelReading         GetEntry(int a_selId);

        Board*          m_board;
        uint16_t        m_entriesNumber;
        uint16_t        m_freeSpaceInBytes;
        istd::Time      m_recentAdditionTimestamp;
        istd::Time      m_recentEraseTimestmap;

        friend class Board;
    };
    /**@}*/
}

#endif /* BMC_SYSTEM_EVENT_LOG_H */

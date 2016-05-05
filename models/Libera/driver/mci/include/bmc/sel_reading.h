 /*
* Copyright (c) 2009 Instrumentation Technologies.
* All Rights Reserved.
*
* $Id: sel_reading.h 8621 2010-09-14 05:54:19Z matej.kenda $
*/

#ifndef SEL_READING_H_
#define SEL_READING_H_

// system headers
#include <string>

// shared headers
#include "istd/time.h"

extern "C" {
    struct platformb_event_record;
}

namespace bmc {

    class SystemEventLog;

	/**
	 * @addtogroup bmc_sel System Event Log handling Classes and functions
	 * @{
	 */

    /** Allow the storage of SEL read*/
    class SelReading {
    public:
        SelReading();

        uint16_t            GetRecordId() const {return m_recordId;}
        uint8_t             GetRecordType() const {return m_recordType;}
        const istd::Time&   GetTimeStamp() const {return m_timestamp;}
        const std::string&  GetMsgDescription() const {return m_code;}
        uint16_t            GetMsgId() const {return m_msgId;}
        const std::string&  GetMsgParam() const {return m_msgParam;}
        const std::string&  GetMsgSeverity() const {return m_msgSeverity;}
        bool                GetValid() const {return m_valid;}

    private:
        explicit SelReading(const SystemEventLog& a_sel, platformb_event_record& a_event);

        void GetSelEntry(platformb_event_record * a_evt);
        void GetSelEntryOemType(platformb_event_record * a_evt);

        const SystemEventLog*   m_sel;
        uint16_t                m_recordId;
        uint8_t                 m_recordType;
        istd::Time              m_timestamp;
        std::string             m_code;
        uint16_t                m_msgId;
        std::string             m_msgParam;
        std::string             m_msgSeverity;
        bool                    m_valid;

        friend class SystemEventLog;
    };
    /**@}*/
}

#endif /* SEL_READING_H_ */

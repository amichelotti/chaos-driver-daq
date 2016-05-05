/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: ostream_helpers.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef BMC_OSTREAM_HELPERS_H
#define BMC_OSTREAM_HELPERS_H

#include <ostream>
#include "bmc/types.h"

namespace bmc {

    class Board;
    class SensorReading;
    class Uri;
    class SystemEventLog;
    class SelReading;
    struct BoardConfiguration;
    struct SpiBusParams;

    std::ostream& operator<< (std::ostream &a_os, const BoardType_e &bt);
    std::ostream& operator<< (std::ostream &a_os, const FirmwareType_e &bt);
    std::ostream& operator<< (std::ostream &a_os, const BoardStatus_e &bs);
    std::ostream& operator<< (std::ostream &a_os, const BoardPowerStatus_e &bps);
    std::ostream& operator<< (std::ostream &a_os, const RedLedStatus_e &a_rls);
    std::ostream& operator<< (std::ostream &a_os, const Board &b);
    std::ostream& operator<< (std::ostream &a_os, const Sensor &sens);
    std::ostream& operator<< (std::ostream &a_os, const SensorLimit_e &sl);
    std::ostream& operator<< (std::ostream &a_os, const SensorType_e &st);
    std::ostream& operator<< (std::ostream &a_os, const SensorReading &sr);
    std::ostream& operator<< (std::ostream &a_os, const Uri &uri);
    std::ostream& operator<< (std::ostream &a_os, const SystemEventLog &sel);
    std::ostream& operator<< (std::ostream &a_os, const SelReading &selReading);
    std::ostream& operator<< (std::ostream &a_os, const BoardConfiguration &a_bCfg);
    std::ostream& operator<< (std::ostream &a_os, const SpiBusParams &a_params);

    std::istream& operator>> (std::istream &a_is, BoardType_e &bt);

    std::string ToString (const appId_subsysId_t& a_sid);

} // namespace

std::ostream& operator<< (std::ostream &a_os, const hwidentification& a_hwid);
std::ostream& operator<< (std::ostream &a_os, const cfg_header& a_hdr);

std::ostream& operator<< (std::ostream &a_os, const hwRevisions_e& a_bps);
std::ostream& operator<< (std::ostream &a_os, const applicationId_e& a_id);

std::istream& operator>> (std::istream &a_is, hwRevisions_e& hr);
std::istream& operator>> (std::istream &a_is, applicationId_e& ai);


/**@}*/

#endif // BMC_OSTREAM_HELPERS_H

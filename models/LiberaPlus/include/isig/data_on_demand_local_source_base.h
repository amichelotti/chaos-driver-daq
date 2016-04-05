/*
 * Copyright (c) 2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: data_on_demand_local_source_base.h 19024 2013-04-24 08:30:24Z damijan.skvarc $
 */

#ifndef DATA_ON_DEMAND_LOCAL_SOURCE_BASE_H_
#define DATA_ON_DEMAND_LOCAL_SOURCE_BASE_H_

#include "isig/data_on_demand_source_base.h"
#include "isig/dod_controller_interface.h"

namespace isig {

    /**
     * @addtogroup signal Signals and Streams handling Classes and functions
     * @{
     * @param DOD_TRAITS  type traits of one record in the atom
     * @param a_name      signal name
     * @param a_traits    actual description of signal atom
     * @see SignalTraits
     */
    template <class DOD_TRAITS>
        class DataOnDemandLocalSourceBase:
            public DataOnDemandSourceBase<DOD_TRAITS>,
            public DodControllerInterface<DOD_TRAITS> {

        public:
            DataOnDemandLocalSourceBase(const std::string &a_name,
                                        const DOD_TRAITS& a_traits = DOD_TRAITS(),
                                        const std::string &a_owner = "") :
                DataOnDemandSourceBase<DOD_TRAITS>(a_name, a_traits, a_owner),
                DodControllerInterface<DOD_TRAITS>()
            {
                istd_FTRC();
            };

        protected:
            /* callback function, called when a system has been announced to change timestamp */
            virtual void OnAnnouncedTimestampChange()
            {
                DodControllerInterface<DOD_TRAITS>::SetAllEnable(false);   /* disable all controllers */
            }

            /* callback function, called when a system has adjusted timestamp */
            virtual void OnAdjustedTimestamp(int64_t a_diff_lmt, uint64_t a_start_lmt)
            {
                DodControllerInterface<DOD_TRAITS>::ResetAllControllers(a_start_lmt);   /* a_start_lmt is timestamp of first signal atom */
                DodControllerInterface<DOD_TRAITS>::SetAllEnable(true);                 /* enabled all controllers                       */
            }
        };

    // DataOnDemandSourceBase for runtime traits are instantiated in the library
    extern template class DataOnDemandLocalSourceBase<SignalTraitsVarUint8>;
    extern template class DataOnDemandLocalSourceBase<SignalTraitsVarInt16>;
    extern template class DataOnDemandLocalSourceBase<SignalTraitsVarUint16>;
    extern template class DataOnDemandLocalSourceBase<SignalTraitsVarInt32>;
    extern template class DataOnDemandLocalSourceBase<SignalTraitsVarUint32>;
    extern template class DataOnDemandLocalSourceBase<SignalTraitsVarInt64>;
    extern template class DataOnDemandLocalSourceBase<SignalTraitsVarUint64>;
    extern template class DataOnDemandLocalSourceBase<SignalTraitsVarFloat>;
    extern template class DataOnDemandLocalSourceBase<SignalTraitsVarDouble>;

}

#endif /* DATA_ON_DEMAND_LOCAL_SOURCE_BASE_H_ */

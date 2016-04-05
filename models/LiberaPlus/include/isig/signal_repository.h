/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: signal_repository.h 19025 2013-04-24 08:48:33Z damijan.skvarc $
 */

#ifndef ISIG_SIGNAL_REPOSITORY_H
#define ISIG_SIGNAL_REPOSITORY_H

#include <typeinfo>
#include <string>

#include "mci/declarations.h"
#include "isig/declarations.h"
#include "isig/signal_source.h"

namespace isig {

	/**
	 * @addtogroup signal
	 * @
	 */
    typedef std::vector<SignalSourceSharedPtr>                SignalSourceSharedPtrs;
    typedef std::map<isig::SignalSourceId, SignalSourcePtr>   SignalMap;
    /**
     * Signal Container class. This class is intended to manage all instantiated signals
     */
    class SignalRepository : public SignalMap
    {

    public:
        static SignalSourceSharedPtrs  GetOwnedSignals(const std::string &a_owner);
        static SignalSourceSharedPtrs  GetNamedSignals(const std::string &a_name);
        static SignalSourceSharedPtr   GetSignal(SignalSourceId a_id);
        static SignalRepository&       Instance();
        static void                    NotifyAnnouncedTimestampChange(const std::string &a_owner);
        static void                    NotifyAdjustedTimestamp(int64_t a_diff_lmt, uint64_t a_start_lmt, const std::string &a_owner);

    private:
        /* Attach, Detach functions should be called only in SignalSource ctor & dtor */
        friend SignalSource::SignalSource(const std::string &a_name, const std::string &a_owner = "");
        friend SignalSource::~SignalSource();

        static void Attach(SignalSourcePtr a_signal_source);
        static void Dettach(SignalSourcePtr a_signal_source);

    private:
        std::mutex m_mutex;
    };

    /**@}*/
}

#endif /* ISIG_SIGNAL_REPOSITORY_H */

/*
 * Copyright (c) 2012 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: phase_locked_loop_base.h 19990 2014-01-07 09:35:55Z damijan.skvarc $
 */

#ifndef PHASE_LOCKED_LOOP_BASE_H
#define PHASE_LOCKED_LOOP_BASE_H

#include "iapp/libera_feature.h"

namespace ifeature {

    /**
     * PLL base interface. Actual implementation is derived from this class.
     */
    class PhaseLockedLoopBase : public iapp::LiberaFeature
    {
    public:
        PhaseLockedLoopBase(
            iapp::LiberaObject &a_parent,
            const std::string &a_name)
        : iapp::LiberaFeature(a_parent, a_name)
        {

        }

        virtual ~PhaseLockedLoopBase()
        {

        }
    };

} // namespace


#endif // PHASE_LOCKED_LOOP_BASE_H

/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: libera_feature.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef LIBERA_FEATURE_H
#define LIBERA_FEATURE_H

#include "libera_object.h"
#include "istd/trace.h"


namespace iapp {

    class LiberaFeature : public LiberaObject
    {
    protected:
        LiberaFeature(LiberaObject& a_parent, const std::string &a_name);
        virtual  ~LiberaFeature();

        virtual bool      OnQueryChild(const LiberaObject &a_child);
    };

} // namespace


#endif // LIBERA_FEATURE_H

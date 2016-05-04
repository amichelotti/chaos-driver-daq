/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: libera_fpga_object.h 19990 2014-01-07 09:35:55Z damijan.skvarc $
 */

#ifndef LIBERA_FPGA_OBJECT_H
#define LIBERA_FPGA_OBJECT_H

#include <vector>
#include "declarations.h"
#include "libera_feature.h"

namespace iapp {

    class LiberaBoard;

    class LiberaFpgaObject : public LiberaFeature
    {
    public:
        static size_t Size()       { return 0; }

    protected:
        LiberaFpgaObject(LiberaObject &a_parent, std::string a_name);
        virtual ~LiberaFpgaObject();
        LiberaBoard&  GetParentBoard() const;
    };

    typedef std::vector<LiberaFpgaObject*> LiberaFpgaObjects;

} //namespace


#endif // LIBERA_FPGA_OBJECT_H

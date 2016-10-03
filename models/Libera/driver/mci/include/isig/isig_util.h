/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: isig_util.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */


#ifndef ISIG_UTIL_H_
#define ISIG_UTIL_H_

#include <iostream>
#include <string>
#include "isig/declarations.h"

namespace i_NET {
    class SignalMeta;
}

namespace isig {

    std::string ToString(const SignalMeta& a_meta);

    int OpenDevice(const std::string& a_fname, bool a_write);
    void CloseDevice(int& a_fd);

    i_NET::SignalMeta SerialiseMeta(const SignalMeta& a_meta);
    SignalMeta DeSerialiseMeta(const i_NET::SignalMeta& a_meta);

} // namespace



#endif

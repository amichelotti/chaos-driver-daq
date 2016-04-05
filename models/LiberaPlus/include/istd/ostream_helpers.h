
/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: ostream_helpers.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef ISTD_OSTREAM_HELPERS_H
#define ISTD_OSTREAM_HELPERS_H

#include <ostream>
class omni_thread_fatal;
class omni_thread_invalid;

std::ostream& operator<< (std::ostream &a_os, const omni_thread_fatal &otf);
std::ostream& operator<< (std::ostream &a_os, const omni_thread_invalid &otf);

#endif // ISTD_OSTREAM_HELPERS_H

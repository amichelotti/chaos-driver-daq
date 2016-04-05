/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: signal_dispatch_base.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef ISIG_SIGNAL_DISPATCH_BASE_H
#define ISIG_SIGNAL_DISPATCH_BASE_H

#include "isig/declarations.h"

namespace isig {

    class SignalDispatchBase {
    public:

        virtual ~SignalDispatchBase() {};

        virtual void Cancel(SuccessCode_e a_code);

    protected:

        virtual void Dispatch(const double* a_buf, size_t a_count, const SignalMeta& a_meta = c_noMeta);
        virtual void Dispatch(const float* a_buf, size_t a_count, const SignalMeta& a_meta = c_noMeta);
        virtual void Dispatch(const uint64_t* a_buf, size_t a_count, const SignalMeta& a_meta = c_noMeta);
        virtual void Dispatch(const int64_t* a_buf, size_t a_count, const SignalMeta& a_meta = c_noMeta);
        virtual void Dispatch(const uint32_t* a_buf, size_t a_count, const SignalMeta& a_meta = c_noMeta);
        virtual void Dispatch(const int32_t* a_buf, size_t a_count, const SignalMeta& a_meta = c_noMeta);
        virtual void Dispatch(const uint16_t* a_buf, size_t a_count, const SignalMeta& a_meta = c_noMeta);
        virtual void Dispatch(const int16_t* a_buf, size_t a_count, const SignalMeta& a_meta = c_noMeta);
        virtual void Dispatch(const uint8_t* a_buf, size_t a_count, const SignalMeta& a_meta = c_noMeta);
        virtual void Dispatch(const int8_t* a_buf, size_t a_count, const SignalMeta& a_meta = c_noMeta);

        template <typename CTYPE, typename BASE_TYPE>
            friend void DispatchBuffer(
                SignalDispatchBase*,
                const CTYPE*, size_t,
                const SignalMeta&, const BASE_TYPE&);

    };
}

#endif /* ISIG_SIGNAL_DISPATCH_BASE_H */

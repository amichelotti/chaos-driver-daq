/*
 * Copyright (c) 2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: stream_filter.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef STREAM_FILTER_H_
#define STREAM_FILTER_H_

#include <memory>

#include "isig/array.h"

namespace isig {

    template <class STREAM_TRAITS>
        class  StreamFilter {

        public:
            typedef STREAM_TRAITS                       Traits;
            typedef MetaBufferPair<Traits>              MetaBuffer;
            typedef typename MetaBuffer::Buffer         Buffer;
            typedef typename MetaBuffer::MetaBufferPtr  MetaBufferPtr;

        public:
            /**
             * Implement this method to to implement filter.
             * @param a_buffer source buffer
             * @return filtered buffer
             */
            virtual const MetaBufferPtr Filter(const MetaBufferPtr& a_buffer) = 0;

            virtual ~StreamFilter() {};
    };

}


#endif /* STREAM_FILTER_H_ */

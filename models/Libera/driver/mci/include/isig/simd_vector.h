/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: simd_vector.h 10002 2010-12-03 08:21:50Z tomaz.juretic $
 */


#ifndef ISIG_SIMD_VECTOR_H_
#define ISIG_SIMD_VECTOR_H_

#define SIMD_VECTOR(_t, _s) _t __attribute__ ((vector_size (_s*sizeof(_t))))

namespace isig {

    struct SimdFloat {
        union {
            std::array<float, 4>    a;
            SIMD_VECTOR(float, 4)   v;
        };
    };

    struct SimdUint32 {
        union {
            std::array<uint32_t, 4>    a;
            SIMD_VECTOR(uint32_t, 4)   v;
        };
    };


} // namespace

#endif /* ISIG_SIMD_VECTOR_H_ */

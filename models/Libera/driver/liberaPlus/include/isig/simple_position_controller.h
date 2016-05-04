/*
 * Copyright (c) 2012 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: simple_position_controller.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef SIMPLE_BUFFER_CONTROLLER_H_
#define SIMPLE_BUFFER_CONTROLLER_H_

#include "dod_position_controller.h"

namespace isig {

    class SimplePositionController;
    typedef std::shared_ptr<SimplePositionController>  SimpleControllerPtr;

    /**
     * Class implements basic logic to provide proper positions for simple buffer
     * implementations of data on demand data.
     */
    class SimplePositionController:
        public virtual DodPositionController,
        public Clonable<SimplePositionController> {

    public:

        /**
         * @param a_size Total size of the buffer in atoms
         */
        explicit SimplePositionController(size_t a_size);
        virtual ~SimplePositionController();

        virtual size_t GetMaxReadSize(int64_t, AccessMode_e) const OVERRIDE;

        virtual SuccessCode_e GetPosition(
            const AccessMode_e  a_mode,
            const uint64_t      a_posIn,
            const int64_t       a_offset,
            const size_t        a_readSize,
            LMT&                a_lmt,
            size_t&             a_absPos,
            SignalMeta&         a_meta) OVERRIDE __attribute__((warn_unused_result));

    private:
        /// Size of the history buffer
        const size_t    m_size;
    };

}

#endif /* SIMPLE_BUFFER_CONTROLLER_H_ */

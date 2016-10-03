/*
 * Copyright (c) 2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: segmented_buffer_controller_base.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef SYNTHETIC_CONTROLLER_H_
#define SYNTHETIC_CONTROLLER_H_

#include <set>

#include "dod_position_controller.h"

namespace isig {

    class SyntheticController;
    typedef std::shared_ptr<SyntheticController>    SyntheticControllerPtr;
    typedef std::shared_ptr<DodPositionController>  PosControllerPtr;

    /**
     * Class implements a simple position controller for synthetic data on
     * demand sources. Since synthetic dod source takes the data from a
     * source dod and transforms it with a mapping function, its position
     * controller relies transparently on the source class position controller,
     * calling directly its functions.
     * Beside this, the SyntheticController introduces some members that are
     * specific to this data type, like supported modes: using this class is
     * possible to set some characteristics that will be specific to the
     * synthetic source.
     */
    class SyntheticController: public virtual DodPositionController,
                               public Clonable<SyntheticController> {
    public:

        /**
         * @param a_ctrl        Position controller of the source dod
         */
        explicit SyntheticController(PosControllerPtr a_srcCtrl);

        virtual ~SyntheticController();


        /**
         * Copy constructor: shall be defined. It is called from the Clone() 
         * function defined in Clonable class from wich this position controller
         * is derived. That clone function is called from DodClient when we 
         * want to read the signal OnEvent or OnSingleEvent. 
         *
         * If we don't define it, the default copy constructor would copy every 
         * member including the source position controller, instead of cloning it.
         * If this is the case, the same event queue will be shared through different 
         * clients and each output signal will steal events to the others.
         * 
         */
        SyntheticController(const SyntheticController& a_syntheticController);

        virtual void Reset(const LMT& a_startLmt = 0ULL) OVERRIDE;
        virtual void Start() OVERRIDE;
        virtual void Stop() OVERRIDE;

        /** SetSupportedModes
         * @param a_modes: modes that should be supported by the controller
         *                 each specified mode should be contained in the
         *                 source dod modes, otherwise it will be discarded.
         */
        void SetSupportedModes(const std::set<AccessMode_e>& a_modes) OVERRIDE;


        /** GetMaxReadSize
         * @param a_offset      offset referred to the source dod
         * @param a_mode        access mode
         */
        virtual size_t GetMaxReadSize(int64_t a_offset, AccessMode_e a_mode) const OVERRIDE;


        /** IsPositionValid: checks is the specified position is valid
         * @param a_pos         absolute buffer position, referred to the source dod
         */
        virtual bool IsPositionValid(const uint64_t& a_pos) const OVERRIDE;


        /**
         * Calculates LMT and absolute position to read the source dod buffer
         * based on the mode and input parameters.
         *
         * Behaviour is dependent on the dod source. It may or may not use all
         * input arguments to calculate the absolute buffer position.
         *
         * @param a_mode        dod acquisition mode (in)
         * @param a_posIn       mode-dependent position (in)
         * @param a_offset      offset in atoms, applied to the position (in)
         * @param a_readSize    requested read size (in)
         * @param a_lmt         position in LMT units (out)
         * @param a_absPos      absolute position in the buffer in atom units (out)
         * @param a_meta        meta data for this position (out)
         * @return              status of the operation
         */
        virtual SuccessCode_e GetPosition(
                const AccessMode_e  a_mode,
                const uint64_t      a_posIn,
                const int64_t       a_offset,
                const size_t        a_readSize,
                LMT&                a_lmt,
                size_t&             a_absPos,
                SignalMeta&         a_meta) OVERRIDE __attribute__((warn_unused_result));

    private:
        // Source position controller
        PosControllerPtr m_srcCtrl;
    };

}

#endif /* SYNTHETIC_CONTROLLER_H_ */

/*
 * Copyright (c) 2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: dod_position_controller.h 19990 2014-01-07 09:35:55Z damijan.skvarc $
 */

#ifndef ISIG_POSITION_CONTROLLER_H
#define ISIG_POSITION_CONTROLLER_H

#include <memory>
#include <mutex>
#include <set>

#include "isig/declarations.h"

namespace isig {

    class DodPositionController;
    typedef std::shared_ptr<DodPositionController>  PosControllerPtr;

    /**
     * Controller that provides mapping between mode-dependent position to
     * absolute position, used to access the data on demand signals.
     *
     * Derived classes implement behaviour for specific types of data on demand
     * buffers.
     */
    class DodPositionController {
    public:
        DodPositionController();
        DodPositionController(const isig::DodPositionController& a_other)
        {
            m_supportedModes = a_other.m_supportedModes;
            m_enabled        = a_other.m_enabled;
        }

        virtual ~DodPositionController();

        /**
         * Verify whether the specified a_mode is supported by the specific
         * implementation of position controller.
         */
        bool IsModeSupported(AccessMode_e a_mode) const;

        /**
         * Get all the modes supported by the position controller
         */
        std::set<AccessMode_e> GetSupportedModes() const;

        virtual void Reset(const LMT& a_startLmt = 0ULL);

        virtual void Start();
        virtual void Stop();

        void SetEnable(bool a_enable);
        bool IsDisabled() const;

        virtual size_t GetMaxReadSize(int64_t a_offset, AccessMode_e a_mode) const;

        /**
         * Calculates LMT and absolute position to read the circular buffer
         * based on the mode and input parameters.
         *
         * Behaviour is implementation dependent. It may or may not use all
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
            SignalMeta&         a_meta) __attribute__((warn_unused_result));

        /**
         * Is position in the zone where it will be just overwritten?
         */
        virtual bool IsPositionValid(const uint64_t& a_pos) const;

        /**
         * Final derived classes implement this function by
         * deriving from templated interface Clonable.
         */
        virtual PosControllerPtr Clone() const = 0;

    protected:
        virtual void SetSupportedModes(const std::set<AccessMode_e>& a_modes);

    private:
        std::set<AccessMode_e>  m_supportedModes;
        bool                    m_enabled;
        mutable std::mutex      m_mutex;
    };

    /**
     * Template to simplify implementation of Template pattern, which is
     * needed by the data on demand signal to create copies of
     * DodPositionController descendants for each of the clients.
     */
    template <class Derived>
        class Clonable: public virtual DodPositionController {
        public:
            typedef DodPositionController Base;

            Clonable()
            {
                static_assert(
                    std::is_base_of<Base, Derived>::value,
                    "Derived is not derived from DodPositionController.");
            }

            virtual PosControllerPtr Clone() const
            {
                // Call copy ctor of derived class
                PosControllerPtr p(new Derived(static_cast<Derived const&>(*this)));
                return p;
            }
        };

}

#endif /* ISIG_POSITION_CONTROLLER_H */

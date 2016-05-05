/*
 * Copyright (c) 2012 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: dod_controller_interface.h 19990 2014-01-07 09:35:55Z damijan.skvarc $
 */

#ifndef ISIG_CONTROLLER_INTERFACE_H
#define ISIG_CONTROLLER_INTERFACE_H

#include <mutex>

#include "istd/trace.h"
#include "isig/array.h"
#include "isig/dod_position_controller.h"

namespace isig {

    /**
     * Interface is used by DodClient and implemented by DOD sources that use
     * DodPositionController.
     */
    template <class DOD_TRAITS>
        class DodControllerInterface {
        public:

            typedef DOD_TRAITS      Traits;
            typedef Array<Traits>   Buffer;

            typedef DodControllerInterface<Traits>  ThisDod;
            typedef std::shared_ptr<ThisDod>        SharedPtr;
            typedef std::weak_ptr<ThisDod>          WeakPtr;

            DodControllerInterface()
            {
            }

            virtual ~DodControllerInterface()
            {
            }

            void SetPositionController(PosControllerPtr& a_ctrl)
            {
                std::lock_guard<std::mutex> l(m_ctrls_x);
                m_posCtrl = a_ctrl;
                if (m_posCtrl) {
                    m_posCtrl->Start();
                    // Initialize the list of controller copies.
                    m_controllers.clear();
                    m_controllers.push_back(a_ctrl);
                }
            }

            virtual PosControllerPtr GetPositionController() const
            {
                std::lock_guard<std::mutex> l(m_ctrls_x);
                PosControllerPtr ctrl;
                if (m_posCtrl) {
                    ctrl = m_posCtrl;
                }
                return ctrl;
            }

            virtual PosControllerPtr GetPositionController(AccessMode_e a_mode) const
            {
                std::lock_guard<std::mutex> l(m_ctrls_x);
                PosControllerPtr ctrl;
                if (m_posCtrl && m_posCtrl->IsModeSupported(a_mode)) {
                    ctrl = m_posCtrl;
                }
                return ctrl;
            }

            virtual PosControllerPtr GetPositionControllerCopy(AccessMode_e a_mode) const
            {
                std::lock_guard<std::mutex> l(m_ctrls_x);
                PosControllerPtr ctrl;
                if (m_posCtrl && m_posCtrl->IsModeSupported(a_mode)) {
                    ctrl = m_posCtrl->Clone();
                    ctrl->Start();
                    m_controllers.push_back(ctrl);
                }
                return ctrl;
            }

            virtual void ReleasePositionControllerCopy(PosControllerPtr &a_pc) const
            {
                std::lock_guard<std::mutex> l(m_ctrls_x);
                // Find the copy that a_pc points to.
                auto i = std::find(m_controllers.begin(), m_controllers.end(), a_pc);
                if (i != m_controllers.end()) {
                    m_controllers.erase(i);
                }
                else {
                    istd_TRC(istd::eTrcMed, "Position controller copy not found.");
                }
            }

            void SetAllEnable(bool a_enable)
            {
                std::lock_guard<std::mutex> l(m_ctrls_x);
                for (auto i = m_controllers.begin(); i != m_controllers.end(); ++i) {
                    (*i)->SetEnable(a_enable);
                }
            }

            /* This function resets all the controllers of a proper type: it 
             * goes through all the clones of a particular controller and calls the Reset()
             * method for each of them. It is used when a synchronization takes place */
            void ResetAllControllers(const LMT& a_startLmt = 0ULL)
            {
                std::lock_guard<std::mutex> l(m_ctrls_x);
                for (auto i = m_controllers.begin(); i != m_controllers.end(); ++i) {
                    (*i)->Reset(a_startLmt);
                }
            }

            virtual SuccessCode_e Read(Buffer& a_buf, size_t a_position, PosControllerPtr a_dodCtrl) = 0;

        protected:
            mutable std::mutex                    m_ctrls_x;
            PosControllerPtr                      m_posCtrl;
            // List of created controller copies.
            mutable std::vector<PosControllerPtr> m_controllers;

    };

}

#endif /* ISIG_CONTROLLER_INTERFACE_H */

/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: signal_node.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef IREG_SIGNAL_NODE_H_
#define IREG_SIGNAL_NODE_H_

#pragma GCC diagnostic ignored "-Wold-style-cast"
#include "inet/i_remote_signal.h"
#pragma GCC diagnostic warning "-Wold-style-cast"

#include "isig/signal_source.h"
#include "ireg/reg_node.h"

namespace isig {
    class i_RemoteSignal_impl;
}

namespace ireg {

    /**
     * @addtogroup register
     * @{
     */
    /**
     * Specific implementation of a RegNode which holds reference to signal
     * and expose different signal properties over the network.
     */
    class SignalNode: public RegNode {
    public:
        i_NET::RemoteSignal_ptr GetRemoteSignalRef();

        virtual ~SignalNode();

    protected:

        SignalNode(const std::string& a_name,
                   isig::SignalSource* a_signal,
                   Flags a_flags = eNfReadable);

        virtual bool Init();

        mci_CREATE_NODE_FRIEND;

    private:

        isig::SignalSourcePtr       m_signal;
        isig::i_RemoteSignal_impl*  m_remoteSignalImpl;
    };
    /**@}*/

}

#endif /* IREG_SIGNAL_NODE_H_ */

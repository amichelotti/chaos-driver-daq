/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: notification_data.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */


#ifndef MCI_NOTIFICATION_DATA_H
#define MCI_NOTIFICATION_DATA_H

#include <memory>

#include "istd/any.h"
#include "mci/declarations.h"
#include "mci/node.h"

namespace mci {

    struct NotificationData {
       NotificationData(const Node &a_node,
                        const NotificationType_e &a_type,
                        const istd::Any &a_data,
                        const size_t &a_pos = 0);

        Node                m_node;
        NotificationType_e  m_type;
        istd::Any           m_data;
        size_t              m_pos;
    };

    std::ostream& operator<< (std::ostream &a_os, const NotificationData &a_data);

}

#endif /* MCI_NOTIFICATION_DATA_H */

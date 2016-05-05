/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: callback.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */


#ifndef MCI_CALLBACK_H_
#define MCI_CALLBACK_H_

#include <functional>
#include "declarations.h"

namespace mci {

    typedef std::function<void(const NotificationData&)>   CallbackFunction;

    class Callback {

    public:
        template <class FUNC, class T>
            Callback(FUNC a_memFunc, T a_instance)
            : m_id(CallbackId::Generate())
            {
                m_localCallback = std::bind(
                    std::mem_fun_ref(a_memFunc),
                    std::ref(*a_instance),
                    std::placeholders::_1);
            }

        Callback(const CallbackFunction& a_func);

        void operator()(const NotificationData& a_data);
        bool operator==(const Callback& a_other) const;

    private:
        CallbackId          m_id;
        CallbackFunction    m_localCallback;
    };

}

#endif /* MCI_CALLBACK_H_ */

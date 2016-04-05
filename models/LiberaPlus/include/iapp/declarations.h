/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: declarations.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */


#ifndef IAPP_DECLARATIONS_H
#define IAPP_DECLARATIONS_H

#include <memory>
#include <vector>

namespace iapp {

    enum ApplicationState_e {eAppUndefined,
                             eAppInitialize,
                             eAppCreateRegistry,
                             eAppLoadConfiguration,
                             eAppRun,
                             eAppShutdown,
                             eAppDestroy};

    class LiberaObject;
    typedef std::weak_ptr<LiberaObject>       LiberaObjectWeakPtr;
    typedef std::shared_ptr<LiberaObject>     LiberaObjectPtr;
    typedef std::vector<LiberaObjectPtr>      LiberaObjects;
}

#endif /* IAPP_DECLARATIONS_H */

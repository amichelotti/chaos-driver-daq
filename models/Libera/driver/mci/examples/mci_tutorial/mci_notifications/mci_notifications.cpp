/*
 * Copyright (c) 2008-2015 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: mci_notifications.cpp 22034 2015-06-19 08:32:00Z damijan.skvarc $
 */

#include <iostream>
#include <chrono>
#include <thread>

#include "istd/trace.h"
#include "mci/mci.h"
#include "mci/callback.h"
#include "mci/notification_client.h"
#include "mci/notification_data.h"
#include "mci/mci_util.h"

#include "../common.h"

void CallbackFnc(const mci::NotificationData& a_data)
{
    istd::Any any = a_data.m_data;

    istd_TRC(istd::eTrcLow, "Notification: " << mci::ToString(a_data.m_node.GetFullPath()) << " = " << any);
}


int main(int a_argc, char* a_argv[])
{
    istd::TraceInit("-");
    istd::TraceStart(istd::eTrcOff);

    mci::Init(a_argc, a_argv);

    /* connect to LiberaBase application running on remote instrument  */
    mci::Node root = mci::Connect(cHost);

    /* get reference to appropriate registry node  */
    mci::Node node = root.GetNode(mci::Tokenize(cTriggerSourceNode));

    mci::Callback             callback(CallbackFnc);
    mci::NotificationClient   client(callback);

    client.Register(node);

    istd::TraceStart(istd::eTrcLow);

    /* ..and modify its value several times */

    bool ok(false);

    ok = node.SetValue("Pulse");
    std::this_thread::sleep_for(std::chrono::seconds(1));
    ok = node.SetValue("Off");
    std::this_thread::sleep_for(std::chrono::seconds(1));
    ok = node.SetValue("External");
    std::this_thread::sleep_for(std::chrono::seconds(1));
    ok = node.SetValue("Off");
    std::this_thread::sleep_for(std::chrono::seconds(1));

    mci::Shutdown();
}


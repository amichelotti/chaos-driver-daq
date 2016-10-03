/*
 * Copyright (c) 2008-2015 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: mci_read_tbt.cpp 22034 2015-06-19 08:32:00Z damijan.skvarc $
 */

#include "mci/mci.h"
#include "mci/mci_util.h"
#include "istd/trace.h"
#include "isig/signal_source.h"
#include "isig/signal_client_base.h"
#include "isig/data_on_demand_remote_source.h"

#include "../common.h"

int main(int a_argc, char* a_argv[])
{
    /* initialize mci library */
    mci::Init(a_argc, a_argv);

    /* connect to LiberaBase application running on remote instrument  */
    mci::Node root = mci::Connect(cHost);

    /* get registry node referring to turn-by-turn (DDC) data on demand signal */
    mci::Node node = root.GetNode(mci::Tokenize(cTbtDod));

    /* instantiate remote signal entity */
    isig::SignalSourceSharedPtr signal = mci::CreateRemoteSignal(node);

    /* ddc_synthetic signal is "data on demand" signal whose basic data type is signed 32 bit */
    /* typecast base signal type to this signal type */
    typedef isig::DataOnDemandRemoteSource<isig::SignalTraitsVarInt32> DOD;

    DOD *rSignal = dynamic_cast<DOD*> (signal.get());

    /* instantiate client for reading data from the signal */
    DOD::Client client = rSignal->CreateClient("myClient");

#if BASE_VER_MAJOR >= 3
    /* don't wait too long in case of no data */
    client.SetReadTimeout(std::chrono::seconds(5));
#endif

    /* open client for reading 10 signal atoms on acquisition trigger event */
    /* to read regardless the trigger event use eModeDodNow                 */
    auto ret = client.Open(isig::eModeDodOnEvent, 10);
    if (ret == isig::eSuccess) {

        auto buffer(client.CreateBuffer(10));
        isig::SignalMeta meta;

        ret = client.Read(buffer, meta);

        if (ret == isig::eSuccess) {
            /* print out data content */
            const auto columns = buffer[0].GetComponents();

            for (size_t i = 0; i < buffer.GetLength(); ++i) {
               for (size_t j = 0; j < columns ; ++j) {
                   std::cout << std::setw(11) <<  buffer[i][j] <<  "  ";
               }
               std::cout << std::endl;
            }
        }
        else {
            std::cout <<  "Read error: " << ret << std::endl;
        }

        /* and close the client */
        client.Close();
    }

    /* shutdown mci library */
    mci::Shutdown();
}

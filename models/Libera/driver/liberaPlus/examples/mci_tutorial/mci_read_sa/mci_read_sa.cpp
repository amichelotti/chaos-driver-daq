/*
 * Copyright (c) 2008-2015 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: mci_read_sa.cpp 22034 2015-06-19 08:32:00Z damijan.skvarc $
 */

#include "mci/mci.h"
#include "mci/mci_util.h"
#include "istd/trace.h"
#include "isig/signal_source.h"
#include "isig/signal_client_base.h"
#include "isig/remote_stream.h"

#include "../common.h"

int main(int a_argc, char* a_argv[])
{
istd::TraceInit("-");
    istd::TraceStart(istd::eTrcLow);

    /* initialize mci library */
    mci::Init(a_argc, a_argv);

    /* connect to LiberaBase application running on remote instrument  */
//    mci::Node root = mci::Connect(cHost);
    mci::Node root = mci::Connect();
    /* get registry node referring to SA stream */
    mci::Node node = root.GetNode(mci::Tokenize(cSaStream));

    /* instantiate remote signal entity */
    isig::SignalSourceSharedPtr signal = mci::CreateRemoteSignal(node);

    /* SA signal is stream signal whose basic data type is signed 32 bit */
    /* typecast base signal type to this signal type */
    typedef isig::RemoteStream<isig::SignalTraitsVarInt32> RStream;

    RStream *rStream = dynamic_cast<RStream*> (signal.get());

    /* instantiate client for reading data from the signal */
    RStream::Client client(rStream, "myClient");

    /* don't wait too long in case of no data */
    client.SetReadTimeout(std::chrono::seconds(5));

    /* open client for reading */
    auto ret = client.Open();
    if (ret == isig::eSuccess) {

        /* read data from SA stream */
        RStream::MetaBufferPtr buf;
        ret = client.Read(buf);

        if (ret == isig::eSuccess) {

            /* print out data content */

            RStream::Buffer& buffer(buf->data);
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
	std::cout<<"closing"<<std::endl;
        /* and close the client */
        client.Close();
    }

    /* shutdown mci library */
    mci::Shutdown();
}

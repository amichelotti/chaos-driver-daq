/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: stream.h 19990 2014-01-07 09:35:55Z damijan.skvarc $
 */

#ifndef ISIG_STREAM_H
#define ISIG_STREAM_H

#include <system_error>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory>
#include "istd/trace.h"
#include "istd/log.h"
#include "istd/assert.h"
#include "istd/exception.h"
#include "isig/array.h"
#include "isig/stream_queue.h"
#include "isig/signal_source.h"
#include "isig/signal_dispatch_base.h"
#include "isig/signal_traits.h"
#include "isig/stream_client.h"

namespace isig {

	/**
	 * @addtogroup signal
	 * @{
	 */

    template <class STREAM_ATOM>
        class StreamClient;

    /**
     * Mode of operation of the stream
     */
    enum class StreamMode_e {

        /**
         * No thread is started. Input data into a stream is fed externally by
         * calling the Dispatch function directly.
         */
        Passive = 0,

        /**
         * Stream instance will call OpenInput and start its own thread on
         * first open, then run ReadInput and Dispatch in a loop while there
         * are any clients connected and close the stream by CloseInput when
         * the last client disconnects.
         */
        Active = 1,

        /**
         * Stream instance will call OpenInput and start its own thread when the
         * object is created, regardless on the number of clients.
         * The read thread will stop when the object is discarded.
         */
        Continuous = 2

    };


    enum class StreamState {
        Closed, Starting, Open, Closing, Shutdown
    };


    /**
     * Template for the stream-type data source.
     *
     * @param STREAM_TRAITS   type traits of one record (atom)
     * @see SignalTraits
     */
    template <class STREAM_TRAITS>
        class Stream:
            public SignalSource,
            public SignalDispatchBase
{
        public:

            typedef STREAM_TRAITS                       Traits;
            typedef typename Traits::Atom               Atom;
            typedef typename Traits::BaseType           BType;

            typedef Stream<Traits>                      ThisStream;
            typedef ThisStream*                         StreamPtr;
            typedef std::shared_ptr<ThisStream>         StreamSharedPtr;
            typedef std::weak_ptr<ThisStream>           StreamWeakPtr;

            typedef MetaBufferPair<Traits>              MetaBuffer;
            typedef typename MetaBuffer::Buffer         Buffer;
            typedef typename MetaBuffer::MetaBufferPtr  MetaBufferPtr;

            typedef StreamClient<Traits>                Client;

        private:
            typedef StreamQueue<Traits>                     ThisStreamQueue;
            typedef typename std::vector<ThisStreamQueue*>  ClientQueues;
            typedef typename ClientQueues::iterator         ClientQIterator;

            const Traits        m_traits;
            const StreamMode_e  m_mode;

        public:

            const static size_t c_defaultCapacity = ThisStreamQueue::c_defaultCapacity;

            /**
             * @param a_traits Signal traits describing the signal contents
             * @param a_active Does this instance start the thread to dispatch data
             */
            explicit Stream(
                const std::string&  a_name,
                const Traits&       a_traits,
                StreamMode_e        a_mode = StreamMode_e::Active,
                const std::string&  a_owner = "")
            : SignalSource(a_name,a_owner),
              m_traits(a_traits),
              m_mode(a_mode),
              m_sequenceNum(0),
              m_state(StreamState::Closed)
            {
                m_queues.reserve(30);
            };

            virtual ~Stream()
            {
                istd_FTRC();
                // Shutdown must be called in each dtor to disconnect any
                // stream client that is currently connected.
                this->ShutDown();

                if (GetStreamState() != StreamState::Shutdown && !m_queues.empty() ) {
                    istd_ASSERT("Stream was not shut down properly.");
                }
            };

            /**
             * Starts to dispatch the signal data in Continuous mode.
             * In Active mode, the dispatching starts when the first client
             * attaches to the stream.
             */
            SuccessCode_e Start()
            {
                if (m_mode != StreamMode_e::Continuous) {
                    return eInvalidArg;
                }

                istd_TRC(istd::eTrcMed,
                    GetName() << ": Continuous mode: starting dispatch thread.");

                std::unique_lock<std::mutex> l(m_queues_x);
                StartDispatch(l);
                return eSuccess;
            }

            virtual void ShutDown() OVERRIDE
            {
                istd_FTRC();

                if (GetStreamState() == StreamState::Shutdown) {
                    return;
                }

                istd_TRC(istd::eTrcMed, GetName() << ": Shutting down..");
                if (m_state != StreamState::Closed) {
                    istd_TRC(istd::eTrcLow,
                        GetName() << ": Cancelling open handles.");
                }
                {
                    std::lock_guard<std::mutex> l(m_queues_x);
                    SetStreamState(StreamState::Shutdown);
                }
                this->CancelClients(eTerminated);
                istd_TRC(istd::eTrcMed, GetName() << ": Shutdown finished.");
            };

            inline virtual std::size_t  AtomSize() const OVERRIDE
            {
                return m_traits.GetAtomSize();
            };

            inline virtual std::size_t GroupSize() const OVERRIDE
            {
                return m_traits.GetGroupSize();
            };

            inline virtual const std::type_info&   AtomType() const OVERRIDE
            {
                return typeid(Atom);
            };

            inline virtual const std::type_info&   BaseType() const OVERRIDE
            {
                return typeid(typename Traits::BaseType);
            };

            virtual std::size_t             Length() const OVERRIDE
            {
                return m_traits.GetComponents();
            }

            inline virtual AccessType_e AccessType() const OVERRIDE
            {
                return AccessType_e::eAccessStream;
            }

            inline const Traits& GetTraits() const
            {
                return m_traits;
            }

            virtual const ComponentNames GetComponentNames() const OVERRIDE
            {
                return ComponentNames(
                    m_traits.GetNames().begin(),
                    m_traits.GetNames().end());
            }

            SuccessCode_e AddQueue(ThisStreamQueue& a_queue)
            {
                istd_FTRC();

                std::unique_lock<std::mutex> l(m_queues_x);

                StreamState st = GetStreamState();
                while (st == StreamState::Closing || st == StreamState::Starting) {
                    // New client wants to attach while the dispatcher thread
                    // is going down after all clients detached
                    // or is starting up.
                    // Wait for the operation to complete first.
                    if (st == StreamState::Closing) {
                        istd_ASSERT(m_queues.size() == 0);
                        m_closed_c.wait(l);
                    }
                    else if (st == StreamState::Starting) {
                        istd_ASSERT(m_queues.size() == 1);
                        m_started_c.wait(l);
                    }
                    st = GetStreamState();
                }
                if (GetStreamState() == StreamState::Shutdown) {
                    istd_TRC(istd::eTrcMed, GetName() << ": Shutdown already in progress.");
                    return eTerminated;
                }

                if (GetStreamState() == StreamState::Closed &&
                    m_mode == StreamMode_e::Continuous) {
                    // Dispatching thread in continuous mode
                    // was not started yet or stopped because of an error.
                    // It is not started again automatically.
                    return eIoError;
                }

                a_queue.Reset();
                m_queues.push_back(&a_queue);
                if (GetStreamState() == StreamState::Closed) {
                    auto numberOfClients = m_queues.size();
                    istd_ASSERT(numberOfClients == 1);

                    // No clients were attached to this stream:
                    // it needs to be opened.
                    StartDispatch(l);
                }
                return eSuccess;
            };

            void RemoveQueue(ThisStreamQueue& a_queue)
            {
                istd_FTRC();
                std::thread copy;
                bool terminateDispather(false);
                {
                    std::lock_guard<std::mutex> l(m_queues_x);

                    auto i = std::find(
                        m_queues.begin(), m_queues.end(), &a_queue
                    );

                    if (i == m_queues.end()) {
                        return;
                    }
                    terminateDispather = RemoveClient(i, eClosed, copy);
                    // m_queues_x unlocked here
                }

                if (terminateDispather) {
                    StopDispatching(copy);
                }
            };

            /**
             * Thread main function to dispatch the stream data if the stream
             * has its own active thread to run the dispatching.
             */
            void operator()()
            {
                istd_FTRC();
                {
                    std::lock_guard<std::mutex> l(m_queues_x);
                    if (GetStreamState() == StreamState::Starting) {
                        SetStreamState(StreamState::Open);
                    }
                    m_started_c.notify_all();
                }

                std::size_t group_size(GroupSize());
                Buffer b(group_size, m_traits);
                try {
                    StreamState st = GetStreamState();
                    while (st == StreamState::Open) {
                        // Allocate new buffer for each read
                        b.Resize(group_size);
                        SuccessCode_e ret = this->ReadInput(b);
                        if (ret != eSuccess && ret != eIncomplete) {
                            istd_LOGe(GetName() << ": Terminating dispatch thread. Cause: " << ret);
                            std::lock_guard<std::mutex> l(m_queues_x);
                            CancelAllClients(ret);
                            SetStreamState(StreamState::Closing);
                            break;
                        }

                        if (b.GetLength() > 0) {
                            st = GetStreamState();
                            if (st == StreamState::Open) {
                                Dispatch(std::move(b), c_noMeta);
                            }
                        }
                        st = GetStreamState();
                    }
                    if (st == StreamState::Shutdown) {
                        istd_TRC(istd::eTrcHigh,
                            GetName() << ": Stream shutdown: dispatcher thread finished.");
                        return;
                    }
                }
                catch (const std::system_error& e) {
                    istd_TRC(istd::eTrcLow,
                        GetName() << ": Dispatching thread error: "
                        << e.code().message());
                    Dispatch(std::move(b), c_noMeta);
                }
                catch (const std::exception& e) {
                    istd_TRC(istd::eTrcLow,
                        GetName() << ": Dispatching thread error: " << e.what());
                    Dispatch(std::move(b), c_noMeta);
                }

                std::lock_guard<std::mutex> l(m_queues_x);
                SetStreamState(StreamState::Closed);
                m_closed_c.notify_all();
            }

            /**
             * Used by dispatching thread (internal or external) to distribute
             * input buffer to all registered clients.
             * NOTE: as a result the specified a_buf buffer will lose ownership on allocated buffer
             */
            void Dispatch(Buffer&& a_buf, const SignalMeta& a_meta)
            {
                std::lock_guard<std::mutex> l(m_queues_x);
                istd_TRC(istd::eTrcDetail,
                    GetName() << ": Dispatching " << istd::CppName(AtomType())
                    << ": #queues: " << m_queues.size()
                    << ", atoms: " << a_buf.GetLength());

                /* create a MetaBufferPair, taking ownership over the specified buffer memory */
                MetaBuffer c(std::move(a_buf), a_meta);

                /* create one shared pointer, which is then pushed to all clients. */
                auto mb = std::make_shared<MetaBuffer>(std::move(c));
                for (auto i = m_queues.begin(); i != m_queues.end(); ++i) {
                    (void)(*i)->Push(mb);
                }
                m_sequenceNum += a_buf.GetLength();
            };

            void Dispatch(const Buffer& a_buf, const SignalMeta& a_meta)
            {
                Buffer copy(a_buf);
                this->Dispatch(std::move(copy), a_meta);
            };

            /**
             * Function to dispatch the buffer. Used by i_SignalReceiver_impl
             * which receives the buffer of atoms as array of Traits::BaseType.
             */
            virtual void Dispatch(
                const BType*        a_buf,
                size_t              a_count,
                const SignalMeta&   a_meta) OVERRIDE
            {
                Buffer b(const_cast<BType*>(a_buf), a_count, m_traits);
                Buffer copy(b);
                this->Dispatch(std::move(copy), a_meta);
            }

            virtual SignalClientBase* NewSignalClient(
                const std::string& a_name,
                size_t a_queueSize = 0) OVERRIDE
            {
                if (a_queueSize == 0) {
                    // Default queue size
                    return new Client(this, a_name);
                }
                return new Client(this, a_name, a_queueSize);
            }

            virtual void Cancel(SuccessCode_e a_code) OVERRIDE
            {
                istd_FTRC();
                this->CancelClients(a_code);
            }

            virtual void Cancel(std::string a_name)
            {
                std::thread copy;
                bool stopDispather(false);

                {
                    std::lock_guard<std::mutex> l(m_queues_x);
                    for (auto i = m_queues.begin(); i != m_queues.end(); ++i) {
                        auto* q(*i);
                        if (q->GetName() == a_name) {
                            stopDispather = RemoveClient(i, eClosed, copy);
                            if (stopDispather) {
                                break;
                            }
                            i = m_queues.begin();
                        }
                    }
                    // mutex unlocks here
                }

                if (stopDispather) {
                    StopDispatching(copy);
                }
            }

        protected:
            /** @name Input interface
             *  These functions are implemented in the derived class
             *  that knows details about how to handle input data.
             *  @{
             */
            virtual void OpenInput()
            {
            }

            virtual void CloseInput()
            {
            }

            /**
             * Dispatching thread uses this function to read source data stream,
             * which is then dispatched to clients.
             * Reading of input buffer is implemented for each specific data source.
             */
            virtual SuccessCode_e ReadInput(Buffer &a_buf)
            {
                if (m_mode == StreamMode_e::Passive) {
                    istd_EXCEPTION(GetName() << ": Running in passive mode.");
                }
                a_buf.Resize(0);
                return eIncomplete;
            }
            ///@}

            StreamMode_e GetStreamMode() const
            {
                return m_mode;
            }

        private:
            inline StreamState GetStreamState() const
            {
                std::lock_guard<std::mutex> l(m_state_x);
                return m_state;
            }

            inline void SetStreamState(StreamState a_state)
            {
                std::lock_guard<std::mutex> l(m_state_x);
                istd_TRC(istd::eTrcHigh,
                    GetName() << ": Stream state: "
                    << m_state << " --> " << a_state);
                m_state = a_state;
            }

            void StartDispatch(std::unique_lock<std::mutex>& a_lock)
            {
                istd_FTRC();
                OpenInput();
                m_sequenceNum = 1;
                if (m_mode == StreamMode_e::Passive) {
                    SetStreamState(StreamState::Open);
                    return;
                }
                SetStreamState(StreamState::Starting);
                if (m_thread.joinable()) {
                    istd_TRC(istd::eTrcMed, GetName() << ": join thread.");
                    m_thread.join();
                }
                istd_TRC(istd::eTrcMed, GetName() << ": Starting thread..");
                m_thread = std::thread(std::ref(*this));
                while (GetStreamState() == StreamState::Starting) {
                    // Wait for the thread to start
                    m_started_c.wait(a_lock);
                }
                istd_TRC(istd::eTrcMed, GetName() << ": Thread started..");
            }

            /**
             * @return Does the dispatch thread needs to be stopped.
             */
            bool RemoveClient(
                ClientQIterator&    a_queue,
                SuccessCode_e       a_code,
                std::thread&        a_thread)
            {
                if (a_queue == m_queues.end()) {
                    return false;
                }

                (*a_queue)->Cancel(a_code);
                m_queues.erase(a_queue);

                istd_TRC(istd::eTrcHigh,
                    GetName() << ": Client removed, queue size: "
                    << m_queues.size());

                if (GetStreamState() != StreamState::Shutdown &&
                    m_mode == StreamMode_e::Continuous) {
                    // Dispatch thread is running unconditionally until
                    // the shutdown even if there are no clients
                    return false;
                }
                if (m_queues.empty()) {
                    std::swap(m_thread, a_thread);
                    SetStreamState(StreamState::Closing);
                    return true;
                }
                return false;
            }

            void StopDispatching(std::thread& a_thread)
            {
                istd_FTRC();

                try {
                    this->CloseInput();
                }
                catch(const std::system_error& e) {
                    istd_TRC(istd::eTrcLow,
                            GetName() << ": WARNING: CloseInput threw: " << e.code().message());
                }
                catch(const std::exception& e) {
                    istd_TRC(istd::eTrcLow,
                            GetName() << ": WARNING: CloseInput threw: " << e.what());
                }
                catch(...) {
                    istd_TRC(istd::eTrcLow,
                            GetName() << ": WARNING: CloseInput threw exception.");
                }
                if (m_mode == StreamMode_e::Passive) {
                    // There is no thread running to set the state back to
                    // StreamState::Closed for passive stream.
                    std::lock_guard<std::mutex> l(m_queues_x);
                    if (GetStreamState() != StreamState::Shutdown) {
                        SetStreamState(StreamState::Closed);
                    }
                    m_closed_c.notify_all();
                }
                else {
                    if (a_thread.joinable()) {
                        istd_TRC(istd::eTrcMed, GetName() << ": Input closed: will join the thread.");
                        a_thread.join();
                        m_sequenceNum = 0;
                    }
                 }
            }

            void CancelAllClients(SuccessCode_e a_code)
            {
                while (!m_queues.empty()) {
                    auto i = m_queues.begin();

                    (*i)->Cancel(a_code);
                    m_queues.erase(i);

                    istd_TRC(istd::eTrcHigh,
                        GetName() << ": Client removed, queue size: "
                        << m_queues.size());
                }
            }

            void CancelClients(SuccessCode_e a_code)
            {
                istd_FTRC();

                const bool stopDispatcher(
                    (m_mode == StreamMode_e::Active) ||
                    (m_mode == StreamMode_e::Continuous && a_code == eTerminated)
                );
                std::thread copy;
                {
                    std::lock_guard<std::mutex> l(m_queues_x);
                    CancelAllClients(a_code);
                    if (stopDispatcher) {
                        std::swap(m_thread, copy);
                        SetStreamState(StreamState::Closing);
                    }
                    // mutex unlocks here
                }

                istd_TRC(istd::eTrcMed, GetName()
                    << ": stop = " << stopDispatcher
                    << ", a_code = " << a_code);

                if (stopDispatcher) {
                    StopDispatching(copy);
                }
            }

            size_t                  m_sequenceNum;
            /**
             * m_state indicates the state of the queues and the
             * dispatching thread, based on how many clients are attached to the
             * stream.
             */
            StreamState             m_state;
            mutable std::mutex      m_state_x;

            ClientQueues            m_queues;
            std::mutex              m_queues_x;
            std::condition_variable m_closed_c;
            std::condition_variable m_started_c;
            std::thread             m_thread;
        };
        /**@}*/

        // Streams for runtime traits are instantiated in the library
        extern template class Stream<SignalTraitsVarUint8>;
        extern template class Stream<SignalTraitsVarInt16>;
        extern template class Stream<SignalTraitsVarUint16>;
        extern template class Stream<SignalTraitsVarInt32>;
        extern template class Stream<SignalTraitsVarUint32>;
        extern template class Stream<SignalTraitsVarInt64>;
        extern template class Stream<SignalTraitsVarUint64>;
        extern template class Stream<SignalTraitsVarFloat>;
        extern template class Stream<SignalTraitsVarDouble>;


        std::ostream& operator<<(std::ostream&, const StreamState&);
}

#endif /* ISIG_STREAM_H */

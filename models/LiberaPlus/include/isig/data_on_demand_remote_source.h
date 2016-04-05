/*
 * Copyright (c) 2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: data_on_demand_remote_source.h 19990 2014-01-07 09:35:55Z damijan.skvarc $
 */

#ifndef DATA_ON_DEMAND_REMOTE_SOURCE_H_
#define DATA_ON_DEMAND_REMOTE_SOURCE_H_

#include <set>
#include <mutex>

#include "istd/trace.h"
#include "istd/system.h"
#include "isig/signal_traits.h"
#include "isig/array.h"

#pragma GCC diagnostic ignored "-Wold-style-cast"
#include "inet/i_remote_signal.h"
#pragma GCC diagnostic warning "-Wold-style-cast"

#include "isig/dod_remote_client.h"
#include "isig/data_on_demand_source_base.h"

namespace isig {

    template <class DOD_TRAITS>
        class DodRemoteClient;

    /**
     * @param DOD_TRAITS    type traits of one record in the atom
     * @see SignalTraits
     */
    template <class DOD_TRAITS>
        class DataOnDemandRemoteSource: public DataOnDemandSourceBase<DOD_TRAITS> {
        public:

            typedef DOD_TRAITS                      Traits;
            typedef DataOnDemandSourceBase<Traits>  DodBase;

            typedef typename DodBase::BType         BType;
            typedef typename DodBase::Atom          Atom;
            typedef typename DodBase::MetaBuffer    MetaBuffer;
            typedef typename DodBase::Buffer        Buffer;

            typedef DataOnDemandRemoteSource<Traits>  ThisDod;
            typedef std::shared_ptr<ThisDod>    DodSharedPtr;
            typedef std::weak_ptr<ThisDod>      DodWeakPtr;

            typedef DodRemoteClient<Traits>     Client;

            using DodBase::AtomSize;
            using DodBase::AtomType;

            DataOnDemandRemoteSource(
                const std::string&      a_name,
                i_NET::RemoteSignal_ptr a_remoteSignal,
                const Traits&           a_traits )
            : DodBase(a_name, a_traits),
              m_remoteSignal(i_NET::RemoteSignal::_duplicate(a_remoteSignal))
            {
                istd_FTRC();
                istd_TRC(istd::eTrcLow,
                    "DataOnDemandRemoteSource: type = " << istd::CppName(AtomType()) );

                this->Open();
             };

            virtual ~DataOnDemandRemoteSource()
            {
                istd_FTRC();
                this->Close();
            };

            Client CreateClient(const std::string& a_name)
            {
                auto ptr = std::dynamic_pointer_cast<ThisDod>(this->shared_from_this());
                return Client(ptr, a_name, this->m_traits);
            }

            i_NET::RemoteSignal_ptr GetRemoteSignal() const
            {
                return i_NET::RemoteSignal::_duplicate(m_remoteSignal);
            }

            virtual void Cancel(SuccessCode_e a_code) OVERRIDE
            {
                std::set<Client*> clients;
                {
                    std::unique_lock<std::mutex> l(m_mutex);
                    clients = m_clients;
                }
                for (auto it = clients.begin(); it != clients.end(); it++) {
                    (*it)->Cancel(a_code);
                }
            }

        private:
            friend class DodRemoteClient<Traits>;

            void Add(Client *a_client)
            {
                std::unique_lock<std::mutex> l(m_mutex);
                m_clients.insert(a_client);
            }

            void Remove(Client *a_client)
            {
                std::unique_lock<std::mutex> l(m_mutex);
                m_clients.erase(a_client);
            }

        private:
            i_NET::RemoteSignal_var m_remoteSignal;
            std::set<Client*>       m_clients;
            std::mutex              m_mutex;
        };

    SuccessCode_e ReadDodRemote(
        i_NET::RemoteSignal_ptr     a_rs,
        i_NET::SignalReceiver_ptr   a_sr,
        const uint64_t              a_position,
        int8_t*                     a_buf,
        const size_t                a_bsize,
        size_t&                     a_read,
        SignalMeta&                 a_meta);

    SuccessCode_e ReadDodRemote(
        i_NET::RemoteSignal_ptr     a_rs,
        i_NET::SignalReceiver_ptr   a_sr,
        const uint64_t              a_position,
        uint8_t*                    a_buf,
        const size_t                a_bsize,
        size_t&                     a_read,
        SignalMeta&                 a_meta);

    SuccessCode_e ReadDodRemote(
        i_NET::RemoteSignal_ptr     a_rs,
        i_NET::SignalReceiver_ptr   a_sr,
        const uint64_t              a_position,
        int16_t*                    a_buf,
        const size_t                a_bsize,
        size_t&                     a_read,
        SignalMeta&                 a_meta);

    SuccessCode_e ReadDodRemote(
        i_NET::RemoteSignal_ptr     a_rs,
        i_NET::SignalReceiver_ptr   a_sr,
        const uint64_t              a_position,
        uint16_t*                   a_buf,
        const size_t                a_bsize,
        size_t&                     a_read,
        SignalMeta&                 a_meta);

    SuccessCode_e ReadDodRemote(
        i_NET::RemoteSignal_ptr     a_rs,
        i_NET::SignalReceiver_ptr   a_sr,
        const uint64_t              a_position,
        int32_t*                    a_buf,
        const size_t                a_bsize,
        size_t&                     a_read,
        SignalMeta&                 a_meta);

    SuccessCode_e ReadDodRemote(
        i_NET::RemoteSignal_ptr     a_rs,
        i_NET::SignalReceiver_ptr   a_sr,
        const uint64_t              a_position,
        uint32_t*                   a_buf,
        const size_t                a_bsize,
        size_t&                     a_read,
        SignalMeta&                 a_meta);

    SuccessCode_e ReadDodRemote(
        i_NET::RemoteSignal_ptr     a_rs,
        i_NET::SignalReceiver_ptr   a_sr,
        const uint64_t              a_position,
        int64_t*                    a_buf,
        const size_t                a_bsize,
        size_t&                     a_read,
        SignalMeta&                 a_meta);

    SuccessCode_e ReadDodRemote(
        i_NET::RemoteSignal_ptr     a_rs,
        i_NET::SignalReceiver_ptr   a_sr,
        const uint64_t              a_position,
        uint64_t*                   a_buf,
        const size_t                a_bsize,
        size_t&                     a_read,
        SignalMeta&                 a_meta);

    SuccessCode_e ReadDodRemote(
        i_NET::RemoteSignal_ptr     a_rs,
        i_NET::SignalReceiver_ptr   a_sr,
        const uint64_t              a_position,
        float*                      a_buf,
        const size_t                a_bsize,
        size_t&                     a_read,
        SignalMeta&                 a_meta);

    SuccessCode_e ReadDodRemote(
        i_NET::RemoteSignal_ptr     a_rs,
        i_NET::SignalReceiver_ptr   a_sr,
        const uint64_t              a_position,
        double*                     a_buf,
        const size_t                a_bsize,
        size_t&                     a_read,
        SignalMeta&                 a_meta);


    // DataOnDemandRemoteSource for runtime traits are instantiated in the library
    extern template class DataOnDemandRemoteSource<SignalTraitsVarUint8>;
    extern template class DataOnDemandRemoteSource<SignalTraitsVarInt16>;
    extern template class DataOnDemandRemoteSource<SignalTraitsVarUint16>;
    extern template class DataOnDemandRemoteSource<SignalTraitsVarInt32>;
    extern template class DataOnDemandRemoteSource<SignalTraitsVarUint32>;
    extern template class DataOnDemandRemoteSource<SignalTraitsVarInt64>;
    extern template class DataOnDemandRemoteSource<SignalTraitsVarUint64>;
    extern template class DataOnDemandRemoteSource<SignalTraitsVarFloat>;
    extern template class DataOnDemandRemoteSource<SignalTraitsVarDouble>;


}

#endif /* DATA_ON_DEMAND_REMOTE_SOURCE_H_ */

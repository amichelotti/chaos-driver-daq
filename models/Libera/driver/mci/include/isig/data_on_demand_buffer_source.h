/*
 * Copyright (c) 2012 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: data_on_demand_buffer_source.h 21249 2014-11-17 14:51:33Z damijan.skvarc $
 */

#ifndef DATA_ON_DEMAND_BUFFER_SOURCE_H_
#define DATA_ON_DEMAND_BUFFER_SOURCE_H_

#include "istd/trace.h"
#include "istd/system.h"
#include "isig/signal_traits.h"
#include "isig/array.h"
#include "isig/data_on_demand_local_source_base.h"
#include "isig/dod_client.h"

namespace isig {

    using namespace std::placeholders;

    /**
     * @addtogroup signal Signals and Streams handling Classes and functions
     * @{
     */

    template <class DOD_TRAITS>
        class DataOnDemandBufferSource:
            public DataOnDemandLocalSourceBase<DOD_TRAITS> {

        public:

            typedef DOD_TRAITS                      Traits;

            typedef DataOnDemandSourceBase<Traits>       DodBase;
            typedef DataOnDemandLocalSourceBase<Traits>  DodLocalBase;
            typedef DodControllerInterface<Traits>       DodCtrlInterface;

            typedef typename DodBase::BType         BType;
            typedef typename DodBase::Atom          Atom;
            typedef typename DodBase::MetaBuffer    MetaBuffer;
            typedef typename DodBase::Buffer        Buffer;

            typedef DataOnDemandBufferSource<Traits> ThisDod;
            typedef std::shared_ptr<ThisDod>        DodSharedPtr;
            typedef std::weak_ptr<ThisDod>          DodWeakPtr;

            typedef DodClient<Traits>               Client;

            using DodBase::AtomType;

            /**
             * Class to turn memory buffer (isig::Array) into DOD signal
             * @param a_name
             * @param a_sourceMemoryBuffer
             */
            DataOnDemandBufferSource(
                const std::string&  a_name,
                const Buffer&       a_sourceMemoryBuffer,
                const std::string&  a_owner = "")
            : DodLocalBase(a_name, a_sourceMemoryBuffer.GetTraits(), a_owner),
              m_sourceBuffer(a_sourceMemoryBuffer)
            {
                istd_FTRC();

                istd_TRC(istd::eTrcLow,
                    "DataOnDemandBufferSource: type = " << istd::CppName(AtomType())
                );
             };

            virtual ~DataOnDemandBufferSource()
            {
                istd_FTRC();
            };

            virtual std::size_t GetSize() const OVERRIDE
            {
                return m_sourceBuffer.GetLength();
            }

            // TODO: Move to DodCtrlInterface?
            Client CreateClient(const std::string& a_name)
            {
                auto tptr = std::dynamic_pointer_cast<ThisDod>(this->shared_from_this());
                auto ptr = std::static_pointer_cast<DodCtrlInterface>(tptr);
                return Client(ptr, a_name, this->m_traits);
            }

            /**
             * Reading data stream of defined atoms.
             * @param a_position  position specified in atoms
             * @return success code of the operation.
             */
            virtual SuccessCode_e Read(
                Buffer&             a_buf,
                size_t              a_position,
                PosControllerPtr) OVERRIDE
            {
                try {
                    m_sourceBuffer.SubArray(a_buf, a_position);
                }
                catch (const istd::Exception &e) {
                    return eIoError;
                }
                return eSuccess;
            };

        protected:

            // TODO: Move to DodCtrlInterface?
            virtual SignalClientBase* NewSignalClient(
                const std::string& a_name,
                size_t a_capacity = 0) OVERRIDE
            {
                auto tptr = std::dynamic_pointer_cast<ThisDod>(this->shared_from_this());
                auto ptr = std::static_pointer_cast<DodCtrlInterface>(tptr);
                return new Client(ptr, a_name, this->m_traits);
            }

        private:
            const Buffer&       m_sourceBuffer;
        };

}

#endif /* DATA_ON_DEMAND_BUFFER_SOURCE_H_ */

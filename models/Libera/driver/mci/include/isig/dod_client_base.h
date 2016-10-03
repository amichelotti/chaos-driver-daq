/*
 * Copyright (c) 2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: dod_client_base.h 21160 2014-10-08 09:11:09Z damijan.skvarc $
 */

#ifndef ISIG_DOD_CLIENT_BASE_H
#define ISIG_DOD_CLIENT_BASE_H

#include "isig/declarations.h"
#include "isig/array.h"
#include "isig/stream_queue.h"
#include "isig/signal_client_base.h"

namespace isig {

    template <class DOD_TRAITS>
        class DodClientBase : public SignalClientBase {
        public:

            typedef DOD_TRAITS                  Traits;

            typedef MetaBufferPair<Traits>              MetaBuffer;
            typedef typename MetaBuffer::Buffer         Buffer;
            typedef typename MetaBuffer::MetaBufferPtr  MetaBufferPtr;

            DodClientBase()
            : m_queue(m_traits, "unusable DOD client")
            {
            }

            DodClientBase(const std::string& a_name, const Traits& a_traits)
            : m_traits(a_traits), m_queue(m_traits, a_name)
            {
                istd_ASSERT(a_traits.GetComponents() > 0);
                istd_TRC(istd::eTrcMed,
                    "Dod client, atom components: " << a_traits.GetComponents());
            }

            virtual ~DodClientBase()
            {
            }

            DodClientBase(const DodClientBase& a_other)
            : m_traits(a_other.m_traits), m_queue(a_other.m_queue)
            {
            }

            DodClientBase& operator=(const DodClientBase& a_other)
            {
                if (&a_other != this) {
                    m_traits = a_other.m_traits;
                    m_queue = a_other.m_queue;
                }
                return *this;
            }

            SuccessCode_e Open(AccessMode_e a_mode) __attribute__((warn_unused_result))
            {
                if (a_mode == eModeDodOnEvent) {
                    // Acquisition size and (optionally) offset
                    // must be specified in open for acquisition on event
                    return eInvalidArg;
                }
                return Open(a_mode, 0, 0);
            }

            /**
             * @param a_mode Data on demand acquisition mode
             * @param a_readSize    eModeDodOnEvent: how many atoms to read on each event
             * @param a_offset      offset to apply on each read
             */
            virtual SuccessCode_e Open(
                AccessMode_e a_mode,
                size_t a_readSize,
                int64_t a_offset = 0) OVERRIDE __attribute__((warn_unused_result))
            {
                return eInternalError;
            }

            virtual bool IsOpen() const OVERRIDE
            {
                return false;
            }

            virtual AccessType_e SignalAccessType() OVERRIDE
            {
                return eAccessDataOnDemand;
            }

            /**
             * @param a_buf output buffer to read data to; buffer size indicates
             *              the required amount of data to be read; it can be
             *              resized if the required amount of data is not available
             * @param a_meta meta data belonging to the buffer
             * @param a_position position to read from (not applicable in all modes)
             */
            SuccessCode_e Read(
                Buffer&         a_buf,
                SignalMeta&     a_meta,
                uint64_t        a_position = 0 )
#ifndef NDEBUG
            __attribute__((warn_unused_result))
#endif
            {
                return eInternalError;
            }

            virtual void Close()
            {
                return;
            }

            Buffer CreateBuffer(size_t a_length)
            {
                return Buffer(a_length, m_traits);
            }

            const Traits& GetTraits() { return m_traits; }

        protected:

            typedef StreamQueue<Traits> BufferQueue;
            Traits      m_traits;
            BufferQueue m_queue;

            virtual SuccessCode_e ForwardTo(i_NET::SignalReceiver_ptr a_rec) OVERRIDE
            {
                return eInternalError;
            }

        private:
        };

    // DodClientBase for runtime traits are instantiated in the library
    extern template class DodClientBase<SignalTraitsVarUint8>;
    extern template class DodClientBase<SignalTraitsVarInt16>;
    extern template class DodClientBase<SignalTraitsVarUint16>;
    extern template class DodClientBase<SignalTraitsVarInt32>;
    extern template class DodClientBase<SignalTraitsVarUint32>;
    extern template class DodClientBase<SignalTraitsVarInt64>;
    extern template class DodClientBase<SignalTraitsVarUint64>;
    extern template class DodClientBase<SignalTraitsVarFloat>;
    extern template class DodClientBase<SignalTraitsVarDouble>;

}

#endif /* ISIG_DOD_CLIENT_BASE_H */

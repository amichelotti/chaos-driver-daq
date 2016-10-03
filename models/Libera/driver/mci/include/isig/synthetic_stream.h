/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: synthetic_stream.h 19990 2014-01-07 09:35:55Z damijan.skvarc $
 */

#ifndef ISIG_SYNTHETIC_H
#define ISIG_SYNTHETIC_H

#include <functional>

#include "istd/trace.h"
#include "istd/exception.h"
#include "isig/stream.h"

namespace isig {

    using namespace std::placeholders;

	/**
	 * @addtogroup signal
	 * @{
	 */

    /**
     * Template class that creates new synthetic stream from an existing
     * stream by applying a function on the input buffer.
     * @verbatim
     *
     * size_t Convert(const Signal::Buffer& i, SuccessCode_e is, SynSignal::Buffer& o)
     * {
     *      size_t outAtoms = 0;
     *      // Magic conversion ;)
     *      return isig::eSuccess;
     * }
     *
     * class TransformClass {
     * public:
     *    SuccessCode_e DoIt(const Signal::Buffer& i, SuccessCode_e is, SynSignal::Buffer& o)
     *    {
     *        return Convert(i, is, o);
     *    }
     *
     *    // TransformClass is a functor
     *    SuccessCode_e operator()(const Signal::Buffer& i, SuccessCode_e is, SynSignal::Buffer& o)
     *    {
     *        return Convert(i, is, o);
     *    }
     * };
     *
     * // Three ways of passing the conversion function to the synthetic signal
     * TransformClass tr;
     * SynSaSignal syn(&input, Convert);
     * SynSaSignal syn(&input, std::ref(tr));
     * SynSaSignal syn(&input, SynSaSignal::MapFunction(&TransformClass::DoIt, tr));
     *
     * @endverbatim
     *
     * @param SRC_TRAITS source stream traits
     * @param DEST_TRAITS destination stream traits
     */
    template <class SRC_TRAITS, class DEST_TRAITS>
        class SyntheticStream: public Stream<DEST_TRAITS> {
        public:

            typedef DEST_TRAITS                 Traits;
            typedef typename Traits::Atom       Atom;

            typedef Stream<SRC_TRAITS>                  SrcStream;
            typedef Stream<SRC_TRAITS>*                 SrcStreamPtr;
            typedef typename SrcStream::MetaBufferPtr   SrcMetaBufferPtr;
            typedef typename SrcStream::Buffer          SrcBuffer;

            typedef Stream<Traits>              DestStream;
            typedef Stream<Traits>*             StreamPtr;
            typedef typename DestStream::Buffer Buffer;

            typedef StreamClient<SRC_TRAITS>    SrcClient;
            typedef StreamClient<Traits>        Client;

            /**
             * Function that converts source buffer to destination buffer.
             * @param a_in input buffer
             * @param a_inStatus status of the read function providing input
             * @param a_inAtoms valid atoms in the input buffer
             * @param a_out output buffer
             * @return number of valid atoms in a_out
             */
            typedef std::function<
                SuccessCode_e(
                    const SrcBuffer&    a_in,
                    SuccessCode_e       a_inStatus,
                    Buffer&             a_out)
            > TransformFunction;

        public:

            explicit SyntheticStream(const std::string&          a_name,
                                     SrcStreamPtr                a_source,
                                     const TransformFunction&    a_func,
                                     StreamMode_e                a_mode = StreamMode_e::Active,
                                     const std::string&          a_owner = "") :
                DestStream( a_name, Traits(), a_mode, a_owner),
                m_source(a_source),
                m_func(a_func),
                m_client(m_source, "syn_" + istd::CppName(typeid(Atom))),
                m_empty(m_client.CreateBuffer(0)),
                m_ignoreMissed(false),
                m_srcSequenceNum(0)
            {

            };

            explicit SyntheticStream(const std::string&          a_name,
                                     SrcStreamPtr                a_source,
                                     const TransformFunction&    a_func,
                                     const Traits&               a_traits,
                                     StreamMode_e                a_mode = StreamMode_e::Active,
                                     const std::string&          a_owner = "") :
                DestStream(a_name, a_traits, a_mode, a_owner),
                m_source(a_source),
                m_func(a_func),
                m_client(m_source, "syn_" + istd::CppName(typeid(Atom))),
                m_empty(m_client.CreateBuffer(0)),
                m_ignoreMissed(false),
                m_srcSequenceNum(0)
            {

            };

            virtual ~SyntheticStream()
            {
                // Shutdown must be called in each dtor to disconnect any
                // stream client that is currently connected.
                this->ShutDown();
            }

            SrcClient& GetClient() {
                return m_client;
            }

            virtual SignalStats GetStatistics() const
            {
                return m_client.GetStatistics();
            }

            template <class F, class T>
                static TransformFunction MapFunction(F a_memFunc, T& a_instance)
                {
                    return std::bind(
                        std::mem_fn(a_memFunc), std::ref(a_instance), _1, _2, _3
                    );
                }

        protected:

        private:
            SrcStream           *m_source;
            TransformFunction   m_func;

            SrcClient           m_client;
            const SrcBuffer     m_empty; // Used to pass an empty buffer to transform function
            SrcMetaBufferPtr    m_srcBuf;

            bool                m_ignoreMissed;
            std::size_t         m_srcSequenceNum;

            /** @name Client interface
             *  Synthetic stream's input is another data stream.
             *  @{
             */
            virtual void OpenInput()
            {
                SuccessCode_e ret = m_client.Open();
                if (ret != eSuccess) {
                    throw istd::Exception(
                        "isig: Synthetic stream. Can't open source stream.");
                }
                m_srcSequenceNum = 0;
            }

            virtual void CloseInput()
            {
                m_client.Close();
            }

            /** Conversion from source buffer to target buffer */
            virtual SuccessCode_e ReadInput(Buffer &a_buf)
            {
                if (this->GetStreamMode() == StreamMode_e::Passive) {
                    istd_EXCEPTION(this->GetName() << ": Passive mode not supported.");
                }

                // Wait for the source buffer to get filled (async)
                SuccessCode_e ret = m_client.Read(m_srcBuf);

                switch (ret) {
                    case eSuccess:
                    case eIncomplete:
                    case eTimeout:
                        if (m_srcBuf) {
                            // Convert it to target buffer using provided function
                            return m_func(m_srcBuf->data, ret, a_buf);
                        }
                        else {
                            // m_srcBuf can be null when ret != eSuccess: pass empty buffer
                            return m_func(m_empty, ret, a_buf);
                        }
                        break;
                    case eInvalidArg:
                    case eClosed:
                    case eTerminated:
                    case eIoError:
                    case eNoData:
                    case eInternalError:
                        istd_TRC(istd::eTrcLow, "SyntheticStream read error: " << ret);
                }
                return ret;
            };
            ///@}

        };
        /**@}*/
} // namespace

#endif /* ISIG_SYNTHETIC_H */

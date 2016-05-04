/*
 * Copyright (c) 2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: data_on_demand_synthetic_source.h 21249 2014-11-17 14:51:33Z damijan.skvarc $
 */

#ifndef DATA_ON_DEMAND_SYNTHETIC_SOURCE_H_
#define DATA_ON_DEMAND_SYNTHETIC_SOURCE_H_

#include <functional>

#include "istd/trace.h"
#include "istd/system.h"
#include "isig/signal_traits.h"
#include "isig/array.h"
#include "isig/dod_controller_interface.h"
#include "isig/data_on_demand_source.h"
#include "isig/dod_client.h"

#include "istd/exception.h"

namespace isig {

    using namespace std::placeholders;

    /**
     * @addtogroup signal Signals and Streams handling Classes and functions
     * @{
     */

    template <class SRC_TRAITS, class DEST_TRAITS>
        class DataOnDemandSyntheticSource:
            public DataOnDemandLocalSourceBase<DEST_TRAITS> {

        public:

            typedef SRC_TRAITS                      SrcTraits;

            typedef DataOnDemandSource<SrcTraits>   SrcDod;
            typedef typename SrcDod::Buffer         SrcBuffer;
            typedef std::shared_ptr<SrcDod>         SrcDodSharedPtr;

            typedef DEST_TRAITS                     Traits;

            typedef DataOnDemandSourceBase<Traits>  DodBase;
            typedef DataOnDemandLocalSourceBase<Traits>  DodLocalBase;
            typedef DodControllerInterface<Traits>  DodCtrlInterface;

            typedef typename DodBase::BType         BType;
            typedef typename DodBase::Atom          Atom;
            typedef typename DodBase::MetaBuffer    MetaBuffer;
            typedef typename DodBase::Buffer        Buffer;

            typedef DataOnDemandSyntheticSource<SrcTraits, Traits>  ThisDod;
            typedef std::shared_ptr<ThisDod>        DodSharedPtr;
            typedef std::weak_ptr<ThisDod>          DodWeakPtr;

            typedef DodClient<Traits>               Client;

            using DodBase::AtomType;

            /**
             * Function that converts source buffer to destination buffer.
             * @param a_in input buffer
             * @param a_inStatus status of the read function providing input
             * @param a_atoms valid atoms in the input buffer
             * @param a_out output buffer
             */
            typedef std::function<
                SuccessCode_e(
                    const SrcBuffer&    a_in,
                    SuccessCode_e       a_inStatus,
                    Buffer&             a_out)
            > TransformFunction;

            template <class FUNC, class T>
                static TransformFunction MapFunction(FUNC a_memFunc, T& a_instance)
                {
                    return std::bind(
                        std::mem_fn(a_memFunc), std::ref(a_instance), _1, _2, _3
                    );
                }

            /**
             *
             * @param a_name
             * @param a_sourceDod
             * @param a_func
             * @param a_traits
             * @param a_srcBufSize    (optional) fixed input buffer size
             * @param a_destBufSize   (optional) fixed output buffer size
             * @param a_outMaxBufSize (optional) max number of atoms that can be read from the source
             */
            DataOnDemandSyntheticSource(
                const std::string&          a_name,
                SrcDodSharedPtr             a_sourceDod,
                const TransformFunction&    a_func,
                const Traits&               a_traits = Traits(), 
                size_t                      a_srcBufSize = 0,
                size_t                      a_destBufSize = 0,
                size_t                      a_maxOutBufSize = 0,
                const std::string&          a_owner = "")
            : DodLocalBase(a_name, a_traits, a_owner),
              m_sourceDod(a_sourceDod), 
              m_func(a_func),
              m_srcBufSize(a_srcBufSize),
              m_destBufSize(a_destBufSize),
              m_maxOutBufSize(a_maxOutBufSize)
            {
                istd_FTRC();

                istd_TRC(istd::eTrcLow,
                    "DataOnDemandSyntheticSource: "
                    << "src type = " << istd::CppName(m_sourceDod->AtomType())
                    << "dest type = " << istd::CppName(AtomType()) );

                // check if a max output buffer size has been specified
                if ((a_maxOutBufSize > 0) && (a_maxOutBufSize < m_sourceDod->GetSize()))
                    m_maxOutBufSize = a_maxOutBufSize;
                else if (a_maxOutBufSize > m_sourceDod->GetSize()) {
                    throw istd::Exception("Not possible to output more samples than the source signal provides.");
                    m_maxOutBufSize = m_sourceDod->GetSize();
                }
                // otherwise set it to the source buffer size
                else
                    m_maxOutBufSize = m_sourceDod->GetSize();


                // check if a fixed input buffer size has been specified out of range
                if (a_srcBufSize > m_sourceDod->GetSize())
                {
                    throw istd::Exception("Not possible to get more samples than the source signal provides.");
                    m_srcBufSize = m_sourceDod->GetSize();
                }

                // check if a fixed output buffer size has been specified out of range
                if ((a_destBufSize > m_maxOutBufSize) || ((a_srcBufSize > 0) && (a_destBufSize >= a_srcBufSize)))
                {
                    throw istd::Exception("Not possible to set fixed output buffer grater than available output samples.");
                    m_destBufSize = m_maxOutBufSize;
                }
            };

            virtual ~DataOnDemandSyntheticSource()
            {
                istd_FTRC();
            };

            virtual std::size_t GetSize() const OVERRIDE
            {
                return m_maxOutBufSize;
            }

            // TODO: Move to DodCtrlInterface?
            Client CreateClient(const std::string& a_name)
            {
                auto tptr = std::dynamic_pointer_cast<ThisDod>(this->shared_from_this());
                auto ptr = std::static_pointer_cast<DodCtrlInterface>(tptr);
                return Client(ptr, a_name, this->m_traits);
            }

            /** 
             *  @note If the class has its own position controller, this method returns it.
             *        Otherwise it returns the source dod controller.
             */
            virtual PosControllerPtr GetPositionController() const OVERRIDE
            {
                if (DodCtrlInterface::m_posCtrl == NULL) {
                    return m_sourceDod->GetPositionController();
                }
                return DodCtrlInterface::GetPositionController();
            }


            /** 
             *  @note If the class has its own position controller, this method returns it.
             *        Otherwise it returns the source dod controller.
             */           
            virtual PosControllerPtr GetPositionController(AccessMode_e a_mode) const OVERRIDE
            {
                if (DodCtrlInterface::m_posCtrl == NULL) {
                    return m_sourceDod->GetPositionController(a_mode);
                }
                return DodCtrlInterface::GetPositionController(a_mode);
            }

            /** 
             *  @note If the class has its own position controller, this method returns a clone of it.
             *        Otherwise it returns a clone of the source dod controller.
             */
            virtual PosControllerPtr GetPositionControllerCopy(AccessMode_e a_mode) const OVERRIDE
            {
                if (DodCtrlInterface::m_posCtrl == NULL) {
                    return m_sourceDod->GetPositionControllerCopy(a_mode);
                }
                return DodCtrlInterface::GetPositionControllerCopy(a_mode);
            }

            /** 
             *  @note If the class has its own position controllers, this method release one of them.
             *        Otherwise it releases one of the source dod controllers.
             */
            virtual void ReleasePositionControllerCopy(PosControllerPtr &a_pc) const OVERRIDE
            {
                if (DodCtrlInterface::m_posCtrl == NULL) {
                    return m_sourceDod->ReleasePositionControllerCopy(a_pc);
                }
                return DodCtrlInterface::ReleasePositionControllerCopy(a_pc);
             }

            /**
             * Reading data stream of defined atoms.
             * @a_position  position specified in atoms
             * @return      success code of the operation.
             */
            virtual SuccessCode_e Read(
                Buffer&             a_buf,
                size_t              a_position,
                PosControllerPtr    a_dodCtrl) OVERRIDE
            {
                // Keep track of the number of samples requested by the user 
                size_t requested_samples = a_buf.GetLength(); 

                // variable to calculate the proper sizes of the destination buffer
                size_t dest_buf_len = requested_samples;

                // temporary destination buffer that is used in the case there is a constraint
                // on the output buffer size and the user wants less samples in output.
                Buffer tmpDestBuf(this->CreateBuffer( dest_buf_len ));


                // EVALUATE THE BUFFER SIZES

                // if a maxOutBufSize has been set, verify that the size of the requested buffer doesn't overtakes it
                if ((m_maxOutBufSize > 0) && (a_buf.GetLength() > m_maxOutBufSize))
                {
                    istd_TRC(istd::eTrcLow, "DataOnDemandSyntheticSource: "
                    << "Read requestred too many atoms, resizing output buffer to " << m_maxOutBufSize);

                    dest_buf_len = m_maxOutBufSize;
                }

                // If requested by the application, resize also the destination buffer before the calculation
                if (m_destBufSize > 0)
                    dest_buf_len = m_destBufSize;

                // ..if only source buffer is specified, output buffer should be the same
                // as source. Of course output buffer size should not exceed optional m_maxOutBufSize parameter
                else if (m_srcBufSize > m_maxOutBufSize)
                    dest_buf_len = m_maxOutBufSize;
                else if (m_srcBufSize > 0)
                    dest_buf_len = m_srcBufSize;

                // Create the source buffer, checking if srcBufSize has been specified
                size_t source_size = m_srcBufSize  > 0 ? m_srcBufSize : dest_buf_len;
                SrcBuffer srcBuf(m_sourceDod->CreateBuffer( source_size ));

                // Read source stream
                SuccessCode_e ret = m_sourceDod->Read(srcBuf, a_position, a_dodCtrl);

                switch (ret) {
                    case eSuccess:
                    case eIncomplete:
                    case eTimeout:
                        // We got data: proceed with conversion
                        break;
                    case eInvalidArg:
                    case eClosed:
                    case eTerminated:
                    case eIoError:
                    case eNoData:
                    case eInternalError:
                        istd_TRC(istd::eTrcLow, "SyntheticDod read error: " << ret);
                        return ret;
                }

                // Do not call convert function in case we didn't get any data
                if (srcBuf.GetLength() == 0) {
                    return ret;
                }

                // Convert data by using supplied function
                if (dest_buf_len <= requested_samples)
                {
                    istd_TRC(istd::eTrcLow, "DataOnDemandSyntheticSource: Calculation from buffer to buffer - no copies");
                    a_buf.Resize(dest_buf_len);
                    ret = m_func(srcBuf, ret, a_buf);
                }
                else
                {
                    // use temporary destination buffer and then copy back the samples
                    istd_TRC(istd::eTrcLow, "DataOnDemandSyntheticSource: Calculation from buffer to tmp-buffer - it uses copies");
                    tmpDestBuf.Resize(dest_buf_len);
                    ret = m_func(srcBuf, ret, tmpDestBuf);
                  
                    for (size_t i = 0; i < a_buf.GetLength(); i++) {
                        a_buf[i] = tmpDestBuf[i];
                    }
                }

                return ret;
            };

        protected:

            // TODO: Move to DodCtrlInterface?
            virtual SignalClientBase* NewSignalClient(
                const std::string& a_name,
                size_t a_capacity = 0) OVERRIDE
            {
                istd_FTRC();
                auto tptr = std::dynamic_pointer_cast<ThisDod>(this->shared_from_this());
                auto ptr = std::static_pointer_cast<DodCtrlInterface>(tptr);
                return new Client(ptr, a_name, this->m_traits);
            }

        private:
            SrcDodSharedPtr     m_sourceDod;
            TransformFunction   m_func;

            // optional fixed input and output buffer sizes
            size_t              m_srcBufSize;
            size_t              m_destBufSize;

            // optional maximum output buffer size
            size_t              m_maxOutBufSize;
        };

}

#endif /* DATA_ON_DEMAND_SYNTHETIC_SOURCE_H_ */

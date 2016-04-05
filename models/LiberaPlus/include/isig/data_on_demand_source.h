/*
 * Copyright (c) 2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: data_on_demand_source.h 21249 2014-11-17 14:51:33Z damijan.skvarc $
 */

#ifndef DATA_ON_DEMAND_SOURCE_H_
#define DATA_ON_DEMAND_SOURCE_H_

#include "istd/trace.h"
#include "istd/system.h"
#include "isig/random_access_buffer.h"
#include "isig/signal_traits.h"
#include "isig/array.h"
#include "isig/dod_position_controller.h"
#include "isig/data_on_demand_local_source_base.h"
#include "isig/dod_controller_interface.h"
#include "isig/dod_client.h"

namespace isig {

    /**
     * @addtogroup signal Signals and Streams handling Classes and functions
     * @{
     */

    /**
     * Template for the data-on-demand-type data source.
     *
     * Template argument defines the type of the atom (indivisible piece of
     * information).
     *
     * Array<TypeTraits> is used to specify read requests. Proper
     * request size (multiple of atom) is therefore guaranteed at compile time.
     *
     * Uses RandomAccessBuffer as the source for the data.
     *
     * Multiple DoD can co-exists in a single RandomAccessBuffer. Constructor
     * parameters a_size and a_offset are used to define the region of each
     * DoD inside the RandomAccessBuffer.
     *
     * @param DOD_TRAITS    type traits of one record in the atom
     * @see SignalTraits
     */
    template <class DOD_TRAITS>
        class DataOnDemandSource:
            public DataOnDemandLocalSourceBase<DOD_TRAITS> {

        public:

            typedef DOD_TRAITS                      Traits;

            typedef DataOnDemandLocalSourceBase<Traits>  DodLocalBase;
            typedef DataOnDemandSourceBase<Traits>       DodBase;
            typedef DodControllerInterface<Traits>       DodCtrlInterface;

            typedef typename DodBase::BType         BType;
            typedef typename DodBase::Atom          Atom;
            typedef typename DodBase::MetaBuffer    MetaBuffer;
            typedef typename DodBase::Buffer        Buffer;

            typedef DataOnDemandSource<Traits>  ThisDod;
            typedef std::shared_ptr<ThisDod>    DodSharedPtr;
            typedef std::weak_ptr<ThisDod>      DodWeakPtr;

            typedef DodClient<Traits>               Client;

            const static size_t c_defaultSize = static_cast<size_t>(-1);

            using DodBase::AtomSize;
            using DodBase::AtomType;

            /**
             *
             * @param a_input       where the data is read from?
             * @param a_bufferStart absolute byte offset of this DOD data inside
             *                      the input buffer
             * @param a_size        the size in bytes inside the input buffer
             *                      default is input size minus offset
             * @param a_prioLevel   default priority level used when passing requests (Read)
             *                      to the input buffer in case if priority level is not specified
             *                      as an argument
             * @param a_readChunkAtomSize maximum size of read chunk in atom units.
             *                      Whole read request is split into several read chunks.
             *                      When set to 0, read request is not split.
             */
            DataOnDemandSource(
                const std::string           &a_name,
                RandomAccessBufferPtr const  a_input,
                size_t                       a_bufferStart = 0,
                size_t                       a_size = c_defaultSize,
                const size_t                 a_readChunkAtomSize = 0,
                const RequestPriorityLevel_e a_prioLevel = ePrioNormal,
                const Traits&                a_traits = Traits(),
                const std::string&           a_owner = "")
            : DodLocalBase(a_name, a_traits, a_owner),
              m_input(a_input),
              m_prioLevel(a_prioLevel),
              m_bufferStart(a_bufferStart),
              m_readChunkAtomSize(a_readChunkAtomSize)
            {
                istd_FTRC();

                if (a_bufferStart >= a_input->GetByteSize()) {
                    istd_EXCEPTION("Starting position of input buffer is not correctly specified.");
                }

                if (a_size > a_input->GetByteSize() - a_bufferStart) {
                    a_size = a_input->GetByteSize() - a_bufferStart;
                }

                if (a_size == 0) {
                    istd_EXCEPTION("Data range from input buffer is not correctly specified.");
                }

                m_size = a_size/AtomSize();

                istd_TRC(istd::eTrcLow,
                    "DataOnDemandSource: type = " << istd::CppName(AtomType())
                    << " size = " << m_size << "(atoms), "
                    << " chunk size = " << m_readChunkAtomSize << "(atoms), "
                    << " offset = " << m_bufferStart << "(bytes)");

                this->Open();
             };

            virtual ~DataOnDemandSource()
            {
                istd_FTRC();
                this->Close();
            };

            virtual std::size_t GetSize() const OVERRIDE
            {
                return m_size;
            }

            // TODO: Move to DodCtrlInterface?
            /**
             * @brief allocate client on the stack
             */
            Client CreateClient(const std::string& a_name)
            {
                auto tptr = std::dynamic_pointer_cast<ThisDod>(this->shared_from_this());
                auto ptr = std::static_pointer_cast<DodCtrlInterface>(tptr);
                return Client(ptr, a_name, this->m_traits);
            }

            /**
             * Reading data stream of defined atoms.
             * @param   a_position  position specified in atoms
             * @return  success code of the operation.
             */
            virtual SuccessCode_e Read(
                Buffer&             a_buf,
                size_t              a_position,
                PosControllerPtr    a_dodCtrl) OVERRIDE
            {
                try {
                    istd_TRC(istd::eTrcDetail,
                        "Reading " << a_buf.GetByteSize()
                        << " atoms at position (atoms) " << a_position);

                    return PriorityRead(a_buf, a_position, a_dodCtrl);
                }
                catch (const istd::Exception& e) {
                    istd_TRC(istd::eTrcMed,
                        "Error reading from history buffer " << e);
                }
                return eIoError;
            };

        protected:

            // TODO: Move to DodCtrlInterface?
            /**
             * @brief allocate client on the heap
             */

            virtual SignalClientBase* NewSignalClient(
                const std::string& a_name,
                size_t a_capacity = 0) OVERRIDE
            {
                istd_FTRC();
                auto tptr = std::dynamic_pointer_cast<ThisDod>(this->shared_from_this());
                auto ptr = std::static_pointer_cast<DodCtrlInterface>(tptr);
                return new Client(ptr, a_name, this->m_traits);
            }

            virtual void DoOpen() {
                m_input->Open();
                if (m_size == 0) {
                    m_size = m_input->GetByteSize() / this->AtomSize();
                    istd_TRC(istd::eTrcLow,
                        "DataOnDemandNew: queried size = " << m_size );
                }
            };

            virtual void DoClose()
            {
                m_input->Close();
            };

        private:
            const RandomAccessBufferPtr     m_input;
            const RequestPriorityLevel_e    m_prioLevel;

            /**
             * Offset in bytes into data inside the RandomAccessBuffer.
             */
            const size_t  m_bufferStart;

            /**
             * Size of logical data in atom units inside the RandomAccessBuffer.
             */
            size_t  m_size;

            /**
             * Maximum size of read chunk in atom units. Whole read request
             * is split into several read chunks.
             */
            size_t  m_readChunkAtomSize;

            /**
             * read data with the specified priority
             */
            SuccessCode_e PriorityRead(Buffer &a_buf, size_t a_position, PosControllerPtr a_dodCtrl)
            {
                const size_t atomsToRead = a_buf.GetLength();
                const size_t asize(this->AtomSize());
                char* buf = reinterpret_cast<char*>(a_buf.GetBuffer());
                size_t bufPos = 0;

                istd_TRC(istd::eTrcHigh, "ReadDod: position (atoms) " << a_position)
                istd_TRC(istd::eTrcHigh, "ReadDod: size (atoms) " << atomsToRead)

                // Split one large read request into multiple smaller chunk reads
                while (bufPos < atomsToRead) {

                    // Calculate how many atoms to read
                    size_t readSize = atomsToRead - bufPos;
                    if (m_readChunkAtomSize > 0) {
                        readSize = std::min(readSize, m_readChunkAtomSize);
                    }

                    // Read the chunk
                    size_t pos = (a_position + bufPos);
                    if (m_input->IsCircular()) {
                        pos %= m_size;
                    }
                    else if (pos >= m_size) {
                        /* no more data */
                        break;
                    }

                    if (!a_dodCtrl) {
                        istd_TRC(istd::eTrcLow, "Position controller becomes invalid, indicating a client, which specified "
                                                "the position controller, has been closed");
                        a_buf.Resize(0);
                        return eClosed;
                    }

                    // Validate the chunk position
                    if (!a_dodCtrl->IsPositionValid(pos)) {
                        istd_TRC(istd::eTrcLow, "Invalid chunk. Skipping remaining chunks.");
                        break;
                    }

                    size_t size = m_input->Read(
                        buf, readSize * asize,
                        (pos * asize) + m_bufferStart,
                        m_prioLevel);

                    if (size == 0) {
                        istd_TRC(istd::eTrcLow, "Received chunk with 0 bytes. Finish reading.");
                        break;
                    }

                    // move read buffer position to next chunk
                    bufPos += size / asize;

                    // Move the dest. buffer position with the size of previous read (in bytes)
                    buf = buf + size;
                }
                a_buf.Resize(bufPos);

                return (bufPos != a_buf.GetLength()) ? eIncomplete : eSuccess;
            }
        };

    // DataOnDemandSource for runtime traits are instantiated in the library
    extern template class DataOnDemandSource<SignalTraitsVarUint8>;
    extern template class DataOnDemandSource<SignalTraitsVarInt16>;
    extern template class DataOnDemandSource<SignalTraitsVarUint16>;
    extern template class DataOnDemandSource<SignalTraitsVarInt32>;
    extern template class DataOnDemandSource<SignalTraitsVarUint32>;
    extern template class DataOnDemandSource<SignalTraitsVarInt64>;
    extern template class DataOnDemandSource<SignalTraitsVarUint64>;
    extern template class DataOnDemandSource<SignalTraitsVarFloat>;
    extern template class DataOnDemandSource<SignalTraitsVarDouble>;

}

#endif /* DATA_ON_DEMAND_SOURCE_H_ */

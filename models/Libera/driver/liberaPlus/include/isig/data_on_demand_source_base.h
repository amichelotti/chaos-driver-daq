/*
 * Copyright (c) 2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: data_on_demand_source_base.h 19990 2014-01-07 09:35:55Z damijan.skvarc $
 */

#ifndef DATA_ON_DEMAND_SOURCE_BASE_H_
#define DATA_ON_DEMAND_SOURCE_BASE_H_

#include "istd/trace.h"
#include "istd/system.h"
#include "isig/signal_traits.h"
#include "isig/array.h"
#include "isig/signal_source.h"

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
     * @param DOD_TRAITS    type traits of one record in the atom
     * @see SignalTraits
     */
    template <class DOD_TRAITS>
        class DataOnDemandSourceBase: public SignalSource {

        public:

            typedef DOD_TRAITS                  Traits;
            typedef typename Traits::BaseType   BType;

            /** Indivisible piece of information (from traits) */
            typedef typename Traits::Atom       Atom;

            /** Buffer type, used for reading atoms */
            typedef MetaBufferPair<Traits>      MetaBuffer;
            typedef typename MetaBuffer::Buffer Buffer;

            DataOnDemandSourceBase(
                const std::string &a_name,
                const Traits& a_traits = Traits(),
                const std::string &a_owner = "")
            : SignalSource(a_name, a_owner), m_traits(a_traits)
            {
                istd_FTRC();

             };

            virtual ~DataOnDemandSourceBase()
            {
                istd_FTRC();
            };

            inline virtual std::size_t AtomSize() const OVERRIDE
            {
                return m_traits.GetAtomSize();
            };

            inline virtual std::size_t GroupSize() const OVERRIDE
            {
                return m_traits.GetGroupSize();
            };

            inline virtual const std::type_info& AtomType() const OVERRIDE
            {
                return typeid(Atom);
            };

            inline virtual const std::type_info&   BaseType() const OVERRIDE
            {
                return typeid(typename Traits::BaseType);
            };

            virtual std::size_t Length() const OVERRIDE
            {
                return m_traits.GetComponents();
            }

            inline virtual AccessType_e AccessType() const OVERRIDE
            {
                return AccessType_e::eAccessDataOnDemand;
            }

            virtual const ComponentNames GetComponentNames() const OVERRIDE
            {
                return ComponentNames(
                    m_traits.GetNames().begin(),
                    m_traits.GetNames().end());
            }

            inline const Traits& GetTraits() const
            {
                return m_traits;
            }

            Buffer CreateBuffer(size_t a_length)
            {
                return Buffer(a_length, m_traits);
            }

            /**
             * This method returns the size (in atoms) that is available from the data source.
             */
            inline virtual std::size_t GetSize() const 
            {
                return 0;
            }

        protected:
            const Traits    m_traits;
            std::mutex      m_input_x;

            void Open() {
                std::lock_guard<std::mutex> l(m_input_x);
                DoOpen();
            };

            void Close()
            {
                std::lock_guard<std::mutex> l(m_input_x);
                DoClose();
            };

            virtual void DoOpen() {
            };

            virtual void DoClose()
            {
            };

        };

    // DataOnDemandSourceBase for runtime traits are instantiated in the library
    extern template class DataOnDemandSourceBase<SignalTraitsVarUint8>;
    extern template class DataOnDemandSourceBase<SignalTraitsVarInt16>;
    extern template class DataOnDemandSourceBase<SignalTraitsVarUint16>;
    extern template class DataOnDemandSourceBase<SignalTraitsVarInt32>;
    extern template class DataOnDemandSourceBase<SignalTraitsVarUint32>;
    extern template class DataOnDemandSourceBase<SignalTraitsVarInt64>;
    extern template class DataOnDemandSourceBase<SignalTraitsVarUint64>;
    extern template class DataOnDemandSourceBase<SignalTraitsVarFloat>;
    extern template class DataOnDemandSourceBase<SignalTraitsVarDouble>;

}

#endif /* DATA_ON_DEMAND_SOURCE_BASE_H_ */

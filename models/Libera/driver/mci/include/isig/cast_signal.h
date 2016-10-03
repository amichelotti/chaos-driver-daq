/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: cast_signal.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef ISIG_CAST_SIGNAL_H_
#define ISIG_CAST_SIGNAL_H_

#include "isig/stream.h"
#include "isig/synthetic_stream.h"
#include "isig/data_on_demand_synthetic_source.h"
#include "isig/signal_traits.h"

namespace isig {

    /**
     * @addtogroup signal Signals and Streams handling Classes and functions
     * @{
     */

    /**
      * Template for signal conversions between different Trait types.
      * ex. from Int32 into Double, ...
      */

    class CastSignal {

    public:

        template <class DST_TRAITS>
        static SignalSourceSharedPtr Type(const std::string& a_name, SignalSource* a_signal)
        {
            isig::AtomType_e compType = a_signal->BaseEnumType();

            switch (compType) {
                case isig::eTypeDouble:
                    return SignalSourceSharedPtr(CreateSyntheticSignal
                            <SignalTraitsVarDouble, DST_TRAITS>(a_name, a_signal));
                    break;
                case isig::eTypeFloat:
                    return SignalSourceSharedPtr(CreateSyntheticSignal
                            <SignalTraitsVarFloat, DST_TRAITS>(a_name, a_signal));
                    break;
                case isig::eTypeUInt64:
                    return SignalSourceSharedPtr(CreateSyntheticSignal
                            <SignalTraitsVarUint64, DST_TRAITS>(a_name, a_signal));
                    break;
                case isig::eTypeInt64:
                    return SignalSourceSharedPtr(CreateSyntheticSignal
                            <SignalTraitsVarInt64, DST_TRAITS>(a_name, a_signal));
                    break;
                case isig::eTypeUInt32:
                    return SignalSourceSharedPtr(CreateSyntheticSignal
                            <SignalTraitsVarUint32, DST_TRAITS>(a_name, a_signal));
                    break;
                case isig::eTypeInt32:
                    return SignalSourceSharedPtr(CreateSyntheticSignal
                            <SignalTraitsVarInt32, DST_TRAITS>(a_name, a_signal));
                    break;
                case isig::eTypeUInt16:
                    return SignalSourceSharedPtr(CreateSyntheticSignal
                            <SignalTraitsVarUint16, DST_TRAITS>(a_name, a_signal));
                    break;
                case isig::eTypeInt16:
                    return SignalSourceSharedPtr(CreateSyntheticSignal
                            <SignalTraitsVarInt16, DST_TRAITS>(a_name, a_signal));
                    break;
                case isig::eTypeUInt8:
                    return SignalSourceSharedPtr(CreateSyntheticSignal
                            <SignalTraitsVarUint8, DST_TRAITS>(a_name, a_signal));
                    break;
                case isig::eTypeInt8:
                    return SignalSourceSharedPtr(CreateSyntheticSignal
                            <SignalTraitsVarInt8, DST_TRAITS>(a_name, a_signal));
                    break;
                case isig::eTypeUnknown:
                    break;
            }
            return SignalSourceSharedPtr();
        }

    private:

        template <class SRC_TRAITS , class DST_TRAITS>
        static SignalSourcePtr CreateSyntheticSignal(const std::string& a_name,
                                               SignalSource *a_signal)
        {
            if (a_signal->AccessType() == isig::eAccessStream) {
                return CreateSyntheticStream<SRC_TRAITS, DST_TRAITS>(a_name, a_signal);
            }
            else if (a_signal->AccessType() == isig::eAccessDataOnDemand) {
                return NULL;
                // Neds to be re-implemented if it is needed
                //return CreateSyntheticDod<SRC_TRAITS, DST_TRAITS>(a_name, a_signal);
            }
            return NULL;
        }

#if 0
        // Reimplement with DataOnDemandSyntheticSource
        template <class SRC_TRAITS , class DST_TRAITS>
        static SignalSourcePtr CreateSyntheticDod(const std::string& a_name, SignalSource *a_signal)
        {
            typedef DST_TRAITS  Traits;
            typedef typename isig::DataOnDemand<SRC_TRAITS>::Buffer SrcBuffer;
            typedef typename isig::DataOnDemand<DST_TRAITS>::Buffer DstBuffer;

            size_t numComp = a_signal->Length();
            ComponentNames components = a_signal->GetComponentNames();
            Traits traits(
                    numComp,
                    typename Traits::Names(components),
                    a_signal->GroupSize());

            typename SyntheticDod<SRC_TRAITS, DST_TRAITS>::TransformFunction ConvertFunc;

            ConvertFunc = CastFunc<SrcBuffer, DstBuffer>;

            DataOnDemand<SRC_TRAITS>* dod = dynamic_cast<DataOnDemand<SRC_TRAITS>*>(a_signal);

            return (new SyntheticDod<SRC_TRAITS, DST_TRAITS>(
               a_name,
               dod,
               ConvertFunc,
               traits));
        }
#endif

        template <class SRC_TRAITS , class DST_TRAITS>
        static SignalSourcePtr CreateSyntheticStream(const std::string& a_name, SignalSource *a_signal)
        {
            typedef DST_TRAITS  Traits;
            typedef typename isig::Stream<SRC_TRAITS>::Buffer SrcBuffer;
            typedef typename isig::Stream<DST_TRAITS>::Buffer DstBuffer;

            size_t numComp = a_signal->Length();
            ComponentNames components = a_signal->GetComponentNames();
            Traits traits(
                    numComp,
                    typename Traits::Names(components),
                    a_signal->GroupSize());

            typename SyntheticStream<SRC_TRAITS, DST_TRAITS>::TransformFunction ConvertFunc;

            ConvertFunc = CastFunc<SrcBuffer, DstBuffer>;

            Stream<SRC_TRAITS>* stream = dynamic_cast<Stream<SRC_TRAITS>*>(a_signal);

            return (new SyntheticStream<SRC_TRAITS, DST_TRAITS>(
               a_name,
               stream,
               ConvertFunc,
               traits));
        }

        /**
          * Callback method for synthetic signal
          */
        template <class SRC, class DST>
            static SuccessCode_e CastFunc(
                const SRC&      a_in,
                SuccessCode_e   a_inStatus,
                DST&            a_out)
            {
                if (a_inStatus != eSuccess && a_inStatus != eIncomplete) {
                    return a_inStatus;
                }

                size_t size = a_in.GetLength();

                if (a_out.GetLength() < size) {
                    a_out.Resize(size);
                }

                if (size == 0) {
                    return eSuccess;
                }

                // just transform from source type into destination type
                for (size_t i = 0; i < size; ++i) {
                   for (size_t j = 0; j < a_in[i].GetComponents(); ++j) {
                       a_out[i][j] = a_in[i][j];
                   }
                }
                return eSuccess;
            }

    };

/**@}*/

    // CastSignals for runtime traits are instantiated in the library
    extern template SignalSourceSharedPtr CastSignal::Type<SignalTraitsVarInt8>(const std::string& a_name, SignalSource* a_signal);
    extern template SignalSourceSharedPtr CastSignal::Type<SignalTraitsVarUint8>(const std::string& a_name, SignalSource* a_signal);
    extern template SignalSourceSharedPtr CastSignal::Type<SignalTraitsVarInt16>(const std::string& a_name, SignalSource* a_signal);
    extern template SignalSourceSharedPtr CastSignal::Type<SignalTraitsVarUint16>(const std::string& a_name, SignalSource* a_signal);
    extern template SignalSourceSharedPtr CastSignal::Type<SignalTraitsVarInt32>(const std::string& a_name, SignalSource* a_signal);
    extern template SignalSourceSharedPtr CastSignal::Type<SignalTraitsVarUint32>(const std::string& a_name, SignalSource* a_signal);
    extern template SignalSourceSharedPtr CastSignal::Type<SignalTraitsVarInt64>(const std::string& a_name, SignalSource* a_signal);
    extern template SignalSourceSharedPtr CastSignal::Type<SignalTraitsVarUint64>(const std::string& a_name, SignalSource* a_signal);
    extern template SignalSourceSharedPtr CastSignal::Type<SignalTraitsVarFloat>(const std::string& a_name, SignalSource* a_signal);
    extern template SignalSourceSharedPtr CastSignal::Type<SignalTraitsVarDouble>(const std::string& a_name, SignalSource* a_signal);

}

#endif /* ISIG_CAST_SIGNAL_H_ */

/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: corba_util.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */


#ifndef CORBA_UTIL_H
#define CORBA_UTIL_H

#include <ostream>
#include <string>
#include <vector>
#include <omniORB4/CORBA.h>

#include "istd/convert.h"
#include "istd/any.h"

// Replace with "GCC diagnostic push" and "GCC diagnostic pop"
// in gcc 4.5
#pragma GCC diagnostic ignored "-Wold-style-cast"
#include "inet/i_notification.h"
#include "inet/i_types.h"
#pragma GCC diagnostic warning "-Wold-style-cast"

namespace istd {
    class Any;
}

namespace inet {

    typedef std::vector<char> ByteBuffer;

    /**
     * @addtogroup corba CORBA utilities
     * @{
     */
    std::string CORBAExceptionToString(const CORBA::Exception &a_exc);
    std::string CORBASysExceptionToString(const CORBA::SystemException &a_exc);

    istd::Any CorbaAnyToAny(const CORBA::Any &a_any);
    CORBA::Any AnyToCorbaAny(const istd::Any& a_any);

    std::string CorbaKindToString(CORBA::TCKind a_kind);

    /**
     * Converts remote type (Corba Any) into local type
     * Implementation for simple types
     */
    template <class RT, class LT>
        inline bool CorbaAnyToLocal(const CORBA::Any &a_any, LT &a_l)
        {
            RT val;
            bool success = (a_any >>= val);
            if (success) {
                a_l = val;
            }
            return success;
        }
    template <>
        inline bool CorbaAnyToLocal<CORBA::Boolean, bool>(const CORBA::Any &a_any, bool &a_l)
        {
            CORBA::Boolean val;
            bool success = (a_any >>= CORBA::Any::to_boolean(val));
            if (success) {
                a_l = val;
            }
            return success;
        }

    /**
     * convert value stored in CORBA::Any instance into local argument type
     * conversion is done indirectly,
     *    by first observing the data type, stored in CORBA::Any argument
     *    next, by extracting data into local variable of the appropriate data type
     *    and finally, by conversion into specified destination data type
     */
    template <class LT>
        inline void CorbaAny2Local(const CORBA::Any &a_any, LT &a_l)
        {
            CORBA::TypeCode_var tc(a_any.type());
            CORBA::TCKind tck(tc->kind());

            if (tck == ::CORBA::tk_alias) {
                tck = tc->content_type()->kind();
                if (tck == ::CORBA::tk_sequence) {
                    tck = tc->content_type()->content_type()->kind();
                }
            }
            switch (tck) {
                case ::CORBA::tk_boolean: {
                    bool v;
                    a_any >>= CORBA::Any::to_boolean(v);
                    istd::Transform<bool,LT> x;
                    a_l = x.Convert(v);
                    break;
                }
                case ::CORBA::tk_long: {
                    CORBA::Long v;
                    a_any >>= v;
                    istd::Transform<CORBA::Long,LT> x;
                    a_l = x.Convert(v);
                    break;
                }
                case ::CORBA::tk_ulong: {
                    CORBA::ULong v;
                    a_any >>= v;
                    istd::Transform<CORBA::ULong,LT> x;
                    a_l = x.Convert(v);
                    break;
                }
                case ::CORBA::tk_longlong: {
                    CORBA::LongLong v;
                    a_any >>= v;
                    istd::Transform<CORBA::LongLong,LT> x;
                    a_l = x.Convert(v);
                    break;
                }
                case ::CORBA::tk_ulonglong: {
                    CORBA::ULongLong v;
                    a_any >>= v;
                    istd::Transform<CORBA::ULongLong,LT> x;
                    a_l = x.Convert(v);
                    break;
                }
                case ::CORBA::tk_double: {
                    CORBA::Double v;
                    a_any >>= v;
                    istd::Transform<CORBA::Double,LT> x;
                    a_l = x.Convert(v);
                    break;
                }
                case ::CORBA::tk_float: {
                    CORBA::Float v;
                    a_any >>= v;
                    istd::Transform<CORBA::Float,LT> x;
                    a_l = x.Convert(v);
                    break;
                }
                case ::CORBA::tk_string: {
                    const char *v(NULL);
                    a_any >>= v;
                    istd::Transform<const char*,LT> x;
                    a_l = x.Convert(v);
                    break;
                }
                case ::CORBA::tk_null:
                    istd_EXCEPTION("The specified Any argument is passed without having a value");
                    break;

                case ::CORBA::tk_sequence:
                    istd_EXCEPTION("Unsupported conversion from sequence value");
                    break;

                default:
                    istd_EXCEPTION("Unsupported conversion from "<< inet::CorbaKindToString(tck));
                    break;
            }
        }

    /**
     * Converts remote type (Corba Any) into local type
     * Implementation for converting corba sequence into vectors
     */
    template <class RT, class LT>
        inline bool CorbaAnyToLocalSeq(const CORBA::Any &a_any, std::vector<LT> &a_l)
        {
            RT *seq;
            bool success = (a_any >>= seq);

            if (success) {
                a_l.resize(seq->length());

                auto *seqBuff = seq->get_buffer();
                std::copy(seqBuff, seqBuff + seq->length(), a_l.begin());
            }

            return success;
        }

    /**
     * Converts remote type (Corba Any) into local type
     * Implementation for converting corba sequence into vectors
     * - Specialization for vector of strings
     */
    template <>
        inline bool CorbaAnyToLocalSeq<i_NET::StringBuffer, std::string >(
                                        const CORBA::Any &a_any,
                                        std::vector<std::string> &a_l)
        {
            i_NET::StringBuffer *seq;
            bool success = (a_any >>= seq);

            if (success) {
                a_l.resize(seq->length());

                for (size_t i = 0; i < a_l.size(); ++i) {
                    a_l[i] = ((*seq)[i]);
                }
           }

            return success;
        }

    /**
     * Converts local type into remote(corba) type and puts it into corba::any
     * Implementation for simple types
     */
    template <class LT, class RT>
        inline CORBA::Any LocalToCorbaAny(const LT &a_l)
        {
            CORBA::Any ret;
            ret <<= static_cast<RT>(a_l);
            return ret;
        }

    /**
     * Converts local type into remote(corba) type and puts it into corba::any
     * Implementation for simple types
     * - Specialization for strings
     */
    template <>
        inline CORBA::Any LocalToCorbaAny<std::string, const char*>(const std::string &a_l)
        {
            CORBA::Any ret;
            ret <<= a_l.c_str();
            return ret;
        }

    /**
     * Converts local type into remote(corba) type and puts it into corba::any
     * Implementation for simple types
     * - Specialization for booleans
     */
    template <>
    inline CORBA::Any LocalToCorbaAny<bool, CORBA::Boolean>(const bool &a_l)
        {
            CORBA::Any ret;
            ret <<= CORBA::Any::from_boolean(static_cast<CORBA::Boolean>(a_l));
            return ret;
        }

    /**
     * Converts local type into remote(corba) type and puts it into corba::any
     * Implementation for converting arrays into corba sequence
     */
    template <class LT, class RT>
        inline CORBA::Any LocalArrayToCorbaAny(const LT *a_l, const size_t a_size)
        {
            CORBA::Any ret;

            RT seq;
            seq.length(a_size);

            auto *seqBuff = seq.get_buffer();

            std::copy(a_l, a_l + a_size, seqBuff);

            ret <<= seq;

            return ret;
        }

    /**
     * Converts local type into remote(corba) type and puts it into corba::any
     * Implementation for converting arrays into corba sequence
     * - Specialization for strings
     */
    template <>
        inline CORBA::Any LocalArrayToCorbaAny<std::string, i_NET::StringBuffer>
                                  (const std::string *a_l, const size_t a_size)
        {
            CORBA::Any ret;

            i_NET::StringBuffer seq;
            seq.length(a_size);

            for (size_t i = 0; i < a_size; ++i) {
                seq[i] = a_l[i].c_str();
            }

            ret <<= seq;
            return ret;
        }


    /**
     * Converts local type into remote(corba) type and puts it into corba::any
     * Implementation for converting vectors into corba sequence
     */
    template <class LT, class RT>
        inline CORBA::Any LocalSeqToCorbaAny(const std::vector<LT> &a_l)
        {
            CORBA::Any ret;

            RT seq;
            seq.length(a_l.size());

            auto *seqBuff = seq.get_buffer();

            std::copy(a_l.begin(), a_l.end(), seqBuff);

            ret <<= seq;
            return ret;
        }

    /**
     * Converts local type into remote(corba) type and puts it into corba::any
     * Implementation for converting vectors into corba sequence
     * - Specialization for strings
     */
    template <>
        inline CORBA::Any LocalSeqToCorbaAny<std::string, i_NET::StringBuffer>
                                    (const std::vector<std::string> &a_l)
        {
            return LocalArrayToCorbaAny<std::string, i_NET::StringBuffer>(&(a_l[0]), a_l.size());
        }

} // namespace

namespace CORBA {
    std::ostream& operator<< (std::ostream &a_os, const SystemException &a_sysExc);
    std::ostream& operator<< (std::ostream &a_os, const Exception &a_exc);
    std::ostream& operator<< (std::ostream &a_os, const Object_ptr &a_obj);
    std::ostream& operator<< (std::ostream &a_os, const Any &a_any);
}

namespace omniORB {
    std::ostream& operator<< (std::ostream &a_os, const fatalException& a_fe );
}

/**@}*/

#endif  //CORBA_UTIL_H

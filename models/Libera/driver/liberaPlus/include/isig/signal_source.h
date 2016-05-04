/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: signal_source.h 19990 2014-01-07 09:35:55Z damijan.skvarc $
 */

#ifndef ISIG_SIGNAL_H
#define ISIG_SIGNAL_H

#include <typeinfo>
#include <string>

#include "mci/declarations.h"
#include "isig/declarations.h"
#include "istd/id.h"

namespace isig {

	/**
	 * @addtogroup signal
	 * @{
	 */

    typedef std::vector<std::string>    ComponentNames;

    struct  Id_SignalSource {};
    typedef istd::Id<Id_SignalSource, uint64_t>  SignalSourceId;

    class SignalClientBase;

    /**
     * Interface class to define signal source. Specific sources of signals
     * are defined by templated derived classes.
     */
    class SignalSource : public std::enable_shared_from_this<SignalSource>{
    public:

        SignalSource(const std::string &a_name, const std::string &a_owner = "");
        SignalSource(const isig::SignalSource &a_signal) = delete;
        virtual ~SignalSource();

        const std::string&   GetName()  const { return m_name;  }
        const SignalSourceId GetId()    const { return m_id;    }
        const std::string&   GetOwner() const { return m_owner; }

        void SetOwner(const std::string &a_owner) { m_owner = a_owner; }

        /**
         * Size of the indivisible piece of data in bytes. Depending on the type
         * of the data source it can be the size of the data record (data on
         * demand) or a fixed buffer (stream).
         */
        virtual std::size_t AtomSize() const = 0;

        /**
         * Atoms are sometimes packed together into groups that are handled
         * together.
         */
        virtual std::size_t GroupSize() const = 0;

        /**
         * RTTI of the record comprising the atom.
         * @see AtomSize
         */
        virtual const std::type_info&   AtomType() const = 0;

        /**
         * Type of the smallest element (numeric type), comprising the signal.
         * As type_info.
         */
        virtual const std::type_info&   BaseType() const = 0;

        /**
         * Number of base type components, comprising the signal.
         */
        virtual std::size_t             Length() const = 0;

        /**
         * Names of the individual components (columns) of the signal.
         */
        virtual const ComponentNames GetComponentNames() const = 0;

        /**
         * Type of access, for example data on demand or stream.
         */
        virtual AccessType_e AccessType() const = 0;

        /**
         * Type of data, defined and interpreted by an application, for example
         * slow acquisition (SA) or turn by turn (TBT).
         */
        virtual unsigned int SourceType() const
        {
            return 0;
        }

        /**
         * Type of the smallest element (numeric type), comprising the signal.
         * As enumeration.
         */
        AtomType_e BaseEnumType() const;

        virtual SignalStats GetStatistics() const;

        virtual SignalClientBase* NewSignalClient(
                const std::string& a_name,
                size_t a_capacity = 0);

        virtual void ShutDown();

        SignalSourceSharedPtr SharedPtr() { return shared_from_this(); }

        /* callback function, called when a system has been announced to change timestamp */
        virtual void OnAnnouncedTimestampChange() {};

        /* callback function, called when a system has changed timestamp */
        virtual void OnAdjustedTimestamp(int64_t a_diff_lmt, uint64_t a_start_lmt) {};

        virtual void Cancel(SuccessCode_e a_code);

    protected:

    private:
        SignalSourceId   m_id;
        std::string      m_name;
        std::string      m_owner;
    };

    /**@}*/
}

#endif /* ISIG_SIGNAL_H */

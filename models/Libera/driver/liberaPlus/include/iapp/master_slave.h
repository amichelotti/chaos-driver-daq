/*
 * Copyright (c) 2008-2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * There are cases where some class instances must be aware about changes which
 * happen in some other instances. The following code deals with this situation.
 * It is actually just a basic framework, keeping relation between Master & Client
 * classes.
 *
 * In the Application, a class which needs to update dependent classes, should be derived
 * from Master class; Classes which need to be informed about this change, should be
 * derived from Slave class.
 *
 * The purpose of this class is to implement synchronized modifications, i.e. when a change
 * is made, slaves must be informed immediately, in the same thread which implements changes.
 * This is different to asynchronous notification, where information to slaves is passed
 * independently to the applied changes, usually by using some other thread. (for example
 * notification mechanism in ireg library)
 *
 * $Id: master_slave_objects.h 14836 2012-01-30 12:22:51Z damijan.skvarc $
 */

#ifndef MASTER_SLAVE_H
#define MASTER_SLAVE_H

#include <mutex>
#include <set>


namespace iapp {

/*--------------------------------------------------------------------------------------*/
/* data type declarations                                                               */

enum Importance_e {eLow,
                   eMedium,
                   eHigh,
                   eCritical};


/*--------------------------------------------------------------------------------------*/
/* forward declarations                                                                 */

class Slave;
class Master;

/*--------------------------------------------------------------------------------------*/
/* Master class declaration                                                             */

class Master
{
public:
    Master();
    virtual ~Master();

    void     Register(Slave* a_slave);

protected:
    void     UpdateSlaves(Importance_e a_importance);

private:
    std::mutex          m_mutex;    /* controls the list of slaves   */
    std::set<Slave*>    m_slaves;   /* list of slaves to be informed */
};


/*--------------------------------------------------------------------------------------*/
/* Slave class declaration                                                              */
/* implemented as template to be able handle of multiple master objects                 */

class Slave
{
public:
    template <typename First, typename... Rest>
       Slave(First& first, Rest&... rest) {
           Register(first, rest...);
       }
    Slave() {};
    virtual ~Slave() {};

protected:
    friend class Master;
    virtual void OnMasterChange(const Master &a_master, Importance_e a_importance)=0;

private:
    template<typename First, typename... Rest>
        void Register(First& first, Rest&... rest)
        {
           if (dynamic_cast< Master *>(&first)) {
               first.Register(this);
           }
           Register(rest...);
        }

    void Register() {};
};

} /* namespace */

#endif //MASTER_SLAVE_H

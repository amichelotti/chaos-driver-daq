/*
 * Copyright (c) 2008-2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * In some situations, multiple settings must be applied into FPGA internal entities
 * There are two mechanism for this implementation "double_buffer" and "update_bit"
 * In the first alternative, there exists two banks of FPGA registers, modifications
 * are written into inactive bank and at the end of modifications, banks are swapped.
 *
 * In the "update_bit" alternative, there exist only one bank of registers. After
 * all changes are written into this bank, one additional bit needs to be triggered to
 * activate changes (force them to be written from I/O bank registers into internal entities
 *
 * The Activator and derived ActivatorBit class handles the later functionality
 *
 * $Id: activator.h 14836 2012-01-30 12:22:51Z damijan.skvarc $
 */

#ifndef ACTIVATOR_H
#define ACTIVATOR_H

#include "ireg/declarations.h"
#include "master_slave.h"


namespace iapp {

/*=================================================================================*/
/* class definition                                                                */

class Activator : public Slave {
    typedef std::unique_lock<std::recursive_mutex> Locker;

public:
    Activator(const std::string &a_name, const std::string &a_reg_name);
    virtual ~Activator();

    void AddRegistry(ireg::TreeNodePtr &parent);

    void Lock();
    void Release();

    const std::string &GetName() const
    {
       return m_name;
    }

    virtual void Set(bool a_val) = 0;
    virtual bool Get() const = 0;

protected:
    virtual void OnMasterChange(const Master &a_master, Importance_e a_importance);
    bool         GetState() const { return m_state; }

private:
    /* register implementation function, valid only in unlocked state */
    bool _SetActive(const bool &a_val);
    bool _GetActive(bool &a_val) const;

    void Apply(bool a_value);

private:
    const std::string     m_name;      /* name of Activator instance                                 */
    const std::string     m_reg_name;  /* name of appropriate registry item                          */
    bool                  m_modified;  /* indicating a value was changed while being in delayed mode */
    bool                  m_locked;    /* delayed mode is active: i.e transactions                   */
    std::recursive_mutex  m_mutex;     /* controlling access to m_locked member                      */
    bool                  m_value;     /* a last value, applied while being in delayed mode          */
    bool                  m_state;     /* applied state                                              */
    ireg::TreeNodePtr     m_node;      /* registry node                                              */
};


/*=================================================================================*/
/* template class, handling "update status" information as single bit which is     */
/* located in address register of different data type                              */

template <class T>
class ActivatorBit : public Activator
{
public:

    ActivatorBit (const std::string &a_name, volatile T &a_address, uint32_t a_bit,
                  bool a_readback = true,
                  const std::string &a_reg_name = "valid") :
        Activator(a_name, a_reg_name), m_address(a_address), m_bit(a_bit)
    {
        if (m_bit >= sizeof(T)*8) {
            istd_EXCEPTION("UpdateBit: Invalid argument");
        }
    }

    virtual ~ActivatorBit() {};

    volatile T *GetBitReference(T &a_mask) const
    {
        a_mask = static_cast<T>(1 << m_bit);
        return GetAddress();
    }

    volatile T *GetAddress() const
    {
        return &m_address;
    }

    /* set or clear the referenced bit */
    virtual void Set(bool a_val)
    {
        if (a_val) {
            m_address |= static_cast<T>(1 << m_bit);
        }
        else {
            m_address &= ~(static_cast<T>(1 << m_bit));
        }
    }

    /* get the state of referenced bit */
    virtual bool Get() const
    {
        if (m_readback) {
            return (static_cast<T>(1 << m_bit) & m_address) ? true : false;
        }
        else {
            return GetState();
        }
    }

private:
    volatile T  &m_address;   /* address of update register                        */
    uint32_t     m_bit;       /* bit location within update register               */
    bool         m_readback;  /* indicating the actual status can be readback      */
                              /* from update register. If not available the        */
                              /* actual state is returned from local cache, stored */
                              /* as Activator::GetState()                          */
};


/*=================================================================================*/
/* ActivatorLocker class is used to control multiple ActivatorBit instances        */
/* which control bits within the same register. The purpose of this class is to    */
/* hold of individual activations and do it once for all controlled Activators     */
/* This is done by using Lock() / Release() features                               */
/* A class is intended to be use dynamically (i.e. it is instantiated multiple     */
/* times during life time of application. Whenever it is instantiated, all         */
/* referenced activators are locked automatically and released in the instance is  */
/* destroyed.                                                                      */

template <class T>
class ActivatorLocker
{
    typedef std::vector<Activator*> Activators;

public:
    ActivatorLocker() {};

    ActivatorLocker(Activators &a_activators) : m_address(NULL), m_mask(0)
    {
        ActivatorBit<T>* activator;

        auto it = a_activators.begin();

        if (it == a_activators.end()) {
            istd_EXCEPTION("Internal error: Activators are not specified");
        }

        activator = dynamic_cast<ActivatorBit<T>*>(*it);
        m_address = activator->GetBitReference(m_mask);

        for (; it < a_activators.end(); it++ ) {
            activator = dynamic_cast<ActivatorBit<T>*>(*it);

            if (activator->GetAddress() != m_address) {
                istd_EXCEPTION("Internal error: Activation entity of specified activator " << activator->GetName() << " is not equal");
            }
            activator->Lock();
            m_activators.push_back(activator);
        }
    }

    template <typename First, typename... Rest>
      ActivatorLocker(First& first, Rest&... rest) :
          m_address(first.GetBitReference(m_mask))
      {
          /* lock automatically the specified activators */
          DoLock(first,rest...);
      }

    virtual ~ActivatorLocker()
    {
        for ( auto it = m_activators.begin() ; it < m_activators.end(); it++ ) {
            (*it)->Release();
        }
    }

private:
    template<typename First, typename... Rest>
    bool DoLock(First& first, Rest&... rest)
    {
       do  {
           /* check first if the referenced address of activator is the same as others */
           if (first.GetAddress() != m_address) {
               istd_EXCEPTION("Internal error: Invalid argument");
           }
           first.Lock();
           m_activators.push_back(&first);
       }
       while (DoLock(rest...));
       return false;
    }

    bool DoLock()
    {
        return false;
    }

private:
    volatile T  *m_address;
    T            m_mask;
    Activators   m_activators;
};

} /* namespace */

#endif /* ACTIVATOR_H */

/*
 * Copyright (c) 2008-2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * MultiPorttable class is used to handle list (array) of parameters
 * which optionally can be activated at the same time, by using specified
 * activator. Each of parameter can be accessed through separate address location
 * which leads to the name MultiPort.... As an alternative there exist tables,
 * whose entries can be access through the same address location (SinglePortTable)
 *
 * Frequently used uses cases for this class are PI controller (two parameters)
 * Phase Shifter (two parameters I/Q) and different filter types (with multiple parameters).
 *
 * Parameter names are passed as an argument.
 * In FPGA memory address list, parameters must be allocated on subsequent locations, where
 * both the basic type must be defined (template keyword) and the number of used bits. Each
 * parameter must be of the same data type.
 *
 * Modifications of parameters can be done through a register interface individually or through
 * array registry interface. In case activator functionality is used (by passing not NULL argument) then
 * after changes are applied through array interface, they are activated automatically. In case changes
 * are applied individually a client (user or Sw) is responsible to activating changes manually.
 * 
 * $Id: multi_port_table.h 15174 2012-03-01 10:24:59Z damijan.skvarc $
 */

#ifndef MULTI_PORT_TABLE_H
#define MULTI_PORT_TABLE_H

#include "ireg/node.h"
#include "bmc/board.h"

#include "iapp/libera_fpga_object.h"

#include "activator.h"


namespace iapp {


/*----------------------------------------------------------------------------*/
/* helper class for handling individual modifications through registry        */
template <class T>
class MultiPortTable;

template <class T>
class MultiPortTableEntry
{
private:
    friend class MultiPortTable<T>;

    MultiPortTableEntry(MultiPortTable<T> &a_table, const std::string &a_name, volatile T &a_address, T a_def_val);
    void   AddReg(ireg::TreeNodePtr a_parent, const ireg::Flags a_flags);
    void   EmitNotification();

    bool   _Set(const T &a_val);
    bool   _Get(T &a_val) const;

private:
    MultiPortTable<T>   &m_table;
    const std::string    m_name;
    volatile T          &m_address;
    ireg::TreeNodePtr    m_node;
};


/*----------------------------------------------------------------------------*/
/* TableParameters class definition                                           */

template <class T>
class MultiPortTable : public iapp::LiberaFpgaObject, public Master
{
public:
    typedef std::string                    DescriptionEntry;
    typedef std::vector<DescriptionEntry>  Description;
    typedef std::vector<T>                 Values;

    MultiPortTable(iapp::LiberaObject &a_parent,
                   FPGAMem_t          *a_reg,
                   const std::string  &a_name,
                   const Description  &a_description,
                   const Values       &a_init_values,
                   Activator          *a_activator = NULL,
                   uint32_t            a_bit_width = sizeof(T)*8,
                   uint32_t            a_bit_shift = 0,
                   uint32_t            a_gap       = 0,
                   const ireg::Flags   a_flags     = ireg::eNfReadable | ireg::eNfWritable);

    ~MultiPortTable();

    size_t        Size();
    bool          SetMultiple(const T *a_values, const size_t a_pos, const size_t a_size);
    size_t        GetMultiple(T *a_values, const size_t a_pos, const size_t a_size) const;

protected:
    virtual void  OnRegistryAdd(ireg::TreeNodePtr &parent);

private:
    bool          Set(volatile T &a_address, const T &a_val);
    bool          Get(volatile T &a_address, T &a_val) const;

    bool          _SetMultiple(const T *a_values, const size_t a_pos, const size_t a_size);

private:
    typedef std::vector<MultiPortTableEntry<T>*>  Entries;
    friend class MultiPortTableEntry<T>;

    volatile T        *m_reg;
    Activator         *m_activator;
    const uint32_t     m_bit_width;
    const uint32_t     m_bit_shift;
    const uint32_t     m_bit_mask;
    const uint32_t     m_gap;
    const ireg::Flags  m_flags;
    Entries            m_entries;
    ireg::TreeNodePtr  m_node;
};

} /* namespace */

#endif /* MULTI_PORT_TABLE_H */

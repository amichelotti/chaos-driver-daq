/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: identification_block.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef _IDENTIFICATION_BLOCK_H
#define _IDENTIFICATION_BLOCK_H

#include "bmc/board.h"
#include "iapp/libera_fpga_object.h"


/*-----------------------------------------------------------------------------*/
class LiberaBoard;
struct tIdent;

class IdentificationBlock : public iapp::LiberaFpgaObject
{
public:
    bool              SetHealthMask(uint32_t a_val);
    bool              GetHealthMask(uint32_t& a_val) const;

    static size_t     Size();

protected:
    virtual void      OnRegistryAdd(ireg::TreeNodePtr &parent);

    IdentificationBlock(iapp::LiberaBoard &a_board,
                        FPGAMem_t *a_reg,
                        const std::string &a_name);

    virtual ~IdentificationBlock();

    template<typename OBJ_TYPE, typename T, typename ...ARGS>
           friend typename std::enable_if<std::is_base_of<LiberaObject,OBJ_TYPE>::value, OBJ_TYPE>::type *
           iapp::Create(T &a_parent, ARGS&& ...a_args);

private:
    volatile tIdent  *m_reg;

    // Constant values from FPGA ID block are cached not be retrieved
    // over PCI on each access.
    uint32_t    m_customerId;
    std::string m_customer;
    uint32_t    m_boardNum;
    uint32_t    m_subsystemId;
    uint32_t    m_applicationId;
    uint32_t    m_boardId;
    uint32_t    m_fpgaRevision;
    uint32_t    m_features;

};


#endif // _IDENTIFICATION_BLOCK_H

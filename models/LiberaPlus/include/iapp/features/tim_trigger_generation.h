/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * Trigger generation fpga object for TIM board
 *
 * $Id: tim_trigger_generation.h 15636 2012-04-05 13:35:51Z matej.kenda $
 */

#ifndef TIM_TRIGGER_GENERATION_H
#define TIM_TRIGGER_GENERATION_H

#include <map>
#include <array>
#include "iapp/libera_fpga_object.h"
#include "iapp/libera_board.h"
#include "ireg/node.h"
#include "bmc/types.h"



enum TriggerSourceType   {  eTrgSrcOff      = 0,       /* see higgs documentation */
                            eTrgSrcExternal = 1,
                            eTrgSrcInternal = 2,
                            eTrgSrcPulse    = 3,
                            eTrgSrcLXI      = 4,
                            eTrgSrcRTC      = 5 };

enum TriggerType          { eTrgLine0  = 0,
                            eTrgLine1  = 1,
                            eTrgLine2  = 2,
                            eTrgTypeMc = 3 };

enum TriggerPropertyType  { eTrgPropName            = 0,
                            eTrgPropSource          = 1,
                            eTrgPropPrescaler       = 2,
                            eTrgPropCode            = 3,
                            eTrgPropDebouncer       = 4,
                            eTrgPropIgnoredCount    = 5,
                            eTrgPropExtIgnoredCount = 6 };

enum BackplaneLineT0_e    { eT0_T0   = 0,
                            eT0_MC   = 1 };

const size_t c_trigLines      (eTrgTypeMc              + 1);
const size_t c_trigProperties (eTrgPropExtIgnoredCount + 1);


/*-----------------------------------------------------------------------------*/

struct TimTriggerGenerator;

struct TrigConfiguration {
    const char* name;
    bool        visible;
    uint32_t    line_id;
};


/*-----------------------------------------------------------------------------*/
namespace ifeature {

class TimTriggerGeneration : public iapp::LiberaFpgaObject
{
public:
    typedef const std::array<TrigConfiguration, c_trigLines>    TriggerConfigurations;

    struct RegNodeProperty {
        const char* name;
        ireg::Flags flags;
    };

    struct RegNodeConfiguration {
        TriggerPropertyType  id;
        RegNodeProperty      property;
    };

    typedef const std::vector<RegNodeConfiguration>  RegistryConfiguration;

    TimTriggerGeneration(
        iapp::LiberaBoard            &a_board,
        FPGAMem_t                    *a_addr,
        const std::string            &a_name,
        const TriggerConfigurations  &a_trigConfig  = c_trigConfigurations,
        const RegistryConfiguration  *a_propConfig  = NULL,
        const bool                    a_hasTrigSwitch = false);

    virtual ~TimTriggerGeneration();

    static size_t  Size();

    static TriggerConfigurations   c_trigConfigurations;

    bool SetBackplaneT0Switch(const BackplaneLineT0_e& a_val);

    bool SetTriggerSource(size_t a_trigLine, const TriggerSourceType &a_val);
    bool GetTriggerSource(size_t a_trigLine, TriggerSourceType &a_val) const;

    bool SetTriggerCode(size_t a_trigLine, const uint32_t &a_val);
    bool GetTriggerCode(size_t a_trigLine, uint32_t &a_val) const;

    bool SetTriggerPrescaler(size_t a_trigLine, const uint32_t &a_val);
    bool GetTriggerPrescaler(size_t a_trigLine, uint32_t &a_val) const;

    bool SetTriggerDebouncer(size_t a_trigLine, const uint32_t &a_val);
    bool GetTriggerDebouncer(size_t a_trigLine, uint32_t &a_val) const;

    bool GetTriggerIgnoredCounter(size_t a_trigLine, uint32_t &a_val) const;

    bool SetLXIDirection(const uint32_t &a_val);
    bool GetLXIDirection(uint32_t &a_val) const;

    bool SetLemoControl(const uint32_t &a_val);
    bool GetLemoControl(uint32_t &a_val) const;

protected:
    virtual void   OnRegistryAdd(ireg::TreeNodePtr &parent);
    virtual void   OnPreConfig();

private:
    const ireg::TreeNodeWeakPtr&  SelectSource(size_t a_trigLine) const;

    typedef      std::array<RegNodeProperty, c_trigProperties> RegNodeProperties;
    static const RegNodeProperties  c_defRegNodeProperties;

    volatile TimTriggerGenerator   *m_reg;
    ireg::TreeNodePtr               m_nodes[c_trigLines];
    ireg::TreeNodePtr               m_backplaneT0Node;
    TriggerConfigurations           m_config;
    RegNodeProperties               m_regNodeProperties;
    bool                            m_hasTrigSwitch;
};

}  // ifeature namespace

#endif // TIM_TRIGGER_GENERATION_H

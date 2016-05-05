/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: config_mng.h 14606 2011-12-21 12:43:41Z matej.kenda $
 */


#ifndef CONFIG_MNG_H_
#define CONFIG_MNG_H_

#include <functional>

#include "icfg/serializable.h"
#include "bmc/types.h"

/**
 * HW initialization code helper functions.
 */
bool ParseCommandParams(const std::string &a_parStr, unsigned int& a_val);
bool ParseCommandParams(const std::string &a_parStr, uint64_t& a_val);

typedef std::function<bool(unsigned int , unsigned int ,
        const istd::StrTokens &,
        uint8_t* , uint32_t* )>  HwInitFunction;

/**
 * XML file contains the value for subsystem ID in the form
 * "<NUM>, <DESCRIPTION>", which is then converted into a tuple.
 * NUM is written to firmware configuration structure.
 */
struct SubsystemIdPair_t {
    uint8_t         id;
    std::string     description;
};

SubsystemIdPair_t StrToSubsystemIdPair(const std::string& a_str);

/**
    * @class ConfigMng
    *
    * @brief
    */
class ConfigMng : public icfg::ISerializable {

public:

    static const std::string cAll;
    static const std::string cDefault;

    struct CmdList_t
    {
        unsigned int    cmdCode;  ///< cmd code
        const char *    mnem;     ///< mnemonic
        unsigned int    pNum;     ///< number of parameters
        unsigned int    pSize;    ///< size (in bytes) of parameters
        HwInitFunction  add_hwinit_fn;  ///< HW init code adder.
    };

    bmc::BoardConfiguration bCfg;

    static ConfigMng &GetInstance();

    bool IsValidStartupOrder(int a_i);
    bool IsValidSubsysBoardPresent(int a_i);
    bool IsValidMacAddr(const std::string& a_is);
    bool IsValidSerialId(const std::string& a_is);
    bool IsValidCustomerId(const std::string& a_is);

    bool HwInitParser(std::string& a_code_str, uint8_t* a_initCode, uint32_t* a_codeSize);

    //ISerializable
    void BaseBoardSetProperties(const icfg::ElementList &a_properties);
    void SubsysBoardSetProperties(const icfg::ElementList &a_properties);
    void HwInitSetProperties(const icfg::ElementList &a_properties);
    bool Deserialize(const icfg::ElementList &a_properties);
    std::string GetClassName();

    bool BoardAndFirmwareTypesMatch();

    ConfigMng(std::string &a_targetConfiguration);
    ConfigMng();
    virtual ~ConfigMng();
    ConfigMng(const ConfigMng&);
    ConfigMng& operator=(const ConfigMng&);

private:

    std::string ReadCode(icfg::ElementPtr &a_node, bool &a_targetMatched,
        const icfg::ElementList &a_hwinitparams);

    void Init();

    std::string m_targetConfiguration;
};

#endif /* CONFIG_MNG_H_ */

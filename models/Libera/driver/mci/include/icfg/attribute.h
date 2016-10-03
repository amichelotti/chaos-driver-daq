/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: attribute.h 19990 2014-01-07 09:35:55Z damijan.skvarc $
 */


#ifndef ICFG_ATTRIBUTE_H_
#define ICFG_ATTRIBUTE_H_

#include <memory>
#include <vector>
#include <string>
#include <boost/any.hpp>
#include <boost/lexical_cast.hpp>

#include "istd/trace.h"
#include "istd/string.h"
#include "istd/exception.h"

#include "icfg/declarations.h"

namespace icfg {

    /**
     * @addtogroup istd_XML Parsing XML structure
     * @{
     */

    class Attribute;
    typedef std::shared_ptr<Attribute> AttributePtr;
    typedef std::vector<AttributePtr> AttributeList;

    /**
    * @class Attribute
    *
    * @brief  pair name-value of XML attribute
    */
    class Attribute {
    public:
       static AttributePtr Create(
           const std::string& a_name,
           const std::string &a_value,
           const DataType_e &a_type = DataType_e::eCtUndefined);


       template <class T>
       bool GetValue(T &a_value) const
       {
           try {
               std::string value = GetValue();
               if (!value.empty()) {
                   a_value = istd::FromString<T>(value);
                   return true;
               }
           }
           catch (istd::Exception &ex) {
               istd_TRC(istd::eTrcMed, "Cannot cast from config value: " << ex.what());
           }
           return false;
       }

       std::string GetName() const;
       std::string GetValue() const;
       DataType_e GetType() const;

       void SetValidValues(const std::vector<std::string> &a_values);
       std::vector<std::string> GetValidValues() const;

    private:
        Attribute(const std::string& a_name,
                        const std::string& a_value,
                        const DataType_e &a_type);

        std::string  m_name;
        std::string  m_value;
        DataType_e m_type;
        std::vector<std::string> m_validValues;
    };

    template<> inline bool Attribute::GetValue<bool>(bool &a_value) const
    {
        std::string value;

        value = GetValue();
        if (!value.empty()) {
            try {
                a_value = istd::ToBool(value);
                return true;
            }
            catch (istd::Exception &ex) {
                istd_TRC(istd::eTrcMed, "Cannot cast from config value: Wrong bool value "
                      << value << " .Error: " << ex.what());
            }
        }
        return false;
    }

    template<> inline bool Attribute::GetValue<std::string>(std::string &a_value) const
    {
        a_value = GetValue();
        return true;
    }

}


#endif /* ICFG_ATTRIBUTE_H_ */

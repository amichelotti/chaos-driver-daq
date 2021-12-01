/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: element.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */


#ifndef ICFG_ELEMENT_H_
#define ICFG_ELEMENT_H_

#include <string>
#include <vector>
#include <memory>

#include <boost/lexical_cast.hpp>

#include "icfg/attribute.h"
#include <istd/string.h>

namespace icfg {

    /**
     * @addtogroup istd_XML Parsing XML structure
     * @{
     */

    class Element;
    typedef std::weak_ptr<Element>   ElementWeakPtr;
    typedef std::shared_ptr<Element> ElementPtr;
    typedef std::vector<ElementPtr>  ElementList;


    /**
    * @class Element
    *
    * @brief  Holds information about nodes from XML configuration file
    */
    class Element : public std::enable_shared_from_this<Element> {
    public:

        /// xsd order indicator
        enum Order_e {eAll, eChoice, eSequence};

        static ElementPtr Create(const std::string& a_name,
                                 const DataType_e &a_type = DataType_e::eCtUndefined);

        template <class T>
        static ElementPtr Create(const std::string& a_name,
                                 const T &a_value,
                                 const DataType_e &a_type = DataType_e::eCtUndefined)
        {
            ElementPtr sp(new Element(a_name, a_value, a_type));
            return sp;
        }

        template <class T>
        bool SetValue(const T &a_value)
        {
            try {
                m_value = ChaosToString(a_value);
                return true;
            }
            catch (boost::bad_lexical_cast &ex) {
                istd_TRC(istd::eTrcMed, "Cannot cast to config value: " << ex.what());
                return false;
            }
        }

        void SetName(const std::string &a_name);

        void AddChild(ElementPtr a_prop);
        void AddAttribute(const AttributePtr &a_attribute);

        std::string  GetName() const;
        std::string  GetValue() const;
        DataType_e GetType() const;


        const ElementList& GetChildren() const;
        const AttributeList& GetAttributes() const;

        /**
         * Search for child a_name and returns its value
         */
        template <class T>
            bool GetValue(const std::string &a_name, T &a_value) const
            __attribute__((warn_unused_result));

        /**
         * Get value of current node
         */
        template <class T>
            bool GetValue(T &a_value) const __attribute__((warn_unused_result));

        /**
         * Search for child a_name and returns its untrimed string value
         */
        bool GetUntrimedValue(const std::string &a_name, std::string &a_value) const;
        bool GetUntrimedValue(std::string &a_value) const;

        /**
         * Get attribute value of current node
         */
        template <class T>
        bool GetAttrValue(const std::string &a_name, T &a_value) const
        {
            AttributePtr attr;
            if (FindAttr(a_name, attr)) {
                return attr->GetValue(a_value);
            }
            return false;
        }

        void SetOrder(const Order_e &a_order);
        Order_e GetOrder() const;

        /**
         * Finds the node with a_name in the tag
        */
        ElementPtr FindChild(const std::string &a_name) const;
        ElementPtr FindSubtree(const std::string &a_name) const;

        void SetParent(const ElementPtr &a_prop);
        ElementPtr GetParent() const;

    private:

        Element();
        Element(const std::string &a_name,
                       const DataType_e &a_type = DataType_e::eCtUndefined);

        template <class T>
        Element(const std::string &a_name,
                       const T &a_value,
                       const DataType_e &a_type = DataType_e::eCtUndefined)
          : m_name(a_name), m_value(ChaosToString(a_value)),
            m_type(a_type)
        {
        }

        inline ElementPtr SharedPtr()
        {
            return this->shared_from_this();
        }

        bool FindAttr(const std::string &a_name, AttributePtr &a_attr) const;

        std::string           m_name;
        std::string           m_value;
        DataType_e        m_type;
        Order_e               m_order;
        ElementList    m_children;
        AttributeList   m_attributes;
        ElementWeakPtr m_parent;
    };

    template <class T>
        bool Element::GetValue(const std::string &a_name, T &a_value) const
        {
            ElementPtr prop;
            prop = FindChild(a_name);
            if (prop) {
                return prop->GetValue(a_value);
            }
            return false;
        }

    template <class T>
        bool Element::GetValue(T &a_value) const
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


}

#endif /* ICFG_ELEMENT_H_ */

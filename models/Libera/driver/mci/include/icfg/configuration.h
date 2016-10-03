/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: configuration.h 19990 2014-01-07 09:35:55Z damijan.skvarc $
 */


#ifndef ISTD_CONFIGURATION_H_
#define ISTD_CONFIGURATION_H_

#include <mutex>
#include <string>

#include <libxml/tree.h>

#include "icfg/serializable.h"


namespace icfg {

    /**
    * @class Configuration
    *
    * @brief  Configuration class encapsulates libxml library and provides
    * Deserialization and Serialization for IConfigSerializable class
    */
    class Configuration
    {
    public:

        enum class CreateXsd_e { eAlways,
        	                     eIfNotExists,
        	                     eNo
                               };

        Configuration(const std::string &a_version = "1.0",
                      const std::string &a_typesXsdPath = "/opt/libera/xsd/2.8/types.xsd");

        virtual ~Configuration();

        /**
         * Initializes libxml2 internal structures. Should be called before any
         * instance of this class is used.
         */
        static void Initialize();

        /**
         * Cleans up libxml2 internal structures. Should be called after all
         * instances of this class are used.
         */
        static void Cleanup();

        /**
         * Opens configuration file and validates it against schema.
         * Throws istd::exception
         */
        void OpenDocument(const std::string &a_file, bool a_validate = true);

        /**
         * Close currently opened XML document
         */
        void CloseDocument();

        /**
         * Reads version of XML document and returns in parameter a_version
         * @return true if successful, otherwise false
         */
        bool GetDocVersion(std::string &a_version);

        /**
         * Reads data from internal XML document and deserialize it into the a_obj
         */
        bool Read(ISerializable &a_obj);

        /**
         * Serialize data from a_obj and writes it into new XML file.
         */
        void Write(ISerializable &a_obj,
                   const std::string &a_fileName,
                   CreateXsd_e a_createXsd = CreateXsd_e::eAlways);

        /**
         * Get value of xml node - provided by absolute path
         */
        bool GetNodeValue(const std::string &a_nodePath, std::string &a_value);

        /**
         * Gett attribute value of xml node - provided by absolute path
         */
        bool GetAttributeValue(const std::string &a_nodePath,
                               const std::string &a_attrName,
                               std::string &a_value);

        /**
         * Default error callback from libxml library (writes into trace file)
         */
        virtual void ErrorCallback(const std::string &a_message);

    private:

        void NewDocument(const std::string &a_fileName);

        std::string GetSchemaFileName(const std::string &a_xmlFile);
        void ValidateDocument(const std::string &a_xmlFile);

        std::string GetPath(ISerializable &a_obj);

        virtual std::string GetApplicationName();

        bool GetValue(bool is_attribute, const std::string &a_nodePath,
                      const std::string &a_attrName, std::string &a_value);

        void GenerateXsd(const std::string &a_fileName,
                         const std::string &a_className,
                         ElementList &a_nodes);

        /**
         * Import external schema
         * @param a_rootNode     Root node of xsd document
         * @param a_fileName     xsd file to include "common.xsd"
         * @param a_namespace    Namespace of included xsd file
         * @param a_ns           Namespace abbreviation for included file
         */
        void ImportSchema(xmlNodePtr a_rootNode,
                          const std::string &a_fileName,
                          const std::string &a_namespace,
                          const std::string &a_ns);

        /**
         * Helper functions for creating new xml nodes in the xsd Doc
         */
        xmlNodePtr CreateXsdNode(xmlNodePtr &a_parentNode,
                                 const std::string &a_name,
                                 xmlNsPtr &a_schema);
        /**
         * Helper function for creating new nodes in the xsd Doc for a_property
         */
        xmlNodePtr CreateXsdNode(xmlNodePtr a_parent,
                                 const ElementPtr &a_property);

        /**
         * Helper functions for creating new attribute in the xsd Doc
         */
        void CreateXsdAttr(xmlNodePtr &a_parentNode,
                           const std::string &a_name,
                           const std::string &a_value);

        /**
         * Get XML node value
         */
        std::string GetXmlNodeValue(xmlDocPtr a_doc,
                                    xmlNodePtr a_curNode);


        /**
         * Reads the node and fills it into the a_elem.
         * It finds also all children nodes and all attributes
         */
        bool GetXmlNode(xmlDocPtr a_doc,
                        xmlNodePtr a_curNode,
                        ElementPtr &a_elem);

        /**
         * Get all attributes from XML node and fill them into a_elem
         */
        void GetXmlNodeAttr(xmlNodePtr a_curNode, ElementPtr &a_elem);

        /**
         * Helper function for creating new xml nodes in the xml Doc. (recursive)
         */
        void CreateXmlNode(xmlNodePtr a_parent, const ElementPtr &a_elem);

        /**
         * Create "element" node in the schema with the corresponding type.
         */
        xmlNodePtr CreateXsdElementNode(xmlNodePtr &a_parentNode,
                                        const std::string &a_name,
                                        DataType_e a_type,
                                        bool a_required = false);

        /**
         * Create node which defines attribute
         */
        void CreateXsdNodeAttr(xmlNodePtr a_parent,
                               const AttributePtr &a_attr);



        std::string m_version;
        std::string m_typesXsdPath;

        std::recursive_mutex m_config_x;

        xmlNsPtr   m_xsdNs;
        xmlDocPtr  m_doc;
    };
}

#endif /* ISTD_CONFIGURATION_H_ */

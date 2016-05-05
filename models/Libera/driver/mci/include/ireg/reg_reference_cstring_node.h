/*
* Copyright (c) 2010 Instrumentation Technologies
* All Rights Reserved.
*
* $Id: reg_reference_cstring_node.h 14606 2011-12-21 12:43:41Z matej.kenda $
*/

#ifndef REG_REFERENCE_CSTRING_NODE_H_
#define REG_REFERENCE_CSTRING_NODE_H_

#include <string>
#include <cstring>

#include "reg_type_node.h"

namespace ireg {

    /**
    * @addtogroup register
    * @{
    */
    /**
    * @brief Template implementation of a RegNode which holds pointer to 
    * external C-type string (char *). 
    *
    * When writing and reading values from this type of nodes, referenced memory
    * location is directly accessed.
    *
    * Example:
    * @dontinclude ut_node.cpp
    *  @skip ReferenceCStringNodeExample()
    *  @until } // ReferenceCStringNodeExample()
    *
    */
    
    class RegReferenceCStringNode: public RegTypeNode<std::string> {
    public:
        
        virtual ~RegReferenceCStringNode()
            {
                
            }

        virtual bool operator==(const TreeNode& a_other) const
        {
            const RegReferenceCStringNode*pOther = 
            dynamic_cast<const RegReferenceCStringNode*>(&a_other);
            return ((pOther != NULL) &&
                    (RegTypeNode<std::string>::operator==(a_other)) &&
                    (m_reference == (pOther->m_reference))
                    );
        }

    protected:
        /**
        * @param a_name      Node name.
        * @param a_maxSize   Reserved length of a string.
        * @param a_val       Pointer to a string.
        * @param a_flags     Various flags can be set to the node
        * @param a_arraySize Size of a string array.
        */
    RegReferenceCStringNode(const std::string& a_name,
                            size_t a_maxSize,
                            char* a_ref,
                            Flags a_flags,
                            size_t a_arraySize = 1)
        : RegTypeNode<std::string>(a_name, a_flags),
            m_reference(a_ref),
            m_maxSize(a_maxSize),
            m_arraySize(a_arraySize)
            {
            }

        mci_CREATE_NODE_FRIEND;

        virtual size_t OnGet(std::string* a_out, size_t a_pos, size_t a_size) const
        {
            for(size_t i(a_pos), o(0); i < a_pos+a_size; ++i, ++o)
                a_out[o] = std::string(&m_reference[i*(m_maxSize)]);

            return a_size;
        }

        virtual void OnSet(const std::string* a_in, size_t a_pos, size_t a_size)
        {
            /* Sanity check: all input strings must be of valid sizes (less
             * than maxSize)
             */
            for(size_t i(0); i < a_size; ++i) {
                size_t inSize = std::strlen(a_in[i].c_str());
                if(inSize >= m_maxSize)
                    throw istd::Exception("String too long.");
            }

            for(size_t o(a_pos), i(0); o < a_pos+a_size; ++o, ++i) {
                size_t inSize = std::strlen(a_in[i].c_str());
                std::strncpy(&m_reference[o*(m_maxSize)], a_in[i].c_str(),
                             inSize);
                m_reference[o*(m_maxSize)+inSize] = '\0';
            }
        }

        virtual size_t OnGetSize(const Path& a_relPath) const
        {
            return m_arraySize;
        }

    private:
        char   *m_reference;
        size_t  m_maxSize;
        size_t  m_arraySize;
    };

    /**@}*/
}

#endif /* REG_REFERENCE_CSTRING_NODE_H_ */

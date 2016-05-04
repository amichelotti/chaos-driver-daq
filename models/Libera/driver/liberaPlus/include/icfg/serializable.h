/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: serializable.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */


#ifndef ISTD_SERIALIZABLE_H_
#define ISTD_SERIALIZABLE_H_

#include "icfg/element.h"

namespace icfg {

    /**
    * @class ISerializable
    *
    * @brief  Interface class for Configuration settings.
    */
    class ISerializable
    {
    public:
        virtual      ~ISerializable(){};

        /**
         * Here set local properties from config values (read)
         */
        virtual bool Deserialize(const ElementList &a_properties) = 0;

        /**
         * Here get values from local properties and set to config (write)
         */
        virtual bool Serialize(ElementList &a_properties);

        /**
         * Returns name of node which is used as root node.
         */
        virtual std::string GetClassName();
    };
    /**@}*/
}

#endif /* ICFG_SERIALIZABLE_H_ */

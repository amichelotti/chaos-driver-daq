/*
 * Copyright (c) 2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * Convertions from any data type to any data type
 *
 * $Id: convert.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef CONVERT_H_
#define CONVERT_H_

#include <string>
#include "exception.h"

// TODO: Cast instead of convert?

namespace istd {

     /**
      * General conversion from any type to any type is done by using static_cast template
      */
     template<class ST, class DT> struct Transform
         {
             inline DT Convert(ST &a_val)
             {
                 return static_cast<DT>(a_val);
             }
         };

     /**
      * specialization conversion from any type into std::string
      */
     template<class ST> struct Transform<ST, std::string>
         {
             inline std::string Convert(ST &a_val)
             {
                 istd_EXCEPTION("Unsupported conversion");
             }
         };

     /**
      * specialization conversion from any type into bool
      */
     template<class ST> struct Transform<ST,  bool>
         {
             inline bool Convert(ST& a_val)
             {
                 istd_EXCEPTION("Unsupported conversion");
             }
         };

     /**
      * specialization conversion from const char* into any type
      */
     template<class DT> struct Transform<const char*, DT>
         {
             inline DT Convert(const char*& a_val)
             {
                 istd_EXCEPTION("Unsupported conversion");
             }
         };

     /**
      * specialization conversion from const char* into std::string
      */
     template<> struct Transform<const char*, std::string>
         {
             inline std::string Convert(const char*& a_val)
             {
                 return std::string(a_val);
             }
         };

     /**
      * specialization conversion from const char* into bool
      */
     template<> struct Transform<const char*, bool>
         {
             inline bool Convert(const char*& a_val)
             {
                 // TODO: support this conversion
                 istd_EXCEPTION("Unsupported conversion");
             }
         };

}

#endif /* CONVERT_H_ */

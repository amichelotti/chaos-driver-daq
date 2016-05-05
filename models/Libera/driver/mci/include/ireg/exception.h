/*
 * Copyright (c) 2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: exception.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef IREG_EXCEPTION_H_
#define IREG_EXCEPTION_H_

#include <string>
#include <initializer_list>
#include <memory>
#include "istd/exception.h"
#include "istd/trace.h"
#include "istd/string.h"

namespace ireg {

    class Expression;
    typedef std::shared_ptr<Expression>             ExpressionPtr;

    class SyntaxError : public istd::Exception {
    public:
        SyntaxError(const std::string& a_op, const std::string& a_desc);
    };

    class ValidationError : public istd::Exception {
    public:
        ValidationError(const ExpressionPtr& a_ex, const std::string& a_value);
    };


} // namespace

#endif /* IREG_EXCEPTION_H_ */

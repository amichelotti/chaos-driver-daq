/*
 * Copyright (c) 2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: terminal.h 11980 2011-06-15 10:36:13Z matej.kenda $
 */

#ifndef IREG_TERMINAL_H_
#define IREG_TERMINAL_H_

#include "ireg/expression.h"

namespace ireg {

    class Terminal : public Expression {
    public:
        virtual std::string ToText() const;

    protected:
        Terminal(const std::string& a_opName);
        virtual ~Terminal();

    };

}

#endif /* IREG_TERMINAL_H_ */

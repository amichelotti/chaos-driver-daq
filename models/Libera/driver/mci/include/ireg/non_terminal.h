/*
 * Copyright (c) 2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: non_terminal.h 11980 2011-06-15 10:36:13Z matej.kenda $
 */

#ifndef IREG_NON_TERMINAL_H_
#define IREG_NON_TERMINAL_H_

#include "ireg/expression.h"

namespace ireg {


    class NonTerminal : public Expression {
    public:
        NonTerminal(
            const std::string& a_opName,
            const Arity& a_arity = Arity::Nary);

        virtual ~NonTerminal();

        virtual void Add(const ExpressionPtr& a_operand);
        virtual void Add(ExpressionPtr&& a_operand);
        virtual void Set(const OperandList& a_operands);
        virtual void Set(OperandList&& a_operands);

        virtual const OperandVector Get() const;

        virtual std::string ToText() const;

    protected:
        OperandVector    m_ops;

    private:
        NonTerminal(NonTerminal const &) = delete;
        NonTerminal & operator=(NonTerminal const &) = delete;

    };

}

#endif /* IREG_NON_TERMINAL_H_ */

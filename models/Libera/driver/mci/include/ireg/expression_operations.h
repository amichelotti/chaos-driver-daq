/*
 * Copyright (c) 2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: expression_operations.h 11980 2011-06-15 10:36:13Z matej.kenda $
 */

#ifndef IREG_EXPRESSION_OPERATIONS_H_
#define IREG_EXPRESSION_OPERATIONS_H_

#include "ireg/non_terminal.h"

namespace ireg {

    namespace expr {

        template <typename T>
            class Not : public NonTerminal {
            public:
                Not() : NonTerminal("not", Arity::Unary) {}

            private:
                Not(Not const &) = delete;
                Not & operator=(Not const &) = delete;

                virtual bool OnEvaluate(const T &a_val) const
                {
                    auto expr = m_ops[0];
                    return !(expr->Evaluate(a_val));
                }
            };

        template <typename T>
            class And : public NonTerminal {
            public:
                And() : NonTerminal("and") { }

            private:
                And(And const &) = delete;
                And & operator=(And const &) = delete;

                virtual bool OnEvaluate(const T &a_val) const
                {
                    for (auto i = m_ops.begin(); i != m_ops.end(); ++i) {
                        if ( !(*i)->Evaluate(a_val) ) {
                            return false;
                        }
                    }
                    return true;
                }
            };

        template <typename T>
            class Or : public ireg::NonTerminal {
            public:

                Or() : NonTerminal("or") { }

            private:
                Or(Or const &) = delete;
                Or & operator=(Or const &) = delete;

                virtual bool OnEvaluate(const T &a_val) const
                {
                    for (auto i = m_ops.begin(); i != m_ops.end(); ++i) {
                        if ((*i)->Evaluate(a_val)) {
                            return true;
                        }
                    }
                    return false;
                }
            };

    } // namespace expr

} // namespace ireg

#if 0
template< class A, class B >
DExpression<DBinaryExpression<DExpression<A>, DExpression<B>, Add> >
operator + (DExpression<A> a, DExpression<B> b)
{
  typedef DBinaryExpression <DExpression<A>, DExpression<B>, Add> ExprT;
  return DExpression<ExprT>(ExprT(a,b));
}

template < class A, class B >
    ireg::ExpressionPtr operator && ( ireg::ExpressionPtr a, ireg::ExpressionPtr b)
    {
        return ireg::Expr<ireg::expr::And<T>>()->Operands(a, b);
    }
#endif

#endif /* IREG_EXPRESSION_OPERATIONS_H_ */

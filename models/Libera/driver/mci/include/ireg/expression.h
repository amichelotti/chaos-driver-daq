/*
 * Copyright (c) 2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: expression.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef IREG_EXPRESSION_H_
#define IREG_EXPRESSION_H_

#include <string>
#include <vector>
#include <initializer_list>
#include <memory>
#include "ireg/exception.h"
#include "istd/trace.h"
#include "istd/string.h"

namespace ireg {

    class Expression;
    typedef std::shared_ptr<Expression>             ExpressionPtr;
    typedef std::initializer_list<ExpressionPtr>    OperandList;
    typedef std::vector<ExpressionPtr>              OperandVector;

    /**
     * Arity of the operation.
     * For example: not is Unary.
     */
    enum class Arity {Nullary, Unary, Binary, Nary};

    /**
     * Factory function to create any expression component from the
     * hierarchy.
     * @param EXPR_TYPE type of the expression components
     * @param ARGS      constructor arguments forwarded to EXPR_TYPE
     * @return Shared pointer to heap-allocated expression
     */
    template<typename EXPR_TYPE, typename ...ARGS>
        ExpressionPtr Expr(ARGS&& ...args)
        {
            ExpressionPtr p( new EXPR_TYPE(std::forward<ARGS>(args)...) );
            return p;
        }

    class Expression : public std::enable_shared_from_this<Expression> {
    public:

        bool Evaluate(const std::string &a_val) const;
        bool Evaluate(const int64_t &a_val) const;
        bool Evaluate(const uint64_t &a_val) const;
        bool Evaluate(const int32_t &a_val) const;
        bool Evaluate(const uint32_t &a_val) const;
        bool Evaluate(const double &a_val) const;
        bool Evaluate(const float &a_val) const;
        bool Evaluate(const bool &a_val) const;

        /**
         * Add arbitrary number of operands. Replaces existing operands.
         * @see Set
         */
        template <typename ...OP>
            Expression& Operands(OP&& ...op)
            {
                Set( OperandList( { op... } ) );
                return *this;
            }

        virtual void Add(const ExpressionPtr& a_expr);
        virtual void Add(ExpressionPtr&& a_expr);
        virtual void Set(const OperandList& a_operands);
        virtual void Set(OperandList&& a_operands);
        virtual const OperandVector Get() const;
        const std::string& OpName() const;
        Arity GetArity() const;

        virtual std::string ToText() const = 0;

        /**
         * Typecast operator.
         */
        operator ExpressionPtr();

        void VerifyArity(size_t a_numOp) const;

        virtual bool OnEvaluate(const std::string &a_val) const;
        virtual bool OnEvaluate(const int64_t &a_val) const;
        virtual bool OnEvaluate(const uint64_t &a_val) const;
        virtual bool OnEvaluate(const int32_t &a_val) const;
        virtual bool OnEvaluate(const uint32_t &a_val) const;
        virtual bool OnEvaluate(const double &a_val) const;
        virtual bool OnEvaluate(const float &a_val) const;
        virtual bool OnEvaluate(const bool &a_val) const;

    protected:
        Expression(const std::string& a_opName, const Arity& a_arity);
        virtual ~Expression();

    private:
        const std::string   m_opName;
        const Arity         m_arity;

        friend class std::shared_ptr<Expression>;

    };

    std::ostream& operator<<(std::ostream&, const Arity&);

} // namespace

#endif /* IREG_EXPRESSION_H_ */

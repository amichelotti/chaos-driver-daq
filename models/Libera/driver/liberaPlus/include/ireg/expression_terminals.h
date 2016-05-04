/*
 * Copyright (c) 2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: expression_terminals.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef IREG_EXPRESSION_TERMINALS_H_
#define IREG_EXPRESSION_TERMINALS_H_

#include <set>

#include "ireg/node.h"
#include "ireg/terminal.h"

namespace ireg {

    namespace expr {

        template <typename T>
            std::string ParamToString(const T& a_param)
            {
                std::ostringstream os;
                os << a_param;
                return os.str();
            }

        template <>
            std::string ParamToString(const Node& a_param);

        /**
         * Equals to any value in the sequence.
         */
        template <typename VT, class TT = VT>
            class Set : public Terminal {
            public:

                template<typename ...ARGS>
                    Set(ARGS&& ...args) : Terminal("{...}"), m_set({args...}) { }

                virtual ~Set() {}

                virtual std::string ToText() const
                {
                    std::ostringstream ostr;

                    ostr << "{";
                    for (auto it = m_set.begin(); it != m_set.end(); ++it) {

                        if (it != m_set.begin()) {
                            ostr << ",";
                        }
                        ostr << *it;
                    }
                    ostr << "}";

                    return ostr.str();
                }

            private:
                const std::set<TT> m_set;

                // This does not work, when nodes are inside set.
                // To support nodes, either implement operator < in the node or
                // go through the set one by one element
                virtual bool OnEvaluate(const VT& a_val) const
                {
                    return m_set.find(a_val) != m_set.end();
                }
            };

        /**
         * Greater than a specified limit.
         */
        template <typename VT, class TT = VT>
            class Gt : public Terminal {
            public:
                Gt(TT a_min) : Terminal(">"), m_min(a_min) { }
                virtual ~Gt() {}

                virtual std::string ToText() const
                {
                    std::ostringstream os;
                    os << ">" << ParamToString(m_min);
                    return os.str();
                }

            private:
                const TT m_min;

                virtual bool OnEvaluate(const VT& a_val) const
                {
                    VT min = m_min;
                    return a_val > min;
                }
            };

            /**
             * Less than a specified limit.
             */
            template <typename VT, class TT = VT>
                class Lt : public Terminal {
                public:
                    Lt(TT a_max) : Terminal("<"), m_max(a_max) { }
                    virtual ~Lt() {}

                    virtual std::string ToText() const
                    {
                        std::ostringstream os;
                        os << "<" << ParamToString(m_max);
                        return os.str();
                    }

                private:
                    const TT m_max;

                    virtual bool OnEvaluate(const VT& a_val) const
                    {
                        VT max = m_max;
                        return a_val < max;
                    }
                };

                /**
                 * Range of values (including limits).
                 * @param VT validation parameter type (one of supported primitive types)
                 * @param TT terminal type, containing the limit value (can be a primitive type or a node)
                 */
                template <typename VT, class TT = VT>
                    class Range : public Terminal {
                    public:
                        Range(const TT& a_min, const TT& a_max)
                        : Terminal("[...]"), m_min(a_min), m_max(a_max) { }
                        virtual ~Range() {}

                        virtual std::string ToText() const
                        {
                            std::ostringstream ostr;
                            ostr
                                << "["
                                << ParamToString(m_min) << ", "
                                << ParamToString(m_max)
                                << "]";
                            return ostr.str();
                        }

                    private:
                        const TT m_min;
                        const TT m_max;

                        virtual bool OnEvaluate(const VT& a_val) const
                        {
                            VT min = m_min;
                            VT max = m_max;
                            return a_val >= min && a_val <= max;
                        }
                    };

    }

}

#endif /* IREG_EXPRESSION_TERMINALS_H_ */

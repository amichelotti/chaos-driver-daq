/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: prog_opt_multi_val.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef PROG_OPT_MULTI_VAL_H_
#define PROG_OPT_MULTI_VAL_H_

#include <string>
#include <vector>
#include <boost/program_options/options_description.hpp>
#include <boost/any.hpp>
#include <boost/lexical_cast.hpp>

namespace istd {

    template<class T>
    class ProgOptMultiVal: public boost::program_options::value_semantic {
    public:
        explicit ProgOptMultiVal(
                std::vector<T> *a_storeTo,
                const size_t a_min,
                const size_t a_max)
        : m_storeTo(a_storeTo), m_min(a_min), m_max(a_max)
        {
        }

        std::string name() const
        {
            return "arg";
        }

        unsigned min_tokens() const
        {
            return m_min;
        }
        unsigned max_tokens() const
        {
            return m_max;
        }
        bool is_composing() const
        {
            // not implemented
            return false;
        }
        bool is_required() const
        {
            return false;
        }
        void parse(boost::any &a_valueStore,
                const std::vector<std::string> &a_newTokens, bool a_utf8) const
        {
            if (!a_valueStore.empty()) {
#if BOOST_VERSION >= 104200
                throw boost::program_options::multiple_occurrences();
#else
                throw boost::program_options::multiple_occurrences(
                    "Non-composing value with multiple occurrences.");
#endif
            }

            for (auto iter = a_newTokens.begin(); iter != a_newTokens.end(); ++iter) {

                try {
                    m_storeTo->push_back(boost::lexical_cast<T>(*iter));
                    a_valueStore = a_newTokens;
                }
                catch (std::exception &ex) {
                    throw boost::program_options::invalid_option_value(
                            std::string("Invalid option value: ") + ex.what());
                }
            }
        }

        bool apply_default(boost::any &) const
        {
            // Default is no
            return false;
        }
        void notify(const boost::any &) const
        {
            // not implemented
        }

    private:
        std::vector<T> *m_storeTo;
        size_t m_min;
        size_t m_max;
    };

}

#endif /* PROG_OPT_MULTI_VAL_H_ */

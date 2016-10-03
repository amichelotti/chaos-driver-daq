/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: reg_fnc_node.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef REG_FNC_NODE_H_
#define REG_FNC_NODE_H_

#include <functional>
#include <limits>

#include "reg_type_node.h"

namespace ireg {

    using namespace std::placeholders;

#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 7)
    typedef std::nullptr_t NullPtr_t;
    const NullPtr_t nilFunc(nullptr);
#elif __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
    // It should be the same as for 4.7,
    // but that causes compiler internal error on gcc 4.6
    typedef intptr_t NullPtr_t;
    const NullPtr_t nilFunc(static_cast<NullPtr_t>(0));
#else
    typedef intptr_t NullPtr_t;
    const NullPtr_t nilFunc(NULL);
#endif


    /**
     * @addtogroup register
     * @{
     */
    /**
     * @brief Template implementation of a RegNode which holds pointer to
     * callback function.
     *
     * When writing and reading values from this type of nodes, callback
     * function is executed where developer can implement specific algorithms
     * for set and get functionality.
     *
     *  @param T  Node value type
     *
     * Example:
     * @dontinclude ut_node.cpp
     *
     *  First define class and implement callback methods for set and get
     *  @skip class CallbackExample
     *  @until };
     *  Than create a node and connect callback methods.
     *  @skip void FunctionNodeExample()
     *  @until }
     */

    template <class T>
        class RegFncNode: public RegTypeNode<T> {
        public:
            typedef std::function<bool(const T&)>  SetFnc;
            typedef std::function<bool(T&)>        GetFnc;

            typedef std::function<bool(const T*, size_t, size_t)> SetArrayFnc;
            typedef std::function<size_t(T*, size_t, size_t)>     GetArrayFnc;

            virtual ~RegFncNode()
            {

            }

        protected:

            /**
             * @param a_name     Node name
             * @param a_set      Callback function which is called on Set().
             * @param a_get      Callback function which is called on Get().
             */
            explicit RegFncNode(
                const std::string& a_name,
                SetFnc a_set,
                GetFnc a_get,
                Flags a_flags)
            : RegTypeNode<T>( a_name, a_flags),
              m_set(a_set),
              m_get(a_get),
              m_size(1)
            {
                // Empty
            }

            /**
             * @param a_name     Node name
             * @param a_size     Node array size
             * @param a_set      Callback function which is called on Set().
             * @param a_get      Callback function which is called on Get().
             */
            explicit RegFncNode(const std::string& a_name,
                       const size_t a_size,
                       SetArrayFnc  a_set,
                       GetArrayFnc  a_get,
                       Flags        a_flags)
            : RegTypeNode<T>(a_name, a_flags | eNfArray),
              m_setArray(a_set),
              m_getArray(a_get),
              m_size(a_size)
            {
                //Empty
            }

            /**
             * Single value, object instance get and set methods.
             */
            template<class SET, class GET, class O>
                explicit RegFncNode(
                    const std::string& a_name,
                    O        a_instance,
                    SET      a_set,
                    GET      a_get,
                    Flags    a_flags = eNfDefault)
                    : RegTypeNode<T>(a_name, a_flags),
                      m_set(std::bind(a_set, a_instance, _1)),
                      m_get(std::bind(a_get, a_instance, _1)),
                      m_size(1)
                {
                    // Empty
                }

            /**
             * Array of values, object instance get and set methods.
             */
            template<class SET, class GET, class O>
                explicit RegFncNode(const std::string& a_name,
                    const size_t a_size,
                    O        a_instance,
                    SET      a_set,
                    GET      a_get,
                    Flags a_flags  = eNfDefault)
                    : RegTypeNode<T>(a_name, a_flags | eNfArray),
                      m_setArray(std::bind(a_set, a_instance, _1, _2, _3)),
                      m_getArray(std::bind(a_get, a_instance, _1, _2, _3)),
                      m_size(a_size)
                {
                    //Empty
                }

            /**
             * Single value, object instance set method only.
             */
            template<class SET, class O>
                explicit RegFncNode(
                    const std::string& a_name,
                    O           a_instance,
                    SET         a_set,
                    NullPtr_t   a_get,
                    Flags a_flags = eNfDefault)
                    : RegTypeNode<T>(a_name, a_flags & ~eNfReadable),
                      m_set(std::bind(a_set, a_instance, _1)),
                      m_size(1)
                {
                    // Empty
                }

            /**
             * Array of values, object instance set method only.
             */
            template<class SET, class O>
                explicit RegFncNode(
                    const std::string& a_name,
                    const size_t a_size,
                    O        a_instance,
                    SET      a_set,
                    NullPtr_t a_get,
                    Flags a_flags = eNfDefault)
                    : RegTypeNode<T>(a_name, (a_flags & ~eNfReadable) | eNfArray ),
                      m_setArray(std::bind(a_set, a_instance, _1, _2, _3)),
                      m_size(a_size)
                {
                    // Empty
                }


            /**
             * Single value, object instance get method only.
             */
            template<class GET, class O>
                explicit RegFncNode(
                    const std::string& a_name,
                    O        a_instance,
                    NullPtr_t a_set,
                    GET      a_get,
                    Flags a_flags = eNfDefault)
                    : RegTypeNode<T>(a_name, a_flags & ~eNfWritable),
                      m_get(std::bind(a_get, a_instance, _1)),
                      m_size(1)
                {
                    // Empty
                }

            /**
             * Array of values, object instance get method only.
             */
            template<class GET, class O>
                explicit RegFncNode(
                    const std::string& a_name,
                    const size_t a_size,
                    O            a_instance,
                    NullPtr_t    a_set,
                    GET          a_get,
                    Flags        a_flags = eNfDefault)
                    : RegTypeNode<T>(a_name, (a_flags & ~eNfWritable) | eNfArray),
                      m_getArray(std::bind(a_get, a_instance, _1, _2, _3)),
                      m_size(a_size)
                {
                    // Empty
                }

            mci_CREATE_NODE_FRIEND;

            virtual size_t OnGet(T* a_out, size_t a_pos, size_t a_size) const
            {
                size_t count = 0;
                if (m_getArray) {
                    count = m_getArray(a_out, a_pos, a_size);
                }
                else if (m_get && a_pos == 0 && a_size == 1) {
                    count = m_get(a_out[0]);
                }
                return count;
            }


            virtual void OnSet(const T* a_in, size_t a_pos, size_t a_size)
            {
                bool ok = false;
                if (m_setArray) {
                    ok = m_setArray(a_in, a_pos, a_size);
                }
                else if (m_set && a_pos == 0 && a_size == 1) {
                    ok = m_set(a_in[0]);
                }

                if (!ok) {
                    istd_EXCEPTION(
                        "ireg: Can't set value: setter function failed: "
                        << this->GetName()
                    );
                }
            }


            virtual bool OnResize(const size_t a_size, const Path& a_relPath = c_noRelPath)
            {
                m_size = a_size;
                return true;
            }

            virtual size_t OnGetSize(const Path& a_relPath) const
            {
                return m_size;
            }

        private:

            SetFnc    m_set;
            GetFnc    m_get;

            SetArrayFnc m_setArray;
            GetArrayFnc m_getArray;

            size_t    m_size;
        };

    typedef RegFncNode<double>         RegFncDoubleNode;
    typedef RegFncNode<float>          RegFncFloatNode;
    typedef RegFncNode<std::string>    RegFncStringNode;
    typedef RegFncNode<bool>           RegFncBoolNode;
    typedef RegFncNode<int32_t>        RegFncInt32Node;
    typedef RegFncNode<int64_t>        RegFncInt64Node;
    typedef RegFncNode<uint32_t>       RegFncUInt32Node;
    typedef RegFncNode<uint64_t>       RegFncUInt64Node;

    /**@}*/
}

#endif /* REG_FNC_NODE_H_ */

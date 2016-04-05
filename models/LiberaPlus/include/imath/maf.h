/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: maf.h 8621 2010-09-14 05:54:19Z damijan.skvarc $
 *
 * Moving Average Filter
 */

#ifndef __MAF__H
#define __MAF__H

#include <queue>

namespace imath {

    /** Moving Average Filter
     *
     *  Filter keeps track of at most m_order number of values, what is specified in the constructor
     *  A new value is moved into a filter with Push() method. The oldest value is moved out of queue
     *  in case the size overcross the order of filter. The result of Push method returns the actual
     *  average value of currently maintained values
     *
     */
    template <class T>
    class Maf
    {
    public:
        Maf(size_t a_order=3) :
            m_order(a_order),
            m_sum(0)
        {
        }

        T Push(const T& a_val)
        {
            while (m_queue.size()>=m_order) {
                m_sum-=m_queue.front();
                m_queue.pop();
            }
            m_sum+=a_val;
            m_queue.push(a_val);
            return m_sum/m_queue.size();
        }

        T Sum()  { return m_sum;          }
        T Size() { return m_queue.size(); }
        T Back() { return m_queue.back(); }

    private:
        std::queue<T> m_queue;
        size_t        m_order;
        T             m_sum;
    };
    /**@}*/
} // namespace

#endif // __MAF__H

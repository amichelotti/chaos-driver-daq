
/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: stl_helpers.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef ISTD_STL_HELPERS_H
#define ISTD_STL_HELPERS_H


namespace istd {

    /**
     * @addtogroup istd_util
     * @{
     */

    /**
     * Functor to delete a member of the STL container. Example:
     *
     * std::for_each(vec.begin(), vec.end(), istd::DeleteElement());
     */
    struct DeleteElement
    {
        // This will be called by for_each() function.
        template<typename T>
            bool operator()(T &elem) const
            {
                delete elem;
                return true;
            }
    };

    /**
     * Functor to delete a member of the map. Example:
     *
     * std::for_each(m.begin(), m.end(), istd::DeleteMapElement());
     */
    struct DeleteMapElement
    {
        // This will be called by for_each() function.
        template<typename PAIR>
            bool operator()(PAIR &elem) const
            {
                delete elem.second;
                return true;
            }
    };

    /**@}*/

} // namespace

#endif // ISTD_STL_HELPERS_H

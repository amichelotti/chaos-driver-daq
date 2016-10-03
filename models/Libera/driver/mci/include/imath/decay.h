/*
* Copyright (c) 2008-2009 Instrumentation Technologies
* All Rights Reserved.
*
* $Id: decay.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
*/

#ifndef DECAY_H
#define DECAY_H


namespace imath {

	/**
	 * @addtogroup applicationSpecific Application specific Classes
	 * @{
	 */

    typedef struct {
        unsigned int offset;
        unsigned int size;
        unsigned int index;
        unsigned int rows;
        double       adcFreq;
    } DecayParams;

    int ComputeDecay(i_MCI::DecayData &a_dData,
                     const i_MCI::RawSignal &a_data,
                     const DecayParams &a_params);

    /**@}*/
} // namespace

#endif // DECAY_H

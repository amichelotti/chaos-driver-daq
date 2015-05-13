// $Id: bbfp.h 2192 2008-10-07 09:13:06Z matejk $

//! \file bbfp.h
//! Bunch-by-Bunch Feedback Processor (BBFP) specific definitions.

#if !defined(_BBFP_H)
#define _BBFP_H

/** Libera BBFP Data on Demand (DD) raw sample. */
typedef libera_sample_t CSPI_DD_RAWATOM;

/** Libera BBFP Data on Demand (DD) sample. */
typedef CSPI_DD_RAWATOM CSPI_DD_ATOM;

/** Placeholder for CSPI types not used by the BBFP. */
typedef struct {}  CSPI_NOTUSED;

/** Not used. Declared for compatibility with CSPI only. */
typedef CSPI_NOTUSED CSPI_SA_ATOM;

/** Not used. Declared for compatibility with CSPI only. */
typedef CSPI_NOTUSED CSPI_ADC_ATOM;

//--------------------------------------------------------------------------

/** Environment parameters or attributes. */
struct tagCSPI_ENVPARAMS
{
	CSPI_ENVPARAMS_BASE;

	// No BBFP specific parameters.
};

/*
 * typedef enum
 * {
 * 	CSPI_ENV_			= CUSTOM_ENV_BIT(0),
 * }
 * CSPI_ENVFLAGS_BBFP;
 */

//--------------------------------------------------------------------------

/** Derived from CSPI_CONPARAMS to handle DD specific
 *  parameters or attributes for BBFP.
 */
typedef struct {
	/** Common connection parameters. */
	CSPI_CONPARAMS_BASE;

	/** The number of bunches per turn. */
	size_t step;
}
CSPI_CONPARAMS_DD;

//--------------------------------------------------------------------------

/** Bit flags corresponding to the CSPI_CONPARAMS_DD structure.
 *  See CSPI_CONPARAMS_DD structure for descriptions.
 */
typedef enum {
	CSPI_CON_STEP = CUSTOM_CON_BIT(0),
}
CSPI_CONFLAGS_DD;

#endif	// _BBFP_H

// $Id: hbpp.h 2192 2008-10-07 09:13:06Z matejk $

//! \file hbpp.h
//! Hadron Beam Position Processor (HBPP) specific definitions.

#if !defined(_HBPP_H)
#define _HBPP_H

/** Libera HBPP Data on Demand (DD) raw sample. */
typedef libera_sample_t CSPI_DD_RAWATOM;

/** Libera HBPP Data on Demand (DD) sample. */
typedef CSPI_DD_RAWATOM CSPI_DD_ATOM;

/** Placeholder for CSPI types not used by the HBPP. */
typedef struct {} CSPI_NOTUSED;

/** Not used. Declared for compatibility with CSPI only. */
typedef CSPI_NOTUSED CSPI_SA_ATOM;

/** Not used. Declared for compatibility with CSPI only. */
typedef CSPI_NOTUSED CSPI_ADC_ATOM;

//--------------------------------------------------------------------------

/** Environment parameters or attributes. */
struct tagCSPI_ENVPARAMS
{
	CSPI_ENVPARAMS_BASE;

	// No HBPP specific parameters.
};

/*
 * typedef enum
 * {
 * 	CSPI_ENV_			= CUSTOM_ENV_BIT(0),
 * }
 * CSPI_ENVFLAGS_HBPP;
 */

//--------------------------------------------------------------------------

/** Derived from CSPI_CONPARAMS to handle DD specific
 *  parameters or attributes for HBPP.
 */
typedef struct {
	/** Common connection parameters. */
	CSPI_CONPARAMS_BASE;

	// TODO: add HBPP-specific parameters.
}
CSPI_CONPARAMS_DD;

//--------------------------------------------------------------------------

/** Bit flags corresponding to the CSPI_CONPARAMS_DD structure.
 *  See CSPI_CONPARAMS_DD structure for descriptions.
 */
//typedef enum {
	// TODO: add bit flags for HBPP-specific parameters.
	//CSPI_CON_xxx = CUSTOM_CON_BIT(0),
//}
//CSPI_CONFLAGS_DD;

#endif	// _HBPP_H

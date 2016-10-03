// $Id: ebpp.c 2429 2009-02-23 15:43:35Z tomazb $

//! \file ebpp.c
//! Electron Beam Position Processor (EBPP) specific module.

/*
CSPI - Control System Programming Interface
Copyright (C) 2004-2006 Instrumentation Technologies

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA
or visit http://www.gnu.org
*/

/*
Implementation note: Public functions have their input parameters validated
in both, debug and nondebug builds. Private functions, on the other hand,
validate input parameters in debug build only.

TAB = 4 spaces.
*/
#define _GNU_SOURCE
#include <stdio.h>	// sprintf, getline
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>

#include <linux/limits.h>

#include <math.h>

#include "cordic.h"

#include "cspi.h"
#include "cspi_impl.h"

#include "dscd.h"
#include "ebpp.h"

#if DEBUG < 3
#define CSPI_LOG( format, ... ) ((void)0)
#else
#define CSPI_LOG( format, ... ) \
        if (debug_init()) _LOG_DEBUG( format, ##__VA_ARGS__ )
#endif

/** Max. DD decimation available on the FPGA. */
#define MAX_DEC 64

/** Private. EBPP specific. Local to this module only.
 *
 *  Represents accelerator specific decimation value
 *  used to validate MAF parameters.
 */
size_t g_decimation = 1;

/** Private. EBPP specific. Local to this module only.
 *
 *  Represents mirrored (cached) environment parameters
 *  used to speed up position calculations.
 */
typedef struct tagCache {
	int Kx; 				//!< Horizontal calibration coefficient.
	int Ky; 				//!< Vertical calibration coefficient.
	int Xoffset;			//!< Electrical/magnetic horizontal offset.
	int Yoffset;			//!< Electrical/magnetic vertical offset.
	int Qoffset;			//!< Electrical offset.
	struct {				//!< single pass data
		int threshold;		//!< threshold
		int n_before;		//!< n samples before threshold
		int n_after;		//!< n samples after threshold
	} sp;
	struct {				//!< spike removal data
		int dsc;			//!< DSC mode must be 2
		int enable;			//!< spike removal enabled in FPGA
		int cspi_enable;	//!< spike removal enabled in CSPI
		int averaging_stop;	//!< end position for calculating average value
		int average_window;	//!< number of samples needed for calculating average value
		int start;			//!< start position of averaging
		int window;			//!< lenght of averaging
	} sr;
	struct {						//!< cw
		unsigned long frequency;	//!< frequency
		unsigned long harmonic;		//!< harmonic
		double frev;				//!< revolutions
	} cw;
} Cache;

/** Private. EBPP specific. Local to this module only.
 *
 *  One and only Cache instance.
 */
Cache cache;

volatile size_t _is_cache_dirty = 0;

/** Private. EBPP specific. Local to this module only.
 *
 *  A mutex to protect the cache from concurrent modifications.
 */
pthread_mutex_t cache_mutex = PTHREAD_MUTEX_INITIALIZER;

/** Private. EBPP specific. Local to this module only.
 *
 *  Cache bitmask.
 */
#define CACHE_MASK (CSPI_ENV_KX | CSPI_ENV_KY | \
					CSPI_ENV_XOFFSET | CSPI_ENV_YOFFSET | CSPI_ENV_QOFFSET |\
					CSPI_ENV_SR | CSPI_ENV_SP | CSPI_ENV_LPLLDSTAT |\
					CSPI_ENV_DSC |\
					CSPI_ENV_FREV \
					)

/** Private. Forward declaration of localy used function to get feature register. */
int cspi_getfeatureparam(Environment *e, CSPI_ENVPARAMS *p, CSPI_BITMASK flags);

//--------------------------------------------------------------------------

/** Private. EBPP specific. Local to this module only.
 *
 *  Update cached environment data.
 *  On success, returns CSPI_OK. Otherwise, returns one of the errors
 *  returned by cspi_getenvparam.
 *  @param e Environment handle.
 */
int ebpp_update_cache( Environment *e )
{
	ASSERT(e);
	CSPI_ENVPARAMS ep;

	// Assume environment has been locked by caller.
	ASSERT( EBUSY == pthread_mutex_trylock( &e->mutex ) );

	int rc = custom_getenvparam( e, &ep, CACHE_MASK );
	if ( CSPI_OK ==  rc ) {

		VERIFY( 0 == pthread_mutex_lock( &cache_mutex ) );

		// Assume ep.Kx and ep.Ky are in nanometers (um).
		// No conversion needed!
		cache.Kx = ep.Kx;
		cache.Ky = ep.Ky;

		cache.Xoffset = ep.Xoffset;
		cache.Yoffset = ep.Yoffset;
		cache.Qoffset = ep.Qoffset;

		libera_cfg_request_t request;

		request.idx = LIBERA_CFG_BCD_XOFFSET;
		if ( -1 == ioctl( e->fd, LIBERA_IOC_GET_CFG, &request ) ) {
			return CSPI_E_SYSTEM;
		}
		cache.Xoffset += request.val;

		request.idx = LIBERA_CFG_BCD_YOFFSET;
		if ( -1 == ioctl( e->fd, LIBERA_IOC_GET_CFG, &request ) ) {
			return CSPI_E_SYSTEM;
		}
		cache.Yoffset += request.val;

		cache.sp.threshold = ep.sp.threshold;
		cache.sp.n_before = ep.sp.n_before;
		cache.sp.n_after = ep.sp.n_after;

		cache.sr.dsc = ep.dsc;
		cache.sr.enable = ep.sr.enable;
		cache.sr.cspi_enable = ep.sr.cspi_enable;
		cache.sr.averaging_stop = ep.sr.averaging_stop;
		cache.sr.average_window = ep.sr.average_window;
		cache.sr.start = ep.sr.start;
		cache.sr.window = ep.sr.window;

		cache.cw.frequency = ep.pll_status.mt_stat.frequency;
		cache.cw.harmonic = ep.pll_status.mt_stat.harmonic;
		cache.cw.frev = ep.frev;

		VERIFY( 0 == pthread_mutex_unlock( &cache_mutex ) );
	}

	return rc;
}
//--------------------------------------------------------------------------

/** Private. EBPP specific. Local to this module only.
 *
 *  Send a message to  the DSC server.
 *  Returns server reply status (typically CSPI_OK on success)
 *  or one of the following errors:
 *  CSPI_E_SYSTEM,
 *  CSPI_E_DSCPROTO.
 *
 *  @param msg_type Message type.
 *  @param msg_val  Pointer to msg_type specific value.
 */
int ebpp_dsc_message( size_t msg_type, size_t size, const void *msg_val )
{
	int rc = CSPI_E_SYSTEM;
	const pid_t pid = getpid();

	char fname[PATH_MAX];
	sprintf( fname, "/tmp/%d.fifo", pid );

	if ( mkfifo( fname, 0600 ) && EEXIST != errno ) return CSPI_E_SYSTEM;

	int srv_fd = open( DSCD_FIFO_PATHNAME, O_WRONLY | O_NONBLOCK );
	if ( -1 == srv_fd ) goto cleanup;

	message msg = { DSCD_MAGIC, msg_type, .val.ival = 0, pid, 0 };
	if( sizeof(double) == size )
		msg.val.dval = *(double*)msg_val;
	else if (msg_val)
		msg.val.ival = *(int*)msg_val;

	ssize_t n = write( srv_fd, &msg, sizeof(message) );

	close( srv_fd );
	if ( n != sizeof(message) ) goto cleanup;

	int fd = open( fname, O_RDONLY );
	if ( -1 == fd ) goto cleanup;

	n = read( fd, &msg, sizeof(message) );
	if ( n != sizeof(message) ) {

		rc = n < 0 ? CSPI_E_SYSTEM : CSPI_E_DSCPROTO;
	}
	else {

		if( sizeof(double) == size )
			*(double*)msg_val = msg.val.dval;
		else if (msg_val)
			*(int*)msg_val = msg.val.ival;
		rc = msg.status;
		/* Interpret DSC errors */
		if (0 > rc) {
			errno = -rc;
			rc = CSPI_E_SYSTEM;
		}
	}
	close(fd);

cleanup:
	unlink( fname );

	return rc;
}

//--------------------------------------------------------------------------

/** Private. EBPP specific. Local to this module only.
 *
 *  Convert gain in dBm to attenuator values.
 *  Returns sum of the first and second channel attenuators or -1 on error.
 *
 *  @param gain Gain value to convert [dBm].
 */
int ebpp_toattn( const int gain )
{
	FILE *fp = fopen( "/opt/dsc/gain.conf", "r" );
	if ( !fp ) return -1;

	int rc = -1;
	char *line = 0;
	size_t size = 0;

	ssize_t nread;
	while ((nread = getline( &line, &size, fp )) != -1 ) {

		const char *p = line;
		while ( isspace(*p) ) { ++p; }
		if (*p == '\0' || *p == '#') continue;

		int g, a1, a2;
		sscanf( p, "%d %d %d", &g, &a1, &a2 );
		if ( gain == g ) {
			rc = a1 + a2;
			break;
		}
	}
	if ( line ) free( line );
	fclose(fp);
	return rc;
}

//--------------------------------------------------------------------------

void signal_handler_hook( const CSPI_EVENTHDR *p )
{
	if (p->id == CSPI_EVENT_CFG) {

		switch (p->param) {

			case LIBERA_CFG_KX:
			case LIBERA_CFG_KY:
			case LIBERA_CFG_XOFFSET:
			case LIBERA_CFG_YOFFSET:
			case LIBERA_CFG_QOFFSET:
			case LIBERA_CFG_LEVEL:
			case LIBERA_CFG_DSC:
			case LIBERA_CFG_SR_CSPI_ENABLE:
			case LIBERA_CFG_SR_AVERAGING_STOP:
			case LIBERA_CFG_SR_AVERAGE_WINDOW:
			case LIBERA_CFG_SR_START:
			case LIBERA_CFG_SR_WINDOW:
			case LIBERA_CFG_SP_THRESHOLD:
			case LIBERA_CFG_SP_N_BEFORE:
			case LIBERA_CFG_SP_N_AFTER:
				_is_cache_dirty = 1;
		}
	}
}

//--------------------------------------------------------------------------

int custom_initop()
{
	if (_is_cache_dirty) {

		_is_cache_dirty = 0;

		VERIFY( 0 == pthread_mutex_lock( &environment.mutex ) );
		int rc = ebpp_update_cache( &environment );
		VERIFY( 0 == pthread_mutex_unlock( &environment.mutex ) );

		return rc;
	}

	return CSPI_OK;
}

//--------------------------------------------------------------------------

/** Private. EBPP specific. Local to this module only.
 *
 *  Validates Kx or Ky.
 *  On success, returns 1. Otherwise, returns 0.
 *  @param p Pointer to Kx or Ky.
 */
static int ebpp_is_validcoef( const void *p )
{
	const int max = INT_MAX/4;

	const size_t *K = p;
	return *K <= max;
}

//--------------------------------------------------------------------------

/** Private. EBPP specific. Local to this module only.
 *
 *  Validates Xoffset, Yoffset or Qoffset.
 *  On success, returns 1. Otherwise, returns 0.
 *  @param p Pointer to offset value.
 */
static int ebpp_is_validoffset( const void *p )
{
	const int min = INT_MIN/4;
	const int max = INT_MAX/4;

	const int offset = *(int *)p;
	return ( offset >= min && offset <= max );
}

//--------------------------------------------------------------------------

/** Private. EBPP specific. Local to this module only.
 *
 *  Validates decimation factor in DD mode.
 *  On success, returns 1. Otherwise, returns 0.
 *  @param p Pointer to a size_t variable with decimation to validate.
 */
static int ebpp_is_validdec( const void *p )
{
	const size_t dec = *(size_t *)p;
	return ( dec == 1 || dec == MAX_DEC );
}

//--------------------------------------------------------------------------

/** Private. EBPP specific. Local to this module only.
 *
 *  Validates switch configuration.
 *  On success, returns 1. Otherwise, returns 0.
 *  @param p Pointer to a size_t variable with switch value to validate.
 */
static int ebpp_is_validswitch( const void *p )
{
	const size_t switches = *(size_t *)p;
	return switches == CSPI_SWITCH_AUTO ||
		   switches == CSPI_SWITCH_DIRECT ||
		   switches >= CSPI_SWITCH_MIN  ||
		   switches <= CSPI_SWITCH_MAX;
}

//--------------------------------------------------------------------------

/** Private. EBPP specific. Local to this module only.
 *
 *  Validates gain setting.
 *  On success, returns 1. Otherwise, returns 0.
 *  @param p Pointer to a int variable with gain value to validate.
 */
static int ebpp_is_validgain( const void *p )
{
	const int MIN = -80, MAX = 0;

	const int gain = *(int *)p;
	return gain >= MIN && gain <= MAX;
}

//--------------------------------------------------------------------------

/** Private. EBPP specific. Local to this module only.
 *
 *  Validates ACG setting.
 *  On success, returns 1. Otherwise, returns 0.
 *  @param p Pointer to a size_t variable with AGC value to validate.
 */
static int ebpp_is_validagc( const void *p )
{
	const size_t agc = *(size_t *)p;
	return CSPI_AGC_AUTO == agc || CSPI_AGC_MANUAL == agc;
}

//--------------------------------------------------------------------------

/** Private. EBPP specific. Local to this module only.
 *
 *  Validates DSC setting.
 *  On success, returns 1. Otherwise, returns 0.
 *  @param p Pointer to a size_t variable with DSC value to validate.
 */
static int ebpp_is_validdsc( const void *p )
{
	const size_t dsc = *(size_t *)p;
	return dsc == CSPI_DSC_OFF    ||
		   dsc == CSPI_DSC_UNITY  ||
		   dsc == CSPI_DSC_AUTO   ||
		   dsc == CSPI_DSC_SAVE_LASTGOOD;
		   //|| dsc == CSPI_DSC_RESET_COEFF;
}

//--------------------------------------------------------------------------

/** Private. EBPP specific. Local to this module only.
 *
 *  Validates interlock parameters.
 *  On success, returns 1. Otherwise, returns 0.
 *  @param p Pointer to a size_t variable with mode to validate.
 */
static int ebpp_is_validilk( const void *p )
{
	const size_t mode = *(size_t *)p;
	return mode == CSPI_ILK_DISABLE ||
		   mode == CSPI_ILK_ENABLE  ||
		   mode == CSPI_ILK_ENABLE_GAINDEP;
}

//--------------------------------------------------------------------------

/** Private. EBPP specific. Local to this module only.
 *
 *  Validates Post Mortem offset parameters.
 *  On success, returns 1. Otherwise, returns 0.
 *  @param p Pointer to a long variable with offset to validate.
 */
static int ebpp_is_validpmoffset( const void *p )
{
	const long offset = *(long *)p;
	return (-512*1024 < offset) && (offset < 512*1024);
}

//--------------------------------------------------------------------------

/** Private. EBPP specific. Local to this module only.
 *
 *  Validates decimation.
 *  On success, returns 1. Otherwise, returns 0.
 *  @param p Pointer to a long variable with decimation to validate.
 */
static int ebpp_is_validdecimation( const void *p )
{
	const long param = *(long *)p;
	return (param == 1) || (param == 64);
}

//--------------------------------------------------------------------------

/** Private. EBPP specific. Local to this module only.
 *
 *  Validates trigger delay parameters.
 *  On success, returns 1. Otherwise, returns 0.
 *  @param p Pointer to a long variable with delay to validate.
 */
static int ebpp_is_validtrigdelay( const void *p )
{
	const long offset = *(long *)p;
	return (0 <= offset) && (offset <= 0x3fff);
}

//--------------------------------------------------------------------------

/** Private. EBPP specific. Local to this module only.
 *
 *  Validates Modified Average Filter length or delay parameter.
 *  On success, returns 1. Otherwise, returns 0.
 *  @param p Pointer to a long variable with parameter to validate.
 */
static int ebpp_is_validmafparameter( const void *p )
{
	const long param = *(long *)p;
	return (param >= 0) && (param < g_decimation);
}

//--------------------------------------------------------------------------

/** Private. EBPP specific. Local to this module only.
 *
 *  Validates 25 bit signed numbers;
 *  On success, returns 1. Otherwise, returns 0.
 *  @param p Pointer to a long variable with parameter to validate.
 */
static int ebpp_is_valid_int25( const void *p )
{
	const int param = *(int *)p;
	return (param >= -0x1000000) && (param <= 0xFFFFFF);
}

//--------------------------------------------------------------------------

/** Private. EBPP specific. Local to this module only.
 *
 *  Validates 5 bit signed numbers;
 *  On success, returns 1. Otherwise, returns 0.
 *  @param p Pointer to a long variable with parameter to validate.
 */
static int ebpp_is_valid_int5( const void *p )
{
	const int param = *(int *)p;
	return (param >= -16) && (param <= 15);
}

//--------------------------------------------------------------------------

/** Private. EBPP specific. Local to this module only.
 *
 *  Validates average vwindow value
 *  only one bit can be set (because division is realized with shifting)
 *  On success, returns 1. Otherwise, returns 0.
 *  @param p Pointer to a long variable with parameter to validate.
 */
static int ebpp_is_valid_averagewindow( const void *p )
{
	const unsigned int param = *(unsigned int *)p;
	unsigned int val;
	if ((param<0) || (param>16)) return 0;
	for (val = (param&0x1F); !(val&1) && val; val>>=1 );
	return (0 == (val>>1));
}

//--------------------------------------------------------------------------

/** Private. EBPP specific. Local to this module only.
 *
 *  Validates 7 bit unsigned numbers;
 *  On success, returns 1. Otherwise, returns 0.
 *  @param p Pointer to a long variable with parameter to validate.
 */
static int ebpp_is_valid_uint7( const void *p )
{
	const int param = *(int *)p;
	return (param >= 0) && (param <= SCHAR_MAX);
}

//--------------------------------------------------------------------------

/** Private. EBPP specific. Local to this module only.
 *
 *  Validates 16 bit signed numbers;
 *  On success, returns 1. Otherwise, returns 0.
 *  @param p Pointer to a long variable with parameter to validate.
 */
static int ebpp_is_valid_int16( const void *p )
{
	const int param = *(int *)p;
	return (param >= SHRT_MIN) && (param <= SHRT_MAX);
}

//--------------------------------------------------------------------------

/** Private. EBPP specific. Local to this module only.
 *
 *  Validates bool value (0 or 1)
 *  On success, returns 1. Otherwise, returns 0.
 *  @param p Pointer to a long variable with parameter to validate.
 */
static int ebpp_is_valid_bool( const void *p )
{
	const int param = *(int *)p;
	return (param >= 0) && (param <= 1);
}

//--------------------------------------------------------------------------

/** Private. EBPP specific. Local to this module only.
 *
 *  Validates pm mode (0,1,2);
 *  On success, returns 1. Otherwise, returns 0.
 *  @param p Pointer to a long variable with parameter to validate.
 */
static int ebpp_is_valid_pmmode( const void *p )
{
	const int param = *(int *)p;
	return (param >= 0) && (param < 3);
}

//--------------------------------------------------------------------------

/** Private. EBPP specific. Local to this module only.
 *
 *  Validates sp interval (0..1023)
 *  On success, returns 1. Otherwise, returns 0.
 *  @param p Pointer to a long variable with parameter to validate.
 */
static int ebpp_is_valid_spint( const void *p )
{
	const int param = *(int *)p;
	return (param >= 0) && (param < 1024);
}

//--------------------------------------------------------------------------

/** Private. EBPP specific. Local to this module only.
 *
 *  Adapts interlock overflow limit to the low-level interface.
 *  On success, returns 1. Otherwise, returns 0.
 *  @param p Pointer to a int variable with overflow limit to adapt.
 */
static int ebpp_is_valid_overflowlimit( const void *p )
{
	/* Checks made in Libera driver due to Brilliance dependency. */
	const int param = *(int *)p;
	return (param >= 0) && (param < SHRT_MAX);
}

//--------------------------------------------------------------------------

/** Private. EBPP specific. Local to this module only.
 *
 *  Adapts interlock overflow limit to the CSPI interface.
 *  Returns 1.
 *  @param p Pointer to a int variable with overflow limit to adapt.
 */
static int ebpp_get_overflowlimit( const void *p )
{
	unsigned int *limit = (unsigned int *)p;	// cast away const
	*limit >>= 16;
	return 1;
}

/** Private. EBPP specific. Local to this module only.
 *
 *  Validates interlock overflow duration parameter.
 *  On success, returns 1. Otherwise, returns 0.
 *  @param p Pointer to a int variable with overflow duration to adapt.
 */
static int ebpp_is_validoverflowdur( const void *p )
{
	/* Checks made in Libera driver due to DDC decimation dependency. */
	const int param = *(int *)p;
	return (param >= 0) && (param < 1024);
}

//--------------------------------------------------------------------------

/** Private. EBPP specific. Local to this module only.
 *
 *  Validates interlock gain limit to the low-level interface.
 *  On success, returns 1. Otherwise, returns 0.
 *  @param p Pointer to a int variable with gain limit to adapt.
 */
static int ebpp_set_gainlimit( const void *p )
{
	int *limit = (int *)p;	// cast away const
	const int attn = ebpp_toattn( *limit );
	if ( -1 == attn ) return 0;

	*limit = ( ((unsigned int) *limit) << 16 ) | ( attn & 0x0000ffff );
	return 1;
}

//--------------------------------------------------------------------------

/** Private. EBPP specific. Local to this module only.
 *
 *  Adapts interlock gain limit to the CSPI interface.
 *  Returns 1.
 *  @param p Pointer to a int variable with gain limit to adapt.
 */
static int ebpp_get_gainlimit( const void *p )
{
	int *limit = (int *)p;	// cast away const
	*limit >>= 16;
	return 1;
}

//--------------------------------------------------------------------------

/** Private.
 *  Represents DSC parameter traits.
 */
typedef struct tagDSC_param_traits {
	const uint64_t mask;		//!< Parameter bitmask.
	const uint64_t msg_type;	//!< DSC server request type.
	const size_t size;			//!< size of data
	VALIDATOR validate;			//!< Validation function.
}
DSC_param_traits;

/** Private.
 *  Maps a DSC parameter to the corresponding DSC_param_traits.
 */
typedef	struct tagDSC_param_map {
	const void *valaddr;		//!< Pointer to parameter value.
	DSC_param_traits traits;	//!< Parameter traits.
}
DSC_param_map;

/** Private.
 *  Macro to create a write-only DSC_param_map entry.
 */
#define DSC_WPARAM( NAME, VALADDR, TYPE, FNC ) \
	{VALADDR, {CSPI_ENV_ ## NAME, DSCD_SET_ ## NAME, sizeof(TYPE), FNC}}

/** Private.
 *  Macro to create a read-only DSC_param_map entry.
 */
#define DSC_RPARAM( NAME, VALADDR, TYPE, FNC ) \
	{VALADDR, {CSPI_ENV_ ## NAME, DSCD_GET_ ## NAME, sizeof(TYPE), FNC}}

//--------------------------------------------------------------------------

int ebpp_dsc_handle_params( DSC_param_map *p, CSPI_BITMASK flags )
{
	int rc = CSPI_OK;

	// Assume DSC_param_map vector is null terminated!
	for ( ; p->traits.mask; ++p ) {

		const DSC_param_traits *traits = &p->traits;
		if ( flags & traits->mask ) {

			if ( traits->validate && !traits->validate( p->valaddr ) ) {

				return CSPI_E_INVALID_PARAM;
			}

			rc = ebpp_dsc_message( traits->msg_type, traits->size, p->valaddr );
			if ( CSPI_OK != rc ) break;
		}
	}
	return rc;
}

//--------------------------------------------------------------------------

int ebpp_setdscparam(Environment *e, const CSPI_ENVPARAMS *p,
                     CSPI_BITMASK flags)
{
	int rc = CSPI_OK;

	if (!(fcntl(e->fd, F_GETFL) & O_RDWR))
	{
		errno = EPERM;
		return CSPI_E_SYSTEM;
	}

	const DSC_param_map map[] =
	{
		DSC_WPARAM( SWITCH, &p->switches, int, ebpp_is_validswitch ),
		DSC_WPARAM( AGC,    &p->agc,      int, ebpp_is_validagc    ),
		DSC_WPARAM( GAIN,   &p->gain,     int, ebpp_is_validgain   ),
		DSC_WPARAM( DSC,    &p->dsc,      int, ebpp_is_validdsc    ),
		{0, {CSPI_ENV_ILK, DSCD_APPLY_SETTINGS, 0, 0}},
		{0, {0,0,0}}
	};

	// NOTE: We can change p->switches here,
	// because its address is passed in DSC_param_map.
	if (p->switches == CSPI_SWITCH_DIRECT)
	{
		CSPI_ENVPARAMS *p1 = (CSPI_ENVPARAMS *)p;
		// Get feature register value.
		rc = cspi_getfeatureparam(e, p1, CSPI_ENV_FEATURE);
		if (CSPI_OK != rc)
			return rc;

		if (LIBERA_IS_BRILLIANCE(p1->feature.itech))
			p1->switches = CSPI_SWITCH_DIRECT_BRILLIANCE;
		else
			p1->switches = CSPI_SWITCH_DIRECT_NOTBRILLIANCE;

	}

	rc = ebpp_dsc_handle_params((DSC_param_map *) map, flags);

	if (flags & CSPI_ENV_FREV)
		_is_cache_dirty = 1;

	return rc;
}

//--------------------------------------------------------------------------

int ebpp_getdscparam(Environment *e, const CSPI_ENVPARAMS *p,
                     CSPI_BITMASK flags)
{
	DSC_param_map map[] = {

		DSC_RPARAM( SWITCH, &p->switches, int,    0 ),
		DSC_RPARAM( GAIN,   &p->gain,     int,    0 ),
		DSC_RPARAM( AGC,    &p->agc,      int,    0 ),
		DSC_RPARAM( DSC,    &p->dsc,      int,    0 ),
		DSC_RPARAM( FREV,   &p->frev,     double, 0 ),
		{0, {0,0,0}}
	};

	return ebpp_dsc_handle_params( map, flags );
}

//--------------------------------------------------------------------------
/** Private.
 *  Represents PLL command parameters traits.
 */
typedef struct tagPLL_param_traits
{
	const size_t mask;      //!< Parameter bitmask.
	char *cmd_strg;  //!< PLL command string.
	VALIDATOR validate;     //!< Validation function.
}
PLL_param_traits;

/** Private.
 *  Maps a PLL parameter.
 */
typedef struct tagPLL_param_map
{
	const void *valaddr;        //!< Pointer to parameter value.
	PLL_param_traits traits;    //!< Parameter traits.
}
PLL_param_map;

/** Private.
 *  Macro to create a PLL_param_map entry.
 */
#define PLL_PARAM( NAME, VALADDR, CMDS, FNC ) \
    {VALADDR, {CSPI_ENV_ ## NAME, CMDS, FNC}}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/** Private. EBPP specific. Local to this module only.
 *
 *  Validates MT VCXO offset parameters (-500 .. 500).
 *  On success, returns 1. Otherwise, returns 0.
 *  @param p Pointer to a size_t variable with mode to validate.
 */
static int ebpp_is_valid_mtvcxoffset(const void *p)
{
	int val = *(int *) p;  // get value.

	return (val <= 500) && (val >= -500);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/** Private. EBPP specific. Local to this module only.
 *
 *  Validates MT NCO shift parameters (0,1).
 *  On success, returns 1. Otherwise, returns 0.
 *  @param p Pointer to a size_t variable with mode to validate.
 */
static int ebpp_is_valid_mtncoshft(const void *p)
{
	int val = *(int *) p;  // get value.

	return (val == 0) || (val == 1);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/** Private. EBPP specific. Local to this module only.
 *
 *  Validates MT phase offset parameter (-12500/2 .. 12500/2).
 *  On success, returns 1. Otherwise, returns 0.
 *  @param p Pointer to a size_t variable with mode to validate.
 */
static int ebpp_is_valid_mtphsoffs(const void *p)
{
	int val = *(int *) p;  // get value.

	return (val <= 12500/2) && (val >= -12500/2);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/** Private. EBPP specific. Local to this module only.
 *
 *  Validates MT unlock threshold parameter (1 .. 30000).
 *  On success, returns 1. Otherwise, returns 0.
 *  @param p Pointer to a size_t variable with mode to validate.
 */
static int ebpp_is_valid_mtunlcktr(const void *p)
{
	int val = *(int *) p;  // get value.

	return (val > 0) && (val <= 30000);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/** Private. EBPP specific. Local to this module only.
 *
 *  Validates MT synchronization input parameter (0,1,2,..).
 *  On success, returns 1. Otherwise, returns 0.
 *  @param p Pointer to a size_t variable with mode to validate.
 */
static int ebpp_is_valid_mtsyncin(const void *p)
{
	int val = *(int *) p;  // get value.

	return (val >= 0);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/** Private. EBPP specific. Local to this module only.
 *
 *  Validates ST unlock threshold parameters (1 .. 30000).
 *  On success, returns 1. Otherwise, returns 0.
 *  @param p Pointer to a size_t variable with mode to validate.
 */
static int ebpp_is_valid_stunlcktr(const void *p)
{
	int val = *(int *) p;  // get value.

	return (val > 0) && (val <= 30000);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/** Private. EBPP specific. Local to this module only.
 *
 *  Read LPLLD staus pipe until it is empty.
 */
int ebpp_pll_clearstatus(int status_pipe)
{
	int rc = CSPI_E_SYSTEM;

    CSPI_LOG("%s", __FUNCTION__);
	int ret;
	pll_status_t pll_status;
	do {
		ret = read(status_pipe, &pll_status, sizeof(pll_status_t));
	}
	while ((ret >= 0) && (errno != EAGAIN));

	if ((ret < 0) && (errno != EAGAIN))
		rc = CSPI_OK;

	return rc;
}

//--------------------------------------------------------------------------
static int read_pll_status_buffer(int status_pipe, void *buf, size_t size)
{
	int ret;
	int len = 0;

	useconds_t timeout = 0;
	useconds_t inc = 1;
	int count = 5; // wait intervales: 0, 1us, 10us, 100us, 1ms

#warning "** Verify pipe comminication with pll: is it necessary to have 3 write/read calls? **"
	do {
		ret = read(status_pipe, (void *) buf + len, size);

		if (ret >= 0)
			len += ret;
		else if (errno == EAGAIN) {
			CSPI_LOG("No data ready yet, sleep for %d us", timeout);
			usleep(timeout);
			ret = 0;
			--count;
			timeout = inc;
			inc *= 10;
		}
	}
	while ((ret >= 0) && (len < size) && (count));

	CSPI_LOG("Read from pll status pipe, size: %d, len: %d", size, len);

	return len==size;
}

//--------------------------------------------------------------------------
/** Private. EBPP specific. Local to this module only.
 *
 *  Read LPLLD status from pipe.
 *  @param pll_status Pointer to a pll_status_t structure that will
 *  receive status values.
 */
int ebpp_pll_getstatus(int status_pipe, pll_status_ptr pll_status)
{
	int rc = CSPI_E_SYSTEM;

    CSPI_LOG("%s", __FUNCTION__);
	int status;

	status = read_pll_status_buffer(status_pipe,
									&pll_status->report_stseqn,
									sizeof(pll_status->report_stseqn));
	if (status)
	status = read_pll_status_buffer(status_pipe,
									&pll_status->mt_stat,
									sizeof(pll_status->mt_stat));
	if (status)
	status = read_pll_status_buffer(status_pipe,
									&pll_status->st_stat,
									sizeof(pll_status->st_stat));
	if (status)
	rc = CSPI_OK;

	CSPI_LOG("Got pll status: %d.", status);

	return rc;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/** Private. EBPP specific. Local to this module only.
 *
 *  Send command string to LPLLD pipe.
 *  @param cmd_strg String with command parameters.
 */
int ebpp_pll_setcmd(const char * cmd_strg)
{
	int rc = CSPI_E_SYSTEM;

	FILE *cmd_pipe = fopen(LPLLD_COMMAND_FIFO_d, "w");
    CSPI_LOG("%s", __FUNCTION__);
	if (NULL != cmd_pipe)
	{
		if (fputs(cmd_strg, cmd_pipe) > 0)
			rc = CSPI_OK;
		fclose(cmd_pipe);
	}
	else
	{
		rc = CSPI_E_INVALID_HANDLE;
	}

	return rc;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/** Private. EBPP specific. Local to this module only.
 *
 *  Function is called by CSPI. It will get status from LPLLD if
 *  CSPI_ENV_LPLLDSTAT bit is set in CSPI_BITMASK.
 *  @param e CSPI environment (not used).
 *  @param p Pointer to CSPI environment structure.
 *  @param flags Bitmask with CSPI_ENV_xxx flags.
 */
int ebpp_get_pllparam(Environment *e, const CSPI_ENVPARAMS *p,
                      CSPI_BITMASK flags)
{
	// Define which ustom params will be handled here.
	PLL_param_map map[] =
	{
		// LPLLD runtime parameters.
		PLL_PARAM(LPLLDSTAT, &p->pll_status, "q ", 0),

		// Note: must be null terminated!
		{ 0, {0, 0, 0} },
	};

	int rc = CSPI_OK;
	PLL_param_map *pmap = NULL;
	// Command to request LPLLD status).
	char *cmdmsg_str = NULL;
	pll_status_ptr pllstat = NULL;
    CSPI_LOG("%s", __FUNCTION__);

	// Assume Param_map vector is null terminated!
	for (pmap = map; pmap->valaddr; ++pmap)
	{
		const PLL_param_traits *traits = &pmap->traits;
		if (flags & traits->mask)
		{
			if (traits->validate && !traits->validate(pmap->valaddr))
			{
				return CSPI_E_INVALID_PARAM;
			}
			// Set command string to status request.
			cmdmsg_str = traits->cmd_strg;
			pllstat = (pll_status_ptr) pmap->valaddr;
		}
	}

	if (cmdmsg_str)
	{
		int status_pipe = open(LPLLD_STATUS_FIFO_d, O_RDONLY | O_NONBLOCK);
		if (-1 == status_pipe) {
			CSPI_ERR("Failed to open pipe, errno: %d.", errno);
			return CSPI_E_INVALID_HANDLE;
		}
		if (-1 != flock(status_pipe, LOCK_EX)) {
			// Clean orphan status messages from previous failures..
			ebpp_pll_clearstatus(status_pipe);
			// Send command string to pipe.
			rc = ebpp_pll_setcmd(cmdmsg_str);
			if (CSPI_OK == rc) {
				// Read LPLLD status structure.
				rc = ebpp_pll_getstatus(status_pipe, pllstat);
			}
			if (-1 == flock(status_pipe, LOCK_UN))
				CSPI_ERR("Failed to release lock on pipe, errno: %d.", errno);
		}
		else {
			CSPI_ERR("Failed to obtain lock on pipe, errno: %d.", errno);
			rc = CSPI_E_INVALID_HANDLE;
		}
		close(status_pipe);
	}

	return rc;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/** Private. EBPP specific. Local to this module only.
 *
 *  Function is called by CSPI. It will set runtime parameters to PLLD for
 *  values defined by CSPI_ENV_xxx flags.
 *  @param e CSPI environment (not used).
 *  @param p Pointer to CSPI environment structure.
 *  @param flags Bitmask with CSPI_ENV_xxx flags.
 */
int ebpp_set_pllparam(Environment *e, const CSPI_ENVPARAMS *p,
                      CSPI_BITMASK flags)
{
	// Define which ustom params will be handled here.
	PLL_param_map map[] =
	{
		// LPLLD runtime parameters.
		PLL_PARAM(MTVCXOFFS, &p->mtvcxoffs, "o%d ", ebpp_is_valid_mtvcxoffset),
		PLL_PARAM(MTNCOSHFT, &p->mtncoshft, "c%d ", ebpp_is_valid_mtncoshft),
		PLL_PARAM(MTPHSOFFS, &p->mtphsoffs, "f%d ", ebpp_is_valid_mtphsoffs),
		PLL_PARAM(MTUNLCKTR, &p->mtunlcktr, "l%d ", ebpp_is_valid_mtunlcktr),
		PLL_PARAM(MTSYNCIN, &p->mtsyncin,   "s%d ", ebpp_is_valid_mtsyncin),
		PLL_PARAM(STUNLCKTR, &p->stunlcktr, "k%d ", ebpp_is_valid_stunlcktr),
		// Note: must be null terminated!
		{ 0, {0, 0, 0} },
	};

	int rc = CSPI_OK;
	PLL_param_map *pmap = NULL;
	// Long enough to host 6 parameters from above, max(6 * 13) chars.
	// "x-2147483648 ".
	char cmd_str[96];
	memset(cmd_str, 0, sizeof(cmd_str));
	int cmdstr_len = 0;

    CSPI_LOG("%s", __FUNCTION__);
	// Assume Param_map vector is null terminated!
	for (pmap = map; pmap->valaddr; ++pmap)
	{
		const PLL_param_traits *traits = &pmap->traits;
		if (flags & traits->mask)
		{
			if (traits->validate && !traits->validate(pmap->valaddr))
			{
				return CSPI_E_INVALID_PARAM;
			}
			// Append command and value to command string.
			cmdstr_len += sprintf(cmd_str + cmdstr_len,
									traits->cmd_strg,
									*(int *) pmap->valaddr);
		}
	}

	if (cmdstr_len)
	{
		// Send command string to pipe.
		rc = ebpp_pll_setcmd(cmd_str);
	}

	return rc;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
#define PARAM_INIT( MAP, NAME, FIELD, OFFSET, FNC ) {\
    MAP.field = FIELD + OFFSET; \
    MAP.traits.mask = CSPI_ENV_ ## NAME; \
    MAP.traits.code = LIBERA_CFG_ ## NAME + OFFSET; \
    MAP.traits.validate = FNC; }

int set_coeff( Environment *e, const CSPI_ENVPARAMS *p,
                        CSPI_BITMASK flags )
{
	Param_map map[2 * LIBERA_CFG_NOTCH_MAX + LIBERA_CFG_FIR_MAX + 1];
	int i = 0;
	int j = 0;

	for (j = 0; j<LIBERA_CFG_NOTCH_MAX; j++, i++)
		PARAM_INIT( map[i], NOTCH1, p->notch1, j, 0)

	for (j = 0; j<LIBERA_CFG_NOTCH_MAX; j++, i++)
		PARAM_INIT( map[i], NOTCH2, p->notch2, j, 0)

	for (j = 0; j<LIBERA_CFG_FIR_MAX; j++, i++)
		PARAM_INIT( map[i], POLYPHASE_FIR, p->polyphase_fir, j, 0)

	map[i].field = 0; // null terminated

	return handle_params( e->fd, (Param_map *)map, flags, SET );
}

//--------------------------------------------------------------------------

int custom_initenv( CSPIHANDLE h ) {

	// Call base version first!
	int rc = base_initenv(h);

	// Now do the EBPP specific part.
	if (0 == rc) {
		Environment *p = (Environment *) h;
		libera_cfg_request_t req = {LIBERA_CFG_DEC_DDC, 0};
		if ( ioctl(p->fd, LIBERA_IOC_GET_CFG, &req) ) {
			rc = CSPI_E_SYSTEM;
		}
		else {
			g_decimation = req.val;
		}
	}
	_is_cache_dirty = 1;

	return rc;
}

//--------------------------------------------------------------------------

int custom_initcon( CSPIHANDLE h, CSPIHANDLE p ) {

	// Call base version only!
	return base_initcon( h, p );
}

//--------------------------------------------------------------------------

#define CSPI_ENV_ILK_MODE			CSPI_ENV_ILK
#define CSPI_ENV_ILK_XLOW			CSPI_ENV_ILK
#define CSPI_ENV_ILK_XHIGH			CSPI_ENV_ILK
#define CSPI_ENV_ILK_YLOW			CSPI_ENV_ILK
#define CSPI_ENV_ILK_YHIGH			CSPI_ENV_ILK
#define CSPI_ENV_ILK_OVERFLOW_LIMIT	CSPI_ENV_ILK
#define CSPI_ENV_ILK_OVERFLOW_DUR	CSPI_ENV_ILK
#define CSPI_ENV_ILK_GAIN_LIMIT		CSPI_ENV_ILK

#define CSPI_ENV_PM_MODE           CSPI_ENV_PM
#define CSPI_ENV_PM_XLOW    CSPI_ENV_PM
#define CSPI_ENV_PM_XHIGH    CSPI_ENV_PM
#define CSPI_ENV_PM_YLOW    CSPI_ENV_PM
#define CSPI_ENV_PM_YHIGH    CSPI_ENV_PM
#define CSPI_ENV_PM_OVERFLOW_LIMIT CSPI_ENV_PM
#define CSPI_ENV_PM_OVERFLOW_DUR   CSPI_ENV_PM

#define CSPI_ENV_SR_ENABLE         CSPI_ENV_SR
#define CSPI_ENV_SR_CSPI_ENABLE    CSPI_ENV_SR
#define CSPI_ENV_SR_AVERAGING_STOP CSPI_ENV_SR
#define CSPI_ENV_SR_AVERAGE_WINDOW CSPI_ENV_SR
#define CSPI_ENV_SR_START          CSPI_ENV_SR
#define CSPI_ENV_SR_WINDOW         CSPI_ENV_SR
#define CSPI_ENV_SR_BLOK_RAM       CSPI_ENV_SR

#define CSPI_ENV_SP_THRESHOLD      CSPI_ENV_SP
#define CSPI_ENV_SP_N_BEFORE       CSPI_ENV_SP
#define CSPI_ENV_SP_N_AFTER        CSPI_ENV_SP

int custom_setenvparam( Environment *e, const CSPI_ENVPARAMS *p,
                        CSPI_BITMASK flags )
{
	ASSERT(e);
	ASSERT(p);

	/* Assume environment has been locked by caller. */
	ASSERT( EBUSY == pthread_mutex_trylock( &e->mutex ) );

	// Custom params.
	const Param_map map[] = {

		PARAM( KX, &p->Kx, ebpp_is_validcoef ),
		PARAM( KY, &p->Ky, ebpp_is_validcoef ),

		PARAM( XOFFSET, &p->Xoffset, ebpp_is_validoffset ),
		PARAM( YOFFSET, &p->Yoffset, ebpp_is_validoffset ),
		PARAM( QOFFSET, &p->Qoffset, ebpp_is_validoffset ),

		PARAM( ILK_MODE, &(p->ilk).mode,  ebpp_is_validilk ),

		PARAM( ILK_XLOW,  &(p->ilk).Xlow,  ebpp_is_valid_int25 ),
		PARAM( ILK_XHIGH, &(p->ilk).Xhigh, ebpp_is_valid_int25 ),
		PARAM( ILK_YLOW,  &(p->ilk).Ylow,  ebpp_is_valid_int25 ),
		PARAM( ILK_YHIGH, &(p->ilk).Yhigh, ebpp_is_valid_int25 ),

		PARAM( ILK_OVERFLOW_DUR,   &(p->ilk).overflow_dur,  ebpp_is_validoverflowdur ),
		PARAM( ILK_OVERFLOW_LIMIT, &(p->ilk).overflow_limit, ebpp_is_valid_overflowlimit ),

		PARAM( ILK_GAIN_LIMIT, &(p->ilk).gain_limit, ebpp_set_gainlimit ),
		PARAM( ILKSTATUS,  &p->ilk_status,  0 ),
		PARAM( PMOFFSET,  &p->PMoffset, ebpp_is_validpmoffset ),
		PARAM( PMDEC,  &p->PMdec, ebpp_is_validdecimation ),
		PARAM( TRIGDELAY, &p->trig_delay, ebpp_is_validtrigdelay),

		PARAM( EXTSWITCH, &p->external_switching, 0),
		PARAM( SWDELAY, &p->switching_delay, 0),
		PARAM( DDC_MAFLENGTH, &p->ddc_maflength, ebpp_is_validmafparameter),
		PARAM( DDC_MAFDELAY, &p->ddc_mafdelay, ebpp_is_validmafparameter),

		PARAM( PM_MODE,  &p->pm.mode, ebpp_is_valid_pmmode ),
		PARAM( PM_XLOW,  &p->pm.min_x_limit, ebpp_is_valid_int25 ),
		PARAM( PM_XHIGH, &p->pm.max_x_limit, ebpp_is_valid_int25 ),
		PARAM( PM_YLOW,  &p->pm.min_y_limit, ebpp_is_valid_int25 ),
		PARAM( PM_YHIGH, &p->pm.max_y_limit, ebpp_is_valid_int25 ),
		PARAM( PM_OVERFLOW_LIMIT, &p->pm.overflow_limit, ebpp_is_valid_overflowlimit ),
		PARAM( PM_OVERFLOW_DUR, &p->pm.overflow_dur, ebpp_is_validoverflowdur ),

		PARAM( SR_ENABLE, &p->sr.enable, ebpp_is_valid_bool ),
		PARAM( SR_CSPI_ENABLE, &p->sr.cspi_enable, ebpp_is_valid_bool ),
		PARAM( SR_AVERAGING_STOP, &p->sr.averaging_stop, ebpp_is_valid_int5 ),
		PARAM( SR_AVERAGE_WINDOW, &p->sr.average_window, ebpp_is_valid_averagewindow ),
		PARAM( SR_START, &p->sr.start, ebpp_is_valid_int5 ),
		PARAM( SR_WINDOW, &p->sr.window, ebpp_is_valid_uint7 ),

		PARAM( SP_THRESHOLD, &p->sp.threshold, ebpp_is_valid_int16 ),
		PARAM( SP_N_BEFORE, &p->sp.n_before, ebpp_is_valid_spint ),
		PARAM( SP_N_AFTER, &p->sp.n_after, ebpp_is_valid_spint),

		// Note: must be null terminated!
		{ 0, {0, 0, 0} },
	};

	// Call "base" method to handle common params.
	int rc = base_setenvparam( e, p, flags );

	if ( CSPI_OK == rc ) {
		// Handle ebpp-specific params.
		rc = handle_params( e->fd, (Param_map *)map, flags, SET );
		if ( CSPI_OK == rc ) {
			// Handle filter coefficients.
			rc = set_coeff( e, p, flags);
			if ( CSPI_OK == rc ) {

				// Handle DSC and DSC-related params in a special way.
				rc = ebpp_setdscparam( e, p, flags );
				if ( CSPI_OK == rc )
				{
					// Handle LPLLD params in a special way.
					rc = ebpp_set_pllparam( e, p, flags );
				}
			}
		}
	}

	if ( flags & CACHE_MASK )
		_is_cache_dirty = 1;

	return rc;
}

//--------------------------------------------------------------------------

int custom_getenvparam( Environment *e, CSPI_ENVPARAMS *p, CSPI_BITMASK flags )
{
	ASSERT(e);
	ASSERT(p);

	// Assume environment has been locked by caller.
	ASSERT( EBUSY == pthread_mutex_trylock( &e->mutex ) );

	// Custom params
	Param_map map[] = {

		PARAM( KX, &p->Kx, 0 ),
		PARAM( KY, &p->Ky, 0 ),

		PARAM( XOFFSET, &p->Xoffset, 0 ),
		PARAM( YOFFSET, &p->Yoffset, 0 ),
		PARAM( QOFFSET, &p->Qoffset, 0 ),

		PARAM( ILK_MODE, &(p->ilk).mode,  0 ),

		PARAM( ILK_XLOW,  &(p->ilk).Xlow,  0 ),
		PARAM( ILK_XHIGH, &(p->ilk).Xhigh, 0 ),
		PARAM( ILK_YLOW,  &(p->ilk).Ylow,  0 ),
		PARAM( ILK_YHIGH, &(p->ilk).Yhigh, 0 ),

		PARAM( ILK_OVERFLOW_DUR,   &(p->ilk).overflow_dur,   0 ),
		PARAM( ILK_OVERFLOW_LIMIT, &(p->ilk).overflow_limit, ebpp_get_overflowlimit ),

		PARAM( ILK_GAIN_LIMIT, &(p->ilk).gain_limit, ebpp_get_gainlimit ),
		PARAM( ILKSTATUS,  &p->ilk_status,  0 ),
		PARAM( PMOFFSET,  &p->PMoffset,  0 ),
		PARAM( PMDEC,  &p->PMdec,  0 ),

		PARAM( TRIGDELAY, &p->trig_delay, 0),

		PARAM( EXTSWITCH, &p->external_switching, 0),
		PARAM( SWDELAY, &p->switching_delay, 0),

		PARAM( DDC_MAFLENGTH, &p->ddc_maflength, 0),
		PARAM( DDC_MAFDELAY, &p->ddc_mafdelay, 0),

		PARAM( MAX_ADC, &p->max_adc, 0),
		PARAM( AVERAGE_SUM, &p->average_sum, 0),
		PARAM( PM_MODE, &p->pm.mode, 0),
		PARAM( PM_XLOW, &p->pm.min_x_limit, 0),
		PARAM( PM_XHIGH, &p->pm.max_x_limit, 0),
		PARAM( PM_YLOW, &p->pm.min_y_limit, 0),
		PARAM( PM_YHIGH, &p->pm.max_y_limit, 0),
		PARAM( PM_OVERFLOW_LIMIT, &p->pm.overflow_limit, ebpp_get_overflowlimit),
		PARAM( PM_OVERFLOW_DUR, &p->pm.overflow_dur, 0),

		PARAM( SR_ENABLE, &p->sr.enable, 0),
		PARAM( SR_CSPI_ENABLE, &p->sr.cspi_enable, 0),
		PARAM( SR_AVERAGING_STOP, &p->sr.averaging_stop, 0),
		PARAM( SR_AVERAGE_WINDOW, &p->sr.average_window, 0),
		PARAM( SR_START, &p->sr.start, 0),
		PARAM( SR_WINDOW, &p->sr.window, 0),

		PARAM( SP_THRESHOLD, &p->sp.threshold, 0),
		PARAM( SP_N_BEFORE, &p->sp.n_before, 0),
		PARAM( SP_N_AFTER, &p->sp.n_after, 0),

		// Note: must be null terminated!
		{ 0, {0, 0, 0} },
	};

	if ( !flags ) return CSPI_OK;

	// Call "base" method to handle common params.
	int rc = base_getenvparam( e, p, flags );
	if ( CSPI_OK == rc ) {

		// Handle ebpp-specific params.
		rc = handle_params( e->fd, map, flags, GET );
		if ( CSPI_OK == rc ) {

			// Handle DSC and DSC-related params in a special way.
			rc = ebpp_getdscparam( e, p, flags );
			if ( CSPI_OK == rc )
			{
				// Handle LPLLD params in a special way.
				rc = ebpp_get_pllparam( e, p, flags );
			}
		}
	}

	return rc;
}

//--------------------------------------------------------------------------

int custom_setconparam( Connection *con, const CSPI_CONPARAMS *p, CSPI_BITMASK flags )
{
	ASSERT(con);
	ASSERT(p);

	// Call base method to handle common connection params.
	int rc = base_setconparam( con, p, flags );
	if ( CSPI_OK != rc ) return rc;

	// Handle DD connection params, specific to EBPP.
	if ( flags & CSPI_CON_DEC ) {

		if ( -1 == con->fd ) return CSPI_E_SEQUENCE;	// Connect first!
		if ( CSPI_MODE_DD != con->mode ) return CSPI_E_ILLEGAL_CALL;

		const CSPI_CONPARAMS_EBPP *q = (CSPI_CONPARAMS_EBPP *)p;
		if ( !ebpp_is_validdec( &q->dec ) ) return CSPI_E_INVALID_PARAM;

		ASSERT(-1 != con->fd);
		if (-1 == ioctl( con->fd, LIBERA_IOC_SET_DEC, &q->dec )) {
			return CSPI_E_SYSTEM;
		}
	}

        // Handle SA connection params, specific to EBPP.
	if ( flags & CSPI_CON_SANONBLOCK ) {

		if ( -1 == con->fd ) return CSPI_E_SEQUENCE;	// Connect first!
		if ( CSPI_MODE_SA != con->mode ) return CSPI_E_ILLEGAL_CALL;

		const CSPI_CONPARAMS_EBPP *q = (CSPI_CONPARAMS_EBPP *)p;

		ASSERT(-1 != con->fd);
		int sa_flags = fcntl( con->fd, F_GETFL, 0);
		if (-1 == sa_flags) {
			return CSPI_E_SYSTEM;
		}
		if ( q->nonblock )
			sa_flags |= O_NONBLOCK;
		else
			sa_flags &= ~O_NONBLOCK;
		if (-1 == fcntl( con->fd, F_SETFL, sa_flags )) {
			return CSPI_E_SYSTEM;
		}
	}

	return rc;
}

//--------------------------------------------------------------------------

int custom_getconparam( Connection *con, CSPI_CONPARAMS *p, CSPI_BITMASK flags )
{
	ASSERT(con);
	ASSERT(p);

	// Call base method to handle common connection params.
	int rc = base_getconparam( con, p, flags );
	if ( CSPI_OK != rc ) return rc;

	// Handle DD connection params, specific to the EBPP.
	if ( flags & CSPI_CON_DEC ) {

		if ( CSPI_MODE_DD != con->mode ) return CSPI_E_ILLEGAL_CALL;

		const CSPI_CONPARAMS_EBPP *q = (CSPI_CONPARAMS_EBPP *)p;

		ASSERT(-1 != con->fd);
		if (-1 == ioctl( con->fd, LIBERA_IOC_GET_DEC, &q->dec )) rc = CSPI_E_SYSTEM;
	}

	return rc;
}

//--------------------------------------------------------------------------

/** Private. EBPP specific. Local to this module only.
 *
 *  Transforms a single CSPI_DD_RAWATOM into CSPI_DD_ATOM.
 *  @param in Pointer to the CSPI_DD_RAWATOM to transform.
 *  @param out Pointer to CSPI_DD_ATOM to overwrite.
 */

void ebpp_transform_dd_single( const void *in, void *out )
{
	CSPI_DD_RAWATOM *p = (CSPI_DD_RAWATOM *)in;
	CSPI_DD_ATOM *q = (CSPI_DD_ATOM *)out;

	const int Va = q->Va = cordic_amp( p->sinVa >> 1, p->cosVa >> 1 );
	const int Vb = q->Vb = cordic_amp( p->sinVb >> 1, p->cosVb >> 1 );
	const int Vc = q->Vc = cordic_amp( p->sinVc >> 1, p->cosVc >> 1 );
	const int Vd = q->Vd = cordic_amp( p->sinVd >> 1, p->cosVd >> 1 );

	const int64_t S = (int64_t)Va+Vb+Vc+Vd;

	int64_t X = (int64_t)Va+Vd-Vb-Vc;
	X *= cache.Kx;
	q->X = (int)(X/S) - cache.Xoffset;

	int64_t Y = (int64_t)Va+Vb-Vc-Vd;
	Y *= cache.Ky;
	q->Y = (int)(Y/S) - cache.Yoffset;

	int64_t Q = (int64_t)Va+Vc-Vb-Vd;
	Q *= cache.Kx;
	q->Q = (int)(Q/S) - cache.Qoffset;

	// Prevent sum overflow
	q->Sum = (int)(S >> 2);
}

//--------------------------------------------------------------------------

/** Private. EBPP specific. Local to this module only.
 *
 *  Transforms a CSPI_DD_RAWATOM into CSPI_DD_ATOM. Returns 0.
 *  @param in Pointer to the CSPI_DD_RAWATOM to transform.
 *  @param out Pointer to CSPI_DD_ATOM to overwrite.
 */
int ebpp_transform_dd( const void *in, void *out, size_t count )
{
	CSPI_DD_RAWATOM *p = (CSPI_DD_RAWATOM *)in;
	CSPI_DD_ATOM *q = (CSPI_DD_ATOM *)out;

	for( size_t i=0; i<count; i++, p++, q++ ) {

		ebpp_transform_dd_single( p, q );
	}

	return 0;
}

//--------------------------------------------------------------------------

/** Private. EBPP specific. Local to this module only.
 *
 *  Transforms a CSPI_DD_RAWATOM into CSPI_DD_ATOM and remove spikes.
 *  Returns 0.
 *  @param in Pointer to the CSPI_DD_RAWATOM to transform.
 *  @param out Pointer to CSPI_DD_ATOM to overwrite.
 */
int ebpp_transform_dd_remove_spikes( const void *in, void *out, size_t count )
{
	CSPI_DD_RAWATOM *p = (CSPI_DD_RAWATOM *)in;
	CSPI_DD_ATOM *q = (CSPI_DD_ATOM *)out;
	CSPI_DD_ATOM *dest;
	CSPI_DD_ATOM *buffer = (CSPI_DD_ATOM *)out;

	size_t i, shifts;	// iterators
	int tb;				// trigger bit

	size_t eb, ee;				// edge begin, edge end
	size_t period;				// number of samples between two triggers
	size_t avestop, avestart;	// first and last samples where average is calculated
	size_t holdstart, holdstop;	// first and last samples where average value is to be applied
	size_t first, last;			// first and last samples where is possible to do calucaltions

	int64_t ave_a, ave_b, ave_c, ave_d;	// acumulated values
	int64_t S, X, Y, Q;					//

	//
	eb = ee = 0;

	// find period
	tb = p->cosVa & 1;
	ebpp_transform_dd_single( p, q );

	for( i=1, p++, q++; (i<count) && ((p->cosVa&1)==tb); i++, p++, q++ )
		ebpp_transform_dd_single( p, q );

	if( i==count ) return -1;	// no spike
	eb = i;

	tb = p->cosVa & 1;
	for( ; (i<count) && ((p->cosVa&1)==tb); i++, p++, q++ )
		ebpp_transform_dd_single( p, q );

	if( i==count ) return -2;	// no spike
	ee = i;

	// period found, calc transformations for rest of data
	for( ; i<count; i++, p++, q++ )
		ebpp_transform_dd_single( p, q );

	// calc shift value for dividing average values
	i = cache.sr.average_window;
	for( shifts=0; !(i&1) && (shifts<64); i>>=1, shifts++ );
	if( (i>>1) ) return -3;	// more than 1 bits are set

	// calc period
	period = ee-eb;

	// move edge begin position to first spike where calculations can be done
	first = -(cache.sr.start+cache.sr.averaging_stop-cache.sr.average_window+1);
	for( ; eb < first; eb += period );

	// calc intervals to do averages
	avestop   = eb+cache.sr.start+cache.sr.averaging_stop;
	avestart  = avestop-cache.sr.average_window+1;
	holdstart = eb+cache.sr.start;
	holdstop  = holdstart+cache.sr.window-1;

	// iterate thru data
	// while first position to apply average is in data range
	while( holdstart<count ) {

		// calculate average values
		ave_a = ave_b = ave_c = ave_d = 0LL;

		for( q=&buffer[avestart], i=avestart; i<=avestop; i++, p++)
		{
			ave_a += (int64_t)q->Va;
			ave_b += (int64_t)q->Vb;
			ave_c += (int64_t)q->Vc;
			ave_d += (int64_t)q->Vd;
		}

		ave_a >>= shifts;
		ave_b >>= shifts;
		ave_c >>= shifts;
		ave_d >>= shifts;

		S = ave_a+ave_b+ave_c+ave_d;
		X = ((ave_a+ave_d-ave_b-ave_c) * cache.Kx) / S - cache.Xoffset;
		Y = ((ave_a+ave_b-ave_c-ave_d) * cache.Ky) / S - cache.Yoffset;
		Q = ((ave_a+ave_c-ave_b-ave_d) * cache.Kx) / S - cache.Qoffset;
		S >>= 2;	// Prevent sum overflow

		// apply average values
		if( holdstop<count )
			last = holdstop;
		else
			last = count-1;

		q=&buffer[holdstart];
		q->Va = (int)ave_a;
		q->Vb = (int)ave_b;
		q->Vc = (int)ave_c;
		q->Vd = (int)ave_d;
		q->X = (int)X;
		q->Y = (int)Y;
		q->Q = (int)Q;
		q->Sum = (int)S;

		for( dest=q+1, i=holdstart+1; i<=last; i++, dest++ )
			memcpy( dest, q, sizeof(CSPI_DD_ATOM) );

		holdstart += period;
		holdstop += period;
		avestart += period;
		avestop += period;
	}

	return 0;
}

//--------------------------------------------------------------------------

/** Private. EBPP specific. Local to this module only.
 *
 *  Transforms ADC atom's values from [0..4095] to [-2047..2048].
 *  @param in Not used.
 *  @param out Pointer to the CSPI_ADC_ATOM to transform.
 */
static int ebpp_transform_adc( const void *in, void *out, size_t count )
{
	/* ADC transform moved to driver due to
	 * Libera Brilliance introduction.
	 */

	return 0;
}

//--------------------------------------------------------------------------

/** Private. EBPP specific. Local to this module only.
 *
 *  Calculate ADC CW valuses.
 *  @param in Not used.
 *  @param out Pointer to the CSPI_ADC_ATOM to transform.
 */
static int ebpp_transform_adc_cw( const void *in, void *out, size_t count )
{
	//* CW
	CSPI_ADC_ATOM *curr, *prev;
	CSPI_ADC_CW_ATOM *curr_out;

	int rc;
	double kx, ky;
	double flmcdhz, fif, fadc;
	double theta, a, b;

	kx = (double)cache.Kx;
	ky = (double)cache.Ky;

	double offx = (double)cache.Xoffset;
	double offy = (double)cache.Yoffset;

	flmcdhz = (double)cache.cw.frequency;

	fadc = flmcdhz/10.0;
	fif = (double)cache.cw.frev*(double)cache.cw.harmonic - flmcdhz*4.0;

	theta = (2.0*M_PI*fif)/fadc;
	a = -(cos(theta)/sin(theta));
	b = 1/sin(theta);

	double qa, qb, qc, qd;
	double va, vb, vc, vd;
	double sum;
	double x, y;
	double d1, d2;

	curr = prev = (CSPI_ADC_ATOM*)in;
	curr_out = (CSPI_ADC_CW_ATOM*)out;

	d1 = (a*(double)prev->chA);	qa = sqrt( 2.0*d1*d1 );
	d1 = (a*(double)prev->chB);	qb = sqrt( 2.0*d1*d1 );
	d1 = (a*(double)prev->chC);	qc = sqrt( 2.0*d1*d1 );
	d1 = (a*(double)prev->chD);	qd = sqrt( 2.0*d1*d1 );

	d1 = (double)curr->chA;	va = sqrt( d1*d1 + qa*qa );
	d1 = (double)curr->chB;	vb = sqrt( d1*d1 + qb*qb );
	d1 = (double)curr->chC;	vc = sqrt( d1*d1 + qc*qc );
	d1 = (double)curr->chD;	vd = sqrt( d1*d1 + qd*qd );

	sum = va + vb + vc + vd;
	x = cache.Kx * (va + vd - vb - vc) / sum;
	y = cache.Ky * (va + vb - vc - vd) / sum;

	curr_out->chA = curr->chA;
	curr_out->chB = curr->chB;
	curr_out->chC = curr->chC;
	curr_out->chD = curr->chD;
	curr_out->X = (int)(x - offx);
	curr_out->Y = (int)(y - offy);
	curr_out->Sum = (int)sum;
	curr_out->Qa = (int)qa;
	curr_out->Qb = (int)qb;
	curr_out->Qc = (int)qc;
	curr_out->Qd = (int)qd;

	++curr_out;
	++curr;

	for( size_t i=1; i<count; i++, prev++, curr++, curr_out++ ) {

		d1 = (a*(double)curr->chA);	d2 = (b*(double)prev->chA);	qa = sqrt( d1*d1 + d2*d2 );
		d1 = (a*(double)curr->chB);	d2 = (b*(double)prev->chB);	qb = sqrt( d1*d1 + d2*d2 );
		d1 = (a*(double)curr->chC);	d2 = (b*(double)prev->chC);	qc = sqrt( d1*d1 + d2*d2 );
		d1 = (a*(double)curr->chD);	d2 = (b*(double)prev->chD);	qd = sqrt( d1*d1 + d2*d2 );

		d1 = (double)curr->chA;	va = sqrt( d1*d1 + qa*qa );
		d1 = (double)curr->chB;	vb = sqrt( d1*d1 + qb*qb );
		d1 = (double)curr->chC;	vc = sqrt( d1*d1 + qc*qc );
		d1 = (double)curr->chD;	vd = sqrt( d1*d1 + qd*qd );

		sum = va + vb + vc + vd;
		x = kx * (va + vd - vb - vc) / sum;
		y = ky * (va + vb - vc - vd) / sum;

		curr_out->chA = curr->chA;
		curr_out->chB = curr->chB;
		curr_out->chC = curr->chC;
		curr_out->chD = curr->chD;
		curr_out->X = (int)(x - offx);
		curr_out->Y = (int)(y - offy);
		curr_out->Sum = (int)sum;
		curr_out->Qa = (int)qa;
		curr_out->Qb = (int)qb;
		curr_out->Qc = (int)qc;
		curr_out->Qd = (int)qd;
	}
	//*/

	return 0;
}

//--------------------------------------------------------------------------

typedef void (*SP_POS_FNC)(double *x, double *y, double *sum,
	 double aa, double ab, double ac, double ad);

/** Private. EBPP specific. Local to this module only.
 *
 *  Calculate ADC SP valuses.
 *  @param in Not used.
 *  @param out Pointer to the CSPI_ADC_ATOM to transform.
 */
static int ebpp_transform_adc_common( const void *in, void *out, size_t count,
	SP_POS_FNC sp_pos)
{
	CSPI_ADC_ATOM *buffer = (CSPI_ADC_ATOM*)in;
	CSPI_ADC_ATOM *curr;
	CSPI_ADC_SP_ATOM *o = (CSPI_ADC_SP_ATOM*)out;

	double kx = (double)cache.Kx;
	double ky = (double)cache.Ky;
	double offx = (double)cache.Xoffset;
	double offy = (double)cache.Yoffset;
	size_t trigger = count;

	int threshold = cache.sp.threshold;
	size_t n_before = cache.sp.n_before;
	size_t n_after = cache.sp.n_after;

	double ea, eb, ec, ed;	// energy
	ea = eb = ec = ed = 0.0;
	double aa, ab, ac, ad;	// amplitude
	aa = ab = ac = ad = 0.0;
	unsigned int i, j=0, k=0;
	double d;
	double sum = 0.0;
	double x = 0.0, y = 0.0;

	for( i=0, curr=buffer; i<count; i++, curr++ )
	{
		if ( ( curr->chA > threshold ) || ( curr->chB > threshold ) ||
			( curr->chC > threshold ) || ( curr->chD > threshold ) )
		{
			trigger = i;
			break;
		}
	}

	if( trigger < count )
	{
#define __min( a, b ) a < b ? a : b
#define __max( a, b ) a > b ? a : b

		if( trigger < n_before ) j=0; else j=trigger-n_before;
		k = __min( trigger+n_after+1, count );

		for( curr=buffer+j, i=j; i<k; i++, curr++ )
		{
			d = (double)curr->chA; ea += d*d;
			d = (double)curr->chB; eb += d*d;
			d = (double)curr->chC; ec += d*d;
			d = (double)curr->chD; ed += d*d;
		}

		aa = sqrt( ea );
		ab = sqrt( eb );
		ac = sqrt( ec );
		ad = sqrt( ed );

		sp_pos(&x, &y, &sum, aa, ab, ac, ad);

		x = kx * x - offx;
		y = ky * y - offy;
	}

	o->trigger = trigger;
	o->threshold = threshold;
	o->n_before = n_before;
	o->n_after = n_after;
	o->X = x;
	o->Y = y;
	o->Sum = sum;

	return 0;
}

void sp_pos_straight(double *x, double *y, double *sum,
					 double aa, double ab, double ac, double ad)
{
		*sum = aa + ab + ac + ad;
		*x = ( aa + ad - ac - ab ) / *sum;
		*y = ( aa + ab - ac - ad ) / *sum;
}

void sp_pos_rot(double *x, double *y, double *sum,
					 double aa, double ab, double ac, double ad)
{
		*sum = aa + ab + ac + ad;
		*x = ( aa - ac ) / ( aa + ac );
		*y = ( ab - ad ) / ( ab + ad );
}

static int ebpp_transform_adc_sp( const void *in, void *out, size_t count )
{
	return ebpp_transform_adc_common(in, out, count, sp_pos_straight);
}

static int ebpp_transform_adc_sp_rot( const void *in, void *out, size_t count )
{
	return ebpp_transform_adc_common(in, out, count, sp_pos_rot);
}

//--------------------------------------------------------------------------

CSPI_AUX_FNC custom_getdefaultop( const Connection *p )
{
	ASSERT(p);
	const int mode = p->mode;

	switch( mode ) {
		case CSPI_MODE_DD:
			custom_initop();
			if( (2 == cache.sr.dsc) && (0 != cache.sr.cspi_enable) ) {
				return ebpp_transform_dd_remove_spikes;
			}
			return ebpp_transform_dd;

		case CSPI_MODE_PM:
			return ebpp_transform_dd;

		case CSPI_MODE_ADC_CW:
			return ebpp_transform_adc_cw;

		case CSPI_MODE_ADC_SP:
			return ebpp_transform_adc_sp;

		case CSPI_MODE_ADC_SP_ROT:
			return ebpp_transform_adc_sp_rot;
	};
	return 0;
}

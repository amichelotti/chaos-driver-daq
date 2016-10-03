// $Id: cspi.c 2481 2009-03-16 07:42:29Z tomaz.beltram $

//! \file cspi.c
//! Implements Control System Programming Interface.

/*
CSPI - Control System Programming Interface
Copyright (C) 2004-2008 Instrumentation Technologies

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

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "eventd.h"		///chek!!!

#include "cspi.h"
#include "cspi_impl.h"
#include "msp.h"

/** A list of error messages corresponding to error codes. */
const char* err_list[] = {
	"success",
	"unknown error",
	"invalid handle",
	"function call sequence error",
	"invalid function parameter",
	"system-level call failed",
	"invalid mode of operation or mode not set",
	"illegal CSPI call",
	"failed to allocate memory",
	"driver version mismatch",
	"DSC server protocol error"
};

/** A list of warning messages corresponding to warning codes. */
const char* warn_list[] = {
	"success",
	"partially completed request",
};

/** Instantiate one and only environment object. */
Environment environment = {
	MAGIC_ENV,
	0,
	PTHREAD_MUTEX_INITIALIZER,
	-1,
	0,
	0,
	-1,
	CSPI_TRIGMODE_UNKNOWN,
	{ CSPI_VER, 0 },
};

//--------------------------------------------------------------------------

int debug_init()
{
	static int done = 0;
	if (!done) {

		// Note: closelog() is optional and therefore not used.
		openlog( "libcspi", LOG_PID|LOG_PERROR, 0 );
		done = 1;
	}
	return done;
}

//--------------------------------------------------------------------------

const char* cspi_strerror(int errnum)
{
	CSPI_LOG("%s(%d)", __FUNCTION__, errnum);

	typedef const char* CS;

	CS* p;
	size_t size;

	if (errnum < 0) {
		p = err_list;
		size = sizeof(err_list);
		errnum = -errnum;
	}
	else {
		p = warn_list;
		size = sizeof(warn_list);
	}

	int maxnum = size/sizeof(CS);
	return (errnum < maxnum) ? p[errnum] : err_list[-CSPI_E_UNKNOWN];
}

//--------------------------------------------------------------------------

int is_validmode( const void *p )
{
	const int *mode = (const int *) p;

	return ( CSPI_MODE_DD      == *mode ||
	         CSPI_MODE_SA      == *mode ||
	         CSPI_MODE_PM      == *mode ||
	         CSPI_MODE_ADC     == *mode ||
	         CSPI_MODE_ADC_CW  == *mode ||
	         CSPI_MODE_ADC_SP  == *mode ||
	         CSPI_MODE_ADC_SP_ROT == *mode ||
	         CSPI_MODE_AVERAGE == *mode);
}

//--------------------------------------------------------------------------

int is_validtrigmode( const void *p )
{
	const int *mode = (const int *) p;

	return ( CSPI_TRIGMODE_GET == *mode || CSPI_TRIGMODE_SET == *mode );
}

//--------------------------------------------------------------------------

int is_validversion( const void *p )
{
	const int *version = (const int *) p;

	// Only one version at present.
	return CSPI_VER == *version;
}

//--------------------------------------------------------------------------

int cspi_allochandle( int type, CSPIHANDLE h, CSPIHANDLE* p )
{
	CSPI_LOG("%s(%d, %p, %p)", __FUNCTION__, type, h, p);

	int rc = CSPI_E_INVALID_PARAM;
	if (!p) return CSPI_E_INVALID_PARAM;

	switch ( type ) {

		case CSPI_HANDLE_ENV:

			ASSERT( 0 == h );
			rc = alloc_env(p);
			if ( CSPI_OK != rc ) { free_env(*p); *p = 0; }

			break;

		case CSPI_HANDLE_CON:

			if ( !is_henv( h ) ) return CSPI_E_INVALID_HANDLE;
			rc = alloc_con( h, p );
			if ( CSPI_OK != rc ) { free_con(*p); *p = 0; }

			break;
	}

	return rc;
}

//--------------------------------------------------------------------------

int cspi_freehandle( int type, CSPIHANDLE h )
{
	CSPI_LOG("%s(%d, %p)", __FUNCTION__, type, h);

	if ( !h ) return CSPI_OK;
	int rc = CSPI_E_INVALID_PARAM;

	switch ( type ) {
		case CSPI_HANDLE_ENV:

			if ( !is_henv( h ) ) return CSPI_E_INVALID_HANDLE;

			rc = destroy_env( h );
			if ( CSPI_OK == rc ) free_env( h );

			break;

		case CSPI_HANDLE_CON:

			if ( !is_hcon( h ) ) return CSPI_E_INVALID_HANDLE;
			if ( ((Connection*) h)->fd > 0 ) return CSPI_E_SEQUENCE;

			rc = destroy_con( h );
			if ( CSPI_OK == rc ) free_con( h );

			break;
	}

	CSPI_LOG("%s: Exiting", __FUNCTION__);

	return rc;
}

//--------------------------------------------------------------------------

int alloc_env(CSPIHANDLE *p)
{
	// Don't really allocate a new environment.
	// Return pointer to the global environment struct instead.
	Environment *e = *p = &environment;

	VERIFY( 0 == pthread_mutex_lock( &e->mutex ) );
	e->usage_count++;
	int rc = custom_initenv(e);
	VERIFY( 0 == pthread_mutex_unlock( &e->mutex ) );

	return rc;
}

//--------------------------------------------------------------------------

void free_env( CSPIHANDLE h )
{
	ASSERT( is_henv(h) );
	Environment *p = (Environment*) h;

	VERIFY( 0 == pthread_mutex_lock( &p->mutex ) );
	ASSERT( p->usage_count > 0 );
	p->usage_count--;
	VERIFY( 0 == pthread_mutex_unlock( &p->mutex ) );
}

//--------------------------------------------------------------------------

int base_initenv( CSPIHANDLE h )
{
	ASSERT( is_henv(h) );
	Environment *p = (Environment *) h;

	/* Assume environment has been locked by caller. */
	ASSERT( EBUSY == pthread_mutex_trylock( &p->mutex ) );
	ASSERT( p->usage_count > 0 );

	if ( 1 == p->usage_count ) {

		int flags = ( (p->module).superuser ) ? O_RDWR : O_RDONLY;
		p->fd = open( "/dev/libera.cfg", flags );

		if (-1 == p->fd) {

			VERIFY( 0 == reset_sighandler() );
			return CSPI_E_SYSTEM;
		}
	}

	ASSERT(p->fd > 0);
	return test_drvmismatch(p->fd);
}

//--------------------------------------------------------------------------

int test_drvmismatch(int fd)
{
	int magic;
	int rc = ioctl( fd, LIBERA_IOC_GET_MAGIC, &magic );
	if (-1 == rc) return CSPI_E_SYSTEM;

	return LIBERA_MAGIC == magic ? CSPI_OK : CSPI_E_VERSION;
}

//--------------------------------------------------------------------------

int destroy_env( CSPIHANDLE h )
{
	ASSERT( is_henv(h) );
	Environment *p = (Environment *) h;

	VERIFY( 0 == pthread_mutex_lock( &p->mutex ) );
	ASSERT( p->usage_count > 0 );

	int rc = 0;
	if ( 1 == p->usage_count ) {

		ASSERT( -1 != p->fd );

		// All connections must have been closed
		// if this is the last environment handle.
		if ( 0 != p->connection_count ) return CSPI_E_SEQUENCE;

		VERIFY( 0 == close( p->fd ) );
		p->fd = -1;

		// Reset signal handler to the default action.
		VERIFY( 0 == reset_sighandler() );
	}

	VERIFY( 0 == pthread_mutex_unlock( &p->mutex ) );
	return ( -1 != rc ) ? CSPI_OK : CSPI_E_SYSTEM;;
}

//--------------------------------------------------------------------------

int reset_sighandler()
{
	struct sigaction sa;
	sigemptyset( &sa.sa_mask );
	sa.sa_handler = SIG_DFL;
	sa.sa_flags = 0;

	return sigaction( LIBERA_SIGNAL, &sa, 0 );
}

//--------------------------------------------------------------------------

int alloc_con( CSPIHANDLE h, CSPIHANDLE *p )
{
	CSPIHANDLE q = malloc( sizeof(Connection) );
	if ( q ) memset( q, 0, sizeof(Connection) );

	*p = q;
	return custom_initcon( h, *p );
}

//--------------------------------------------------------------------------

void free_con( CSPIHANDLE h )
{
	ASSERT( is_hcon(h) );
	free( h );
}

//--------------------------------------------------------------------------

int base_initcon( CSPIHANDLE h, CSPIHANDLE hc )
{
	ASSERT( is_henv(h) );

	static int id = 0;
	Connection *p = (Connection*) hc;

	p->type_id = MAGIC_CON;
	p->connection_id = ++id;
	p->fd = -1;
	p->event_mask = 0xffff;
	p->environment = (Environment*) h;

	// Assume connection structure is memset to 0 when allocated.
	ASSERT( CSPI_MODE_UNKNOWN == p->mode );

	ASSERT( 0 == p->timestamp.st.tv_sec );
	ASSERT( 0 == p->timestamp.st.tv_nsec );
	ASSERT( 0 == p->timestamp.mt );

	ASSERT( 0 == p->handler );
	ASSERT( 0 == p->pid );

	insert_con( p );
	return CSPI_OK;
}

//--------------------------------------------------------------------------

int destroy_con( CSPIHANDLE h )
{
	ASSERT( is_hcon(h) );
	Connection *p = (Connection*) h;

	ASSERT( CSPI_MODE_UNKNOWN == p->mode );
	ASSERT( -1 == p->fd );

	remove_con( p );
	p->environment = 0;

	return CSPI_OK;
}

//--------------------------------------------------------------------------

void insert_con( CSPIHANDLE h )
{
	CSPI_LOG("%s(%p)", __FUNCTION__, h);
	ASSERT( is_hcon(h) );
	Connection *p = (Connection*) h;

	ASSERT( is_henv( p->environment ) );
	Environment *e =  p->environment;

	ASSERT( 0 == p->next );
	ASSERT( 0 == p->prev );

	VERIFY( 0 == pthread_mutex_lock( &e->mutex ) );

	if ( e->head ) {
		p->next = e->head;
		p->next->prev = p;
	}

	e->head = p;
	e->connection_count++;
	CSPI_LOG("e: %p, connection_count: %d, mask: 0x%08x, handler: %p",
		   	e, e->connection_count, p->event_mask, p->handler);

	VERIFY( 0 == pthread_mutex_unlock( &e->mutex ) );
}

//--------------------------------------------------------------------------

void remove_con( CSPIHANDLE h )
{
	CSPI_LOG("%s: (%p)", __FUNCTION__, h);
	ASSERT( is_hcon(h) );
	Connection *p = (Connection*) h;

	ASSERT( is_henv( p->environment ) );
	Environment *e =  p->environment;

	VERIFY( 0 == pthread_mutex_lock( &e->mutex ) );

	if ( p->prev ) p->prev->next = p->next;
	if ( p->next ) p->next->prev = p->prev;

	if ( e->head == p ) e->head = p->next;
	p->prev = p->next = 0;
	e->connection_count--;
	CSPI_LOG("e: %p, connection_count: %d, mask: 0x%08x, handler: %p",
		   	e, e->connection_count, p->event_mask, p->handler);

	if( 0 == e->connection_count ) {
		unregister_event_handler( p );
		destroy_event_thread();
	}

	VERIFY( 0 == pthread_mutex_unlock( &e->mutex ) );
	CSPI_LOG("%s: Exiting", __FUNCTION__);
}

//--------------------------------------------------------------------------

int cspi_setlibparam( const CSPI_LIBPARAMS *p, CSPI_BITMASK flags )
{
	CSPI_LOG("%s(%p, %llu)", __FUNCTION__, p, flags);

	if ( !p ) return CSPI_E_INVALID_PARAM;
	Library *module = &environment.module;

	VERIFY( 0 == pthread_mutex_lock( &environment.mutex ) );
	int rc = set_libparam( module, p, flags );
	VERIFY( 0 == pthread_mutex_unlock( &environment.mutex ) );

	return rc;
}

//--------------------------------------------------------------------------

int set_libparam( Library *module, const CSPI_LIBPARAMS *p, CSPI_BITMASK flags )
{
	ASSERT( module );
	ASSERT( p );

	/* Assume environment has been locked by caller. */
	if ( flags & CSPI_LIB_VERSION ) {

		if ( !is_validversion( &p->version ) ) return CSPI_E_INVALID_PARAM;
		module->version = p->version;
	}
	if ( flags & CSPI_LIB_SUPERUSER ) module->superuser = p->superuser;

	return CSPI_OK;
}

//--------------------------------------------------------------------------

int cspi_getlibparam( CSPI_LIBPARAMS *p, CSPI_BITMASK flags )
{
	CSPI_LOG("%s(%p, %llu)", __FUNCTION__, p, flags);

	if ( !p ) return CSPI_E_INVALID_PARAM;
	Library *module = &environment.module;

	VERIFY( 0 == pthread_mutex_lock( &environment.mutex ) );
	int rc = get_libparam( module, p, flags );
	VERIFY( 0 == pthread_mutex_unlock( &environment.mutex ) );

	return rc;
}

//--------------------------------------------------------------------------

int get_libparam( Library *module, CSPI_LIBPARAMS *p, CSPI_BITMASK flags )
{
	ASSERT( module );
	ASSERT( p );

	/* Assume environment has been locked by caller. */
	if ( flags & CSPI_LIB_VERSION ) p->version = module->version;
	if ( flags & CSPI_LIB_SUPERUSER ) p->superuser = module->superuser;

	return CSPI_OK;
}

//--------------------------------------------------------------------------

int cspi_sethealthparam(Environment *e, const CSPI_ENVPARAMS *p,
                        CSPI_BITMASK flags)
{
	// SET not supported for HEALTH parameters
	if ( flags & CSPI_ENV_HEALTH )
		return CSPI_E_ILLEGAL_CALL;

	return CSPI_OK;
}

//--------------------------------------------------------------------------

int cspi_health_get_temp( int *temp, const char *proc_filename )
{

	FILE *fp = fopen (proc_filename, "r" );
	if ( !fp ) return CSPI_E_SYSTEM;

	fscanf( fp, "%d\n",temp );
	fclose( fp );
	*temp = *temp / 1000;
	return CSPI_OK;
}

int cspi_health_get_fan( int *speed, const char *proc_filename )
{
	FILE *fp = fopen( proc_filename, "r" );
	if ( !fp ) return CSPI_E_SYSTEM;

	fscanf( fp, "%d\n", speed );
	fclose( fp );

	return CSPI_OK;
}

int cspi_health_get_voltages( int *voltage )
{
	int rc = CSPI_OK;
	size_t nread, i;
	msp_atom_t msp_atom;
	const char *mspdev = "/dev/msp0";

	int fd = open(mspdev, O_RDONLY);
	if ( fd < 0 ) return CSPI_E_SYSTEM;

	nread = read( fd, &msp_atom, sizeof(msp_atom_t));
	if ( ( nread != sizeof(msp_atom_t) ) &&
			( nread < 0 ) )
		rc = CSPI_E_SYSTEM;

	for (i = 0; i < 8; i++)
		*(voltage + i) = msp_atom.voltage[i];

	close(fd);
	return rc;
}

//--------------------------------------------------------------------------

int cspi_gethealthparam(Environment *e, CSPI_ENVPARAMS *p,
                        CSPI_BITMASK flags)
{
	const char *dir       = "/sys/class/i2c-adapter/i2c-0";
	const char *check_dir = "/sys/class/i2c-adapter/i2c-0/device/0-0029";
	const char *proc_temp = "/0-0029/temp1_input";
	const char *proc_fan0 = "/0-004b/fan1_input";
	const char *proc_fan1 = "/0-0048/fan1_input";
	char p_temp[1024];
	char p_fan0[1024];
	char p_fan1[1024];

	int rc = CSPI_OK;

	if ( flags & CSPI_ENV_HEALTH ) {

		if (access(check_dir, F_OK)==0) {
			sprintf(p_temp, "%s/device%s", dir, proc_temp);
			sprintf(p_fan0, "%s/device%s", dir, proc_fan0);
			sprintf(p_fan1, "%s/device%s", dir, proc_fan1);
		}
		else {
			sprintf(p_temp, "%s%s", dir, proc_temp);
			sprintf(p_fan0, "%s%s", dir, proc_fan0);
			sprintf(p_fan1, "%s%s", dir, proc_fan1);
		}

    	// Temperature
		rc = cspi_health_get_temp( &p->health.temp, p_temp );
		if ( CSPI_OK != rc ) return rc;

		// Front fan
		rc = cspi_health_get_fan( &p->health.fan[0], p_fan0 );
		if ( CSPI_OK != rc ) return rc;
		// Back fan
		rc = cspi_health_get_fan( &p->health.fan[1], p_fan1 );
		if ( CSPI_OK != rc ) return rc;

		// PS voltages
		rc = cspi_health_get_voltages( p->health.voltage );
		if ( CSPI_OK != rc ) return rc;
	}
	return rc;
}

//--------------------------------------------------------------------------

int cspi_setfeatureparam(Environment *e, const CSPI_ENVPARAMS *p,
                        CSPI_BITMASK flags)
{
	// SET not supported for FEATURE parameters
	if ( flags & CSPI_ENV_FEATURE )
		return CSPI_E_ILLEGAL_CALL;

	return CSPI_OK;
}

//--------------------------------------------------------------------------

int cspi_getfeatureparam(Environment *e, CSPI_ENVPARAMS *p,
                        CSPI_BITMASK flags)
{
	if ( flags & CSPI_ENV_FEATURE ) {
		// Read form cfg device.
		libera_cfg_request_t req;

		req.idx = LIBERA_CFG_FEATURE_CUSTOMER;
		if ( -1 == ioctl( e->fd, LIBERA_IOC_GET_CFG, &req ) )
			return CSPI_E_SYSTEM;
		p->feature.customer = req.val;

		req.idx = LIBERA_CFG_FEATURE_ITECH;
		if ( -1 == ioctl( e->fd, LIBERA_IOC_GET_CFG, &req ) )
			return CSPI_E_SYSTEM;
		p->feature.itech = req.val;
	}
	return CSPI_OK;
}

//--------------------------------------------------------------------------

int cspi_setenvparam( CSPIHENV h, const CSPI_ENVPARAMS *p, CSPI_BITMASK flags )
{
	CSPI_LOG("%s(%p, %p, %llu)", __FUNCTION__, h, p, flags);

	if ( !is_henv(h) ) return CSPI_E_INVALID_HANDLE;
	if ( !p ) return CSPI_E_INVALID_PARAM;

	Environment *e =  (Environment*) h;
	VERIFY( 0 == pthread_mutex_lock( &e->mutex ) );

	// Call derived (customized) function to set parameters.
	int rc = custom_setenvparam( e, p, flags );

	VERIFY( 0 == pthread_mutex_unlock( &e->mutex ) );
	return rc;
}

//--------------------------------------------------------------------------

int base_setenvparam( Environment *e, const CSPI_ENVPARAMS *p, CSPI_BITMASK flags )
{
	int rc = CSPI_OK;

	ASSERT(e);
	ASSERT(p);

	// Assume environment has been locked by caller.
	ASSERT( EBUSY == pthread_mutex_trylock( &e->mutex ) );

	if ( !flags ) return CSPI_OK;

	// Base parameter(s).
	Param_map map[] = {

		PARAM( TRIGMODE, &p->trig_mode, is_validtrigmode ),

		//PARAM( SCPLL, p->pll.sc, 0 ), // PLL status _SET_ not viable.
		//PARAM( MCPLL, p->pll.mc, 0 ), // PLL status _SET_ not viable.

		//...

		// Note: must be null terminated!
		{ 0, {0, 0} },
	};

        // Handle base params
	rc = handle_params( e->fd, map, flags, SET );
	if ( CSPI_OK == rc ) {

		// Handle HEALTH params.
		rc = cspi_sethealthparam( e, p, flags );
		if ( CSPI_OK == rc ) {

			// Handle FEATURE params.
			rc = cspi_setfeatureparam( e, p, flags );
		}
	}

	return rc;
}

//--------------------------------------------------------------------------

int cspi_getenvparam( CSPIHENV h, CSPI_ENVPARAMS *p, CSPI_BITMASK flags )
{
	CSPI_LOG("%s(%p, %p, %llu)", __FUNCTION__, h, p, flags);

	if ( !is_henv(h) ) return CSPI_E_INVALID_HANDLE;
	if ( !p ) return CSPI_E_INVALID_PARAM;

	Environment *e =  (Environment*) h;
	VERIFY( 0 == pthread_mutex_lock( &e->mutex ) );

	// Call derived (customized) function to retrieve parameter values.
	int rc = custom_getenvparam( e, p, flags );

	VERIFY( 0 == pthread_mutex_unlock( &e->mutex ) );
    return rc;
}

//--------------------------------------------------------------------------

int handle_params( int fd, Param_map *p, CSPI_BITMASK flags, int op )
{
	ASSERT(fd > 0);

	ASSERT(p);
	ASSERT(SET==op || GET==op);

	typedef int (*DX)(int, int*, const Param_traits*);
	DX proxy = ((SET==op) ? set_param : get_param);

	int rc = CSPI_OK;

	// Assume Param_map vector is null terminated!
	for ( ; p->field; ++p ) {

		const Param_traits *traits = &p->traits;
		if ( flags & traits->mask ) {
			rc = proxy( fd, (int *)p->field, traits );
			if ( CSPI_OK != rc ) break;
		}
	}

	return rc;
}

//--------------------------------------------------------------------------

int set_param( int fd, int *p, const Param_traits *traits )
{
	ASSERT(fd > 0);

	ASSERT(p);
	ASSERT(traits);

	VALIDATOR validate = traits->validate;
	if ( validate && !validate(p) ) return CSPI_E_INVALID_PARAM;

	libera_cfg_request_t request = { traits->code, *p };
	if ( -1 == ioctl( fd, LIBERA_IOC_SET_CFG, &request ) ) return CSPI_E_SYSTEM;

	return CSPI_OK;
}

//--------------------------------------------------------------------------

int get_param( int fd, int *p, const Param_traits *traits )
{
	ASSERT(fd > 0);

	ASSERT(p);
	ASSERT(traits);

	libera_cfg_request_t request = { traits->code, 0 };
	if ( -1 == ioctl( fd, LIBERA_IOC_GET_CFG, &request ) ) return CSPI_E_SYSTEM;

	*p = request.val;

	VALIDATOR validate = traits->validate;
	if ( validate && !validate(p) ) return CSPI_E_INVALID_PARAM;

	return CSPI_OK;
}

//--------------------------------------------------------------------------

#define CSPI_ENV_SCPLL		CSPI_ENV_PLL
#define CSPI_ENV_MCPLL		CSPI_ENV_PLL

int base_getenvparam( Environment *e, CSPI_ENVPARAMS *p, CSPI_BITMASK flags )
{
	int rc = CSPI_OK;

	ASSERT(e);
	ASSERT(p);

	// Assume environment has been locked by caller.
	ASSERT( EBUSY == pthread_mutex_trylock( &e->mutex ) );

	if ( !flags ) return CSPI_OK;

	// Base parameter(s).
	Param_map map[] = {

		PARAM( TRIGMODE, &p->trig_mode, 0 ),

		PARAM( SCPLL, &p->pll.sc, 0 ),
		PARAM( MCPLL, &p->pll.mc, 0 ),

		//...

		// Note: must be null terminated!
		{ 0, {0, 0} },
	};

	// Handle base params
	rc = handle_params( e->fd, map, flags, GET );
	if ( CSPI_OK == rc ) {

		// Handle HEALTH params.
		rc = cspi_gethealthparam( e, p, flags );
		if ( CSPI_OK == rc ) {

			// Handle FEATURE params.
			rc = cspi_getfeatureparam( e, p, flags );
		}
	}

	return rc;
}

//--------------------------------------------------------------------------

int cspi_setenvparam_fa( CSPIHENV h, size_t offset,
                         const void *pbuf, size_t size, size_t count )
{
	CSPI_LOG("%s(%p, %u, %p, %u, %u)",
		__FUNCTION__, h, offset, pbuf, size, count);

	if ( !is_henv(h) ) return CSPI_E_INVALID_HANDLE;
	if ( !pbuf ) return CSPI_E_INVALID_PARAM;

	// size must be aligned on a 4-byte boundary
	if ( size % 4 ) return CSPI_E_INVALID_PARAM;

	Environment *e =  (Environment*) h;
	int fd = open( "/dev/libera.fa", O_WRONLY );
	if ( -1 == fd ) return CSPI_E_SYSTEM;

	int rc = CSPI_E_SYSTEM;
	VERIFY( 0 == pthread_mutex_lock( &e->mutex ) );

	if ((off_t)-1 != lseek(fd, offset, SEEK_SET)) {
		if ((count*size) == write(fd, pbuf, count*size)) rc = CSPI_OK;
	}

	VERIFY( 0 == pthread_mutex_unlock( &e->mutex ) );
	VERIFY( 0 == close(fd) );
    return rc;
}

//--------------------------------------------------------------------------

int cspi_getenvparam_fa( CSPIHENV h, size_t offset,
                         void *pbuf, size_t size, size_t count )
{
	CSPI_LOG("%s(%p, %u, %p, %u, %u)",
		__FUNCTION__, h, offset, pbuf, size, count);

	if ( !is_henv(h) ) return CSPI_E_INVALID_HANDLE;
	if ( !pbuf ) return CSPI_E_INVALID_PARAM;

	// size must be aligned on a 4-byte boundary
	if ( size % 4 ) return CSPI_E_INVALID_PARAM;

	Environment *e =  (Environment*) h;
	int fd = open( "/dev/libera.fa", O_RDONLY );
	if ( -1 == fd ) return CSPI_E_SYSTEM;

	int rc = CSPI_E_SYSTEM;
	VERIFY( 0 == pthread_mutex_lock( &e->mutex ) );

	if ((off_t)-1 != lseek(fd, offset, SEEK_SET)) {
		if ((count*size) == read(fd, pbuf, count*size)) rc = CSPI_OK;
	}

	VERIFY( 0 == pthread_mutex_unlock( &e->mutex ) );
	VERIFY( 0 == close(fd) );
    return rc;
}

//--------------------------------------------------------------------------

int cspi_setconparam( CSPIHCON h, const CSPI_CONPARAMS *p, CSPI_BITMASK flags )
{
	CSPI_LOG("%s(%p, %p, %llu)", __FUNCTION__, h, p, flags);

	if ( !is_hcon(h) ) return CSPI_E_INVALID_HANDLE;
	if ( !p ) return CSPI_E_INVALID_PARAM;

	// Do not check if connected!
	// Base connection parameters are handled internally and
	// do not require a valid device file descriptor.

	// If necessary, custom_setconparam should take care of the check itself!
	return custom_setconparam( (Connection *)h, p, flags );
}

//--------------------------------------------------------------------------

int base_setconparam( Connection *con, const CSPI_CONPARAMS *p, CSPI_BITMASK flags )
{
	ASSERT(con);
	ASSERT(p);

	int rc = CSPI_OK;

	// Handle base CSPI_CONPARAMS structure.
	if ( !flags ) return CSPI_OK;

	if ( flags & CSPI_CON_MODE ) {

		if ( !is_validmode( &p->mode ) ) {

			return CSPI_E_INVALID_PARAM;
		}
		con->mode = p->mode;
	}

	// Event handler depends on event mask. Set mask before handler!
	if ( flags & CSPI_CON_EVENTMASK ) con->event_mask = p->event_mask;

	if ( flags & CSPI_CON_HANDLER && p->handler) {
		CSPI_LOG("Setting connection handler: %p.", p->handler);

		con->pid = getpid();
		con->handler = p->handler;
		VERIFY( 0 == pthread_mutex_lock( &(con->environment)->mutex ) );
		rc = create_event_thread( con );
		VERIFY( 0 == pthread_mutex_unlock( &(con->environment)->mutex ) );
	}

	if ( flags & CSPI_CON_USERDATA ) con->user_data = p->user_data;

	return rc;
}

//--------------------------------------------------------------------------

int cspi_getconparam( CSPIHCON h, CSPI_CONPARAMS *p, CSPI_BITMASK flags )
{
	CSPI_LOG("%s(%p, %p, %llu)", __FUNCTION__, h, p, flags);

	if ( !is_hcon(h) ) return CSPI_E_INVALID_HANDLE;
	if ( !p ) return CSPI_E_INVALID_PARAM;

	Connection *con =  (Connection*) h;
	if ( -1 == con->fd ) return CSPI_E_SEQUENCE;	// Connect first!

	return custom_setconparam( con, p, flags );
}

//--------------------------------------------------------------------------

int base_getconparam( Connection *con, CSPI_CONPARAMS *p, CSPI_BITMASK flags )
{
	ASSERT(con);
	ASSERT(p);

	if ( !flags ) return CSPI_OK;

	// Handle base CSPI_CONPARAMS structure.
	if ( flags & CSPI_CON_MODE ) p->mode = con->mode;
	if ( flags & CSPI_CON_HANDLER ) p->handler = con->handler;
	if ( flags & CSPI_CON_USERDATA ) p->user_data = con->user_data;
	if ( flags & CSPI_CON_EVENTMASK ) p->event_mask = con->event_mask;

	return CSPI_OK;
}

//--------------------------------------------------------------------------

int cspi_connect( CSPIHCON h )
{
	CSPI_LOG("%s(%p)", __FUNCTION__, h);

	if ( !is_hcon(h) ) return CSPI_E_INVALID_HANDLE;

	Connection *p = (Connection *) h;
	if ( -1 != p->fd ) return CSPI_E_SEQUENCE;	// Already connected?

	if ( CSPI_MODE_UNKNOWN == p->mode ) return CSPI_E_INVALID_MODE;

	int fd = open( get_devicename( p->mode ), O_RDONLY );
	if ( -1 == fd ) return CSPI_E_SYSTEM;

	p->fd = fd;
	memset( &p->timestamp, 0, sizeof(CSPI_TIMESTAMP) );

	return CSPI_OK;
}

//--------------------------------------------------------------------------

int cspi_disconnect( CSPIHCON h )
{
	CSPI_LOG("%s(%p)", __FUNCTION__, h);

	if ( !is_hcon(h) ) return CSPI_E_INVALID_HANDLE;
	Connection *p = (Connection *) h;

	if ( -1 != p->fd ) {
		if ( -1 == close( p->fd ) ) return CSPI_E_SYSTEM;
		p->fd = -1;
	}
	p->mode = CSPI_MODE_UNKNOWN;

	return CSPI_OK;
}

//--------------------------------------------------------------------------

const char* get_devicename( int mode )
{
	// Note: Must match the order defined by CSPI_MODE.
	static char *devicename[] = {
		"notused",
		"/dev/libera.dd",
		"/dev/libera.sa",
		"/dev/libera.pm",
		"/dev/libera.adc",
		"/dev/libera.dd",
		"/dev/libera.adc",
		"/dev/libera.adc",
		"/dev/libera.adc"
	};

	ASSERT(mode > 0 && mode < sizeof(devicename)/sizeof(char*));
	return devicename[ mode ];
}

//--------------------------------------------------------------------------

int cspi_seek( CSPIHCON h, unsigned long long *offset, int origin )
{
	CSPI_LOG("%s(%p, %p, %d)", __FUNCTION__, h, offset, origin);

	if ( !is_hcon(h) ) return CSPI_E_INVALID_HANDLE;
	if ( 0 == offset ) return CSPI_E_INVALID_PARAM;

	if ( CSPI_SEEK_MT != origin &&
	     CSPI_SEEK_ST != origin &&
	     CSPI_SEEK_TR != origin ) return CSPI_E_INVALID_PARAM;

	Connection *p = (Connection*) h;
	if ( -1 == p->fd ) return CSPI_E_SEQUENCE;	// Not connected?

	// Must be a DD connection!
	if ( (CSPI_MODE_DD!=p->mode) && (CSPI_MODE_AVERAGE!=p->mode) )
		return CSPI_E_ILLEGAL_CALL;

	if ( (off_t)-1 != lseek( p->fd, *offset, origin ) ) return CSPI_OK;

	return CSPI_E_SYSTEM;
}

//--------------------------------------------------------------------------

int cspi_read( CSPIHCON h,
               void *dest, size_t count,
               size_t *nread )
{
	CSPI_LOG("%s(%p, %p, %u, %p)", __FUNCTION__, h, dest, count, nread);

	return cspi_read_ex( h, dest, count, nread, custom_getdefaultop(h) );
}

//--------------------------------------------------------------------------

int cspi_read_ex( CSPIHCON h,
                  void *dest, size_t count,
                  size_t *nread,
                  CSPI_AUX_FNC op  )
{
	CSPI_LOG("%s(%p, %p, %u, %p, %p)", __FUNCTION__, h, dest, count, nread, op);

	if ( !is_hcon(h) ) return CSPI_E_INVALID_HANDLE;
	if ( !dest ) return CSPI_E_INVALID_PARAM;

	Connection *p = (Connection*) h;
	if ( -1 == p->fd ) return CSPI_E_SEQUENCE;	// Not connected?

	// Must be a non-streaming mode!
	if ( is_streamingmode( p->mode ) ) return CSPI_E_ILLEGAL_CALL;

	if ( ( CSPI_MODE_ADC == p->mode ) ||
	     ( CSPI_MODE_ADC_CW == p->mode ) ||
	     ( CSPI_MODE_ADC_SP == p->mode ) ||
	     ( CSPI_MODE_ADC_SP_ROT == p->mode ) )
	     return read_adc(p, dest, count, nread, op);

	if ( CSPI_MODE_AVERAGE == p->mode )
		return read_avg(p, dest, nread, op);

	ASSERT(CSPI_MODE_DD == p->mode || CSPI_MODE_PM == p->mode );
	return read_dd(p, dest, count, nread, op);
}

//--------------------------------------------------------------------------

int read_avg( Connection *p,
              void *dest,
              size_t *nread,
              CSPI_AUX_FNC op )
{
	ASSERT(p);
	ASSERT(dest);

	CSPI_LOG("%s(%p, %p, %u, %p)", __FUNCTION__, p, dest, nread, op);

	int rc;
	const size_t atomsize = sizeof(CSPI_DD_RAWATOM);
	CSPI_DD_RAWATOM atom;
	CSPI_AVERAGE_ATOM *pdata = (CSPI_AVERAGE_ATOM*)dest;
	// The number of bytes to retrieve.
	*nread = 0;

	// dd device requires this three lines
	unsigned long long offset = 0ULL;
	rc = cspi_seek(p, &offset, CSPI_SEEK_TR);
	if (CSPI_OK != rc) {
		CSPI_ERR("%s: cspi_seek error: %d, errno: %d", __FUNCTION__, rc, errno);
		return rc;
	}

	ssize_t nb = read( p->fd, &atom, atomsize );
	if ( -1 == nb ) {
		CSPI_ERR("%s: read error, errno: %d", __FUNCTION__, errno);
		return CSPI_E_SYSTEM;
	}

	rc = ioctl( p->fd, LIBERA_IOC_GET_DD_TSTAMP, &p->timestamp );
	if ( -1 == rc ) {
		CSPI_ERR("%s: ioctl error, errno: %d", __FUNCTION__, errno);
		return CSPI_E_SYSTEM;
	}

	CSPI_ENVPARAMS params;
	const CSPI_BITMASK mask = CSPI_ENV_AVERAGE_SUM;

	rc = cspi_getenvparam(p->environment, &params, mask);
	if (CSPI_OK != rc) return rc;

	*nread = 1;
	pdata->avesum = params.average_sum;
	if (op) {

		if ( (rc=custom_initop()) != CSPI_OK ) return rc;
		op( dest, dest, 1 );
	}

	return rc;
}

//--------------------------------------------------------------------------

int read_dd( Connection *p,
             void *dest, size_t count,
             size_t *nread,
             CSPI_AUX_FNC op  )
{
	ASSERT(p);
	ASSERT(dest);

	CSPI_LOG("%s(%p, %p, %u, %p, %p)", __FUNCTION__, p, dest, count, nread, op);

	const size_t atomsize = sizeof(CSPI_DD_RAWATOM);
	// The number of bytes to retrieve.
	const size_t nbytes = count * atomsize;

	ssize_t nb = read( p->fd, dest, nbytes );
	if ( -1 == nb ) {
		CSPI_ERR("%s: read error, errno: %d", __FUNCTION__, errno);
		return CSPI_E_SYSTEM;
	}

	ASSERT(CSPI_MODE_DD == p->mode || CSPI_MODE_PM == p->mode);

	const int cd = CSPI_MODE_DD == p->mode ?
		LIBERA_IOC_GET_DD_TSTAMP : LIBERA_IOC_GET_PM_TSTAMP;

	int rc = ioctl( p->fd, cd, &p->timestamp );
	if ( -1 == rc ) return CSPI_E_SYSTEM;

	// Read may return less than the requested number
	// of bytes (History Buffer overrun).
	nb /= atomsize;
	if (nread) *nread = nb;

	size_t n = 0;	// Number of atoms left to process.
	if (op) {

		if ( (rc=custom_initop()) != CSPI_OK ) return rc;

		CSPI_DD_ATOM *p2 = dest;
		ASSERT(sizeof(CSPI_DD_RAWATOM) == sizeof(CSPI_DD_ATOM));

		// Apply auxiliary operator to each atom.
		op(p2, p2, (size_t)nb);
	}

	// Not completed if not enough atoms or atoms left to process.
	rc = (nb!=count || n) ? CSPI_W_INCOMPLETE : CSPI_OK;

	return rc;
}

//--------------------------------------------------------------------------

int read_adc( Connection *p,
			  void *dest, size_t count,
			  size_t *nread,
			  CSPI_AUX_FNC op  )
{
	ASSERT(p);
	ASSERT(dest);

	const size_t atomsize = sizeof(CSPI_ADC_ATOM);

	const size_t nbytes = count * atomsize;
	void* buff;
	if ( CSPI_MODE_ADC == p->mode ) {
		buff = dest;
	}
	else {
		buff = calloc( count, atomsize );
		if ( !buff ) return CSPI_E_MALLOC;
	}
	if ( NULL == buff ) return -1; // check return value (out of memory)
	size_t nb = read( p->fd, buff, nbytes );

	int rc = CSPI_E_SYSTEM;
	if ( -1 != nb ) {

		rc = nbytes != nb ? CSPI_W_INCOMPLETE : CSPI_OK;
		size_t eread = nb/atomsize;
		if ( nread ) *nread = eread;

		if (op) {
			
			if ( (rc=custom_initop()) != CSPI_OK ) return rc;
			
			op( buff, dest, eread );
			if (( CSPI_MODE_ADC_SP == p->mode ) ||
				( CSPI_MODE_ADC_SP_ROT == p->mode ))
				if ( nread ) *nread = 1; // one atom returned
		}
	}

	if ( CSPI_MODE_ADC != p->mode ) {
		free( buff );
	}

	return rc;
}

//--------------------------------------------------------------------------

int cspi_get( CSPIHCON h, void *atom )
{
	CSPI_LOG("%s(%p, %p)", __FUNCTION__, h, atom);

	if ( !is_hcon(h) ) return CSPI_E_INVALID_HANDLE;
	if ( !atom ) return CSPI_E_INVALID_PARAM;

	Connection *p = (Connection*) h;
	if ( -1 == p->fd ) return CSPI_E_SEQUENCE;	// Not connected?

	// Must be an SA connection!
	if ( CSPI_MODE_SA != p->mode ) return CSPI_E_ILLEGAL_CALL;

	ssize_t nb = read( p->fd, atom, sizeof(CSPI_SA_ATOM) );
	if ( -1 == nb ) return CSPI_E_SYSTEM;

	ASSERT( sizeof(CSPI_SA_ATOM) == nb );
	CSPI_AUX_FNC op = custom_getdefaultop(h);

	if (op) op( atom, atom, 1 );
	return CSPI_OK;
}

//--------------------------------------------------------------------------

int cspi_gettimestamp( CSPIHCON h, CSPI_TIMESTAMP *ts )
{
	CSPI_LOG("%s(%p, %p)", __FUNCTION__, h, ts);

	if ( !is_hcon(h) ) return CSPI_E_INVALID_HANDLE;
	if ( !ts ) return CSPI_E_INVALID_PARAM;

	Connection *p = (Connection*) h;
	// Assume a valid timestamp struct has ST > 0.
	if ( -1 == p->fd || 0 >= p->timestamp.st.tv_sec ) return CSPI_E_SEQUENCE;

	// Must be a DD or average connection!
	// average mode temporary uses DD connection too for reading timestamp - should be changed
	if ( (CSPI_MODE_DD!=p->mode) && (CSPI_MODE_PM!=p->mode) && (CSPI_MODE_AVERAGE!=p->mode) )
		return CSPI_E_ILLEGAL_CALL;

	memcpy( ts, &p->timestamp, sizeof(p->timestamp) );
	return CSPI_OK;
}

//--------------------------------------------------------------------------

int cspi_settime( CSPIHENV h, CSPI_SETTIMESTAMP *ts, CSPI_BITMASK flags )
{
	CSPI_LOG("%s(%p, %p, %llu)", __FUNCTION__, h, ts, flags);

	// Not used: if ( !is_henv(h) ) return CSPI_E_INVALID_HANDLE;
	if ( !ts ) return CSPI_E_INVALID_PARAM;
	if (!flags) return CSPI_OK;

	int fd = open( LIBERA_EVENT_FIFO_PATHNAME, O_RDONLY );
	if ( -1 == fd ) return CSPI_E_SYSTEM;

	int rc = 0;

	if ( flags & CSPI_TIME_MT )
		rc = ioctl( fd, LIBERA_EVENT_SET_MT, ts );

	if ( -1 != rc &&  (flags & CSPI_TIME_ST) )
		rc = ioctl( fd, LIBERA_EVENT_SET_ST, ts );

	close(fd);
	return 0 == rc ? CSPI_OK : CSPI_E_SYSTEM;
}

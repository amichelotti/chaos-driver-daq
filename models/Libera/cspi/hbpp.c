// $Id: hbpp.c 2192 2008-10-07 09:13:06Z matejk $

//! \file hbpp.c
//! Hadron Beam Position Processor (HBPP) specific module.

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

#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <sys/ioctl.h>

#include "cspi.h"
#include "cspi_impl.h"

#include "hbpp.h"

//--------------------------------------------------------------------------

void signal_handler_hook( const CSPI_EVENTHDR *p ) { /*not used*/ }

//--------------------------------------------------------------------------

int custom_initenv( CSPIHANDLE h )
{
	// Call base version only!
	return base_initenv(h);
}

//--------------------------------------------------------------------------

int custom_initcon( CSPIHANDLE h, CSPIHANDLE p )
{
	// Call base version only!
	return base_initcon( h, p );
}

//--------------------------------------------------------------------------

int custom_setenvparam( Environment *e, const CSPI_ENVPARAMS *p,
                        CSPI_BITMASK flags )
{
	// Call base version only!
	return base_setenvparam( e, p, flags );
}

//--------------------------------------------------------------------------

int custom_getenvparam( Environment *e, CSPI_ENVPARAMS *p, CSPI_BITMASK flags )
{
	// Call base version only!
	return base_getenvparam( e, p, flags );
}

//--------------------------------------------------------------------------

int custom_setconparam( Connection *con, const CSPI_CONPARAMS *p, CSPI_BITMASK flags )
{
	// Call base version only!
	return base_setconparam( con, p, flags );
}

//--------------------------------------------------------------------------

int custom_getconparam( Connection *con, CSPI_CONPARAMS *p, CSPI_BITMASK flags )
{
	// Call base version only!
	return base_getconparam( con, p, flags );
}

//--------------------------------------------------------------------------

CSPI_AUX_FNC custom_getdefaultop( const Connection *p )
{
	return 0;
}

//--------------------------------------------------------------------------

int custom_initop() { /*not used*/ return CSPI_OK; }

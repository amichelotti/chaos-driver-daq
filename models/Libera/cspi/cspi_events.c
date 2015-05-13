// $Id: cspi.c 2272 2008-11-04 22:04:50Z hsljurijb $

//! \file cspi_events.c
//! Implements event hadling for cspi library

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
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

#include "eventd.h"
#include "debug.h"

#include "cspi.h"
#include "cspi_impl.h"

//--------------------------------------------------------------------------

volatile sig_atomic_t termination = 0;

struct event_thread_args {
	pid_t pid;
	int fd;
};

int create_event_pipe(int pid )
{
	CSPI_LOG("%s", __FUNCTION__);

	char fifo_name[32];
	int fd;

	sprintf( fifo_name, EVENT_FIFO_PID_NAME, pid );
	if ( 0 != mkfifo( fifo_name, 0666 ) && EEXIST != errno) {
		CSPI_ERR("%s - fifo: %s, errno: %d", __FUNCTION__, fifo_name, errno);
		return -1;
	}

	CSPI_LOG("Created event fifo: %s", fifo_name);

	fd = open( fifo_name, O_RDONLY | O_NONBLOCK);
	if( -1 == fd ) {
		CSPI_ERR("%s - Failed to open fifo in read mode.", __FUNCTION__);
	}
	else {
		CSPI_LOG("Opened read handle to event fifo.");
	}

	return fd;
}

void *event_thread( void *args )
{
	_LOG_DEBUG("%s", __FUNCTION__);

	pid_t pid;
	int fdr, fdw;
	fd_set rfds;
	int exit = 0;
	int nread, nleft = 0;
	char fifo_name[32];
	struct timeval tv;
	int rc;

	pid = ((struct event_thread_args *)args)->pid;
	fdr = ((struct event_thread_args *)args)->fd;
	free(args);

	sprintf( fifo_name, EVENT_FIFO_PID_NAME, pid );
	fdw = open( fifo_name, O_WRONLY|O_NONBLOCK );
	if( -1 == fdw ) {
		CSPI_ERR("%s - Failed to open fifo in write mode.", __FUNCTION__);
		goto event_thread_close_reader;
	}

	_LOG_DEBUG("%s: Opened write handle to event fifo.", __FUNCTION__);

	while ( !termination ) {

		FD_ZERO( &rfds );
		FD_SET( fdr, &rfds );
		tv.tv_sec = 1;
		tv.tv_usec = 0;

		int rc = select( fdr+1, &rfds, 0, 0, &tv );

		if ( termination )	// break;
			break;

		if ( 0 == rc )	// timeout
			continue;

		if ( -1 == rc ) {

			if ( EINTR == errno ) {

				exit = 1;
			}
			else {

				continue;
			}
		}

		if ( FD_ISSET( fdr, &rfds ) ) {

			_LOG_DEBUG("%s: Read from event fifo.", __FUNCTION__);
			CSPI_EVENTHDR eventhdr;
			CSPI_EVENT msg;
			unsigned char *p;

			do {

				nleft = sizeof(CSPI_EVENTHDR);
				p = (unsigned char*)&eventhdr;
				do {

					nread = read( fdr, p, nleft );

					if ( -1 == nread ) {

						switch( errno ) {

						case EAGAIN:
							continue;
						case EINTR:
							exit = 1;
							break;
						default:
							break;
						}
					}
					else {

						p += nread;
						nleft -= nread;
					}
				}
				while( (nleft > 0) && (nread > 0) && !termination );

				if ( 0 == nleft ) {

					msg.hdr.id = eventhdr.id;
					msg.hdr.param = eventhdr.param;

					signal_handler_hook( &eventhdr );

					// Dispatch notification message to all connections.
					Connection *p = environment.head;
					while ( p ) {

						_LOG_DEBUG("%s: Connection handler: %p, mask: 0x%08x.", __FUNCTION__,
							p->handler, p->event_mask);
						if ( p->handler && 
							((eventhdr.id & p->event_mask) == eventhdr.id) ) {

							msg.user_data = p->user_data;
							if ( 0 == p->handler( &msg ) ) break;
						}
						p = p->next;
					}
				}
			}
			while( (nread > 0) && (0 == exit) && !termination );
		}
	}

	close( fdw );

event_thread_close_reader:

	close( fdr );

	rc = unlink( fifo_name );
	if (-1 == rc)
		_LOG_DEBUG("unlink(%s) failed -- (%d) %s",
			fifo_name, errno, strerror(errno));
	else
		_LOG_DEBUG("Removed event fifo: %s", fifo_name);

	pthread_exit(NULL);

	return NULL;
}

//--------------------------------------------------------------------------

int do_event_handler_registration( int pid, int uid, size_t mask )
{
	_LOG_DEBUG("%s mask:0x%08x", __FUNCTION__, mask);

	int rc = CSPI_OK;
	const Request req = { pid, uid, mask };

	int fd = open( EVENTD_REQ_FIFO_PATHNAME, O_WRONLY );
	if ( -1 == fd ) {

		rc = -1;
	}
	else {

		if ( -1 == (rc = write( fd, &req, sizeof(req) ) ) ) {

			close( fd );
		}
		else {

			rc = close( fd );
		}
	}

	_LOG_DEBUG("%s returning %d", __FUNCTION__, rc);
	
	return rc;
}

//--------------------------------------------------------------------------

int register_event_handler( Connection* p )
{
	return do_event_handler_registration( p->pid, p->connection_id, p->event_mask );
}

//--------------------------------------------------------------------------

int unregister_event_handler( Connection* p )
{
	return do_event_handler_registration( p->pid, p->connection_id, 0 );
}

//--------------------------------------------------------------------------

int create_event_thread( Connection* p )
{
	_LOG_DEBUG("%s", __FUNCTION__);

	if( -1 != environment.evaction ) {

		_LOG_DEBUG("evaction is set: Skipped event thread creation, registering handler.");
		return register_event_handler( p );
	}

	if( (p->event_mask == 0) || (p->handler == 0) ) {
		_LOG_DEBUG("No event thread for mask: 0x%08x, handler: %p",
			p->event_mask, p->handler);
		return CSPI_OK;
	}

	int fd = create_event_pipe(p->pid);
	if ( -1 == fd )
		return CSPI_E_SYSTEM;

	pthread_attr_t attr;
	pthread_attr_init( &attr );
	pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_JOINABLE );
	pthread_attr_setstacksize( &attr, 1024*128 );

	struct event_thread_args *args;
	args = malloc( sizeof(struct event_thread_args) );
	args->pid = p->pid;
	args->fd = fd;

	// Clear termination flag. Will be set in destroy_event_thread.
	termination = 0;

	int rc = pthread_create( &environment.evaction, &attr, event_thread, args );
	pthread_attr_destroy( &attr );

	if ( 0 != rc ) {
		CSPI_ERR("%s - Error creating event thread", __FUNCTION__);
		return CSPI_E_SYSTEM;
	}

	_LOG_DEBUG("Created new event thread.");
	// this sleep helps task manager to switch current process to event thread
	sleep( 0 );
	return register_event_handler( p );
}

//--------------------------------------------------------------------------

void destroy_event_thread( void )
{
	_LOG_DEBUG("%s: evaction = %p", __FUNCTION__, environment.evaction);

	if ( -1 != environment.evaction ) {

		termination = 1;
		pthread_join( environment.evaction, NULL );
		environment.evaction = -1;
	}
	_LOG_DEBUG("%s: Exit.", __FUNCTION__);
}


// $Id: eventd.h 2301 2008-11-18 09:47:01Z tomazb $

//! \file eventd.h
//! Declares interface for CSPI Event Daemon.

#if !defined(_EVENTD_H)
#define _EVENTD_H

#include <sys/types.h>	// defines pid_t

#ifdef __cplusplus
extern "C" {
#endif

/** Event signal. */
#define LIBERA_SIGNAL SIGUSR1

/** Process identifier (PID) pathname. */
#define EVENTD_PID_PATHNAME		"/var/run/leventd.pid"

/** Request FIFO (named pipe) pathname. */
#define EVENTD_REQ_FIFO_PATHNAME	"/tmp/leventd.fifo"

/** Libera event device. */
#define LIBERA_EVENT_FIFO_PATHNAME	"/dev/libera.event"

/** Per process (CSPI connect) event FIFO (named pipe) pathname. */
#define EVENT_FIFO_PID_NAME "/tmp/%d.event"

//--------------------------------------------------------------------------
// Interface.

/** Represents event daemon request. */
typedef struct {
	pid_t pid;
	int uid;
	size_t mask;
}
Request;

/** Typedef. See struct tagListener for more information. */
typedef struct tagListener Listener;	// forward decl

/** Represents a member of the listener list. */
struct tagListener {
	pid_t pid;
	int uid;
	size_t mask;
	Listener *prev, *next;
};

#ifdef __cplusplus
}
#endif
#endif	// _EVENTD_H

// $Id: dscd.h 2429 2009-02-23 15:43:35Z tomazb $

//! \file dscd.h
//! Declares interface for the DSC Daemon.

#if !defined(_DSC_H)
#define _DSC_H

#include <sys/types.h>		// defines pid_t

#ifdef __cplusplus
extern "C" {
#endif

// Idle period in seconds between two iterations.
#define DSCD_ITER_PERIOD	1

// Process identifier (PID) pathname.
#define DSCD_PID_PATHNAME		"/var/run/ldscd.pid"

// Request FIFO (named pipe) pathname.
#define DSCD_FIFO_PATHNAME	"/tmp/ldscd.fifo"

// Libera dsc device filename
#define LIBERA_DSC_DEVICE_FILE	"/dev/libera.dsc"

//--------------------------------------------------------------------------
// Interface.

// server magic numbers
enum {
	DSCD_MAGIC = 90205,
};

// list of server message types
enum {
	DSCD_FIRST = 0,
	DSCD_SET_AGC,
	DSCD_GET_AGC,
	DSCD_SET_DSC,
	DSCD_GET_DSC,
	DSCD_SET_GAIN,
	DSCD_GET_GAIN,
	DSCD_SET_SWITCH,
	DSCD_GET_SWITCH,
	DSCD_GET_FREV,
	DSCD_APPLY_SETTINGS,
	DSCD_LAST
};

// server message
typedef struct {
	// magic number
	size_t magic;
	// request type
	size_t type;
	// request-specific value
	union {
		int ival;
		double dval;
	} val;
	// PID of a process sending the request
	pid_t pid;
	// reply status
	int  status;
}
message;

#ifdef __cplusplus
}
#endif
#endif	// _DSC_H

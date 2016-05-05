/* $Id: lplld.h 2299 2008-11-14 15:38:07Z tomazb $ */

//! \file libera_lplld.h
//! Declares interface for Libera PLL daemon.

/*
LIBERA PLL DAEMONS - Libera GNU/Linux PLL daemons
Copyright (C) 2004 Instrumentation Technologies
Copyright (C) 2006-2007 Michael Abbott, Diamond Light Source Ltd.

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

TAB = 4 spaces.
*/

#if !defined(_LIBERA_LPLLD_H)
#define _LIBERA_LPLLD_H

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
/** LPLLD process identifier (PID) pathname. */
#define LPLLD_PID_PATHNAME_d        "/var/run/lplld.pid"

/** Command FIFO for receiving LPLLD commands. */
#define LPLLD_COMMAND_FIFO_d        "/tmp/lplld.command"

/** Status FIFO for reporting LPLLD status. */
#define LPLLD_STATUS_FIFO_d         "/tmp/lplld.status"
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/** Structure with flags indicating change of particular MT runtime parameter. */
typedef struct _mt_has_changed_tag
{
    char vcxo_offset : 1; ///< RF-VCXO detuning offset has changed.
    char nco_shift : 1; ///< NCO frequency shift flag has changed.
    char phase_offset : 1; ///< VCXO Phase offset has changed.
    char unlock_threshold : 1; ///< Unlock threshold has changed.
    char sync_in : 1; ///< Synchronization input has changed.
} mt_has_changed_t;

/** Structure with MT controller parameters. */
typedef struct _mt_parameters_tag
{
    unsigned long mc_prescale; ///< MC prescaler. Default=MT_DEFAULT_MCPRESC.
    unsigned long u_nominal; ///< DAC nominal offset. Default=MT_DEFAULT_UNOMINAL.
    long vcxo_offset; ///< RF-VCXO detuning offset (*40Hz). Default=MT_DEFAULT_VCXOFF.
    int nco_shift; ///< NCO frequency shift flag (Y=1, N=0). Default=MT_DEFAULT_NCOSHIFT.
    long phase_offset; ///< VCXO phase offset. Default=MT_DEFAULT_PHOFFSET.
    unsigned long harmonic; ///< Harmonic number. Default MT_DEFAULT_HARMONIC.
    unsigned long unlock_threshold; ///< Unlock threshold. Default=MT_DEFAULT_UNLOCK.
    int sync_in; ///< Synchronization input.
    char *plldebug; ///< Pointer to filename path for debug data. Default=NULL.
    mt_has_changed_t has_chngd; ///< Bitset with flags indicating change of particular parameter.
} mt_parameters_t, *mt_parameters_ptr;

/** Structure with current MT status. */
#pragma pack(4)
typedef struct _mt_status_tag
{
    unsigned long seqno; ///< Update status sequence counter;
    unsigned long dac; ///< DAC output value.
    long long phase_error; ///< Phase error.
    unsigned long frequency; ///< Frequency in dHz.
    unsigned long harmonic; ///< Harmonic number.
    int locked_status; ///< Locked flag (0=unlocked, 1=locked).
    int sync_state; ///< Synchronization state. Default=0.
    long vcxo_offset; ///< RF-VCXO detuning offset (*40Hz).
    int nco_shift; ///< NCO frequency shift flag (Y=1, N=0).
    long phase_offset; ///< VCXO phase offset.
    unsigned long unlock_threshold; ///< Unlock threshold.
    int sync_in; ///< Synchronization input.
    int status; ///< Controller internal status. Libera utility "MT controller RtmSts".
} mt_status_t, *mt_status_ptr;
#pragma pack()
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/** Structure with flags indicating change of particular ST runtime parameter. */
typedef struct _st_has_changed_tag
{
    char unlock_threshold : 1; ///< Unlock threshold has changed.
} st_has_changed_t;

/** Structure with ST controller parameters. */
typedef struct _st_parameters_tag
{
    unsigned long u_nominal; ///< DAC nominal offset. Default=ST_DEFAULT_UNOMINAL.
    unsigned long unlock_threshold; ///< Unlock threshold. Default=ST_DEFAULT_UNLOCK.
    char *plldebug; ///< Pointer to filename path for debug data. Default=NULL.
    st_has_changed_t has_chngd; ///< Bitset with flags indicating change of particular parameter.
} st_parameters_t, *st_parameters_ptr;

/** Structure with current ST status. */
#pragma pack(4)
typedef struct _st_status_tag
{
    unsigned long seqno; ///< Update status sequence counter;
    unsigned long dac; ///< DAC output value.
    long long phase_error; ///< Phase error.
    unsigned long frequency; ///< Frequency in dHz.
    int locked_status; ///< Locked flag (0=unlocked, 1=locked).
    int status; ///< Controller internal status. Libera utility "ST controller RtmSts".
    unsigned long unlock_threshold; ///< Unlock threshold.
} st_status_t, *st_status_ptr;
#pragma pack()
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/** Structure with PLL status. This stucture is raw byte stream copied to 
 *  status pipe. The read side of pipe should instantiate this structure directly
 *  from recived raw byte stream.
 *  @note This approach was chosen, because the assumption is that both sides 
 *  of the pipe (write and read) will allways be on the same machine (no machine 
 *  boundaries crossing).
 */
typedef struct _pll_status_tag
{
    unsigned long report_stseqn; ///< Reported status sequence counter;
    mt_status_t mt_stat; ///< MT status;
    st_status_t st_stat; ///< ST status.
} pll_status_t, *pll_status_ptr;
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/** Enumerator values used to define internal status of controller.
 */
typedef enum {
    CTRL_OK = 0, ///< Controller status running and OK.
    CTRL_NOT_INITIALIZED, ///< Controller not yet initialized.
    CTRL_NO_CLOCK, ///< No external clock signal.
    CTRL_DEF_UNLOCK, ///< Phase error too big.
    CTRL_TERMINATED, ///< Controller loop terminated.
} controller_status_enum;

#ifdef __cplusplus
}
#endif
#endif    // _LIBERA_LPLLD_H

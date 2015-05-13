/* $Id: libera.h 2454 2009-03-04 14:47:44Z tomaz.beltram $ */

/** \file libera.h */
/** Public include file for GNU/Linux Libera driver. */

/*
LIBERA - Libera GNU/Linux device driver
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


#ifndef _LIBERA_H_
#define _LIBERA_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <linux/ioctl.h>
#ifndef __KERNEL__
#include <time.h>
#include <limits.h>
#endif

#ifdef __cplusplus
#undef  _IOC_TYPECHECK
#define _IOC_TYPECHECK(t) sizeof(t)
#endif

/** Libera magic number for ioctl() calls */
#define LIBERA_IOC_MAGIC    'l'
#define LIBERA_EVENT_MAGIC  'e'
#define LIBERA_DSC_MAGIC    'd'

/** Libera description */
typedef struct {
    int magic;           //!< Magic number.
    char name[32];       //!< Libera family member name.
    char fpga[32];       //!< FPGA version.
} libera_desc_t;


/** Libera signed 32-bit */
typedef int libera_S32_t;
/** Libera unsigned 32-bit */
typedef unsigned int libera_U32_t;
/** Libera signed 64-bit */
typedef long long libera_S64_t;
/** Libera unsigned 64-bit */
typedef unsigned long long libera_U64_t;
/** Libera 64-bit time storage type. Used for (L)MT & (L)ST */
typedef unsigned long long libera_hw_time_t; 
/** Libera timing pair, LMT & LST */
typedef struct 
{
    libera_hw_time_t lst;  //!< Libera System Time
    libera_hw_time_t lmt;  //!< Libera Machine Time
} libera_Ltimestamp_t;
/** Libera userland timing pair, MT & ST */
typedef struct 
{
    struct timespec  st;   //!< System Time
    libera_hw_time_t mt;   //!< Machine Time
} libera_timestamp_t;
/** Libera High resolution userland timing pair, MT + D & ST */
#pragma pack(4) // used by generic server on the other side
typedef struct 
{
    struct timespec  st;    //!< System Time
    libera_hw_time_t mt;    //!< Machine Time
    unsigned long    phase; //!< LMT phase (0...D-1)
} libera_HRtimestamp_t;
#pragma pack()

/** Libera event structure */
typedef struct 
{
    int id;                //!< Event ID
    int param;             //!< Event specific parameter
} libera_event_t;

/** Helper macro for events & event masks */
#define EVENT(n)  (1 << n)
/** Event IDs to be used in libera_event_t.id */
typedef enum
{
    LIBERA_EVENT_USER       = UINT_MAX/2, //!< User defined event
    LIBERA_EVENT_OVERFLOW   = EVENT(0),   //!< Fifo overflow
    LIBERA_EVENT_CFG        = EVENT(1),   //!< Configuration change
    LIBERA_EVENT_SA         = EVENT(2),   //!< Slow Acq. sample available
    LIBERA_EVENT_INTERLOCK  = EVENT(3),   //!< Interlock fired
    LIBERA_EVENT_PM         = EVENT(4),   //!< Post Mortem trigger
    LIBERA_EVENT_FA         = EVENT(5),   //!< Fast Application trigger
    LIBERA_EVENT_TRIGGET    = EVENT(6),   //!< GET Trigger trigger
    LIBERA_EVENT_TRIGSET    = EVENT(7),   //!< SET Trigger trigger
} libera_event_id_t;

/** Libera event specific parameter values. */
typedef enum
{
    /* OVERFLOW */
    LIBERA_OVERFLOW_DD_FPGA = 0, //!< Data on demand fifo overflow in FPGA
    LIBERA_OVERFLOW_SA_FPGA,     //!< Slow Acq. fifo overflow in FPGA
    LIBERA_OVERFLOW_SA_DRV,      //!< Slow Acq. fifo overflow in driver
    /* FA */
    LIBERA_TRIG_FA_MC0,          //!< FA MC trigger #0
    LIBERA_TRIG_FA_MC1,          //!< FA MC trigger #1
    LIBERA_TRIG_FA_SC0,          //!< FA SC trigger #0
    LIBERA_TRIG_FA_SC1,          //!< FA SC trigger #1
} libera_event_param_t;

/** Libera interlock parameter values. */
typedef enum
{
    LIBERA_INTERLOCK_X    = (1<<0), //!< IL position X out of limit
    LIBERA_INTERLOCK_Y    = (1<<1), //!< IL position Y out of limit
    LIBERA_INTERLOCK_ATTN = (1<<2), //!< Attenuators set higher than predefined value
    LIBERA_INTERLOCK_ADCF = (1<<3), //!< ADC Overflow  (filtered)
    LIBERA_INTERLOCK_ADC  = (1<<4), //!< ADC Overflow  (not filtered)
} libera_interlock_param_t;

/** Available modes of operation. */
// TOOD: Possibly remove from the driver
typedef enum {
	LIBERA_MODE_UNKNOWN = 0,	//!< Libera mode unknown or not set.
	LIBERA_MODE_DD,				//!< Data-on-demand mode.
	LIBERA_MODE_SA,				//!< Slow Acquisition mode.
	LIBERA_MODE_PM,				//!< Post-Mortem mode.
	LIBERA_MODE_ADC,			//!< ADC-rate buffer mode.
	LIBERA_MODE_AVERAGE,		//!< calc average value between two triggers
	LIBERA_MODE_ADC_CW,			//!< ADC-rate buffer mode CW.
	LIBERA_MODE_ADC_SP,			//!< ADC-rate buffer mode SP.
	LIBERA_MODE_ADC_SP_ROT,		//!< ADC-rate buffer mode SP w/ -45deg rotation.
	LIBERA_MODE_LAST			//!< last item
} LIBERA_MODE;

/** Available trigger modes. */
typedef enum {
	LIBERA_TRIGMODE_UNKNOWN = 0,	//!< Unknown trigger mode.
	LIBERA_TRIGMODE_GET,	        //!< Get trigger mode.
	LIBERA_TRIGMODE_SET,	        //!< Set trigger mode.
} LIBERA_TRIGMODE;


/* LIBERA ioctl() command identifiers */
/* NOTE: The ordinal numbers (2nd paramter to _IO* IOCTL macros) 
 *       are divided into subsets corresponding to each set/group of
 *       LIBERA commands. The upper bit (MSB) is group/subset bit and 
 *       the rest 7 bits are sequential number bits. This gives us room for
 *       hosting 128 commands per group.
 *       GET_ commands MUST use _IOR macro.
 *       SET_ commands MUST use _IOW macro.
 */
#define LIBERA_IOC_IS_SET_METHOD(number) ((number) & 0x01)
#define LIBERA_IOC_IS_GET_METHOD(number) (!((number) & 0x01))
#define LIBERA_IOC_MASK  0xE0 /* 3 MSB bits                       */
#define LIBERA_IOC_CFG      0 /* Common Configuration Parameters  */
#define LIBERA_IOC_SA      32 /* Slow Acquisition Parameters      */
#define LIBERA_IOC_FA      64 /* Fast Application Parameters      */
#define LIBERA_IOC_DD      96 /* Data on Demand Parameters */
#define LIBERA_IOC_PM     128 /* Post Mortem Parameters */
#define LIBERA_IOC_DSC    160 /* DSC Parameters */


/* Libera CFG device parameter IOC tags */
typedef enum
{
    LIBERA_CFG_REQUEST = LIBERA_IOC_CFG,
    LIBERA_CFG_MAGIC,
    LIBERA_CFG_DESC,
#ifdef DEBUG
    LIBERA_CFG_MODULERESET,
#endif
} libera_cfg_tags_t;

/* Libera SA device parameter IOC tags */
typedef enum
{
    LIBERA_SA_NONE = LIBERA_IOC_SA,
} libera_sa_tags_t;

/* Libera FA device parameter IOC tags */
typedef enum
{
    LIBERA_FA_NONE = LIBERA_IOC_FA,
} libera_fa_tags_t;

/* Libera DD device parameter IOC tags */
typedef enum
{
    LIBERA_DD_DEC = LIBERA_IOC_DD,
    LIBERA_DD_TSTAMP,
} libera_dd_tags_t;

/* Libera PM device parameter IOC tags */
typedef enum
{
    LIBERA_PM_TSTAMP = LIBERA_IOC_PM,
} libera_pm_tags_t;


typedef struct
{
    unsigned int idx;
    unsigned int val;
} libera_cfg_request_t;


/** Configuration Parameters, common to all Libera members */
typedef enum {
    /** Trigger mode (set, get, ...) */
    LIBERA_CFG_TRIGMODE = 0,
    /** MC PLL status */
    LIBERA_CFG_MCPLL,
    /** SC PLL status */
    LIBERA_CFG_SCPLL,
    /** Customer Feature Register */
    LIBERA_CFG_FEATURE_CUSTOMER,
    /** ITECH Feature Register */
    LIBERA_CFG_FEATURE_ITECH,
    /** First custom (Libera member specific) parameter */
    LIBERA_CFG_CUSTOM_FIRST = 128,
} LIBERA_CFG_COMMON;

/** Feature detection using ioctl(CFG_GET, LIBERA_CFG_FEATURE_ITECH) */
#define LIBERA_INSTRUMENT_BPM      (0x00)
#define LIBERA_INSTRUMENT_BBB      (0x01)
#define LIBERA_INSTRUMENT_HDR      (0x10)

#define LIBERA_TYPE_ELECTRON       (0x00)
#define LIBERA_TYPE_BRILLIANCE     (0x01)

#define LIBERA_TYPE_BBBQ           (0x00)
#define LIBERA_TYPE_BBBS           (0x01)

#define LIBERA_INSTRUMENT(_cfg)    ((_cfg & (0xF << 28)) >> 28)
#define LIBERA_INS_TYPE(_cfg)      ((_cfg & (0xF << 24)) >> 24)
#define LIBERA_ATOM_SIZE(_cfg)     ((_cfg & (0xF << 20)) >> 20)

#define LIBERA_IS_BPM(_cfg)	   (LIBERA_INSTRUMENT(_cfg) == LIBERA_INSTRUMENT_BPM)
#define LIBERA_IS_BBB(_cfg)	   (LIBERA_INSTRUMENT(_cfg) == LIBERA_INSTRUMENT_BBB)

#define LIBERA_IS_BRILLIANCE(_cfg) (LIBERA_IS_BPM(_cfg) && LIBERA_INS_TYPE(_cfg) == LIBERA_TYPE_BRILLIANCE)

#define LIBERA_IS_BBBQ(_cfg) (LIBERA_IS_BBB(_cfg) && LIBERA_INS_TYPE(_cfg) == LIBERA_TYPE_BBBQ)
#define LIBERA_IS_BBBS(_cfg) (LIBERA_IS_BBB(_cfg) && LIBERA_INS_TYPE(_cfg) == LIBERA_TYPE_BBBS)

#define LIBERA_IS_DCC(_cfg) (LIBERA_IS_BPM(_cfg) && ((_cfg & 0xc0) == 0x40))
#define LIBERA_IS_MAF(_cfg) (LIBERA_IS_BPM(_cfg) && ((_cfg & 0x30) == 0x10))
#define LIBERA_IS_GBETHERNET(_cfg) (LIBERA_IS_BPM(_cfg) && ((_cfg & 8) == 8))
#define LIBERA_IS_GBE_DEMO(_cfg) (LIBERA_IS_BPM(_cfg) && ((_cfg & 4) == 4))
#define LIBERA_IS_DESY_MOLEX(_cfg) (LIBERA_IS_BPM(_cfg) && ((_cfg & 2) == 2))
#define LIBERA_IS_GROUPING(_cfg) (LIBERA_IS_BPM(_cfg) && ((_cfg & 1) == 1))

enum libera_ioc_ids_t
{
    /***********************************/
    /* Common Configuration Parameters */
    /***********************************/
    /* GENERIC: Generic, nameless CFG parameters */
    LIBERA_IOC_GET_CFG       = _IOWR(LIBERA_IOC_MAGIC,
				     LIBERA_CFG_REQUEST,
				     libera_cfg_request_t),
    LIBERA_IOC_SET_CFG       = _IOW(LIBERA_IOC_MAGIC,
				    LIBERA_CFG_REQUEST,
				    libera_cfg_request_t),
    /* MAGIC: Libera magic number */
    LIBERA_IOC_GET_MAGIC     = _IOR(LIBERA_IOC_MAGIC,
				    LIBERA_CFG_MAGIC, int),
    /* DESC: Libera description */
    LIBERA_IOC_GET_DESC      = _IOR(LIBERA_IOC_MAGIC,
				    LIBERA_CFG_DESC, libera_desc_t),
    
    
    /*******************************/
    /* Slow Acquisition Parameters */
    /*******************************/
    /* NONE */
    
    /*******************************/
    /* Fast Acquisition Parameters */
    /*******************************/
    /* NONE */    
    
    /*****************************/
    /* Data on Demand Parameters */
    /*****************************/
    /* DEC: Decimation */
    LIBERA_IOC_GET_DEC       = _IOR(LIBERA_IOC_MAGIC, 
				    LIBERA_DD_DEC, libera_U32_t),
    LIBERA_IOC_SET_DEC       = _IOW(LIBERA_IOC_MAGIC,
				    LIBERA_DD_DEC, libera_U32_t),
    /* DD_TSTAMP: DD data timestamp */
    LIBERA_IOC_GET_DD_TSTAMP = _IOR(LIBERA_IOC_MAGIC, 
				    LIBERA_DD_TSTAMP, 
				    libera_timestamp_t),

    /*****************************/
    /* Post Mortem Parameters */
    /*****************************/
    /* PM_TSTAMP: PM data timestamp */
    LIBERA_IOC_GET_PM_TSTAMP = _IOR(LIBERA_IOC_MAGIC, 
				    LIBERA_PM_TSTAMP, 
				    libera_timestamp_t),

     /* All DEBUG IDs have to be declared at the bottom of enum! */
    
};


/**********************/
/* Event device IOCTL */
/**********************/
typedef enum
{
    LIBERA_EVENT_DAC_A,
    LIBERA_EVENT_DAC_B,
    LIBERA_EVENT_SC_TRIG,
    LIBERA_EVENT_MC_TRIG,
    LIBERA_EVENT_ST,
    LIBERA_EVENT_MT,
    LIBERA_EVENT_SC_EVENT,
    LIBERA_EVENT_SC_TRIGGER_19,
    LIBERA_EVENT_SC_TRIGGER_10,
    LIBERA_EVENT_SC_TRIGGER_9,
    LIBERA_EVENT_MC_TRIGGER_19,
    LIBERA_EVENT_MC_TRIGGER_10,
    LIBERA_EVENT_FLMC,
    LIBERA_EVENT_CTIME,
    LIBERA_EVENT_TRIG_TRIGGER,
    LIBERA_EVENT_TRIGGER_BLOCK,
    LIBERA_EVENT_MASK,
    LIBERA_EVENT_FLUSHING,
    LIBERA_EVENT_PMBUF,
    LIBERA_EVENT_MCPHI,
    LIBERA_EVENT_SCPHI,
    LIBERA_EVENT_MC_TRIGGER_1,
    LIBERA_EVENT_MC_TRIGGER_0,
    LIBERA_EVENT_NCO,
    LIBERA_EVENT_MCPLL,
    LIBERA_EVENT_SCPLL,
    LIBERA_EVENT_FIFOLEN_SC,
    LIBERA_EVENT_FIFOLEN_MC,

    /* All DEBUG IDs have to be declared at the bottom of enum! */
#ifdef DEBUG
    LIBERA_EVENT_PEEK_POKE,
    LIBERA_EVENT_EVENTSIM,
    LIBERA_EVENT_OFFPLL,
#endif
} libera_event_tags_t;

#ifdef DEBUG
typedef struct 
{
    unsigned long offset;
    unsigned long value;
} libera_peek_poke_t;
#endif

enum libera_event_ids_t
{    
    LIBERA_EVENT_SET_DAC_A       =  _IOW(LIBERA_EVENT_MAGIC,
					 LIBERA_EVENT_DAC_A,
					 libera_S32_t),
    
    LIBERA_EVENT_SET_DAC_B       =  _IOW(LIBERA_EVENT_MAGIC,
					 LIBERA_EVENT_DAC_B,
					 libera_S32_t),
    
    LIBERA_EVENT_SET_SC_EVENT    =  _IOW(LIBERA_EVENT_MAGIC,
					 LIBERA_EVENT_SC_EVENT,
					 libera_S32_t),
    
    LIBERA_EVENT_ENABLE_SC_TRIG  =  _IOW(LIBERA_EVENT_MAGIC,
					 LIBERA_EVENT_SC_TRIG,
					 libera_S32_t),
    
    LIBERA_EVENT_ENABLE_MC_TRIG  =  _IOW(LIBERA_EVENT_MAGIC,
					 LIBERA_EVENT_MC_TRIG,
					 libera_S32_t),
    
    LIBERA_EVENT_GET_SC_TRIGGER_19= _IOR(LIBERA_EVENT_MAGIC, 
					 LIBERA_EVENT_SC_TRIGGER_19,
					 libera_hw_time_t),
    
    LIBERA_EVENT_GET_MC_TRIGGER_19= _IOR(LIBERA_EVENT_MAGIC, 
					 LIBERA_EVENT_MC_TRIGGER_19,
					 libera_hw_time_t),
    
    LIBERA_EVENT_GET_SC_TRIGGER_10= _IOR(LIBERA_EVENT_MAGIC, 
					 LIBERA_EVENT_SC_TRIGGER_10,
					 libera_hw_time_t),
    
    LIBERA_EVENT_GET_SC_TRIGGER_9=  _IOR(LIBERA_EVENT_MAGIC, 
					 LIBERA_EVENT_SC_TRIGGER_9,
					 libera_hw_time_t),
    
    LIBERA_EVENT_GET_MC_TRIGGER_10= _IOR(LIBERA_EVENT_MAGIC, 
					 LIBERA_EVENT_MC_TRIGGER_10,
					 libera_hw_time_t),

    /* ST: System Time */
    LIBERA_EVENT_GET_ST            = _IOR(LIBERA_EVENT_MAGIC,
					  LIBERA_EVENT_ST,
					  struct timespec),
    LIBERA_EVENT_SET_ST            = _IOW(LIBERA_EVENT_MAGIC,
					  LIBERA_EVENT_ST,
					  libera_HRtimestamp_t),
    
    /* MT: Machine Time */
    LIBERA_EVENT_GET_MT            = _IOR(LIBERA_EVENT_MAGIC,
					  LIBERA_EVENT_MT, 
					  libera_hw_time_t),
    LIBERA_EVENT_SET_MT            = _IOW(LIBERA_EVENT_MAGIC,
					  LIBERA_EVENT_MT,
					  libera_HRtimestamp_t),
    
    /* Transfer of measured MC frequency f_MC */
    LIBERA_EVENT_GET_FLMC          = _IOR(LIBERA_EVENT_MAGIC,
					  LIBERA_EVENT_FLMC,
					  libera_U32_t),
    LIBERA_EVENT_SET_FLMC          = _IOW(LIBERA_EVENT_MAGIC,
					  LIBERA_EVENT_FLMC,
					  libera_U32_t),
    
    LIBERA_EVENT_GET_MCPHI         = _IOR(LIBERA_EVENT_MAGIC,
					  LIBERA_EVENT_MCPHI,
					  libera_hw_time_t),

    LIBERA_EVENT_SET_MCPHI        = _IOW(LIBERA_EVENT_MAGIC,
					  LIBERA_EVENT_MCPHI,
					  libera_hw_time_t),

    LIBERA_EVENT_GET_SCPHI         = _IOR(LIBERA_EVENT_MAGIC,
					  LIBERA_EVENT_SCPHI,
					  libera_hw_time_t),

    LIBERA_EVENT_SET_SCPHI        = _IOW(LIBERA_EVENT_MAGIC,
					  LIBERA_EVENT_SCPHI,
					  libera_hw_time_t),

    /* Current Libera time */
    LIBERA_EVENT_GET_CTIME         = _IOR(LIBERA_EVENT_MAGIC,
					  LIBERA_EVENT_CTIME,
					  libera_Ltimestamp_t),
    
    /* Libera time of the last TRIGGER trigger */
    LIBERA_EVENT_GET_TRIG_TRIGGER  = _IOR(LIBERA_EVENT_MAGIC,
					  LIBERA_EVENT_TRIG_TRIGGER, 
					  libera_Ltimestamp_t),
    
    LIBERA_EVENT_GET_MASK          = _IOR(LIBERA_EVENT_MAGIC,
					 LIBERA_EVENT_MASK, 
					 libera_U32_t),
    
    LIBERA_EVENT_SET_MASK          = _IOW(LIBERA_EVENT_MAGIC,
					 LIBERA_EVENT_MASK, 
					 libera_U32_t),
    
    LIBERA_EVENT_FLUSH             = _IOW(LIBERA_EVENT_MAGIC,
					 LIBERA_EVENT_FLUSHING,
					 libera_U32_t),

    LIBERA_EVENT_ACQ_PM            = _IOW(LIBERA_EVENT_MAGIC,
					  LIBERA_EVENT_PMBUF,
					  libera_U32_t),

    LIBERA_EVENT_GET_MC_TRIGGER_1 = _IOR(LIBERA_EVENT_MAGIC,
                                         LIBERA_EVENT_MC_TRIGGER_1,
                                         libera_hw_time_t),

    LIBERA_EVENT_GET_MC_TRIGGER_0 = _IOR(LIBERA_EVENT_MAGIC,
                                         LIBERA_EVENT_MC_TRIGGER_0,
                                         libera_hw_time_t),

    LIBERA_EVENT_SET_NCO           = _IOW(LIBERA_EVENT_MAGIC,
                                          LIBERA_EVENT_NCO,
                                          libera_U32_t),

    LIBERA_EVENT_SET_MCPLL         = _IOW(LIBERA_EVENT_MAGIC,
                                          LIBERA_EVENT_MCPLL,
                                          libera_U32_t),

    LIBERA_EVENT_SET_SCPLL         = _IOW(LIBERA_EVENT_MAGIC,
                                          LIBERA_EVENT_SCPLL,
                                          libera_U32_t),

    LIBERA_EVENT_GET_FIFOLEN_SC    = _IOWR(LIBERA_EVENT_MAGIC,
                                           LIBERA_EVENT_FIFOLEN_SC,
                                           libera_U32_t),

    LIBERA_EVENT_GET_FIFOLEN_MC    = _IOWR(LIBERA_EVENT_MAGIC,
                                           LIBERA_EVENT_FIFOLEN_MC,
                                           libera_U32_t),

    /* All DEBUG IDs have to be declared at the bottom of enum! */
#ifdef DEBUG
    LIBERA_EVENT_GET_TRIGGER_BLOCKED = _IOR(LIBERA_EVENT_MAGIC,
					    LIBERA_EVENT_TRIGGER_BLOCK, 
					    libera_Ltimestamp_t),

    LIBERA_EVENT_SET_OFFPLL          = _IOW(LIBERA_EVENT_MAGIC,
					    LIBERA_EVENT_OFFPLL,
					    libera_hw_time_t),
    
    LIBERA_EVENT_PEEK             = _IOR(LIBERA_EVENT_MAGIC,
					 LIBERA_EVENT_PEEK_POKE,
					 libera_peek_poke_t),
    
    
    LIBERA_EVENT_POKE             = _IOW(LIBERA_EVENT_MAGIC,
					 LIBERA_EVENT_PEEK_POKE,
					 libera_peek_poke_t),
    
    LIBERA_EVENT_EVENT_SIM        = _IOW(LIBERA_EVENT_MAGIC,
					 LIBERA_EVENT_EVENTSIM, 
					 libera_U32_t),
    
#endif
};


/**********************/
/* DSC device IOCTL */
/**********************/
typedef enum
{
    LIBERA_DSC_SET = LIBERA_IOC_DSC,
    LIBERA_DSC_BCD,
} libera_dsc_tags_t;

typedef struct
{
    int X;
    int Y;
} libera_offsets_t;

enum libera_dsc_ids_t
{    
    LIBERA_DSC_SET_DSC       =  _IOW(LIBERA_DSC_MAGIC,
                                     LIBERA_DSC_SET,
                                     libera_U32_t),

    LIBERA_DSC_BCD_OFFSETS   =  _IOW(LIBERA_DSC_MAGIC,
                                     LIBERA_DSC_BCD,
                                     libera_offsets_t),
};

/* Include family member specifics */
#ifdef EBPP
#include "ebpp.h"
#endif
#ifdef BBFP
#include "bbfp.h"
#endif
#ifdef HBPP
#include "hbpp.h"
#endif
#ifdef DPP
#include "dpp.h"
#endif


#define TRIGGER_BIT(x)     (1 << (x+22))
	
#ifdef __cplusplus
}
#endif

#endif // _LIBERA_H

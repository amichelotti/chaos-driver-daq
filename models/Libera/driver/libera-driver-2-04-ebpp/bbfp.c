/* $Id: bbfp.c 2233 2008-10-20 12:14:46Z matejk $ */

/** \file bbfp.c */
/** Implements Libera Bunch by Bunch Feedback Processor (BBFP) specifics. */

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

#ifndef __KERNEL__
#  define __KERNEL__
#endif

#define __NO_VERSION__
#include <asm/uaccess.h>

#include "libera_kernel.h"
#include "libera_event.h"
#include "libera_dd.h"
#include "libera.h"

extern int
getBlockedFromFIFO(struct libera_fifo* const q,
		   wait_queue_head_t* const wq,
		   libera_hw_time_t *time);

extern void
libera_dma_get_DMAC_csize(libera_dma_t *dma);

extern int
libera_sctrig_enable(struct libera_event_device * dev,
		     const unsigned long mask);
extern int
libera_mctrig_enable(struct libera_event_device * dev,
		     const unsigned long mask);


/**
* Size of the libera atom in the DDR. Must be 8 bytes precisely.
*/
const static size_t mem_atom_size = sizeof(libera_atom_dd_t);

/** Maximum number of samples that fit in the HB */
const size_t libera_max_samples = LIBERA_HB_SIZE/sizeof(libera_sample_t);


/** Global Libera description */
libera_desc_t libera_desc = {
    magic: LIBERA_MAGIC,
    name: "BBFP",
    fpga: "Unknown",
};

/** All BBFP ARM triggers (MC module) */
const int BBFP_Arms  = TRIGGER_BIT(T_TRIG_POST_MORTEM) |
                       TRIGGER_BIT(T_TRIG_FA1);
/** All BBFP TRIGGER triggers (MC module) */
const int BBFP_Trigs = TRIGGER_BIT(T_TRIG_TRIGGER) |
                       TRIGGER_BIT(T_TRIG_FA0);


/** Helper function for writing to the DD Command Buffer (CB) FIFO.
 *
 * Constructs a DD command and writes it to Command Buffer FIFO.
 * On success, 0 is returned.
 * On error, meaningful negative errno is returned.
 */
static ssize_t
libera_dd_write_CBfifo_BBFP(size_t cb_PostFilt,
                            size_t cb_StartAddr,
                            size_t cb_AddrStep,
                            size_t cb_SnglSize,
                            size_t cb_NoTrans)
{
    ssize_t ret = 0;

    PDEBUG2("cb_PostFilt  = 0x%x\n", cb_PostFilt);
    PDEBUG2("cb_StartAddr = 0x%x\n", cb_StartAddr);
    PDEBUG2("cb_AddrStep  = 0x%x\n", cb_AddrStep);
    PDEBUG2("cb_SnglSize  = 0x%x\n", cb_SnglSize);
    PDEBUG2("cb_NoTrans   = 0x%x\n", cb_NoTrans);

    /* Write to DD_CB FIFO_NOTRANS triggers the transfer */
    writel(cb_PostFilt, iobase + DD_CB_FIFO_POSTFILT);
    writel(cb_StartAddr, iobase + DD_CB_FIFO_STARTADDR);
    writel(cb_AddrStep, iobase + DD_CB_FIFO_ADDRSTEP);
    writel(cb_SnglSize, iobase + DD_CB_FIFO_SNGLSIZE);
    writel(cb_NoTrans, iobase + DD_CB_FIFO_NOTRANS);

    return ret;
}


/** Initialize Libera BBFP specific internal device structures.
 *
 * Initializes Libera BBFP specific internal device structures.
 */
int libera_init_specific(void)
{
    struct libera_event_device   *event   = &libera_event;
    libera_hw_time_t virgin_trig;
    int ret = 0;

    /* Initial trigger offset */
    virgin_trig = LIBERA_DD_CIRCBUF_ATOMS*LIBERA_DD_ATOM_MTS/2;
    putLSTtoCircBuf(&event->paired_timestamps[T_TRIG_TRIGGER], &virgin_trig);
    putLMTtoCircBuf(&event->paired_timestamps[T_TRIG_TRIGGER], &virgin_trig);

    if (atomsize == 0) {
	/* Atom size not specified as a parameter
	* detect from the feature register
	*/
	uint32_t feature = readl(iobase + FPGA_FEATURE_ITECH);
	LIBERA_LOG("BbB atom size autodetection.\n");
	if (LIBERA_IS_BBBQ(feature)) {
		atomsize = 4;
	} else if (LIBERA_IS_BBBS(feature)) {
		atomsize = LIBERA_ATOM_SIZE(feature);
		if (atomsize == 0) {
			atomsize = 5;
			LIBERA_LOG("WARNING: Missing atom size register. Using default: %ld.\n",
			atomsize);
		}
	} else {
		atomsize = 4;
		LIBERA_LOG(
			"WARNING: unable to detect Bunch by Bunch type. Using atom size: %ld.\n",
			atomsize);
	}
    }
    LIBERA_LOG("BbB atom size = %ld samples\n", atomsize);
    
    /* Valid triggers */
    event->ValidTrigVector = BBFP_Arms;

    /* Enable additional triggers */
    ret = libera_sctrig_enable(event,
			       TRIGGER_BIT(T_TRIG_FA1) |
			       TRIGGER_BIT(T_TRIG_FA0)
			       );
    if (ret) return ret;
    ret = libera_mctrig_enable(event,
			       TRIGGER_BIT(T_TRIG_FA1) |
			       TRIGGER_BIT(T_TRIG_FA0)
			       );
    if (ret) return ret;

    return ret;
}


/** Libera Configuration Device: BBFP specific GET methods.
 *
 * Takes care of CFG GET actions on BBFP Libera family member specific
 * parameters.
 */
int
libera_cfg_get_specific(struct libera_cfg_device *dev,
			libera_cfg_request_t *req)
{
    int ret = 0;

    switch (req->idx) {

    default:
	PDEBUG("CFG: Invalid GET request index %d (file: %s, line: %d)\n",
	       req->idx, __FILE__, __LINE__);
	return -EINVAL;

    } // switch (req->idx)

    return ret;
}


/** Libera Configuration Device: BBFP specific SET methods.
 *
 * Takes care of CFG SET actions on BBFP Libera family member specific
 * parameters.
 */
int
libera_cfg_set_specific(struct libera_cfg_device *dev,
			libera_cfg_request_t *req)
{
    int ret = 0;

    switch (req->idx) {

    default:
	PDEBUG("CFG: Invalid SET request index %d (file: %s, line: %d)\n",
	       req->idx, __FILE__, __LINE__);
	return -EINVAL;

    } // switch (req->idx)

    return ret;
}


/** Takes care of proper trigger sequence and can signal to ignore a
 *  certain trigger.
 */
int
libera_valid_trigger(struct libera_event_device *event,
                     int trigno)
{
    /* NOTE: For proper BBFP operation, only one TRIGGER trigger
     *       is allowed after the ARM trigger and vice versa.
     */
    if ( event->ValidTrigVector & (TRIGGER_BIT(trigno)) ) {
        if (event->ValidTrigVector & BBFP_Arms)
            event->ValidTrigVector = BBFP_Trigs;
        else if (event->ValidTrigVector & BBFP_Trigs)
            event->ValidTrigVector = BBFP_Arms;
        return 1;
    } else
        return 0;
}


/** Libera BBFP specific SC trigger handler */
void
libera_SCtrig_specific(struct libera_event_device *event,
		       const libera_hw_time_t *stamp,
		       const libera_hw_time_t *stamp_raw,
		       unsigned long m, unsigned int i)
{
    switch(i)
    {
    case T_TRIG_FA0: /* BBFP_TRIGGER */
	/* NOTE: T_TRIG_FA0 (BBFP_TRIGGER) should be treated the same as
	 *       TRIGGER trigger in BBFP case.
	 *       Therefore, we'll use the TRIGGER trigger's circular
	 *       queue.
	 */
	/* Read the first half of atom */
	putLSTtoCircBuf(&event->paired_timestamps[T_TRIG_TRIGGER], stamp);
	PDEBUG2("SC BBFP_TRIGGER: LST = 0x%08lx%08lx, j = %lu\n",
                ULL(*stamp), jiffies);
	/* Notify userland about new event */
	libera_send_event(LIBERA_EVENT_FA,
			  LIBERA_TRIG_FA_SC0);
	break;

    case T_TRIG_FA1: /* BBFP_ARM */
    case T_TRIG_POST_MORTEM: /* HW_ARM */
	/* NOTE: Nothing to do here. We'll ARM in MC module, just after
	 *       handling this SC module.
	 */
	PDEBUG2("SC BBFP_ARM: LST = 0x%08lx%08lx, j = %lu\n",
                ULL(*stamp), jiffies);
	break;

    default:
	if (putToFIFO(& event->sc_timestamps[i], *stamp) < 0 &&
	    (event->sc_trigVec & m)) //this should never happen
	    printk(KERN_CRIT "libera: SC irq: FIFO %d overflow.\n",i);
    }
}


/** Calculates new HB offset */
static inline void
libera_renew_HB_offset(struct libera_event_device *event,
                       const libera_hw_time_t *stamp)
{
    DEBUG2_ONLY(
    libera_Ltimestamp_t *lasttrig =
        getFromCircBuf(&event->paired_timestamps[T_TRIG_TRIGGER]);
    );

    event->HB_offset_lmt = *stamp %
        (LIBERA_DD_CIRCBUF_ATOMS*LIBERA_DD_ATOM_MTS);

    PDEBUG2("  lasttrig->lmt = 0x%08lx%08lx\n",
            ULL(lasttrig->lmt));
    PDEBUG2("  HB_offset_lmt = 0x%08lx%08lx\n",
            ULL(event->HB_offset_lmt));
}


/** Libera BBFP specific MC trigger handler */
void
libera_MCtrig_specific(struct libera_event_device *event,
		       const libera_hw_time_t *stamp,
		       const libera_hw_time_t *stamp_raw,
		       unsigned long m, unsigned int i)
{
    switch(i)
    {
    case T_TRIG_FA0: /* BBFP_TRIGGER */
	/* NOTE: T_TRIG_FA0 (BBFP_TRIGGER) should be treated the same as
	 *       TRIGGER trigger in BBFP case.
	 *       Therefore, we'll use the TRIGGER trigger's circular
	 *       queue.
	 */
        if (!libera_valid_trigger(event,i)) break;
	PDEBUG2("MC BBFP_TRIGGER: LMT = 0x%08lx%08lx, j = %lu\n",
                ULL(*stamp), jiffies);
	/* Read the second half of atom & increment CircBuf pointer */
	putLMTtoCircBuf(&event->paired_timestamps[T_TRIG_TRIGGER], stamp);
	/* Notify userland about new event  */
	libera_send_event(LIBERA_EVENT_FA,
			  LIBERA_TRIG_FA_MC0);
	break;

    case T_TRIG_POST_MORTEM: /* HW_ARM */
        if (!libera_valid_trigger(event,i)) break;
        PDEBUG2("POST PORTEM (ARM): LMT = 0x%08lx%08lx, j = %lu\n",
                ULL(*stamp), jiffies);
	putLMTtoCircBuf(&event->paired_timestamps[T_TRIG_POST_MORTEM], stamp);
/*        libera_renew_HB_offset(event, stamp); */
	libera_send_event(LIBERA_EVENT_PM, 0);
	break;

    case T_TRIG_FA1: /* BBFP_ARM */
        if (!libera_valid_trigger(event,i)) break;
	PDEBUG2("MC BBFP_ARM: LMT = 0x%08lx%08lx, j = %lu\n",
                ULL(*stamp), jiffies);
	putLMTtoCircBuf(&event->paired_timestamps[T_TRIG_POST_MORTEM], stamp);
/*        libera_renew_HB_offset(event, stamp); */
	/* Notify userland about new event  */
	libera_send_event(LIBERA_EVENT_FA,
			  LIBERA_TRIG_FA_MC1);
	break;


    default:
	if (putToFIFO(& event->mc_timestamps[i], *stamp) < 0 &&
	    (event->mc_trigVec & m)) //this should never happen
	    printk(KERN_CRIT "libera: MC irq: FIFO %d overflow.\n",i);
    }
}


/** Libera EBPP specific SC helper IRQ handler */
void
libera_schelp_handler_specific(struct libera_event_device  *event)
{
    /* Unused. */
}


/** Libera EBPP specific MC helper IRQ handler */
void
libera_mchelp_handler_specific(struct libera_event_device  *event)
{
    // TODO: Consider using event generation for HB start/stop.
    return;

    DEBUG_ONLY(static unsigned long count; count++;);

    /* Start HB at appropriate time -- simple event generation */
    if ( ((event->mc_time + 0x4000000ULL) <= event->HB_start_lmt ) &&
	 ( (event->mc_time + 0x8000000ULL) > event->HB_start_lmt ) ) {
	libera_hw_time_t offset26;

	offset26 = event->HB_start_lmt - (event->mc_time + 0x4000000ULL);
	DEBUG2_ONLY(
		    if (offset26 >= 0x4000000ULL)
		    PDEBUG2("VERY STRANGE: offset26 >= 0x4000000ULL\n");
		    );
	if (offset26 < 0x2000000ULL) {
	    // Schedule event -- start/enable HB.
	    writel((1 << T_EG_HB), iobase + T_MC_EVENTGENL);
	    writel((unsigned long)(offset26 & 0x00000000FFFFFFFFULL),
		   iobase + T_MC_EVENTGENH);
	    PDEBUG("HELPER #%lu: HB/SA start: offset26 = 0x%08lx\n", count,
		   (unsigned long)(offset26 & 0x00000000FFFFFFFFULL));
	}
    }
}


/** Libera EBPP specific SC SELF_INC trigger handler */
void
libera_sc_selfinc_specific(struct libera_event_device  *event)
{
    /* Unused. */
}


/** Libera EBPP specific MC SELF_INC trigger handler */
void
libera_mc_selfinc_specific(struct libera_event_device  *event)
{
    // TODO: Consider using event generation for HB start/stop.
    return;

    DEBUG_ONLY(static unsigned long count; count++;);

    /* Start HB at appropriate time -- simple event generation */
    if ( (event->mc_time <= event->HB_start_lmt ) &&
         ( (event->mc_time + 0x4000000ULL) > event->HB_start_lmt ) ) {
        libera_hw_time_t offset26;

        offset26 = event->HB_start_lmt - event->mc_time;
        DEBUG2_ONLY(
                    if (offset26 >= 0x4000000ULL)
                    PDEBUG2("VERY STRANGE: offset26 >= 0x4000000ULL\n");
                    );
        if (offset26 >= 0x2000000ULL) {
            // Schedule event -- start/enable HB.
	    writel((1 << T_EG_HB), iobase + T_MC_EVENTGENL);
            writel((unsigned long)(offset26 & 0x00000000FFFFFFFFULL),
                   iobase + T_MC_EVENTGENH);
            PDEBUG("SELF #%lu: HB/SA start: offset26 = 0x%08lx\n", count,
                   (unsigned long)(offset26 & 0x00000000FFFFFFFFULL));
        }
    }
}


/** Libera SA interrupt handler BBFP specific Top Half (TH).
 *
 * Takes care of reading the data from SA fifo on FPGA to internal buffer.
 * The rest is done in the tasklet Bottom Half (BH).
 */
void
libera_sa_handler_specific(void)
{
    /* Unused. */
}

/** Libera SA interrupt handler tasklet BBFP specific Bottom Half (BH).
 *
 */
void libera_sa_do_tasklet_specific(unsigned long data)
{
    /* Unused. */
}

/** Libera Interlock handler BBFP specific.
 *
 */
void libera_ilk_handler_specific(unsigned long data)
{
    /* Unused. */
}


static inline uint32_t mod_op(uint64_t dividend, uint32_t divisor)
{
	uint32_t rem;
	(void)div_u64_rem(dividend, divisor, &rem);
	return rem;
}

/**
 * Updates the timestamp in dd_local according to the relative position
 * since the ARM timestamp.
 */
int libera_update_timestamp(libera_dd_local_t *dd_local, loff_t f_pos) {
	libera_Qtimestamp_t *Qts = &dd_local->Qts;
	int ret = 0;
	
	Qts->L.lmt = dd_local->arm_lmt*LIBERA_DD_MT_SAMPLES + f_pos;
	Qts->U.mt = Qts->L.lmt/LIBERA_DD_MT_SAMPLES;
	ret = lmt2lst(&(Qts->U.mt), &(Qts->L.lst), &(Qts->lasttrig));
	if (ret != 0) {
		return ret;
	}
	lst2st(&(Qts->L.lst), &(Qts->U.st));
	memcpy(&(dd_local->tstamp), &(Qts->U), sizeof(dd_local->tstamp));
	return 0;
}

int libera_position_to_trigger(struct file *file, libera_Ltimestamp_t *trig_time)
{
	libera_dd_local_t *dd_local = (libera_dd_local_t *)file->f_version;
	libera_Qtimestamp_t *Qts = &dd_local->Qts;
	struct libera_event_device *event = &libera_event;
	libera_Ltimestamp_t ctime;
	libera_Ltimestamp_t arm_time;
	int ret = 0;
	long timeout_diff;
	long timeout_jiff;

	const uint32_t l = atomsize;
	libera_hw_time_t trigger_pos;
	loff_t trigger_pos_samples;
	loff_t residue;

	PDEBUG2("%s: -->\n", __FUNCTION__);
	PDEBUG2("%s: Waiting for HB to stop.\n", __FUNCTION__);
	/* Wait for the trigger to be set */
	ret = libera_get_CTIME(&ctime);
	if (ret != 0) return ret;

	/* Wait for the second half of the DDR to be filled and the DDR overflow
	*  counter to be updated. Some additional time is added to be sure that writing is complete. */
	timeout_diff = (Qts->lasttrig.lmt - ctime.lmt + LIBERA_DD_CIRCBUF_ATOMS*LIBERA_DD_ATOM_MTS/2);
	if (timeout_diff > 0) {
		timeout_jiff = (timeout_diff * FLMC_DECI_HZ * HZ)/flmcdHz + 1;
		PDEBUG2("%s: Delaying for %ld.\n", __FUNCTION__, timeout_jiff);
		if (timeout_jiff < HZ) {
			PDEBUG3("Waiting %ld jiffies (HB stop)\n", timeout_jiff);
			ret = libera_delay_jiffies_interruptible(timeout_jiff);
			if (ret) {
				return ret;
			}
		} else {
			PDEBUG("DD (timing): Timeout waiting for HB stop (%ld jiffies).\n",
					timeout_jiff);
			return -EDEADLK;
		}
	}
	PDEBUG2("%s: Ready to set to trigger position.\n", __FUNCTION__);

	memcpy(&Qts->lasttrig, trig_time, sizeof(libera_Ltimestamp_t));

/*
Alignment of atoms (logical and DDR):

logical |-+-+-+-+-+|-+-+-+-+-+|-+-+-+-+-+|-+-+-+-+-+|-+-+-+-+-+|-+-+-+-+-+|...
ddr     |-+-+-+-+|-+-+-+-+|-+-+-+-+|-+-+-+-+|-+-+-+-+|-+-+-+-+|-+-+-+-+|...

        ^                 ^   ^
        arm               tr  |
                              tr_l

On seek we have to transform the trigger position from DDR atom units into
logical atom units.

Logical and ddr positions are aligned on arm (ddr position 0).

DDR atoms size is 64-bits (4 samples of 16-bit).

First multiple of logical atom after the DDR atom is calculated as the reference
trigger position in logical units. Relative position is then added to the
logical trigger position.
*/

	memcpy(
		&arm_time,
		getFromCircBuf(&event->paired_timestamps[T_TRIG_POST_MORTEM]),
		sizeof(libera_Ltimestamp_t));

	/* Needs serious reconsideration: The whole 5->4 reconstruction is not valid if ARM 
	 * is the latest event and thus the ARM timestamp geater than TRIG timeastamp.
	 * That is the case of free running History Buffer.
	 * For now, we will simply reject such requests.
	 */
	if (trig_time->lmt < arm_time.lmt) return -EPERM;

	/* Number of atoms between arm and trigger */
	trigger_pos = trig_time->lmt - arm_time.lmt;

	/* calculate the first multiple (in samples) of logical atom
	len after the trigger, which is measured in memory atom units */
	trigger_pos_samples = trigger_pos * LIBERA_DD_MT_SAMPLES;
	residue = mod_op(l - mod_op(trigger_pos_samples + l, l), l);

	PDEBUG2("%s: l = %u\n", __FUNCTION__, l);
	PDEBUG2("%s: trigger_pos_samples (1) = %llu\n", __FUNCTION__, trigger_pos_samples);
	PDEBUG2("%s: LIBERA_DD_CIRCBUF_SAMP = %d\n", __FUNCTION__, LIBERA_DD_CIRCBUF_SAMP);

	/* relative position in samples since arm at trigger */
	trigger_pos_samples = trigger_pos_samples + residue;

	file->f_pos = trigger_pos_samples;
	dd_local->trig_position = trigger_pos_samples;
	dd_local->arm_lmt = arm_time.lmt;

	libera_update_timestamp(dd_local, trigger_pos_samples);

	/* TODO: is it necessary to store the offset at all? */
	Qts->relative = 0;

	PDEBUG2("%s: trigger_pos = %llu\n", __FUNCTION__, trigger_pos);
	PDEBUG2("%s: residue = %llu\n", __FUNCTION__, residue);
	PDEBUG2("%s: trigger_pos_samples = %llu\n", __FUNCTION__, trigger_pos_samples);
	PDEBUG2("%s: <--\n", __FUNCTION__);

	return 0;
}

int libera_new_trigger(struct file *file, libera_Ltimestamp_t *trig_time)
{
	libera_dd_local_t *dd_local = (libera_dd_local_t *)file->f_version;

	/* Acquire the last trigger timestamp -- the middle point (bunch #0) in HB */
	memcpy(
		trig_time,
		getFromCircBuf(&libera_event.paired_timestamps[T_TRIG_TRIGGER]),
		sizeof(libera_Ltimestamp_t));

	return (dd_local->Qts.lasttrig.lmt != trig_time->lmt);
}

/** Libera BBFP DD Device: Called on lseek()
 *
 * Seek is (ab)used for providing the timing request information to the Libera
 * GNU/Linux driver.
 * SEEK_CUR is used for providing sample offset, relative to the trigger
 *          defining bunch #0
 * SEEK_SET Unused.
 * SEEK_END Unused.
 * File version (f_version) stores all the relevant timing info.
 *
 * For details see: Alessandro Rubini et al., Linux Device Drivers, pp. 163.
 */
loff_t
libera_dd_llseek_specific(struct file *file, loff_t offset, int whence)
{
	libera_dd_local_t *dd_local = (libera_dd_local_t*)file->f_version;
	libera_Ltimestamp_t trig_time;
	loff_t bof, eof;

	PDEBUG2("%s: -->\n", __FUNCTION__);

	switch(whence) {
	case SEEK_CUR:
		/* Interval starting time given in samples (bunches)
		* relative to the trigger.
		* Implementation of seek is below.
		*/
		break;

	case SEEK_SET:
	case SEEK_END:
		return -EINVAL; // Not supported in BBFP
		break;

	default: /* can't happen */
		return -EINVAL;
	}

	if (libera_new_trigger(file, &trig_time)) {
		/* New trigger was set --> Reposition to trigger */
		int ret = libera_position_to_trigger(file, &trig_time);
		if (ret != 0) return ret;
	}

	/* Verification of seek over HB boundaries. */
	eof =
		dd_local->trig_position + LIBERA_DD_CIRCBUF_SAMP/2 -
		mod_op(dd_local->trig_position, atomsize);
	bof = eof - LIBERA_DD_CIRCBUF_SAMP;

	PDEBUG2("%s: offset = %llu\n", __FUNCTION__, offset);
	file->f_pos += offset;

	if (file->f_pos > eof) {
		file->f_pos = eof;
	} else if (file->f_pos < bof) {
		file->f_pos = bof;
	}
	
	libera_update_timestamp(dd_local, file->f_pos);
	
	PDEBUG2("%s: <-- 0x%08lx%08lx\n", __FUNCTION__, ULL(file->f_pos));

	return file->f_pos;
	/*
	 * TODO: Return value in DDR address or in bytes? See code for brilliance.
	 */
}


/** Helper function for OB-fifo synchronization.
 *
 * Checks OB-fifo status register and waits (by blocking) for available
 * data in OB-fifo.
 * At least one DD atom should be present in OB-fifo for both DMA
 * and non-DMA transfers to be able to continue properly.
 *
 * On success, 0 is returned.
 * On error, meaningful negative errno is returned.
 */
static inline ssize_t libera_OB_fifo_sync(void)
{
	int wait_count = 0;
	ssize_t ret = 0;
	libera_dma_t *dma = &lgbl.dma;

	while ( (dma->remaining) && !(DD_OB_SIZE(dma->obFIFOstatus) )) {
		/* FIFO size not large enough, but still expecting more
		* data to appear through OB-fifo -> reading too fast.
		*/
		PDEBUG3("DD read(): Reading TOO FAST: sleeping...\n");
		libera_delay_jiffies(DD_WAIT_STEP);
		dma->obFIFOstatus = readl(iobase + DD_OB_STATUS);
		PDEBUG2("DD read(): OB_STATUS = 0x%08lx, OB_SIZE = 0x%08lx\n", dma->obFIFOstatus, DD_OB_SIZE(dma->obFIFOstatus));

		/* Deadlock avoidance */
		if ( wait_count++ > HZ) {
			PDEBUG2("DD: Timeout waiting for OB-fifo data.\n");
			PDEBUG2("Error in file: %s, line: %d)\n",
					__FILE__, __LINE__);
					return -EIO;
		}
	}

	return ret;
}


ssize_t copy_buffer_to_user(
	libera_sample_t *buf,
	size_t bufsize,
	char __user *usrbuf,
	unsigned long dec,
	size_t *dec_offset)
{
	unsigned long copied = bufsize;
	unsigned long ret = 0;

	if (bufsize == 0) {
		return 0;
	}
	if (*dec_offset > bufsize) {
		*dec_offset -= bufsize;
		return 0;
	}
	bufsize -= *dec_offset;
	buf += *dec_offset;

	PDEBUG2("%s: -->\n", __FUNCTION__);

/* TODO: Use hardware (FPGA) decimation if step is divisable by DDR atom size (4). */

	if (dec > 1) {
		/* collect samples for one copy_to_user */
		size_t i = 0;
		size_t j = 1;
		libera_sample_t *s = buf;
		PDEBUG2("%s: Decimating.\n", __FUNCTION__);
		for (i = dec; i < bufsize; i += dec, ++j) {
			s[j] = s[i];
//			PDEBUG2("%s: s[%ld] = s[%ld];.\n", __FUNCTION__, j, i);
		}
		copied = j;
		*dec_offset = i - bufsize;
		PDEBUG2("%s: dec_offset=%ld\n", __FUNCTION__, *dec_offset);
	}
	else {
		*dec_offset = 0;
	}
	PDEBUG2("%s: dec = %lu\n", __FUNCTION__, dec);
	PDEBUG2("%s: bufsize = %lu\n", __FUNCTION__, bufsize);
	PDEBUG2("%s: copied = %lu\n", __FUNCTION__, copied);

	ret = copy_to_user(usrbuf, buf, copied * sizeof(libera_sample_t));

	if (ret != 0) {
		PDEBUG2("%s: copy_to_user failed = %ld\n", __FUNCTION__, ret);
		return -EFAULT;
	}

	PDEBUG2("%s: <-- %ld\n", __FUNCTION__, copied);

	return copied;
}

ssize_t dma_copy_fifo_to_buffer(libera_dma_t *dma)
{
	ssize_t ret = 0;
	int sleep_count = 0;
	int sync_ret;
	wait_queue_t wait;

	PDEBUG2("%s: -->\n", __FUNCTION__);

	if ( (!dma->DMAC_transfer) && (dma->remaining > 0) ) {

		PDEBUG2("%s: Issue new DMA command.\n", __FUNCTION__);

		dma->obFIFOstatus = readl(iobase + DD_OB_STATUS);

		/* Reading speed sync */
		sync_ret = libera_OB_fifo_sync();
		if ( sync_ret < 0 ) return sync_ret;

		libera_dma_get_DMAC_csize(dma);

		/* NOTE: At this stage, it is still possible that dma->csize == 0.
		*       If it equals zero, it can only mean that our DMA fifo
		*       is full. In this case we just pass control to the
		*       main DMA while loop to empty the DMA fifo first.
		*/
		if (!(dma->csize > 0)) {
			if  ( (LIBERA_DMA_FIFO_ATOMS - 1 - lenDMA_FIFO(dma) ) == 0) {
				ret = lenDMA_FIFO(dma) * LIBERA_DD_MT_SAMPLES;
				PDEBUG2("%s: <-- %ld\n", __FUNCTION__, ret);
				return ret;
			}
		}

		PDEBUG2("%s: libera_dma_command. dma->csize = %u\n", __FUNCTION__, dma->csize);

		libera_dma_command(
			(LIBERA_IOBASE + DD_OB_FIFOBASE),
			&dma->buf[dma->put],
			dma->csize*sizeof(libera_atom_dd_t));
	}

	/* Check fifo size & wait/sleep if neccessary */

	if (emptyDMA_FIFO(dma)) {
		init_waitqueue_entry(&wait, current);
		add_wait_queue(&(libera_dd.DMA_queue), &wait);
		for (;;) {
			PDEBUG2("%s: Waiting for DMA transfer to complete 1.\n", __FUNCTION__);
			if (not_emptyDMA_FIFO(dma)) {
				break;
			}
			set_current_state(TASK_INTERRUPTIBLE);

			if (!signal_pending(current)) {
				schedule();
			}
			else {
				/* Process received signal, exit */
				ret = -ERESTARTSYS;
				break;
			}
			PDEBUG2("%s: Waiting for DMA transfer to complete 2.\n", __FUNCTION__);
			// NOTE: This will only work for small read() requests.
			if ( sleep_count++ > 1000) {
				PDEBUG2("BREAKING DMA sleeping loop.\n");
				PDEBUG2("Error in file: %s, line: %d)\n",
				__FILE__, __LINE__);
				ret = -EDEADLK;
				break;
			};
		}
		set_current_state(TASK_RUNNING);
		remove_wait_queue(&(libera_dd.DMA_queue), &wait);
	}

	if (not_emptyDMA_FIFO(dma) && ret == 0) {
		/* Return the number of samples copied */
		ret = lenDMA_FIFO(dma) * LIBERA_DD_MT_SAMPLES;
	}

	PDEBUG2("%s: <-- ret = %lu\n", __FUNCTION__, ret);

	return ret;
}

ssize_t libera_dd_read_specific(
	struct file *file,
	char __user *buf,
	size_t count,
	loff_t *f_pos)
{
	struct libera_dd_device *dev = (struct libera_dd_device *) file->private_data;
	libera_dd_local_t *dd_local = (libera_dd_local_t*)file->f_version;
	libera_dma_t *dma = &lgbl.dma;
	libera_Ltimestamp_t trig_time;

	/* Prevent reading over the end of buffer */
        // TODO: loff_t ?
	off_t to_eof = *f_pos -
		dd_local->trig_position + LIBERA_DD_CIRCBUF_SAMP/2 -
		mod_op(dd_local->trig_position, atomsize);

	/* From/to in samples */
	size_t samples_to_process = MIN(to_eof, (count / LIBERA_DD_SAMP_SIZE - 1) * dd_local->dec + 1);
	loff_t from = *f_pos;
	loff_t to = *f_pos + samples_to_process;

	/* From/to in ddr atoms */
	//loff_t from_at = div64_64(from, LIBERA_DD_MT_SAMPLES);
	//loff_t to_at = div64_64(to, LIBERA_DD_MT_SAMPLES);
        /* TODO: div64_64() produces erroneous results at large dividend,
         *       because of its dynamic precision. To be verified.
         *
         * Using shift instead.
         */
	loff_t from_at = from >> (fls(LIBERA_DD_MT_SAMPLES) - 1);
	loff_t to_at = to >> (fls(LIBERA_DD_MT_SAMPLES) - 1);

	/* Samples to skip from first ddr atom when copying to user space.
	   Reason:
	   * current position in samples might be in the middle of of the ddr atom.
	*/
	loff_t smpl_offset = from - (from_at * LIBERA_DD_MT_SAMPLES);
	size_t dec_offset = 0;

	/* ddr address and count to copy */
	size_t ddr_start = mod_op(from_at, LIBERA_DD_CIRCBUF_ATOMS);
	size_t ddr_atom_count = 0;

	ssize_t processed = 0;
	ssize_t copied = 0;
	uint32_t loop_count = 0;

	void __user *buf_ptr = buf;
	int ret = 0;

	if (from == to) {
		return 0;
	}
	/* read one DDR atom more to read partial last logical atom */
	if (mod_op(to, LIBERA_DD_MT_SAMPLES) != 0) {
		++to_at;
	}
	ddr_atom_count = to_at - from_at;

	PDEBUG2("%s: -->\n", __FUNCTION__);
	PDEBUG2("%s: count = %lu\n", __FUNCTION__, count);
	PDEBUG2("%s: dec = %lu\n", __FUNCTION__, dd_local->dec);
	PDEBUG2("%s: samples_to_process = %lu\n", __FUNCTION__, samples_to_process);
	PDEBUG2("%s: from, to = %llu - %llu\n", __FUNCTION__, from, to);
	PDEBUG2("%s: from_at, to_at = %llu - %llu\n", __FUNCTION__, from_at, to_at);
	PDEBUG2("%s: smpl_offset = %lu\n", __FUNCTION__, smpl_offset);
	PDEBUG2("%s: ddr_start = %lu\n", __FUNCTION__, ddr_start);
	PDEBUG2("%s: ddr_atom_count = %lu\n", __FUNCTION__, ddr_atom_count);

	if (libera_new_trigger(file, &trig_time)) {
		/* New trigger was set --> Reposition to trigger */
		ret = libera_position_to_trigger(file, &trig_time);
		if (ret != 0) return ret;
	}
	else {
		libera_update_timestamp(dd_local, file->f_pos);
	}

	if (mutex_lock_interruptible(&dev->sem)) {
		return -ERESTARTSYS;
	}

	PDEBUG2("%s: Starting FIFO transfer.\n", __FUNCTION__);

	ret = libera_dd_write_CBfifo_BBFP(0, ddr_start, 1, 1, ddr_atom_count);
	if (ret < 0) {
		goto out;
	}

	/* Initialize DMA structures */
	dma->Overrun = FALSE;
	dma->DMAC_transfer = FALSE;
	dma->remaining = ddr_atom_count;

	PDEBUG2("%s: Flushing DMA FIFO.\n", __FUNCTION__);

	flushDMA_FIFO(dma);
	PDEBUG2("%s: Entering copy loop.\n", __FUNCTION__);

	/* Copy the samples into dma buffer and further to user buffer */
	do {
		ssize_t copied_samples = dma_copy_fifo_to_buffer(dma);
		PDEBUG2("%s: copied_samples = %ld\n", __FUNCTION__, copied_samples);
		if (copied_samples < 0) {
			ret = copied_samples;
			break;
		}

		if (copied_samples > 0) {
			ssize_t to_copy = MIN(
				copied_samples - smpl_offset,
				samples_to_process - processed);

			libera_sample_t *src = NULL;
			
			PDEBUG2("%s: samples_to_process = %ld\n", __FUNCTION__, samples_to_process);
			PDEBUG2("%s: processed = %ld\n", __FUNCTION__, processed);
			PDEBUG2("%s: dec_offset = %ld\n", __FUNCTION__, dec_offset);
			PDEBUG2("%s: to_copy = %ld\n", __FUNCTION__, to_copy);

			if (dma->get + to_copy/LIBERA_DD_MT_SAMPLES > LIBERA_DMA_FIFO_ATOMS) {
				/* Available data is wrapped over the end of the buffer.
				   Copy tail first.
				*/
				size_t tail_ddr_smpl = (LIBERA_DMA_FIFO_ATOMS - dma->get) * LIBERA_DD_MT_SAMPLES;
				ssize_t tail_bsize = tail_ddr_smpl - (smpl_offset + dec_offset);
				libera_sample_t *src = (libera_sample_t*)&dma->buf[dma->get];
				src += smpl_offset;

				PDEBUG2("%s: Copy tail first. tail_bsize = %ld\n", __FUNCTION__, tail_bsize);

				ret = copy_buffer_to_user(
					src, tail_bsize,
					buf_ptr,
					dd_local->dec, &dec_offset);

				if (ret < 0) {
					/* TODO: Do we need todo any cleanup */
					break;
				}
				processed += tail_bsize;
				to_copy -= tail_bsize;
				copied_samples -= tail_ddr_smpl;
				copied += ret;
				buf_ptr += ret * LIBERA_DD_SAMP_SIZE;
				dma->get = 0;
				smpl_offset = 0;

				PDEBUG2("%s: Remaining after copying tail. to_copy = %ld\n", __FUNCTION__, to_copy);
			}

			src = (libera_sample_t*)&dma->buf[dma->get];
			src += smpl_offset;
			ret = copy_buffer_to_user(
				src, to_copy,
				buf_ptr,
				dd_local->dec, &dec_offset);
			
			if (ret < 0) {
				/* TODO: Do we need todo any cleanup */
				break;
			}
			processed += to_copy;
			copied += ret;
			smpl_offset = 0;

			buf_ptr += ret * LIBERA_DD_SAMP_SIZE;

			/* Move the tail of the queue forward in atom units */
			dma->get = (dma->get + copied_samples / LIBERA_DD_MT_SAMPLES) & LIBERA_DMA_FIFO_MASK;
		}
		if (signal_pending(current)) {
			break;
		}

		if (processed > samples_to_process) {
			PDEBUG2("%s: The loop should stop! processed = %ld\n", __FUNCTION__, processed);
/*			break; */
		}
		if (++loop_count > ddr_atom_count) {
			printk(LIBERA_SYSLOG_LEVEL "%s: Deadlock detected! loop_count = %lu\n", __FUNCTION__, loop_count);
			ret = -EDEADLK;
			break;
		}
	}
	while ( (dma->remaining) || (not_emptyDMA_FIFO(dma)) );

	/* TODO: Verification of correct DMA transfer */

	/* We got new trigger while reading. Read is invalid */
	if (libera_new_trigger(file, &trig_time)) {
		ret = -EIO;
	}

	if (ret < 0) {
		PDEBUG2("%s: Error while reading.\n", __FUNCTION__);
		// error copying data
	} else {
		/* Update the position for next read. Decimation considered. */
		*f_pos += processed + dec_offset;
		ret = copied * LIBERA_DD_SAMP_SIZE;
		PDEBUG2("%s: New file->f_pos = %llu.\n", __FUNCTION__, *f_pos);
	}

out:
	PDEBUG2("%s: <-- %ld\n", __FUNCTION__, ret);
	mutex_unlock(&dev->sem);

	return ret;
}

ssize_t
libera_acq_pm(void)
{
    return 0;
}

/* $Id: hbpp.c 2201 2008-10-07 12:43:16Z matejk $ */

/** \file hbpp.c */
/** Implements Libera Hadron (HBPP) specifics. */

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
#include <asm/io.h>
#include <linux/delay.h>
#include <linux/interrupt.h>

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


/** Global Libera description */
libera_desc_t libera_desc = {
    magic: LIBERA_MAGIC,
    name: "HBPP",
    fpga: "Unknown",
};

/** All HBPP ARM triggers (MC module) */
const int HBPP_Arms  = TRIGGER_BIT(T_TRIG_POST_MORTEM) |
                       TRIGGER_BIT(T_TRIG_FA1);
/** All HBPP TRIGGER triggers (MC module) */
const int HBPP_Trigs = TRIGGER_BIT(T_TRIG_TRIGGER) |
                       TRIGGER_BIT(T_TRIG_FA0);


/** Helper function for writing to the DD Command Buffer (CB) FIFO.
 *
 * Constructs a DD command and writes it to Command Buffer FIFO.
 * On success, 0 is returned.
 * On error, meaningful negative errno is returned.
 */
static ssize_t
libera_dd_write_CBfifo_HBPP(size_t cb_PostFilt,
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


/** Initialize Libera HBPP specific internal device structures.
 *
 * Initializes Libera HBPP specific internal device structures.
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

    /* Valid triggers */
    event->ValidTrigVector = HBPP_Arms;

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


/** Libera Configuration Device: HBPP specific GET methods.
 *
 * Takes care of CFG GET actions on HBPP Libera family member specific
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


/** Libera Configuration Device: HBPP specific SET methods.
 *
 * Takes care of CFG SET actions on HBPP Libera family member specific
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
    /* NOTE: For proper HBPP operation, only one TRIGGER trigger
     *       is allowed after the ARM trigger and vice versa.
     */
    if ( event->ValidTrigVector & (TRIGGER_BIT(trigno)) ) {
        if (event->ValidTrigVector & HBPP_Arms)
            event->ValidTrigVector = HBPP_Trigs;
        else if (event->ValidTrigVector & HBPP_Trigs)
            event->ValidTrigVector = HBPP_Arms;
        return 1;
    } else
        return 0;
}


/** Libera HBPP specific SC trigger handler */
void
libera_SCtrig_specific(struct libera_event_device *event,
		       const libera_hw_time_t *stamp,
		       const libera_hw_time_t *stamp_raw,
		       unsigned long m, unsigned int i)
{
    switch(i)
    {
    case T_TRIG_FA0: /* HBPP_TRIGGER */
	/* NOTE: T_TRIG_FA0 (HBPP_TRIGGER) should be treated the same as
	 *       TRIGGER trigger in HBPP case.
	 *       Therefore, we'll use the TRIGGER trigger's circular
	 *       queue.
	 */
	/* Read the first half of atom */
	putLSTtoCircBuf(&event->paired_timestamps[T_TRIG_TRIGGER], stamp);
	PDEBUG2("SC HBPP_TRIGGER: LST = 0x%08lx%08lx, j = %lu\n",
                ULL(*stamp), jiffies);
	/* Notify userland about new event */
	libera_send_event(LIBERA_EVENT_FA,
			  LIBERA_TRIG_FA_SC0);
	break;

    case T_TRIG_FA1: /* HBPP_ARM */
        case T_TRIG_POST_MORTEM: /* HW_ARM */
	/* NOTE: Nothing to do here. We'll ARM in MC module, just after
	 *       handling this SC module.
	 */
	PDEBUG2("SC HBPP_ARM: LST = 0x%08lx%08lx, j = %lu\n",
                ULL(*stamp), jiffies);
	/* Notify userland about new event */
	//libera_send_event(LIBERA_EVENT_FA,
        //LIBERA_TRIG_FA_SC1);
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

/*     event->HB_offset_lmt = ( *stamp + event->HB_offset_lmt - 1 - */
/*         (lasttrig->lmt +  */
/*         (LIBERA_DD_CIRCBUF_ATOMS*LIBERA_DD_ATOM_MTS)/2) ) %  */
/*         (LIBERA_DD_CIRCBUF_ATOMS*LIBERA_DD_ATOM_MTS); */

    PDEBUG2("  lasttrig->lmt = 0x%08lx%08lx\n",
            ULL(lasttrig->lmt));
    PDEBUG2("  HB_offset_lmt = 0x%08lx%08lx\n",
            ULL(event->HB_offset_lmt));
}


/** Libera HBPP specific MC trigger handler */
void
libera_MCtrig_specific(struct libera_event_device *event,
		       const libera_hw_time_t *stamp,
		       const libera_hw_time_t *stamp_raw,
		       unsigned long m, unsigned int i)
{
    switch(i)
    {
    case T_TRIG_FA0: /* HBPP_TRIGGER */
	/* NOTE: T_TRIG_FA0 (HBPP_TRIGGER) should be treated the same as
	 *       TRIGGER trigger in HBPP case.
	 *       Therefore, we'll use the TRIGGER trigger's circular
	 *       queue.
	 */
	PDEBUG2("MC HBPP_TRIGGER: LMT = 0x%08lx%08lx, j = %lu\n",
                ULL(*stamp), jiffies);
	/* Read the second half of atom & increment CircBuf pointer */
	putLMTtoCircBuf(&event->paired_timestamps[T_TRIG_TRIGGER], stamp);
	/* Notify userland about new event  */
	libera_send_event(LIBERA_EVENT_FA,
			  LIBERA_TRIG_FA_MC0);
	break;

    case T_TRIG_POST_MORTEM:
        if (!libera_valid_trigger(event,i)) break;
        PDEBUG2("POST PORTEM (ARM): LMT = 0x%08lx%08lx, j = %lu\n",
                ULL(*stamp), jiffies);
        libera_renew_HB_offset(event, stamp);
	break;

    case T_TRIG_FA1: /* HBPP_ARM */
	PDEBUG2("MC HBPP_ARM: LMT = 0x%08lx%08lx, j = %lu\n",
                ULL(*stamp), jiffies);
        libera_renew_HB_offset(event, stamp);
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
    /* Unused. */
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
    /* Unused. */
}


/** Libera SA interrupt handler HBPP specific Top Half (TH).
 * Libera HBPP DD Device: Called on lseek()
 *
 * Takes care of reading the data from SA fifo on FPGA to internal buffer.
 * The rest is done in the tasklet Bottom Half (BH).
 */
void
libera_sa_handler_specific(void)
{
    /* Unused. */
}

/** Libera SA interrupt handler tasklet HBPP specific Bottom Half (BH).
 *
 */
void libera_sa_do_tasklet_specific(unsigned long data)
{
    /* Unused. */
}

/** Libera Interlock handler HBPP specific.
 *  *
 *   */
void libera_ilk_handler_specific(unsigned long data)
{
	    /* Unused. */
}


/** Libera HBPP DD Device: Called on lseek()
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
libera_dd_llseek_specific(struct file *file, loff_t time, int whence)
{
    libera_dd_local_t *dd_local = (libera_dd_local_t *)file->f_version;
    libera_Qtimestamp_t *Qts = &dd_local->Qts;

    switch(whence) {
    case 1: /* SEEK_CUR */
	/* Interval starting time given in samples (bunches)
	 * relative to the trigger.
	 */
	memcpy(&(Qts->relative), &time, sizeof(loff_t));
	DEBUG2_ONLY(
	{
	    unsigned long timeh;
	    unsigned long timel;
	    timeh = time >> 32;
	    timel = time & (unsigned long long) 0x00000000FFFFFFFFULL;
	    PDEBUG2("llseek(): timeh = 0x%08lx\n", timeh);
	    PDEBUG2("llseek(): timel = 0x%08lx\n", timel);
	});
        break;

    case 0: /* SEEK_SET */
    case 2: /* SEEK_END */
	return -EINVAL; // Not supported in HBPP
        break;

    default: /* can't happen */
        return -EINVAL;
    }

    return 0;
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
static inline ssize_t libera_OB_fifo_sync(unsigned long *obFIFOstatus)
{
    int wait_count = 0;
    ssize_t ret = 0;
    libera_dma_t *dma = &lgbl.dma;

    while ( (dma->remaining) &&
	    !(DD_OB_SIZE(*obFIFOstatus) )) {
	/* FIFO size not large enough, but still expecting more
	 * data to appear through OB-fifo -> reading too fast.
	 */
	PDEBUG3("DD read(): Reading TOO FAST: sleeping...\n");
	libera_delay_jiffies(DD_WAIT_STEP);
	*obFIFOstatus = readl(iobase + DD_OB_STATUS);
	PDEBUG2("DD read(): OB_STATUS = 0x%08lx, OB_SIZE = 0x%08lx\n", *obFIFOstatus, DD_OB_SIZE(*obFIFOstatus));

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


/** Helper function for reading from the DD Output Buffer (OB) FIFO, via
 *  DMA fifo, using DMA transfer, to userspace.
 *
 * Reads \param atom_count atoms from DD Output Buffer (OB) FIFO
 * to userland buffer \param buf.
 * On success, the number of read atoms is returned.
 * On error, meaningful negative errno is returned.
 */
static inline ssize_t
libera_dd_transfer_OBfifoToUser_DMA(char *buf, size_t atom_count,
                                    const size_t sample_count,
				    size_t atom_offset,
				    size_t step,
				    libera_hw_time_t *slmt)
{
    struct libera_dd_device  *dd  = &libera_dd;
    libera_dma_t *dma = &lgbl.dma;
    wait_queue_t wait;

    size_t samples_written = 0;
    unsigned long OBAtoms = 0;
    unsigned long fifo_atoms;
    register unsigned long i;
    ssize_t sync_ret;
    int ret = 0;
    size_t FirstAtomNo = (*slmt / LIBERA_DD_ATOM_SAMPLES) %
	LIBERA_DD_CIRCBUF_ATOMS;
    size_t AtomNo = FirstAtomNo;

    DEBUG2_ONLY(int sleep_count = 0);
    DEBUG2_ONLY(int dma_while_count = 0);

    PDEBUG3("slmt FirstAtomNo = 0x%08lx%08lx, %lu\n",
            ULL(*slmt), FirstAtomNo);

    /* Get status */
    dma->obFIFOstatus = readl(iobase + DD_OB_STATUS);
    PDEBUG3("Starting DMA OB transfer: OB_size = 0x%lx\n",
	    DD_OB_SIZE(dma->obFIFOstatus));

    /* Initialize DMA structures */
    dma->Overrun = FALSE;
    dma->DMAC_transfer = FALSE;
    dma->remaining = atom_count;
    PDEBUG3("Flushing DMA fifo...\n");
    flushDMA_FIFO(dma);
    libera_dma_get_DMAC_csize(dma);

    /* NOTE: OB-fifo size should be > 0 here, otherwise we would not get
     *       DD interrupt, however, this does not guarantee that
     *       dma->csize > 0 as well.
     *       This is highly unlikely, since SBC is slow compared to FPGA,
     *       but to eliminate the risk of getting DMA bus errors, we
     *       sync to OB-fifo size before starting dma transfers.
     */
    sync_ret = libera_OB_fifo_sync(&dma->obFIFOstatus);
    if ( sync_ret < 0 ) return sync_ret;
    // TODO: LIBERA_IOBASE is not configurable !!!
    libera_dma_command((LIBERA_IOBASE + DD_OB_FIFOBASE),
                       &dma->buf[dma->put],
                       dma->csize*sizeof(libera_atom_dd_t));

    /* Loop until the DMA transfer is over */
    do {
#if 0 // UNINTERRUPTIBLE
        /* Check fifo size & wait/sleep if neccessary */
        if (emptyDMA_FIFO(dma))
	{
	    init_waitqueue_entry(&wait,current);
	    add_wait_queue(&dd->DMA_queue,&wait);
	    for (;;)
	    {
		set_current_state(TASK_UNINTERRUPTIBLE);
		if (not_emptyDMA_FIFO(dma)) {
		    break;
		}

		if (!schedule_timeout(100)) {
		    PDEBUG("Timeout in file: %s, line: %d\n",
			       __FILE__, __LINE__);
		}

		DEBUG2_ONLY(
		    if ( sleep_count++ > 10000) {
			PDEBUG2("BREAKING DMA sleeping loop.\n");
			PDEBUG2("Error in file: %s, line: %d)\n",
				__FILE__, __LINE__);
			ret = -EDEADLK;
			break;
		    });
	    }
	    current->state = TASK_RUNNING;
	    remove_wait_queue(&dd->DMA_queue, &wait);
	}

        if (ret) return ret;
#else // INTERRRUPTIBLE
        /* Check fifo size & wait/sleep if neccessary */
        if (emptyDMA_FIFO(dma))
	{
	    init_waitqueue_entry(&wait,current);
	    add_wait_queue(&dd->DMA_queue,&wait);
	    for (;;)
	    {
	        set_current_state(TASK_INTERRUPTIBLE);
		if (not_emptyDMA_FIFO(dma)) {
		    break;
		}
		if (!signal_pending(current)) {
		    schedule();
		    continue;
		}
		// NOTE: This will only work for small read() requests.
		DEBUG2_ONLY(
		    if ( sleep_count++ > 10000) {
			PDEBUG2("BREAKING DMA sleeping loop.\n");
			PDEBUG2("Error in file: %s, line: %d)\n",
				__FILE__, __LINE__);
			ret = -EDEADLK;
			break;
		    });
		ret = -ERESTARTSYS;
		break;
	    }
	    set_current_state(TASK_RUNNING);
	    remove_wait_queue(&dd->DMA_queue, &wait);
	}

        if (ret) return ret;
#endif

        /* Get data from DMA fifo */
        fifo_atoms = lenDMA_FIFO(dma);
        PDEBUG3("DMA-OB: fifo_atoms = %lu\n", fifo_atoms);
        PDEBUG3("atom_count = %lu\n", atom_count);
        PDEBUG3("sample_count = %lu\n", sample_count);
        if (fifo_atoms)
	{
	    /* Empty DMA fifo, one by one atom */
	    for (i=0; i < fifo_atoms; i++) {
		if (OBAtoms < atom_count)
		{
                    PDEBUG3("OBAtoms, AtomNo = %lu, %lu\n",
                            OBAtoms, AtomNo);
		    if ( ((OBAtoms++ + FirstAtomNo) %
                          LIBERA_DD_CIRCBUF_ATOMS) == AtomNo ) {
			/* At least one sample in this atom -- transfer */
			while ( (atom_offset < LIBERA_DD_ATOM_SAMPLES) &&
                                (samples_written < sample_count) ) {
			    if (copy_to_user(buf,
					 ((char *)&dma->buf[dma->get] +
					 atom_offset*sizeof(libera_sample_t)),
					 sizeof(libera_sample_t))) {
				return -EFAULT;
			    }
			    else
    {
				samples_written++;
				buf += sizeof(libera_sample_t);
				atom_offset += step;
				*slmt += step;
			    }
			} // while()
			/* Refresh for the next "atom hunting" */
			AtomNo = ((*slmt / LIBERA_DD_ATOM_SAMPLES) %
			    LIBERA_DD_CIRCBUF_ATOMS);
			atom_offset = atom_offset % LIBERA_DD_ATOM_SAMPLES;
                        PDEBUG3("Next (slmt, AtomNo): 0x%08lx%08lx %lu\n",
                                ULL(*slmt), AtomNo);
		    }
		}
		/* Free this atom in the fifo */
		dma->get = (dma->get + 1) & LIBERA_DMA_FIFO_MASK;
	    }
	    PDEBUG2("Copied %u HBPP samples to Userland.\n", samples_written);
	}
	else
	{
	    /* DMA FIFO empty ?!
	     * This should never happen when we're awake.
	     */
	    printk(KERN_CRIT "DD read(): BUG: Awake when DMA FIFO empty ?!\n");
	    return -EFAULT;
	}

	/* Start DMA again if needed */
	if ( (!dma->DMAC_transfer) && (dma->remaining) ) {
	    /* Refresh status */
	    dma->obFIFOstatus = readl(iobase + DD_OB_STATUS);

	    /* Reading speed sync */
	    sync_ret = libera_OB_fifo_sync(&dma->obFIFOstatus);
	    if ( sync_ret < 0 ) return sync_ret;

	    libera_dma_get_DMAC_csize(dma);

	    /* NOTE: At this stage, it is still possible that dma->csize == 0.
	     *       If it equals zero, it can only mean that our DMA fifo
	     *       is full. In this case we just pass control to the
	     *       main DMA while loop to empty the DMA fifo first.
	     */
	    if (!(dma->csize > 0)) {
		if  ( (LIBERA_DMA_FIFO_ATOMS - 1 - lenDMA_FIFO(dma)) == 0)
		    continue;
	    }

	    /* NOTE: At this stage, dma->csize should not in any case be
	     *       dma->csize == 0. If it equals zero, we will get
	     *       DMA BUS ERROR -> print debug info.
	     */
	    DEBUG_ONLY(
	        if (!(dma->csize > 0)) {
		    PDEBUG("dma->csize = %ld\n",
			   dma->csize);
		    PDEBUG("dma->remaining = %ld\n",
			   dma->remaining);
		    PDEBUG("dma->obFIFOstatus = 0x%lx\n",
			   dma->obFIFOstatus);
		    PDEBUG("OB_size = %lu\n",
			   DD_OB_SIZE(dma->obFIFOstatus));
		    PDEBUG("OB_busy = %lu\n",
			   DD_OB_BUSY(dma->obFIFOstatus));
		    PDEBUG("tailDMA_FIFO(dma) = %d\n",
			   tailDMA_FIFO(dma));
		    PDEBUG("DMA fifo free = %d\n",
			   (LIBERA_DMA_FIFO_ATOMS - 1 - lenDMA_FIFO(dma)));
		    PDEBUG("DMA put = %lu\n", dma->put);
		    PDEBUG("DMA get = %lu\n", dma->get);
		    PDEBUG("OBAtoms = %lu\n", OBAtoms);
		    PDEBUG("samples_written = %lu\n",
			   (unsigned long)samples_written);
		});

	    // TODO: LIBERA_IOBASE is not configurable !!!
	    libera_dma_command((LIBERA_IOBASE + DD_OB_FIFOBASE),
			       (unsigned long)&dma->buf[dma->put],
			       dma->csize*sizeof(libera_atom_dd_t));
	}

	// NOTE: This will only work for small read() requests.
	DEBUG2_ONLY({
	    if ( dma_while_count++ > 10000000) {
		PDEBUG2("BREAKING DMA while loop (dma->remaining = %ld, lenDMA_FIFO(dma) = %d).\n", dma->remaining, lenDMA_FIFO(dma));
		PDEBUG2("Error in file: %s, line: %d)\n",
			__FILE__, __LINE__);
		return -EDEADLK;
	    }
	});

    } while ( (dma->remaining) ||
              (not_emptyDMA_FIFO(dma)) );


    /* End of DMA transfer
     * Check for OB_size inconsistency.
     */
    dma->obFIFOstatus = readl(iobase + DD_OB_STATUS);
    PDEBUG3("End of DMA transfer: OB_status = 0x%lx, OB_size = 0x%lx\n",
	    dma->obFIFOstatus,
	    DD_OB_SIZE(dma->obFIFOstatus));
    if (DD_OB_SIZE(dma->obFIFOstatus)) {
	/* This should never happen. */
	PDEBUG("DD read(): BUG: OB_size=0x%lx after DMA transfer.\n",
	       DD_OB_SIZE(dma->obFIFOstatus));
	PDEBUG("Error in file: %s, line: %d)\n",
	       __FILE__, __LINE__);
	return -EIO;
    }


    /* Check for FPGA size consistency */
    if (OBAtoms != atom_count)
    {
        PDEBUG("DD read(): FPGA size inconsistency: %d : %lu\n",
	       atom_count, OBAtoms);
	return -EFAULT;
    }

    return samples_written;
}


/** Libera HBPP DD Device: Called on read()
 *
 * Reads data from proper position in the circular buffer.
 * \param count denotes the No. of aoms to read in bytes +
 * sizeof(libera_timestamp_t).
 * Only "atom-aligned" read() requests are allowed.
 *
 * On success, number of read and returned bytes is returned.
 * On failure, meaningful negative errno is returned.
 *
 * For details see: Alessandro Rubini et al., Linux Device Drivers, pp. 64.
 */
ssize_t
libera_dd_read_specific(struct file *file, char *buf,
			size_t count, loff_t *f_pos)
{
    struct libera_dd_device *dev =
    	(struct libera_dd_device *) file->private_data;
    struct libera_event_device   *event   = &libera_event;
    libera_dd_local_t *dd_local = (libera_dd_local_t *)file->f_version;
    libera_Qtimestamp_t *Qts = &dd_local->Qts;

    ssize_t ret = 0;
    ssize_t CB_ret;
    ssize_t OB_ret = 0;
    ssize_t IRQ_ret;
    size_t sizeS, sizeSD, sizeA;
    size_t atom_offset, atom_start;
    libera_hw_time_t slmt;
    libera_hw_time_t slmt_HB;
    libera_hw_time_t HB_offset_samp =
        event->HB_offset_lmt*LIBERA_DD_MT_SAMPLES;
    libera_Ltimestamp_t ctime;
    long timeout_diff;
    long timeout_jiff;
    libera_hw_time_t dd_dummy;

    DEBUG2_ONLY(static int read_syscall_counter);
    PDEBUG2("Entering read() system call (N=%d).\n", read_syscall_counter++);

    /* Lock the whole device */
    if (mutex_lock_interruptible(&dev->sem))
	return -ERESTARTSYS;

    /* Sanity check regarding count */
    if ( count >
         (HBPP_MAX_SAMPLES*sizeof(libera_sample_t)) ) {
	PDEBUG("DD: read(): Parameter count too big.\n");
	ret = -EINVAL;
	goto out;
    }
    /* Filter out strange, non-atomically-dividable values. */
    if (count % sizeof(libera_sample_t)) {
	PDEBUG("DD: read(): Inapropriate count size.\n");
    	ret = -EINVAL;
	goto out;
    }

    if ( (Qts->relative < 0) || (Qts->relative >= LIBERA_DD_CIRCBUF_ATOMS) ) {
	PDEBUG("DD: read(): Inapropriate trigger relative offset.\n");
    	ret = -EINVAL;
	goto out;
    }

    /* Grab the last trigger -- the starting point in HB */
    memcpy(&Qts->lasttrig,
           getFromCircBuf(&event->paired_timestamps[T_TRIG_TRIGGER]),
           sizeof(libera_Ltimestamp_t));

    /* Make sure the HB is stopped */
    ret = libera_get_CTIME(&ctime);
    if (ret) goto out;
    timeout_diff = (Qts->lasttrig.lmt - ctime.lmt +
        LIBERA_DD_CIRCBUF_ATOMS*LIBERA_DD_ATOM_MTS/2);
    if (timeout_diff > 0) {
        timeout_jiff = (timeout_diff * FLMC_DECI_HZ * HZ)/flmcdHz + 1;
        if (timeout_jiff < HZ) {
            PDEBUG2("Waiting %ld jiffies (HB stop)\n", timeout_jiff);
            ret = libera_delay_jiffies_interruptible(timeout_jiff);
            if (ret) goto out;
        } else {
            PDEBUG("DD (timing): Timeout waiting for HB stop (%ld jiffies).\n",
                    timeout_jiff);
            ret = -EDEADLK;
            goto out;
        }
    }

    /* NOTE: Because of the virtual 4x sampling rate (phase shift
     *       on 4 channels), the LMT does not advance on every
     *       sample, but on every 4th sample. Thus the driver
     *       internally uses SLMT -- sample rate LMT -- as the highest
     *       resolution clock/position info.
     *       Since the decimation D = 1, the timestamp's MT and LMT should
     *       be the same. Instead of providing redundant info to the
     *       user, we store SLMT in the LMT part of the timestamp
     *       (->L.lmt), and LMT = MT in the MT part of the timestamp
     *       (->U.mt).
     *       SLMT on 64 bits will overflow 4x sonner than LMT, but
     *       that is still more than 1000 years @ 125Mhz MC machine
     *       clock...
     */
    Qts->U.mt = Qts->lasttrig.lmt + Qts->relative/LIBERA_DD_MT_SAMPLES;
    Qts->L.lmt = LIBERA_DD_MT_SAMPLES*Qts->lasttrig.lmt + Qts->relative;

    /* Define the missing parts of Qts (LST & ST) */
    slmt = Qts->L.lmt;
    ret = lmt2lst(&(Qts->U.mt), &(Qts->L.lst), &Qts->lasttrig);
    PDEBUG2("Qts(mt, lst, lasttrig_lmt, lasttrig_lst, relative): 0x%08lx%08lx, 0x%08lx%08lx, 0x%08lx%08lx, 0x%08lx%08lx, 0x%08lx%08lx\n",
            ULL(Qts->U.mt),
            ULL(Qts->L.lst),
            ULL(Qts->lasttrig.lmt),
            ULL(Qts->lasttrig.lst),
            ULL(Qts->relative));

    if (ret) {
	PDEBUG2("DD read(): Error in lmt2lst().\n");
	goto out;
    }
    lst2st(&(Qts->L.lst), &(Qts->U.st));

    /* Store timestamp for later ioctl() retrieval */
    memcpy(&dd_local->tstamp, &(Qts->U), sizeof(libera_timestamp_t));

    /* Transform the request in atom units */
    slmt_HB = slmt - HB_offset_samp;
    atom_offset =  slmt_HB % LIBERA_DD_ATOM_SAMPLES;
    atom_start  = (slmt_HB / LIBERA_DD_ATOM_SAMPLES) % LIBERA_DD_CIRCBUF_ATOMS;
    sizeS = count/sizeof(libera_sample_t);
    sizeSD = sizeS; //No decimation in HBPP
    sizeA = ((sizeSD + atom_offset) / LIBERA_DD_ATOM_SAMPLES) + 1; // ceil()

    PDEBUG2("HB_offset_samp = 0x%08lx%08lx\n", ULL(HB_offset_samp));
    PDEBUG2("slmt_HB = 0x%08lx%08lx\n", ULL(slmt_HB));
    PDEBUG2("atom_offset = %u\n", atom_offset);
    PDEBUG2("atom_start = %u\n", atom_start);
    PDEBUG2("sizeS = %u\n", sizeS);
    PDEBUG2("sizeSD = %u\n", sizeSD);
    PDEBUG2("sizeA = %u\n", sizeA);

    /* Zero-atom-length request returns no data */
    if (!sizeA)
	goto out_zero;

    /* Write command to CB fifo (initiate data transfer) */
    CB_ret = libera_dd_write_CBfifo_HBPP(0,
                                         atom_start,
                                         1,
                                         1,
                                         sizeA);
    if (CB_ret < 0)
    {
	ret = CB_ret;
	goto out;
    }

    /* Wait for DD interrupt */
    PDEBUG3("DD read(): Waiting for DD interrupt...\n");
    IRQ_ret = getBlockedFromFIFO(&dev->dd_irqevents,
				 &dev->DD_queue,
				 &dd_dummy);
    if (IRQ_ret) goto out;
    PDEBUG3("DD read(): Awoken by DD interrupt handler.\n");

    /* And now begin the data transfer */
    OB_ret = libera_dd_transfer_OBfifoToUser_DMA(buf, sizeA, sizeS,
						 atom_offset,
						 1,
						 &slmt_HB);

    /* Re-enable DD interrupt */
    PDEBUG3("DD read(): Re-enabling DD interrupt.\n");
    set_irqMask((get_irqMask() | LIBERA_INTERRUPT_DD_MASK));

    if (OB_ret < 0)
    {
	ret = OB_ret;
	goto out;
    }
    else
	ret += OB_ret * sizeof(libera_sample_t);

    PDEBUG3("DD (%p): Read %lu samples from MT = 0x%08lx%08lx\n",
	   file, (unsigned long)OB_ret, ULL(Qts->U.mt));

 out_zero:
    /* MT & LMT increment for future read() requests */
    Qts->U.mt += (slmt_HB + HB_offset_samp - Qts->L.lmt) /
        LIBERA_DD_MT_SAMPLES;
    Qts->L.lmt += slmt_HB + HB_offset_samp;
    PDEBUG3("Incremented f_pos SLMT = 0x%08lx%08lx\n", ULL(Qts->L.lmt));

 out:
    PDEBUG2("read() returning %d\n", ret);
    mutex_unlock(&dev->sem);
    return ret;
}


ssize_t
libera_acq_pm(void)
{
    return 0;
}

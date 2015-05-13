/* $Id: dpp.c 2192 2008-10-07 09:13:06Z matejk $ */

/** \file dpp.c */
/** Implements Libera Digital Pulse Processor (DPP) specifics. */

/*
LIBERA - Libera GNU/Linux device driver
Copyright (C) 2004-2007 Instrumentation Technologies

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
#include <linux/interrupt.h>
#include <linux/delay.h>

#include "libera_kernel.h"
#include "libera_event.h"
#include "libera_dd.h"
#include "libera_sa.h"
#include "libera.h"


/** Global Libera description */
libera_desc_t libera_desc = {
    magic: LIBERA_MAGIC,
    name: "DPP",
    fpga: "Unknown",
};


/** Libera Configuration Device: Specific GET methods.
 *
 * Takes care of CFG GET actions on DPP Libera family member specific
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


/** Libera Configuration Device: Specific SET methods.
 *
 * Takes care of CFG SET actions on DPP Libera family member specific
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


/** Initialize Libera DPP specific internal device structures.
 *
 * Initializes Libera DPP specific internal device structures. 
 */
int libera_init_specific(void)
{
    int ret = 0;

    return ret;
}


/** Takes care of proper trigger sequence and can signal to ignore a
 *  certain trigger.
 */
int
libera_valid_trigger(struct libera_event_device *event, 
                     int trigno)
{
    /* No restriction to trigger sequence in DPP */
    return 1;
}


/** Libera DPP specific SC trigger handler */
void 
libera_SCtrig_specific(struct libera_event_device *event,
		       const libera_hw_time_t *stamp,
		       const libera_hw_time_t *stamp_raw,
		       unsigned long m, unsigned int i)
{
    switch(i) 
    {
      
    default:
        if (putToFIFO(& event->sc_timestamps[i], *stamp) < 0 && 
	    (event->sc_trigVec & m)) //this should never happen
	  printk(KERN_CRIT "libera: SC irq: FIFO %d overflow.\n",i);
    }
}

/** Libera DPP specific MC trigger handler */
void 
libera_MCtrig_specific(struct libera_event_device *event,
		       const libera_hw_time_t *stamp,
		       const libera_hw_time_t *stamp_raw,
		       unsigned long m, unsigned int i)
{
    switch(i) 
    {
	
    default:
	if (putToFIFO(& event->mc_timestamps[i], *stamp) < 0 && 
	    (event->mc_trigVec & m)) //this should never happen
	    printk(KERN_CRIT "libera: MC irq: FIFO %d overflow.\n",i);
    }
}


/** Libera DPP specific SC helper IRQ handler */
void
libera_schelp_handler_specific(struct libera_event_device  *event)
{
    /* Unused. */
}


/** Libera DPP specific MC helper IRQ handler */
void
libera_mchelp_handler_specific(struct libera_event_device  *event)
{
    /* Unused. */
}


/** Libera DPP specific SC SELF_INC trigger handler */
void
libera_sc_selfinc_specific(struct libera_event_device  *event)
{
    /* Unused. */
}


/** Libera DPP specific MC SELF_INC trigger handler */
void
libera_mc_selfinc_specific(struct libera_event_device  *event)
{
    /* Unused. */
}


/** Increments SA circular buffer pointer in a way that nobody sees
 *  an intermediate value.
 */
static inline void libera_sa_incr_atom(volatile libera_atom_sa_t **atomp)
{
    struct libera_sa_device  *dev  = &libera_sa;

    if (*atomp == (dev->buf + SA_FIFO_DEPTH - 1))
        *atomp = dev->buf;  /* Wrap */
    else
        (*atomp)++;    
}

/** Libera SA interrupt handler DPP specific Top Half (TH).
 *
 * Takes care of reading the data from SA fifo on FPGA to internal buffer.
 * The rest is done in the tasklet Bottom Half (BH).
 */
void
libera_sa_handler_specific(void)
{
    struct libera_sa_device  *dev  = &libera_sa;
    unsigned FIFOstatus;
    int i;
    DEBUG3_ONLY(static int count);

    /* Read status once */ 
    FIFOstatus = readl(iobase + SA_STATUS);
    PDEBUG3("SA irq: Entering SA handler: status = 0x%08x\n", FIFOstatus);

    /* Check for Input Overrun */
    if (SA_INOVR(FIFOstatus))
	dev->inputovr = TRUE;

    /* Read all the available atoms from FIFO.
     * Reading in one block (multiple atoms) for greater efficiency is 
     * out of the question, beacuse of the circular buffer between top
     * and bottom half!
     */
    for (i = 0; i < SA_SIZE(FIFOstatus); i++)
        {
	DEBUG3_ONLY(
	    if ( !(count++ % 10) ) {
		PDEBUG3("Reading from %p and %p\n", 
			(unsigned long *)dev->buf_head,
			(unsigned long *)dev->buf_head +
			sizeof(libera_atom_sa_t)/sizeof(unsigned long));
	    }
	    );
	/* Read SA data first */
	libera_readlBlock((unsigned long *)(iobase + SA_FIFO_BASE),
			  (unsigned long *)dev->buf_head,
			  (sizeof(libera_atom_sa_t)/sizeof(unsigned long)));
	libera_sa_incr_atom(&dev->buf_head);

    } 
    FIFOstatus = readl(iobase + SA_STATUS);
    /* Check for Input Overrun */
    if (SA_INOVR(FIFOstatus))
	dev->inputovr = TRUE;
    PDEBUG3("SA IRQ: Read 0x%x samples. New Status = 0x%08x\n", i, FIFOstatus);
    
    /* NOTE: At this stage, the SA_SIZE(FIFOstatus) might be =/= 0,
     *       but we will leave it for the next inerrupt.
     *       Think: FIFO interrupt treshold.
     */
}


/** Libera SA interrupt handler tasklet DPP specific Bottom Half (BH).
 *
 * Takes care of writing the data to SA pipes (software buffers). 
 * The responsibility of this function is to signal FPGA fifo overflows &
 * software pipe overflows as well.
 * 
 * This function (BH) can be interrupted, by its corresponding TH.
 * Data corruption prevention is therefore essential and solved by using 
 * a circular buffer (dev->buf) betwen top and bottom half. 
 * This buffer has got nothing to do with the SA fifos between bottom half and 
 * reader(s) in userland.
 */
void libera_sa_do_tasklet_specific(unsigned long data)
{
    struct libera_sa_device  *dev  = &libera_sa;
    
    int count = 0;
    int pipe_no;
    ssize_t ret;
    
    /* FIFO Input Overrun check */
    if (dev->inputovr)
    {
	dev->inputovr = FALSE;
	PDEBUG("WARNING: FPGA SA buffer overflow.\n");
	libera_send_event(LIBERA_EVENT_OVERFLOW,
			  LIBERA_OVERFLOW_SA_FPGA);
    }

    do 
    {
	/* Dispatch data to SA pipes */
	for (pipe_no = 0; pipe_no < LIBERA_SA_MAX_READERS; pipe_no++) {
	    if (dev->pipe.sa_pipe[pipe_no])
	    {
		/* We need to wake up the reader as libera_pipe_write() 
		 * does not take care of that already. 
		 */
		ret = libera_pipe_write(dev->pipe.sa_pipe[pipe_no], 
					(char *)(dev->buf_tail), 
					sizeof(libera_atom_sa_t));
		if (ret == -EFAULT) {
		    PDEBUG("WARNING: buffer overflow in SA pipe #%d\n", 
			   pipe_no);
		    libera_send_event(LIBERA_EVENT_OVERFLOW, 
				      LIBERA_OVERFLOW_SA_DRV);
		}
		else {
			wake_up_interruptible(&dev->pipe.sa_pipe[pipe_no]->wait);
		}    
	   }
	}
	/* Increment buffer tail pointer */
	libera_sa_incr_atom(&dev->buf_tail);
        count++;
    } while (dev->buf_tail != dev->buf_head);
    libera_send_event(LIBERA_EVENT_SA, count);
}

/** Libera Interlock handler DPP specific.
 *
 */
void libera_ilk_handler_specific(unsigned long data)
{
    /* Unused. */
}


/** Libera DPP DD Device: Called on lseek()
 *
 * For details see: Alessandro Rubini et al., Linux Device Drivers, pp. 163.
 */
loff_t
libera_dd_llseek_specific(struct file *file, loff_t time, int whence)
{
    /* Unseekable by default */
    return -ESPIPE; 
}


/** Libera DPP DD Device: Called on read()
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
    /* Unused. */
    return -ENOSYS;
}


/** Acqires PM data into PM buffer. 
 *
 * Takes care of reading the data from History Buffer to internal
 * buffer for later retrieval using read() on PM device. 
 */
ssize_t
libera_acq_pm(void)
{
    return 0;
}

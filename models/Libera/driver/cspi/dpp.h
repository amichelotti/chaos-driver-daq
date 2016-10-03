/* $Id: dpp.h 2192 2008-10-07 09:13:06Z matejk $ */

/** \file dpp.h */
/** Public include file for Libera Digital Pulse Processor (DPP). */

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

#ifndef _DPP_H_
#define _DPP_H_

/** Libera magic number */
#define LIBERA_MAGIC 0xdd00


/** DPP specific CFG parameters. */
typedef enum {
    LIBERA_CFG_NONE = LIBERA_CFG_CUSTOM_FIRST, //!< Default.
} LIBERA_CFG_DPP_GENERIC;


/** Libera DPP  sample */
typedef struct {
    unsigned long sample; // Should be short, but... minimal size = 4!
} libera_sample_t;

/* Atom vs. sample vs. longwords & bytes constants */
#define LIBERA_DD_CIRCBUF_ATOMS  (32*1024*1024) //<! No. of atoms in HB.
#define LIBERA_DD_MT_SAMPLES           1        //<! No. of samples in 1 MT.
#define LIBERA_DD_ATOM_MTS             1        //<! No. of MTs in 1 atom.
#define LIBERA_DD_ATOM_SAMPLES   (LIBERA_DD_ATOM_MTS*LIBERA_DD_MT_SAMPLES) //<! No. of samples in 1 ATOM.
/** No. of DD atoms in one DMA block */
#define LIBERA_DMA_BLOCK_ATOMS  512
/* DMA buffer size = PAGE_SIZE << LIBERA_DMA_PAGE_ORDER */
#define LIBERA_DMA_PAGE_ORDER     5
/** No. of DD atoms in DMA fifo  - Must be power of 2 and sync
 *  with LIBERA_DMA_PAGE_ORDER */
#define LIBERA_DMA_FIFO_ATOMS  16384
#define LIBERA_DMA_FIFO_MASK (LIBERA_DMA_FIFO_ATOMS - 1)
/** Maximal No. of samples in read(request) */
#define DPP_MAX_SAMPLES   (2*1024*1024)

/** Libera DPP  Data on Demand (DD) atom */
/*  NOTE: The size of libera_atom_dd_t structure is important. 
 *        The minimal lenght of libera_atom_dd_t is 4 bytes and it must be 
 *        integer (4-byte) aligned!
 */
typedef struct {
    unsigned long sample; // Should be short, but... minimal size = 4!
} libera_atom_dd_t;


/* Libera DPP ADC-rate Data (ADC) atom */
/* NOTE: The size of libera_atom_adc_t structure is important. 
 *       The minimal lenght of libera_atom_adc_t is 4 bytes and it must be 
 *       integer (4-byte) aligned!
 */
typedef void* libera_atom_adc_t;  // Unused


/** Libera DPP Slow Acquisition (SA) atom */
/*  NOTE: The size of libera_atom_sa_t structure is important. 
 *        PAGE_SIZE MUST be a multiple of sizeof(libera_atom_sa_t) for proper 
 *        buffer wrapping. Pad this structure to the nearest common 
 *        denominator of PAGE_SIZE and sizeof(libera_atom_sa_t).
 *        The minimal lenght of libera_atom_sa_t is 4 bytes and it must be 
 *        integer (4-byte) aligned!
 */
typedef struct {
    unsigned long sample; // Should be short, but... minimal size = 4!
} libera_atom_sa_t;

#define SA_FIFO_DEPTH  (16*1024) // Top/botoom half SA buffer length in atoms.
#define SA_SIZE_WIDTH  13
#define SA_LOG         (16*1024) // SA terminal pipe buffer depth. Must be 2^n.

#endif // _DPP_H_

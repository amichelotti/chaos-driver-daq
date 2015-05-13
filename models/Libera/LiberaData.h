/* 
 * File:   LiberaData.h
 * Author: michelo
 *
 * Created on May 11, 2015, 11:26 AM
 */

#ifndef LIBERADATA_H
#define	LIBERADATA_H

#define LIBERA_IOP_TURN_BY_TURN_DATA 0x1
#define LIBERA_IOP_TURN_BY_TURN_DATA_TRIGGERED 0x2
#define LIBERA_IOP_DATA_DECIMATED 0x4
#define LIBERA_IOP_DATA_SLOW_DAQ 0x8
#define LIBERA_IOP_DATA_ADC_OUT 0x10
#define LIBERA_IO_DATA_ACQUIRE 0x20




#ifdef	__cplusplus
extern "C" {
#endif

    struct LiberaData{
        int32_t va;
        int32_t vb;
        int32_t vc;
        int32_t vd;
        int32_t x,y,q;
        int32_t sum;
        int32_t q1,q2;
    } ;
    
    typedef union {
        struct LiberaData libera;
        int32_t raw[10];
    } liberaData_t;

    
#ifdef	__cplusplus
}
#endif

#endif	/* LIBERADATA_H */


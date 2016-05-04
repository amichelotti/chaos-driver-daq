/*
 * Copyright (c) 2010 Instrumentation Technologies.
 * All Rights Reserved.
 *
 * $Id: serial_flash.h 11980 2011-06-15 10:36:13Z matej.kenda $
 */

#ifndef SERIAL_FLASH_H
#define SERIAL_FLASH_H

/*-----------------------------------------------------------------------------*/
/* flash spec.                                                                 */

/*
 *  SPI packet lengths
 */
#define FLASH_OP_CODE_LEN 1
#define FLASH_ADDR_LEN    3
#define FLASH_PAGE_LEN    256
//#define FLASH_BLOCK_LEN           // not in use !!

/*
 *  FLASH standard commands.
 */
#define FLASH_WRITE_STATUSREG       0x01
#define FLASH_WRITE_DATA            0x02
#define FLASH_READ_DATA             0x03
#define FLASH_WRITE_DISABLE         0x04
#define FLASH_READ_STATUSREG        0x05
#define FLASH_WRITE_ENABLE          0x06
#define FLASH_FASTREAD_DATA         0x0B
#define FLASH_READ_ID               0x9F
#define FLASH_BULK_ERASE            0xC7
#define FLASH_SECTOR_ERASE          0xD8

/*
 *  FLASH status flags.
 */
#define FLASH_WRITE_IN_PROCESS      0x01
#define FLASH_WRITE_ENABLED         0x02

/*
 *  FLASH sector sizes
 */
#define FLASH_SECTOR_SIZE_64KB      0x010000 //  64KByte
#define FLASH_SECTOR_SIZE_256KB     0x040000 // 256KByte

/*
 *  FLASH addr calc.
 */
#define FLASH_SECTOR_START_ADDR(_x_,_sectorSize_) ((_x_)*(_sectorSize_))
#define FLASH_SECTOR_END_ADDR(_x_,_sectorSize_) (FLASH_SECTOR_START_ADDR((_x_+1),(_sectorSize_))-1)

/*-----------------------------------------------------------------------------*/
/* flash id's for supported flash devices                                      */

/*
 *  flash id structure
 */
typedef struct {
    unsigned char manId;     // manufacturer ID
    unsigned char memType;   // memory type
    unsigned char memCap;    // memory capacity
} tFlashId;

// notice: manufacturer identification is assigned by JEDEC

// invalid IDs
#define FLASH_INVALID_MAN_ID_1          0x00
#define FLASH_INVALID_MAN_ID_2          0xFF

// valid manufacturer IDs
#define FLASH_SPANSION_ID               0x01
#define FLASH_NUMONYX_OR_ST_MAN_ID      0x20
#define FLASH_INTEL_MAN_ID              0x89

// mem. types
// these were equal to man. id on all chips we had !!

// mem. capacities
// numonyx or st
#define FLASH_NUMONYX_OR_ST_MEM_CAPACITY_256MBIT          0x19 // not yet supported !!
#define FLASH_NUMONYX_OR_ST_MEM_CAPACITY_128MBIT          0x18
#define FLASH_NUMONYX_OR_ST_MEM_CAPACITY_64MBIT           0x17
#define FLASH_NUMONYX_OR_ST_MEM_CAPACITY_64MBIT_          0x16 // used on old boards
                                                               // not sure about size
                                                               // was it 32MBit ?

// intel
#define FLASH_INTEL_MEM_CAPACITY_64MBIT                   0x13

/*-----------------------------------------------------------------------------*/
/* flash configurations for different flash devices                            */



/*
 *  basic device storage information
 */
typedef struct {
    unsigned int sectorSize;
    unsigned int numOfSectors;
    unsigned int maxWriteLen;
    unsigned int maxReadLen;
} tStorageDeviceInfo;

typedef struct {
    int (*ready)(void);
    int (*erase)(tStorageDeviceInfo * a_info, unsigned int a_address);
    int (*read)(tStorageDeviceInfo * a_info, unsigned int a_address, unsigned char *a_data, unsigned int a_length);
    int (*write)(tStorageDeviceInfo * a_info, unsigned int a_address, unsigned char *a_data, unsigned int a_length);
} tStorageDeviceFun;

/*
 *  storage device section information
 */
typedef struct {
    unsigned int startSector;
    unsigned int endSector;
    unsigned int startAddr;
    unsigned int endAddr;
} tStorageSectionInfo;
/*
typedef struct {
    unsigned char type;
    unsigned char bus;
    unsigned char addr;
} tStorageDeviceDesc;
*/
/*
 *  storage device basic func.
 */

/*
 *  storage device
 */
// storage device
typedef struct {
    tStorageDeviceInfo  info;
    tStorageDeviceFun   fun;
    tStorageSectionInfo fpgaImage;
    tStorageSectionInfo sel;
} tStorageDevice;



/*
 *  ### common ###
 */
#define FPGA_START_SECTOR_CFG 0

/*
 *  ### 64kb config. ###
 */
#define FPGA_START_SECTOR_CFG_64KB FPGA_START_SECTOR_CFG
#define FPGA_END_SECTOR_CFG_64KB 120

#define SEL_START_SECTOR_CFG_64KB  124
#define SEL_END_SECTOR_CFG_64KB    125

// see: c_flashCfg64Kb

/*
 *  ### 256kb config. ###
 */
#define FPGA_START_SECTOR_CFG_256KB FPGA_START_SECTOR_CFG
#define FPGA_END_SECTOR_CFG_256KB 60

#define SEL_START_SECTOR_CFG_256KB  61
#define SEL_END_SECTOR_CFG_256KB    61

// see: c_flashCfg256Kb

//----------------------------------------------------------------------------

#endif /*SERIAL_FLASH_H*/

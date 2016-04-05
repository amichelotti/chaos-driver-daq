/*
 * Copyright (c) 2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: i2c_device_spd_eeprom.h 19990 2014-01-07 09:35:55Z damijan.skvarc $
 */


#ifndef SPD_EEPROM_H
#define SPD_EEPROM_H

#include "bmc/i2c_device.h"

/*-----------------------------------------------------------------------------*/

/* I2C SpdEeprom chip address. */

#define SPD_EEPROM_MEM_I2C_ADDRESS  0x53 //0x50 //0x28 ///< memory address       0101|S2=0|S1=1|S0=1
#define SPD_EEPROM_TS_I2C_ADDRESS   0x33               ///< temp. sensor address 0011|S2=0|S1=1|S0=1

#define SPD_EEPROM_SIZE_IN_BYTES    2048 ///< 2kb

/*-----------------------------------------------------------------------------*/
struct tDdr3RegMap {
    /* General Section: Bytes 0-59 */
    unsigned char info_size_crc;   /*  0 # bytes written into serial memory,
                                             CRC coverage */
    unsigned char spd_rev;         /*  1 Total # bytes of SPD mem device */
    unsigned char mem_type;        /*  2 Key Byte / Fundamental mem type */
    unsigned char module_type;     /*  3 Key Byte / Module Type */
    unsigned char density_banks;   /*  4 SDRAM Density and Banks */
    unsigned char addressing;      /*  5 SDRAM Addressing */
    unsigned char module_vdd;      /*  6 Module nominal voltage, VDD */
    unsigned char organization;    /*  7 Module Organization */
    unsigned char bus_width;       /*  8 Module Memory Bus Width */
    unsigned char ftb_div;         /*  9 Fine Timebase (FTB)
                                             Dividend / Divisor */
    unsigned char mtb_dividend;    /* 10 Medium Timebase (MTB) Dividend */
    unsigned char mtb_divisor;     /* 11 Medium Timebase (MTB) Divisor */
    unsigned char tCK_min;         /* 12 SDRAM Minimum Cycle Time */
    unsigned char res_13;          /* 13 Reserved */
    unsigned char caslat_lsb;      /* 14 CAS Latencies Supported,
                                             Least Significant Byte */
    unsigned char caslat_msb;      /* 15 CAS Latencies Supported,
                                             Most Significant Byte */
    unsigned char tAA_min;         /* 16 Min CAS Latency Time */
    unsigned char tWR_min;         /* 17 Min Write REcovery Time */
    unsigned char tRCD_min;        /* 18 Min RAS# to CAS# Delay Time */
    unsigned char tRRD_min;        /* 19 Min Row Active to
                                             Row Active Delay Time */
    unsigned char tRP_min;         /* 20 Min Row Precharge Delay Time */
    unsigned char tRAS_tRC_ext;    /* 21 Upper Nibbles for tRAS and tRC */
    unsigned char tRAS_min_lsb;    /* 22 Min Active to Precharge
                                             Delay Time */
    unsigned char tRC_min_lsb;     /* 23 Min Active to Active/Refresh
                                             Delay Time, LSB */
    unsigned char tRFC_min_lsb;    /* 24 Min Refresh Recovery Delay Time */
    unsigned char tRFC_min_msb;    /* 25 Min Refresh Recovery Delay Time */
    unsigned char tWTR_min;        /* 26 Min Internal Write to
                                             Read Command Delay Time */
    unsigned char tRTP_min;        /* 27 Min Internal Read to Precharge
                                             Command Delay Time */
    unsigned char tFAW_msb;        /* 28 Upper Nibble for tFAW */
    unsigned char tFAW_min;        /* 29 Min Four Activate Window
                                             Delay Time*/
    unsigned char opt_features;    /* 30 SDRAM Optional Features */
    unsigned char therm_ref_opt;   /* 31 SDRAM Thermal and Refresh Opts */
    unsigned char res_32_59[28];   /* 32-59 Reserved, General Section */

    /* Module-Specific Section: Bytes 60-116 */
    union {
        struct {
            /* 60 (Unbuffered) Module Nominal Height */
            unsigned char mod_height;
            /* 61 (Unbuffered) Module Maximum Thickness */
            unsigned char mod_thickness;
            /* 62 (Unbuffered) Reference Raw Card Used */
            unsigned char ref_raw_card;
            /* 63 (Unbuffered) Address Mapping from
                              Edge Connector to DRAM */
            unsigned char addr_mapping;
            /* 64-116 (Unbuffered) Reserved */
            unsigned char res_64_116[53];
        } unbuffered;
        struct {
            /* 60 (Registered) Module Nominal Height */
            unsigned char mod_height;
            /* 61 (Registered) Module Maximum Thickness */
            unsigned char mod_thickness;
            /* 62 (Registered) Reference Raw Card Used */
            unsigned char ref_raw_card;
            /* 63 DIMM Module Attributes */
            unsigned char modu_attr;
            /* 64 RDIMM Thermal Heat Spreader Solution */
            unsigned char thermal;
            /* 65 Register Manufacturer ID Code, Least Significant Byte */
            unsigned char reg_id_lo;
            /* 66 Register Manufacturer ID Code, Most Significant Byte */
            unsigned char reg_id_hi;
            /* 67 Register Revision Number */
            unsigned char reg_rev;
            /* 68 Register Type */
            unsigned char reg_type;
            /* 69-76 RC1,3,5...15 (MS Nibble) / RC0,2,4...14 (LS Nibble) */
            unsigned char Ddr3rcw[8];
        } registered;
        unsigned char uc[57]; /* 60-116 Module-Specific Section */
    } mod_section;

    /* Unique Module ID: Bytes 117-125 */
    unsigned char mmid_lsb;        /* 117 Module MfgID Code LSB - JEP-106 */
    unsigned char mmid_msb;        /* 118 Module MfgID Code MSB - JEP-106 */
    unsigned char mloc;            /* 119 Mfg Location */
    unsigned char mdate[2];        /* 120-121 Mfg Date */
    unsigned char sernum[4];       /* 122-125 Module Serial Number */

    /* CRC: Bytes 126-127 */
    unsigned char crc[2];          /* 126-127 SPD CRC */

    /* Other Manufacturer Fields and User Space: Bytes 128-255 */
    unsigned char mpart[18];       /* 128-145 Mfg's Module Part Number */
    unsigned char mrev[2];         /* 146-147 Module Revision Code */

    unsigned char dmid_lsb;        /* 148 DRAM MfgID Code LSB - JEP-106 */
    unsigned char dmid_msb;        /* 149 DRAM MfgID Code MSB - JEP-106 */

    unsigned char msd[26];         /* 150-175 Mfg's Specific Data */
    unsigned char cust[80];        /* 176-255 Open for Customer Use */
};
/*-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------*/
struct tSpdEepromRegMap
{
    union {
        unsigned char eepromMemSpace[SPD_EEPROM_SIZE_IN_BYTES];
        tDdr3RegMap ddr3;
    } regMap;
};


static_assert(sizeof(tSpdEepromRegMap) == SPD_EEPROM_SIZE_IN_BYTES, "tSpdEepromRegMap register map size mismatch");
static_assert(sizeof(tDdr3RegMap)      == 256,                      "tDdr3RegMap register map size mismatch");


/*-----------------------------------------------------------------------------*/
namespace bmc {

    class SpdEeprom : public I2cDevice
    {
    public:
        SpdEeprom(BusController  &a_controller,
                  std::string   a_name,
                  BusDeviceId   a_id,
                  uint8_t       a_device_address,
                  uint8_t       a_bus_address);
        virtual ~SpdEeprom();

        static const uint8_t c_i2cAddr;

        bool SpdEepromReadDdrMemoryMap(tSpdEepromRegMap& a_memMap);

    protected:

    private:
        U_TEST_FRIEND

        /* Helpers */
        int Crc16(const char *a_ptr, int a_count);
        bool Ddr3SpdCheck(tDdr3RegMap* a_spd);

        /* basic i2c read & write functions */
        void SpdEepromWrite(uint8_t a_regAddr, uint8_t a_data);
        uint8_t SpdEepromRead (uint8_t a_regAddr);
        bool SpdEepromIsPresent();

        tSpdEepromRegMap    *m_regMapPtr;
    };
}

#endif // SPD_EEPROM_H

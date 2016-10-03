/*
 * Copyright (c) 2011 Instrumentation Technologies.
 * All Rights Reserved.
 *
 * $Id: ipmi_itech_extensions.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef IPMI_ITECH_EXTENSIONS_H
#define IPMI_ITECH_EXTENSIONS_H

#include <stdint.h>
#include <ipmi-ext/ipmi_itech_types.h>

//----------------------------------------------------------------------------
// packing alignment for IPMI data structures must be 1 byte
#ifdef CROSSWORKS
#pragma pack(push,1)
#endif
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Returned IPMI cmd. type.
typedef enum {
  e_stdIpmiCmd  = 0,   ///< std. cmd will be returned
  e_itechExtCmd = 1    ///< ext. i-tech cmd will be returned
} ipmi_cmd_ext_t;

#define ITECH_CMD_EXT_CODE 0x0e

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// IPMI PlatformB definitions for IPMI_NETFN_APP.

/**
 * IPMI PlatformB IPMI_NETFN_APP command extension to get
 * brief status of all system.
 * @note ARM LPC2xxx CPU is using Little-Endian byte order.
 */
#define BMC_GET_BRIEF_STATUS   0xA0

/**
 * Structure for custom PlatformB BMC_GET_BRIEF_STATUS.
 */
struct ipm_devid_briefsts_rsp {
    uint8_t present_boards; ///< Bit-set value indicating presence of boards in uTCA backplane.
    uint8_t mgmt_power; ///< Bit-set value of ICB Management power control register.
    uint8_t main_power; ///< Bit-set value of ICB Main power (12V) control register.
    uint8_t power_fault; ///< Bit-set value of ICB Power fault status register.
    uint8_t enabled_boards; ///< Bit-set value of ICB Enabled boards status register.
    uint8_t power_state; ///< ICB board current power state.
} __attribute__ ((packed));

typedef struct ipm_devid_briefsts_rsp ipm_devid_briefsts_rsp_t;

/**
 * Extended PlatformB IPMI device identification information.
 */
struct ipm_devid_ext_rsp {
    // standard part of the response
    struct ipm_devid_rsp    std; ///< Standard IPMI device identification members.

    // Non-standard part
    uint8_t     app_id;         ///< Application identifier.
    uint8_t     subsys_id;      ///< Subsystem identifier.
    uint8_t     startup_order;  ///< This satellite board startup order.

    uint8_t     state;          ///< Current state of board.
    uint16_t    status;         ///< Reflecting status of board.
    uint8_t     info_flags;     ///< Flags used to indicate various information regarding board.
} __attribute__ ((packed));


struct ipm_devid_ext_rsp_libera_base_2_2 {
    // standard part of the response
    struct ipm_devid_rsp    std; ///< Standard IPMI device identification members.

    // Non-standard part
    uint8_t     state; ///< Current state of board.
    uint8_t     geo_addr; ///< Board GEO position on uTCA backplane.
    uint16_t    status; ///< Reflecting status of board.
    uint8_t     startup_order; ///< This satellite board startup order.
    uint8_t     info_flags; ///< Flags used to indicate various information regarding board.
    uint8_t     app_id; ///< Application identifier.
    uint8_t     subsys_id; ///< Subsystem identifier.
    uint8_t     hw_revision; ///< This satellite board HW revision.
    uint8_t     fw_type; ///< Firmware type.
} __attribute__ ((packed));

typedef struct ipm_devid_rsp ipm_devid_rsp_t;
typedef struct ipm_devid_ext_rsp ipm_devid_ext_rsp_t;
typedef struct ipm_devid_ext_rsp_libera_base_2_2 ipm_devid_ext_rsp_libera_base_2_2_t;

typedef struct ipm_selftest_rsp ipm_selftest_rsp_t;

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// IPMI PlatformB definitions for IPMI_NETFN_PICMG.

#define HPM_ITECH_DEBUG     0xA0

#define HPMFWUPG_COMP_DUMP_FWDATA 11 // custom hpm dump control code.
// PlatformB HPM structure for HPMFWUPG_COMP_DUMP_FWDATA control code.
struct HpmfwupgGetFirmwareData {
   struct HpmfwupgGetComponentPropertiesReq proprq;
   uint8_t addr1;
   uint8_t addr2;
   uint8_t addr3;
}__attribute__ ((packed));

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// IPMI PlatformB definitions for SEL (IPMI_NETFN_STORAGE).
/**
 * @brief IPMI standard SEL information structure.
 * @note Members of this structure were reverse engineered from ipmi_sel.c.
 */
struct sel_info {
    uint8_t     sel_version;
    uint16_t    entries_number;
    uint16_t    free_space_in_bytes;
    uint32_t    most_recent_addition_timestamp;
    uint32_t    most_recent_erase_timestmap;
    uint8_t     operation_support;
} __attribute__((packed));


struct sel_add_entry_rsp {
    uint16_t entry_id;
} __attribute__ ((packed));

struct sel_get_next_entry_rsp {
    uint16_t next_id;
    struct sel_event_record sel_entry;
} __attribute__ ((packed));

struct sel_reserve_rsp {
    uint16_t reserve_id;
} __attribute__ ((packed));

struct sel_set_time_req {
    uint32_t timesec;
} __attribute__ ((packed));

struct sel_get_time_rsp {
    uint32_t timesec;
} __attribute__ ((packed));

#define SEL_CODE_CLEAR   {'C', 'L', 'R', 0xAA}

struct sel_clear_req {
    uint16_t reserve_id;
    uint8_t code[4]; ///< Should be set to "CLR\xAA".
} __attribute__ ((packed));

// PlatformB SEL record type structures.
/**
 * SEL PlatformB OEM structure to transport 1 byte and 2 short int parameters.
 */
struct st1byte2shuint
{
    uint8_t byte;
    uint16_t shuintg1;
    uint16_t shuintg2;
} __attribute__ ((packed));

/**
 * Platform B OEM SEL structure.
 * It is hosted in oem_nots_spec_sel_rec standard IPMI record type (SEL_OEMNOTS_SPEC_RECORD).
 * Size of this structure must fit in SEL_OEM_NOTS_DATA_LEN = 13 length.
 */
struct platformb_oem_sel_rec {
    uint32_t    timestamp;
    uint16_t    timestamp_msec;
    union {
        uint16_t    code;
        struct {
#if WORDS_BIGENDIAN
            uint16_t    severity : 2;
            uint16_t    param : 4;
            uint16_t    id  : 10;
#else
            uint16_t    id  : 10;
            uint16_t    param : 4;
            uint16_t    severity : 2;
#endif
        } msg;
    } msg_dscr;
    union {
        uint8_t  byte;              ///< Data map for SEL_MPAR_1BYTE
        uint8_t  byte2[2];          ///< Data map for SEL_MPAR_2BYTE
        uint8_t  byte3[3];          ///< Data map for SEL_MPAR_3BYTE
        uint8_t  byte4[4];          ///< Data map for SEL_MPAR_4BYTE
        uint8_t  byte5[5];          ///< Data map for SEL_MPAR_5BYTE
        int16_t  shintg;            ///< Data map for SEL_MPAR_1SHORTINT
        int16_t  shintg2[2];        ///< Data map for SEL_MPAR_2SHORTINT
        int32_t  intg;              ///< Data map for SEL_MPAR_1INT
        uint16_t shuintg;           ///< Data map for SEL_MPAR_1USHORTINT
        uint16_t shuintg2[2];       ///< Data map for SEL_MPAR_2USHORTINT
        uint32_t uintg;             ///< Data map for SEL_MPAR_1UINT
        struct st1byte2shuint st12; ///< Data map for SEL_MPAR_1BYTE2USHORTINT
        uint8_t  blob[5];           ///< Data map for SEL_MPAR_BLOB
    } msg_data;
};

/**
 * Standard IPMI sel_event_record structure. It is 16 byte size structure.
 */
struct platformb_event_record {
    uint16_t    record_id;
    uint8_t     record_type;
    union {
        struct standard_spec_sel_rec standard_type;
        struct oem_ts_spec_sel_rec oem_ts_type;
        struct oem_nots_spec_sel_rec oem_nots_type; ///< NOT USED
        struct platformb_oem_sel_rec platb_type; ///< Place holder for PlatforB SEL.
    } sel_type;
} __attribute__ ((packed));

// SEL PlatformB message severity codes.
#define SEL_MSEV_INFO 0
#define SEL_MSEV_WARN 1
#define SEL_MSEV_ERR  2

// SEL PlatformB message parameters type info.
#define SEL_MPAR_NULL               0
#define SEL_MPAR_1BYTE              1
#define SEL_MPAR_2BYTE              2
#define SEL_MPAR_3BYTE              3
#define SEL_MPAR_4BYTE              4
#define SEL_MPAR_5BYTE              5
#define SEL_MPAR_1SHORTINT          6
#define SEL_MPAR_2SHORTINT          7
#define SEL_MPAR_1INT               8
#define SEL_MPAR_1USHORTINT         9
#define SEL_MPAR_2USHORTINT         10
#define SEL_MPAR_1UINT              11
#define SEL_MPAR_1BYTE2USHORTINT    12
#define SEL_MPAR_BLOB               13

// SEL PlatformB message codes.
#define SEL_MCODE2ID(_x)    (_x & 0x003FF)
#define SEL_MCODE2SEV(_x)    ((_x >> 14) & 0x3)
#define SEL_MCODE2PAR(_x)    ((_x >> 10) & 0xF)

#define SEL_SEV2MCODE(_x)    (_x << 14)
#define SEL_PAR2MCODE(_x)    (_x << 10)

/** SEL event log full. */
#define ERROR_SEL_OVERFLOW            (SEL_SEV2MCODE(SEL_MSEV_ERR)  | SEL_PAR2MCODE(SEL_MPAR_NULL)  | 0x01)
/** SEL initialization completed. */
#define INFO_SEL_INIT_COMPLETE        (SEL_SEV2MCODE(SEL_MSEV_INFO) | SEL_PAR2MCODE(SEL_MPAR_NULL)  | 0x02)
/** SEL initialization failed. Error=%d. */
#define ERROR_SEL_INIT_FAILED         (SEL_SEV2MCODE(SEL_MSEV_ERR)  | SEL_PAR2MCODE(SEL_MPAR_1INT)  | 0x03)
/** Start health monitoring request. Current=%c. */
#define INFO_START_HEALTH_REQUEST     (SEL_SEV2MCODE(SEL_MSEV_INFO) | SEL_PAR2MCODE(SEL_MPAR_1BYTE) | 0x04)
/** Request to turn RED LED off. Current=%c. */
#define INFO_REDLED_OFF_REQUEST       (SEL_SEV2MCODE(SEL_MSEV_INFO) | SEL_PAR2MCODE(SEL_MPAR_1BYTE) | 0x05)
/** Time stamp synchronization. Time=%u. */
#define INFO_TIME_SYNC                (SEL_SEV2MCODE(SEL_MSEV_INFO) | SEL_PAR2MCODE(SEL_MPAR_1UINT) | 0x06)
/** FPGA image boot succeed. */
#define INFO_FPGA_BOOT_SUCCEED        (SEL_SEV2MCODE(SEL_MSEV_INFO) | SEL_PAR2MCODE(SEL_MPAR_NULL)  | 0x07)
/** FPGA boot timeout. Timeout=%u. */
#define ERROR_FGPA_BOOT_TIMEOUT       (SEL_SEV2MCODE(SEL_MSEV_ERR)  | SEL_PAR2MCODE(SEL_MPAR_1UINT) | 0x08)
/** FPGA health monitoring failure. Failure=%u. */
#define ERROR_FPGA_HEALTH_FAILURE     (SEL_SEV2MCODE(SEL_MSEV_ERR)  | SEL_PAR2MCODE(SEL_MPAR_1UINT) | 0x09)
/** Power button action. Action=%d*/
#define INFO_POWERBUTTON_ACTION       (SEL_SEV2MCODE(SEL_MSEV_INFO)  | SEL_PAR2MCODE(SEL_MPAR_1UINT) | 0x0A)
/** Detected board(s) with wrong GEO position. Boards=%hu. */
#define ERROR_BOARDBADGEO_FAILURE     (SEL_SEV2MCODE(SEL_MSEV_ERR)  | SEL_PAR2MCODE(SEL_MPAR_1USHORTINT) | 0x0B)
/** Failed to boot board(s). Boards=%hu. */
#define ERROR_BOARDSBOOT_FAILURE      (SEL_SEV2MCODE(SEL_MSEV_ERR)  | SEL_PAR2MCODE(SEL_MPAR_1USHORTINT) | 0x0C)
/** Failed to initialize USB2514 hub. Failure=%hu. */
#define ERROR_USB2514INIT_FAILURE     (SEL_SEV2MCODE(SEL_MSEV_ERR)  | SEL_PAR2MCODE(SEL_MPAR_1BYTE) | 0x0D)
/** ARM base board configuration data is corrupted. Failure=%d. */
#define ERROR_BASEBOARD_CONFIG_DATA_CORRUPTED   (SEL_SEV2MCODE(SEL_MSEV_ERR)  | SEL_PAR2MCODE(SEL_MPAR_1INT)  | 0x0E)
/** ARM subsystem board configuration data is corrupted. Failure=%d. */
#define ERROR_SUBSYSBOARD_CONFIG_DATA_CORRUPTED   (SEL_SEV2MCODE(SEL_MSEV_ERR)  | SEL_PAR2MCODE(SEL_MPAR_1INT)  | 0x0F)
/** Get base board configuration data failed. Failure=%d. */
#define ERROR_BASEBOARD_GETCONFIG_DATA (SEL_SEV2MCODE(SEL_MSEV_ERR)  | SEL_PAR2MCODE(SEL_MPAR_1INT)  | 0x10)
/** Detected board(s) with wrong bus position. Boards=%hu. */
#define ERROR_BOARDBADPOS_FAILURE     (SEL_SEV2MCODE(SEL_MSEV_ERR)  | SEL_PAR2MCODE(SEL_MPAR_1USHORTINT) | 0x11)
/** Failed to start and get board(s) info. Boards=%hu. */
#define ERROR_BOARDINFO_FAILURE       (SEL_SEV2MCODE(SEL_MSEV_ERR)  | SEL_PAR2MCODE(SEL_MPAR_1USHORTINT) | 0x12)
/** Board post-power initialization failed. Board=%c. Status1=%hu. Status2=%hu.*/
#define ERROR_POSTPOWER_FAILURE       (SEL_SEV2MCODE(SEL_MSEV_ERR)  | SEL_PAR2MCODE(SEL_MPAR_1BYTE2USHORTINT) | 0x13)
/** Failed to post-power initialize board(s). Boards=%hu. */
#define ERROR_BOARDSPOSTPOWER_FAILURE (SEL_SEV2MCODE(SEL_MSEV_ERR)  | SEL_PAR2MCODE(SEL_MPAR_1USHORTINT) | 0x14)
/** Failed to set time for board(s). Boards=%hu. */
#define ERROR_BOARDSETTIME_FAILURE    (SEL_SEV2MCODE(SEL_MSEV_ERR)  | SEL_PAR2MCODE(SEL_MPAR_1USHORTINT) | 0x15)
/** Board(s) HW information mismatch. Boards=%hu. */
#define ERROR_BOARDHWINFO_FAILURE     (SEL_SEV2MCODE(SEL_MSEV_ERR)  | SEL_PAR2MCODE(SEL_MPAR_1USHORTINT) | 0x16)
/** Subsystem board HW information mismatch. Failure=%d. */
#define ERROR_SUBSYSHWINFO_FAILURE    (SEL_SEV2MCODE(SEL_MSEV_ERR)  | SEL_PAR2MCODE(SEL_MPAR_1INT)  | 0x17)
/** Subsystem board HW initialization failed. Status1=%hu. Status2=%hu. */
#define ERROR_SUBSYSHWINIT_FAILURE    (SEL_SEV2MCODE(SEL_MSEV_ERR)  | SEL_PAR2MCODE(SEL_MPAR_2USHORTINT)  | 0x18)
/** Base board HW initialization failed. Status1=%hu. Status2=%hu. */
#define ERROR_BASEHWINIT_FAILURE      (SEL_SEV2MCODE(SEL_MSEV_ERR)  | SEL_PAR2MCODE(SEL_MPAR_2USHORTINT)  | 0x19)
/** Post power base board HW initialization failed. Status1=%hu. Status2=%hu. */
#define ERROR_BASEPOSTHWINIT_FAILURE  (SEL_SEV2MCODE(SEL_MSEV_ERR)  | SEL_PAR2MCODE(SEL_MPAR_2USHORTINT)  | 0x20)
/** Failed to enable usb port on board(s). Boards=%hu. */
#define ERROR_USB_PORT_ENABLE_FAILURE (SEL_SEV2MCODE(SEL_MSEV_ERR)  | SEL_PAR2MCODE(SEL_MPAR_1USHORTINT) | 0x21)
/** FPGA to ARM spi interface test failure. Status1=%hu. Status2=%hu. */
#define ERROR_FPGA_ARM_SPI_TEST_FAILURE  (SEL_SEV2MCODE(SEL_MSEV_ERR)  | SEL_PAR2MCODE(SEL_MPAR_2USHORTINT)  | 0x22)
/** FPGA AMBA read/write test failure. Status1=%hu. Status2=%hu. */
#define ERROR_FPGA_AMBA_RW_TEST_FAILURE  (SEL_SEV2MCODE(SEL_MSEV_ERR)  | SEL_PAR2MCODE(SEL_MPAR_2USHORTINT)  | 0x23)
//----------------------------------------------------------------------------

#ifndef ARM_IPMI_DECLS /* Skip declarations below if included from ARM build. */

struct sel_event_message_types {
    uint16_t code;
    const char* message;
} __attribute__ ((packed));

static struct sel_event_message_types oem_platformb_event_message_types[] __attribute__((unused)) = {
    { SEL_MCODE2ID(ERROR_SEL_OVERFLOW),             "SEL event log full." },
    { SEL_MCODE2ID(INFO_SEL_INIT_COMPLETE),         "SEL initialization completed."},
    { SEL_MCODE2ID(ERROR_SEL_INIT_FAILED),          "SEL initialization failed. (Error)"},
    { SEL_MCODE2ID(INFO_START_HEALTH_REQUEST),      "Start health monitoring request. (Current state)"},
    { SEL_MCODE2ID(INFO_REDLED_OFF_REQUEST),        "Request to turn RED LED off. (Current state)"},
    { SEL_MCODE2ID(INFO_TIME_SYNC),                 "Time stamp synchronization. (Time)"},
    { SEL_MCODE2ID(INFO_FPGA_BOOT_SUCCEED),         "FPGA image boot succeed."},
    { SEL_MCODE2ID(ERROR_FGPA_BOOT_TIMEOUT),        "FPGA boot timeout. (Timeout)"},
    { SEL_MCODE2ID(ERROR_FPGA_HEALTH_FAILURE),      "FPGA health monitoring detected failure. (Failure)"},
    { SEL_MCODE2ID(INFO_POWERBUTTON_ACTION),        "Power button action. (Action)"},
    { SEL_MCODE2ID(ERROR_BOARDBADGEO_FAILURE),      "Detected board(s) with wrong GEO position. (BoardBits)"},
    { SEL_MCODE2ID(ERROR_BOARDSBOOT_FAILURE),       "Failed to boot board(s). (BoardBits)"},
    { SEL_MCODE2ID(ERROR_USB2514INIT_FAILURE),      "Failed to initialize USB2514 hub. (Failure)"},
    { SEL_MCODE2ID(ERROR_BASEBOARD_CONFIG_DATA_CORRUPTED), "Base board configuration data is corrupted. (Failure)"},
    { SEL_MCODE2ID(ERROR_SUBSYSBOARD_CONFIG_DATA_CORRUPTED), "Subsystem board configuration data is corrupted. (Failure)"},
    { SEL_MCODE2ID(ERROR_BASEBOARD_GETCONFIG_DATA), "Read of base board configuration data failed. (Failure)"},
    { SEL_MCODE2ID(ERROR_BOARDBADPOS_FAILURE),      "Detected board(s) with wrong bus position. (BoardBits)"},
    { SEL_MCODE2ID(ERROR_BOARDINFO_FAILURE),        "Failed to start and get board(s) info. (BoardBits)"},
    { SEL_MCODE2ID(ERROR_POSTPOWER_FAILURE),        "Board post-power initialization failed. (BoardId,Status1,Status2)"},
    { SEL_MCODE2ID(ERROR_BOARDSPOSTPOWER_FAILURE),  "Failed to post-power initialize board(s). (BoardBits)"},
    { SEL_MCODE2ID(ERROR_BOARDSETTIME_FAILURE),     "Failed to set time for board(s). (BoardBits)"},
    { SEL_MCODE2ID(ERROR_BOARDHWINFO_FAILURE),      "Board(s) HW information mismatch. (BoardBits)"},
    { SEL_MCODE2ID(ERROR_SUBSYSHWINFO_FAILURE),     "Subsystem board HW information mismatch. (Failure)"},
    { SEL_MCODE2ID(ERROR_SUBSYSHWINIT_FAILURE),     "Subsystem board HW initialization failed. (Status1,Status2)"},
    { SEL_MCODE2ID(ERROR_BASEHWINIT_FAILURE),       "Base board HW initialization failed. (Status1,Status2)"},
    { SEL_MCODE2ID(ERROR_BASEPOSTHWINIT_FAILURE),   "Post power base board HW initialization failed. (Status1,Status2)"},
    { SEL_MCODE2ID(ERROR_USB_PORT_ENABLE_FAILURE),  "USB port enable failure. (BoardBits)"},
    { SEL_MCODE2ID(ERROR_FPGA_ARM_SPI_TEST_FAILURE),"FPGA to ARM spi interface test failure. (Status1,Status2)"},
    { SEL_MCODE2ID(ERROR_FPGA_AMBA_RW_TEST_FAILURE),"FPGA AMBA read/write test failure. (Status1,Status2)"},

//Template:    { SEL_MCODE2ID(), ""},
    { 0x00, NULL },
};

#endif //ARM_IPMI_DECLS



//----------------------------------------------------------------------------
/**
*   ipmi basic data transfer defines
*/

/**
 *  uTCA 7-bit I2C address.
 */
#define UTCA_I2C_AMC_ADDR_BROADCAST  0x00

#define UTCA_I2C_MCH_ADDR(_slot_) ( (0x80 + ((_slot_) * 2)) >> 1 )

#define UTCA_I2C_SHM_ADDR   (0x20>>1)            ///< Shelf manager

#define UTCA_I2C_MCH1_ADDR  UTCA_I2C_MCH_ADDR(1) ///< uTCA carrier hub 1
#define UTCA_I2C_MCH2_ADDR  UTCA_I2C_MCH_ADDR(2) ///< uTCA carrier hub 2

#define UTCA_I2C_AMC_ADDR(_slot_)               ((0x70 + ((_slot_) * 2)) >> 1 )

// ipmb-l bus address starts with 0x72 for AMC boards (same as we use with ipmitool -t 0x72)
#define UTCA_IPMBL_AMC_ADDR(_slot_)             ((0x70 + ((_slot_) * 2)))      ///
#define UTCA_IPMBL_AMC_ADDR_TO_SLOT_NUM(_addr_) ((((_addr_)) - 0x70) / 2)      ///< slot number 1..n


#define UTCA_AMC_SLOT_ID_FIRST (1)
#define UTCA_AMC_SLOT_ID_LAST  (12)

#define IPMI_I2C_DATA_LENGTH_MAX_SIZE 31  // without crc !!
#define IPMI_I2C_REQ_DATA_MAX_SIZE    25  // 31 - 6
#define IPMI_I2C_RSP_DATA_MAX_SIZE    24  // 31 - 7

//----------------------------------------------------------------------------
/**
* device support flags for device id response
*/
#define DEV_SUPPORT_CHASSIS   (1<<7)
#define DEV_SUPPORT_BRIDGE    (1<<6)
#define DEV_SUPPORT_EVENT_GEN (1<<5)
#define DEV_SUPPORT_EVENT_REC (1<<4)
#define DEV_SUPPORT_FRU_INV   (1<<3)
#define DEV_SUPPORT_SEL       (1<<2)
#define DEV_SUPPORT_SDR       (1<<1)
#define DEV_SUPPORT_SENSORS   (1<<0)

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// PICMG
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
//      AdvancedTCA
//----------------------------------------------------------------------------
#define PICMG_IDENTIFIER    0x00 ///< for PICMG defined group value of 0x00 shall be used
#define FRU_DEVICE_ID       0x00 ///< local fru ID

#define PICMG_FRU_CONTROL_CAPABILITIES_CMD  0x1E ///<

/**
 *  Get PICMG Properties req. struct
 */
typedef struct {
        unsigned char  picmg_id;        ///< indicates that this is PICMG extension command
} get_picmg_properties_com_req_t;

/**
 *  Get PICMG Properties rsp. struct
 */
typedef struct {
        unsigned char  picmg_id;            ///< indicates that this is PICMG extension command
        unsigned char  picmg_ext_ver_maj:4; ///< PICMG extension version (major)
        unsigned char  picmg_ext_ver_min:4; ///< PICMG extension version (minor)
        unsigned char  max_fru_id;          ///< numerically largest FRU device ID
        unsigned char  fru_device_id;       ///< FRU id of IPM controller
} get_picmg_properties_com_rsp_t;

/**
 *  FRU control command req. struct
 */
typedef struct {
        unsigned char  picmg_id;  ///< indicates that this is PICMG extension command
        unsigned char  fru_device_id;   ///< FRU device ID
        unsigned char  led_ctrl;        ///< FRU control
} fru_ctrl_com_req_t;

/**
 *  FRU control command rsp. struct
 */
typedef struct {
    unsigned char  picmg_id;        ///< indicates that this is PICMG extension commandnd
} fru_ctrl_com_rsp_t;


/**
 *  IPMI LED IDs
 */
#define FRU_LED_BLUE_ID     0x00   ///< blue led
#define FRU_LED1_ID         0x01   ///< led1
#define FRU_LED2_ID         0x02   ///< led2
#define FRU_LED3_ID         0x03   ///< led3
#define FRU_LED_APP_SPEC_ID 0x04   ///< app. specific leds (0x04-0xfe)
#define FRU_LED_RESERVED_ID 0xff   ///< led3

/**
 *  I-tech LED IDs
 */
typedef enum {
  e_led_id_blue           = FRU_LED_BLUE_ID,    ///< blue led
  e_led_id_red            = FRU_LED1_ID,        ///< red led
  e_led_id_green          = FRU_LED2_ID         ///< green led
} fru_led_id_t;

/**
 *  Get FRU LED properties req. structure
 */
typedef struct {
        unsigned char  picmg_id;    ///< indicates that this is PICMG extension command
        unsigned char  fru_device_id;   ///< FRU device ID
} get_fru_led_properties_req_t;

/**
 *  Get FRU LED properties rsp. structure
 */
typedef struct {
        unsigned char picmg_id;         ///< indicates that this is PICMG extension command
        unsigned char status;           ///< fru's ability to control four LEDs
        unsigned char app_spec_led_cnt; ///< number of specific LED's
                                        ///< under control of IPM controller
} get_fru_led_properties_rsp_t;

/**
 *  Get LED color capabilities req. structure
 */
typedef struct {
        unsigned char  picmg_id;        ///< indicates that this is PICMG extension command
        unsigned char  fru_device_id;   ///< FRU device ID
        unsigned char  led_id;          ///< LED id (fru_led_id_t)
} get_led_color_cap_req_t;

/**
 *  Get LED color capabilities rsp. structure
 */
typedef struct {
        unsigned char  picmg_id;         ///< indicates that this is PICMG extension command
        unsigned char  led_color_cap;    ///< led color capabilities (supported colors)
        unsigned char  led_col_local;    ///< default led colour (in local control state)
        unsigned char  led_col_override; ///< default led colour (in override control state)
} get_led_color_cap_rsp_t;

/**
 *  Set FRU LED state req. structure
 */
typedef struct {
        unsigned char  picmg_id;        ///< indicates that this is PICMG extension command
        unsigned char  fru_device_id;   ///< FRU device ID
        unsigned char  led_id;          ///< LED id (fru_led_id_t)
        unsigned char  led_func;        ///< led function
        unsigned char  on_duration;     ///< on duration in tens of ms
        unsigned char  color_when_ilum; ///< color when illuminated
} set_fru_led_state_req_t;

/**
 *  Set FRU LED state rsp. structure
 */
typedef struct {
    unsigned char  picmg_id;        ///< indicates that this is PICMG extension commandnd
} set_fru_led_state_rsp_t;


/**
 *  Get FRU LED state req. structure
 */
typedef struct {
        unsigned char  picmg_id;        ///< indicates that this is PICMG extension command
        unsigned char  fru_device_id;   ///< FRU device ID
        unsigned char  led_id;          ///< LED id (fru_led_id_t)
} get_fru_led_state_req_t;

/**
 *  Get FRU LED state rsp. structure
 */
typedef struct {
        unsigned char  picmg_id;                ///< indicates that this is PICMG extension command
        unsigned char  led_states;              ///< led states
        unsigned char  loc_ctrl_led_fn;         ///< local control LED function
        unsigned char  loc_ctrl_on_dur;         ///< local control on-duration
        unsigned char  loc_ctrl_color;          ///< local control color
        unsigned char  override_state_led_fn;   ///< override state led function
        unsigned char  override_state_on_dur;   ///< override state on-duration
        unsigned char  override_state_color;    ///< override state color
        unsigned char  lamp_test_duration;      ///< lamp test duration
} get_fru_led_state_rsp_t;

/**
 *  Get device locator id req. structure
 */
typedef struct {
        unsigned char  picmg_id;        ///< indicates that this is PICMG extension command
        unsigned char  fru_device_id;   ///< FRU device ID
} get_device_locator_record_id_req_t;

/**
 *  Get device locator id rsp. structure
 */
typedef struct {
        unsigned char  picmg_id;        ///< indicates that this is PICMG extension command
        unsigned char  record_id_ls;    ///< record ID (least significant byte)
        unsigned char  record_id_ms;    ///< record ID (most significant byte)
} get_device_locator_record_id_rsp_t;

//----------------------------------------------------------------------------
//      AdvancedMC
//----------------------------------------------------------------------------

/**
 *  Clock info struct
 */
typedef struct {
    unsigned char  pll_ctrl:2;      ///< pll control
    unsigned char  clk_dir:1;       ///< clock direction
    unsigned char  clk_sta:1;       ///< clock enable / disable
    unsigned char  clk_res:4;       ///< reserved (write as 0)
    unsigned char  clk_familiy;     ///< clock family
    unsigned char  clk_acc_level;   ///< accuracy level
    unsigned char  clk_freq_hz[4];  ///< clock frequency in Hz
} clock_state_cmd_info_t;

/**
 *  Set clock state command req. struct
 */
typedef struct {
        unsigned char  picmg_id;            ///< indicates that this is PICMG extension command
        unsigned char  clock_id;            ///< identifies clock being configured
        clock_state_cmd_info_t clk_info;    ///< clock info
        unsigned char  clk_res_id;          ///< resource ID
} set_clock_state_command_req_t;

/**
 *  Set clock state command rsp. struct
 */
typedef struct {
        unsigned char  picmg_id;        ///< indicates that this is PICMG extension command
} set_clock_state_command_rsp_t;


/**
 *  Get clock state command req. struct
 */
typedef struct {
        unsigned char  picmg_id;        ///< indicates that this is PICMG extension command
        unsigned char  clock_id;        ///< identifies clock being queried
        unsigned char  clk_res_id;      ///< resource ID
} get_clock_state_command_req_t;

/**
 *  Get clock state command rsp. struct
 */
typedef struct {
        unsigned char  picmg_id;            ///< indicates that this is PICMG extension command
        clock_state_cmd_info_t clk_info;    ///< clock info
        unsigned char  clk_res_id;          ///< resource ID
} get_clock_state_command_rsp_t;

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// sensors/events commands
//----------------------------------------------------------------------------

// ipmi cmds
#define SE_SET_SENSOR_EVENT_RECEIVER 0x00
#define SE_PLATFORM_EVENT_MSG        0x02

#define SE_AMC_ENTITY_ID             0xc1 ///< PICMG AdvancedMC Module
#define SE_MCH_ENTITY_ID             0xc2 ///< PICMG MCH Module
#define SE_AMC_ENTITY_INST           0x60 ///< notice:  without slot pos. offset !

#define SE_MAX_ID_STRING_LEN         16   ///< sensor ID string length

#define SE_END_OF_SENSOR_LIST        0xffff

// event receiver ctrl
typedef struct set_event_receiver_req {
        unsigned char  event_rec_slave_addr;
        unsigned char  event_rec_lun;       // only bits [1:0] 
} set_event_receiver_req_t;

// sensor types are specified in ipmi_event_sensor_types sensor_specific_types[] structure
// in ipmitool/ipmi_sdr.h
typedef enum {
   // standard
   eIpmiSensorTypeTemperature    = 0x01,
   eIpmiSensorTypeVoltage        = 0x02,
   eIpmiSensorTypeCurrent        = 0x03,
   eIpmiSensorTypeFan            = 0x04,
   eIpmiSensorTypeFruLoc         = 0x09,
   eIpmiSensorTypeHotSwap        = 0xf2,
   // OEM (I-tech)
   eIpmiSensorTypeOemBoardState  = 0xe0,
   eIpmiSensorTypeOemBoardStatus = 0xe1,
   eIpmiSensorTypeOemBoardInfo   = 0xe2
} tIpmiSensorType;

// event msgs
typedef struct platform_event_msg platform_event_msg_t;
#define SE_EVENT_MSG_REV  0x04

// hot swap event
#define SE_HANDLE_EVENT_MASK         0x1f

typedef enum {
  e_handle_closed           = 0,
  e_handle_opened           = 1,
  e_handle_quiesced         = 2, // used to describe pausing or altering the state 
  e_backend_power_failure   = 3,
  e_backend_power_shut_down = 4
  // 5 -0xf (reserved)
} se_hot_swap_event_t;

//----------------------------------------------------------------------------
typedef enum {
    eMAX6698ChRemote1     = 1,
    eMAX6698ChRemote2     = 2,
    eMAX6698ChRemote3     = 3,
    eMAX6698ChThermistor1 = 4,
    eMAX6698ChThermistor2 = 5,
    eMAX6698ChThermistor3 = 6,
    eMAX6698ChLocal       = 7
} tMAX6698Channel;
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
/**
*   FRU information
*/

#define FRU_BOARD_MANUFACTURER_STR "I-tech"

#define BOARD_MAN_STR_LEN  7    // 6 + NULL
#define BOARD_TYPE_STR_LEN 6    // 5 + NULL
#define APP_ID_STR_LEN     6    // 5 + NULL
#define PART_NUM_STR_LEN   2    // 1 + NULL
#define FRU_VER_STR_LEN    7    // 6 + NULL
#define FRU_TAG_STR_LEN    5    // 4 + NULL
#define FRU_ID_STR_LEN    10    // 9 + NULL

/**
 * Structure for FRU board info (board hw information)
 */
typedef struct {
    char mfg[BOARD_MAN_STR_LEN];        ///< manufacturer
    char prod[BOARD_TYPE_STR_LEN];      ///< board name (board type -> str)
    char ser[HWIDENT_SERIALNO_LEN];     ///< complete serial number
    char part_num[PART_NUM_STR_LEN];    ///< model number (hwRevisions_e)
    char fru_id[FRU_ID_STR_LEN];        ///< file name from which info was loaded
} board_info_area_t;

/**
 * Structure for FRU product info (appl. information)
 */
typedef struct {
    char mfg[BOARD_MAN_STR_LEN];        ///< manufacturer
    char prod[APP_ID_STR_LEN];          ///< product name (applicationId_e)
    char part_num[PART_NUM_STR_LEN];    ///< (customer_id_string)
    char ver[FRU_VER_STR_LEN];          ///< product version (?)
    char ser[HWIDENT_SERIALNO_LEN];     ///< complete serial number
    char tag[FRU_TAG_STR_LEN];          ///< historyRecords
    char fru_id[FRU_ID_STR_LEN];        ///< file name from which info was loaded
} product_info_area_t;

typedef struct {
    unsigned char board_type;
    char board_type_str[BOARD_TYPE_STR_LEN];
} board_type_enum_cast_struct_t;

typedef struct {
    applicationId_e app_Id;
    char app_id_str[BOARD_TYPE_STR_LEN];
} app_id_enum_cast_struct_t;

#define FRU_BASE_BOARD_ID 0

typedef struct read_fru_data_cmd_req {
        unsigned char  fru_dev_id;                // FRU Device ID. FFh = reserved
        unsigned char  fru_inventory_offset_lsb;  // FRU Inventory Offset to read, LS Byte
        unsigned char  fru_inventory_offset_msb;  // FRU Inventory Offset to read, MS Byte
        unsigned char  count_to_read;             // Count to read --- count is  1  based
} read_fru_data_cmd_req_t;

typedef struct read_fru_data_cmd_rsp {
        unsigned char count_returned;                    // Count returned --- count is  1  based
        unsigned char data[IPMI_I2C_RSP_DATA_MAX_SIZE];  // 3:2+N Requested data - TODO check size
} read_fru_data_cmd_rsp_t;

// fru info
typedef struct fru_info_req {
        unsigned char  fru_dev_id;                // FRU Device ID. FFh = reserved
} fru_info_req_t;

typedef struct fru_info_rsp {
        unsigned char  fru_info_size_in_bytes_lsb;
        unsigned char  fru_info_size_in_bytes_msb;  
        unsigned char  access:1;
} fru_info_rsp_t;

#define FRU_BYTE_ACCESS 0x0
#define FRU_WORD_ACCESS 0x1
typedef struct fru_info fru_info_t;

#define FRU_NO_MORE_INFO_FIELDS_INDICATION 0xc1

// fru common header
#define FRU_FORMAT_VER_NUM 0x1
#define FRU_AREA_NOT_PRESENT 0x0

typedef struct fru_header fru_header_t;

// board info area
#define FRU_ENGLISH_LAN_CODE 0x19         // 25
#define FRU_FORMAT_TYPE_ASCII_LATIN1 0x3 

typedef struct fru_area_board_header {
 uint8_t area_ver;
        uint8_t area_len;
        uint8_t lang;
 uint8_t mfg_date_time[3];
} fru_area_board_header_t;

typedef struct fru_area_product_header {
 uint8_t area_ver;
        uint8_t area_len;
 uint8_t lang;
} fru_area_product_header_t;

typedef struct fru_multirec_header fru_multirec_header_t;


#define FRU_INTERNAL_DATA_LENGTH_BYTES (8*1)

// notice: there should be some checking for memory overflow !! 
// e.g.: FRU_BOARD_INFO_HEADER_LENGTH_BYTE >= sizeof(area) !!

#define FRU_BOARD_INFO_HEADER_LENGTH_MOF8  8 // 64 bytes
#define FRU_BOARD_INFO_HEADER_LENGTH_BYTES (8*FRU_BOARD_INFO_HEADER_LENGTH_MOF8)
#define FRU_BOARD_INFO_DATA_LEN (FRU_BOARD_INFO_HEADER_LENGTH_BYTES-sizeof(fru_area_board_header_t))

#define FRU_PRODUCT_INFO_HEADER_LENGTH_MOF8  8 // 64 bytes
#define FRU_PRODUCT_INFO_HEADER_LENGTH_BYTES (8*FRU_PRODUCT_INFO_HEADER_LENGTH_MOF8)
#define FRU_PRODUCT_INFO_DATA_LEN (FRU_PRODUCT_INFO_HEADER_LENGTH_BYTES-sizeof(fru_area_product_header_t))

#define FRU_MULTIREC_AREA_LENGTH_MOF8  2 // 16 bytes
#define FRU_MULTIREC_AREA_LENGTH_BYTES (8*FRU_MULTIREC_AREA_LENGTH_MOF8)
//#define FRU_MULTIREC_AREA_FREE_BYTES (FRU_MULTIREC_AREA_LENGTH_BYTES-sizeof(fru_current_req_rec_t))

typedef struct fru_internal_ {
    unsigned char data[FRU_INTERNAL_DATA_LENGTH_BYTES];
} fru_internal_t;

typedef struct fru_area_board_data {
    fru_area_board_header_t header;
    unsigned char data[FRU_BOARD_INFO_DATA_LEN];
} fru_area_board_data_t;

typedef struct fru_area_product_data {
    fru_area_product_header_t header;
    unsigned char data[FRU_PRODUCT_INFO_DATA_LEN];
} fru_area_product_data_t;

#define FRU_MULTIREC_END_OF_LIST 0x80
#define FRU_REC_FORMAT_VER 0x02

typedef struct fru_current_req_data {
    unsigned char manufacturer_id[3];
    unsigned char picmg_rec_id;
    unsigned char rec_format_ver;
    unsigned char current_draw;
} fru_current_req_data_t;

typedef struct fru_current_req_rec {
    fru_multirec_header_t header; 
    fru_current_req_data_t data;
} fru_current_req_rec_t;

typedef struct fru_clock_cfg_rec_header {
    unsigned char manufacturer_id[3];
    unsigned char picmg_rec_id;
    unsigned char rec_format_ver;
    unsigned char clock_resource_id;
    unsigned char clock_cfg_desc_cnt;
} fru_clock_cfg_rec_header_t;

typedef struct fru_clock_cfg_desc_header {
    unsigned char clock_id;
    unsigned char clock_ctrl;
    unsigned char indirect_clk_desc_cnt;
    unsigned char direct_clk_desc_cnt;
} fru_clock_cfg_desc_header_t;

typedef struct fru_indir_clk_desc {
    unsigned char clk_feat;
    unsigned char dependent_clk_id;
} fru_indir_clk_desc_t;

typedef struct fru_dir_clk_desc {
    unsigned char clk_feat;
    unsigned char clk_family;
    unsigned char clk_accuracy_level;
    unsigned int clk_freq;
    unsigned int min_clk_freq;
    unsigned int max_clk_freq;
} fru_dir_clk_desc_t;


// ---------------------------------------------------------------------------

// clk cfg rec.

// predefined clock IDs for AMC clocks
#define FRU_CLK_CFG_TCLKA 0x01
#define FRU_CLK_CFG_TCLKB 0x02
#define FRU_CLK_CFG_TCLKC 0x03
#define FRU_CLK_CFG_TCLKD 0x04
#define FRU_CLK_CFG_FCLKA 0x05

// clock control 
#define FRU_CLK_CFG_CLK_CTRL_BY_CARR 0x00 
#define FRU_CLK_CFG_CLK_CTRL_BY_APPL 0x01 

// clock features
#define FRU_CLK_CFG_CON_THOU_PLL  (1<<1)
#define FRU_CLK_CFG_ASYM_SOURCE   (1<<0)
#define FRU_CLK_CFG_ASYM_RECEIVER (0<<0)

// clock family
#define FRU_CLK_FAMILY_DEF_PCI_EXPRESS 0x02

// p2p rec.

// record type
#define FRU_P2P_REC_TYPE_AMC_MODULE (1<<7)
#define FRU_P2P_REC_TYPE_ON_CAR     (0<<7)


typedef struct fru_clock_cfg_rec_data {
          fru_clock_cfg_rec_header_t cfg_rec_header;
          // for each clock cfg. desc.
          struct {
            fru_clock_cfg_desc_header_t header;
            // indirect desc.
            // fru_indir_clk_desc_t indir_clk_desc;
            // direct desc.
            fru_dir_clk_desc_t dir_clk_desc_pcie_in;
          } clk_cfg_desc;
} fru_clock_cfg_rec_data_t;

typedef struct fru_p2p_con_rec_header {
    unsigned char manufacturer_id[3];
    unsigned char picmg_rec_id;
    unsigned char rec_format_ver;
    unsigned char oem_guid_cnt;  
} fru_p2p_con_rec_header_t;


typedef struct fru_link_desc_list {
    unsigned int link_grup_id:6;
    unsigned int link_type_ext:4;
    unsigned int link_type:8;
    unsigned int link_desig:12;
} fru_link_desc_list_t;

// amc ch desc
typedef struct fru_p2p_con_rec_ch_desc {
  // notice: bytes must be arranged in little endian order
  unsigned char lane0_port_num:5;
  unsigned char lane1_port_num_low3:3;

  unsigned char lane1_port_num_high2:2;
  unsigned char lane2_port_num:5;
  unsigned char lane3_port_num_low1:1;

  unsigned char lane3_port_num_high4:4;
  unsigned char reserved:4;
} fru_p2p_con_rec_ch_desc_t;

// link desc
typedef struct fru_p2p_con_rec_link_desc {
  unsigned char link_desc_link_designator_amd_ch;

  unsigned char link_desc_link_designator_lanes:4;
  unsigned char link_desc_link_type_low4:4;

  unsigned char link_desc_link_type_high4:4;
  unsigned char link_desc_link_type_ext:4;

  unsigned char link_desc_grouping_id;

  unsigned char link_desc_amc_asy_match:2;
  unsigned char link_desc_res:6;
} fru_p2p_con_rec_link_desc_t;

#define FRU_P2P_CH_DESC_NUM   2
#define FRU_P2P_LINK_DESC_NUM 2

#define OEM_GUIDS_IN_RECORD    1

typedef struct fru_p2p_con_rec_data {
          fru_p2p_con_rec_header_t p2c_con_rec_header;
          // for each link desc.
          unsigned char oem_guid_list[16*OEM_GUIDS_IN_RECORD];
          unsigned char rec_type_and_con_dev_id;
          unsigned char ch_desc_cnt;
          fru_p2p_con_rec_ch_desc_t p2c_con_ch_desc[FRU_P2P_CH_DESC_NUM];
          fru_p2p_con_rec_link_desc_t p2c_con_link_desc[FRU_P2P_LINK_DESC_NUM];
} fru_p2p_con_rec_data_t;

typedef struct fru_base_board_clk_cfg_rec {
        fru_multirec_header_t header; 
        fru_clock_cfg_rec_data_t data;
} fru_base_board_clk_cfg_rec_t;

typedef struct fru_p2p_con_rec {
        fru_multirec_header_t header; 
        fru_p2p_con_rec_data_t data;
} fru_p2p_con_rec_t;

typedef struct fru_base_board {
    // common header
    fru_header_t header;
    // internal
    fru_internal_t internal;
    // board
    fru_area_board_data_t board_info;
    // product
    fru_area_product_data_t product_info;
    struct {
        // power requirements
        fru_current_req_rec_t current_req_rec;
        fru_base_board_clk_cfg_rec_t clk_cfg_rec;
        fru_p2p_con_rec_t p2p_con_rec;
    } multirec;
} fru_base_board_t;

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------

#define FRU_P2P_CH_DESC_EXCL_LANE 0x1f

/*
 *  I-TECH OEM link types
 */
#define OEM_FRU_PICMGEXT_AMC_LINK_TYPE_USB     0xf0 ///< USB
#define OEM_AMC_LINK_TYPE_EXT_USB_2_0          0x02 ///< USB 2.0

/*
 *  I-TECH fixed com. ports
 */
#define ITECH_AMC_USB_PORT_NUM_1                1    ///< port 1
#define ITECH_AMC_PCIE_PORT_NUM_4               4    ///< port 4
#define ITECH_AMC_PCIE_PORT_NUM_5               5    ///< port 5
#define ITECH_AMC_PCIE_PORT_NUM_6               6    ///< port 6
#define ITECH_AMC_PCIE_PORT_NUM_7               7    ///< port 7
#define ITECH_AMC_LANE_NOT_INCLUDED          0x1f    ///< lane not included

/*
 *  PICMG: set amc port state
 */
typedef struct set_amc_port_state_req {       
    unsigned char  picmg_id;

    // - little endian !
    unsigned char  amc_ch_id;
    
    unsigned char link_designator_lanes:4;
    unsigned char link_type_low4:4;

    unsigned char  link_type_high4:4;
    unsigned char  link_type_ext:4;

    unsigned char  link_grouping_id;
    // - little endian !

    unsigned char link_state;
    unsigned char on_carrier_dev_id;
} set_amc_port_state_req_t;

typedef struct set_amc_port_state_rsp {       
    unsigned char  picmg_id;
} set_amc_port_state_rsp_t;

/*
 *  PICMG: get amc port state
 */
typedef struct get_amc_port_state_req {
    unsigned char picmg_id;
    unsigned char amc_ch_id;
    unsigned char on_carrier_dev_id;
} get_amc_port_state_req_t;

typedef struct get_amc_port_state_rsp {
    unsigned char picmg_id;

    // - little endian !
    unsigned char link_designator_lanes:4;
    unsigned char link_type_low4:4;

    unsigned char  link_type_high4:4;
    unsigned char  link_type_ext:4;

    unsigned char  link_grouping_id;
    // - little endian !

    // link states
    unsigned char link_state;
    unsigned char link_info2[2];
    unsigned char link_state2;
    unsigned char link_info3[2];
    unsigned char link_state3;
    unsigned char link_info4[2];
    unsigned char link_state4;
} get_amc_port_state_rsp_t;

// set clock state
typedef struct set_amc_clk_state_req {       
    unsigned char  picmg_id; 
    unsigned char  clk_id;
    unsigned char  clk_cfg_desc_index;
    
    // clock setting 
    unsigned char  reserved:4;
    unsigned char  clk_state:1;
    unsigned char  clk_dir:1;
    unsigned char  clk_family;
    unsigned char  clk_acc_level;
    unsigned char  clk_freq;
    unsigned char  clk_res_id;
} set_amc_clk_state_req_t;

typedef struct set_amc_clk_state_rsp {       
    unsigned char  picmg_id;
} set_amc_clk_state_rsp_t;

// get clock state
//----------------------------------------------------------------------------

// SDR IPMI structures.
typedef struct sdr_repo_info_rs sdr_repo_info_rs_t;
typedef struct sdr_device_info_rs sdr_device_info_rs_t;
typedef struct sdr_reserve_repo_rs sdr_reserve_repo_rs_t;
typedef struct sdr_get_rq sdr_get_rq_t;
typedef struct sensor_set_thresh_rq sensor_set_thresh_rq_t;
typedef struct sdr_record_full_sensor sdr_record_full_sensor_t;
typedef struct sdr_record_compact_sensor sdr_record_compact_sensor_t;
typedef struct sdr_record_fru_locator sdr_record_fru_locator_t;
typedef struct sdr_record_mc_locator sdr_record_mc_locator_t;

//----------------------------------------------------------------------------
// NOTE: Structure below is derived from how IPMI function ipmi_sdr_get_record()
// is expecting it will get bytes back.
struct sensor_record {
    // 5 byte header.
    uint16_t id;
    uint8_t version;
    uint8_t type;
    uint8_t length; // Length of sensor record.
    // Sensor record.
    union {
        struct sdr_record_full_sensor full;
        struct sdr_record_compact_sensor compact;
        struct sdr_record_eventonly_sensor eventonly;
        struct sdr_record_generic_locator genloc;
        struct sdr_record_fru_locator fruloc;
        struct sdr_record_mc_locator mcloc;
        struct sdr_record_entity_assoc entassoc;
        struct sdr_record_oem oem;
    } record;
    uint16_t next; // id of next sensor, 0xFFFF for end of list.
    union {
        int (*get_full_sensor)(const sdr_record_full_sensor_t * sensor,
                          unsigned char * val);
        int (*get_compact_sensor)(const sdr_record_compact_sensor_t * sensor,
                          unsigned char * val);
    } get_reading_fn;
} __attribute__ ((packed));

typedef struct sensor_record sensor_record_t;
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// end of packing alignment for IPMI data structures
#ifdef CROSSWORKS
#pragma pack(pop)
#endif
//----------------------------------------------------------------------------

#endif /*IPMI_ITECH_EXTENSIONS_H*/

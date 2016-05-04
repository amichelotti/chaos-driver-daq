/*
 * Copyright (c) 2011 Instrumentation Technologies.
 * All Rights Reserved.
 *
 * $Id: ipmi_itech_types.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef IPMI_ITECH_TYPES_H
#define IPMI_ITECH_TYPES_H

#include <stdint.h>

//----------------------------------------------------------------------------
// packing alignment for IPMI data structures must be 1 byte
#ifdef CROSSWORKS
#pragma pack(push,1)
#endif
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// IPMI I2C transfer structures

#define I2C_SEND_MAX_PAYLOAD_SIZE   100//25
#define I2C_RECV_MAX_PAYLOAD_SIZE   100//25

#define I2C_IPMI_CRC_SIZE_IN_BYTES    1

/**
 *  request i2c data structure
 */
struct ipmi_i2c_req {
    // header (7 bytes)
    struct {
        // con. header
        struct {
            uint8_t rsSA;    ///< responder slave address
            uint8_t rsLUN:2; ///< responser's LUN
            uint8_t netFn:6; ///< network function
            uint8_t crcCH;   ///< checksum for connection header
        }  ch;
        uint8_t rqSA;       ///< requester slave address
        uint8_t rqLUN:2;    ///< requester's LUN
        uint8_t rqSeq:6;    ///< requester's sequence number
        uint8_t cmd;        ///< command
    } h;
    // data
    uint8_t data[I2C_SEND_MAX_PAYLOAD_SIZE+I2C_IPMI_CRC_SIZE_IN_BYTES];
    // additional info
    uint8_t dataLen;
} __attribute__ ((packed));

typedef struct ipmi_i2c_req ipmi_i2c_req_t;

/**
 *  response data structure
 */

struct ipmi_i2c_rsp {
    // header (8 bytes)
    struct {
        // con. header
        struct {
            uint8_t rqSA;       ///< requester slave address
            uint8_t rqLUN:2;    ///< requester's LUN
            uint8_t netFn:6;    ///< network function
            uint8_t crcCH;      ///< checksum for connection header
        } ch;
        uint8_t rsSA;       ///< responder slave address
        uint8_t rsLUN:2;    ///< responser's LUN
        uint8_t rsSeq:6;    ///< requester's sequence number
        uint8_t cmd;        ///< command
        uint8_t ccode;      ///< completion code
    }  h;
    uint8_t data[I2C_RECV_MAX_PAYLOAD_SIZE+I2C_IPMI_CRC_SIZE_IN_BYTES];
    // additional info
    uint8_t dataLen;
} __attribute__ ((packed));

typedef struct ipmi_i2c_rsp ipmi_i2c_rsp_t;

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// IPMI PlatformB definitions for IPMI_NETFN_SE.

/**
 * Structure for standard sensor reading, undefined in the IPMI headers.
 */
struct sdr_reading_rsp {
    /**
     * Standard IPMI part of the sensor reading response.
     */
    uint8_t raw;         ///< 8bit raw value of sensor.
    uint8_t valid_mask;  ///< If READING_UNAVAILABLE is set, then raw value is not valid.
    uint8_t status_mask; ///< Check IPMI doc for this byte interpretation (e.g holds threshold bits for analog sensors).
} __attribute__ ((packed));

typedef struct sdr_reading_rsp sdr_reading_rsp_t;

/**
 * Extended structure for sensor reading, i-tech OEM.
 */
struct sdr_reading_ext_rsp {
    /**
     * Standard IPMI part of the sensor reading response.
     */
    uint8_t raw;         ///< 8bit raw value of sensor.
    uint8_t valid_mask;  ///< If READING_UNAVAILABLE is set, then raw value is not valid.
    uint8_t status_mask; ///< Check IPMI doc for this byte interpretation (e.g holds threshold bits for analog sensors).

    union {
        uint8_t err; ///< In case if valid_mask bit is READING_UNAVAILABLE, this holds FW error code.
        uint8_t state_2; ///< Value valid for discrete sensor.
        uint16_t val16; ///< Value of 12bit ADC count for analog sensors (Temperature, Volt, Current, RPM).
        struct
        {
            uint16_t val16_1; ///< Value 1 of discrete sensors.
            uint16_t val16_2; ///< Value 2 of discrete sensors.
        } d;
    } ext;
} __attribute__ ((packed));

typedef struct sdr_reading_ext_rsp sdr_reading_ext_rsp_t;

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// IPMI PlatformB definitions for IPMI_NETFN_CHASSIS.

/**
 * @brief IPMI PlatformB IPMI_NETFN_CHASSIS command code.
 * @note ARM LPC2xxx CPU is using Little-Endian byte order.
 */
#define IPMI_CHASSIS_CTL 0x2 // Commands extension.

//NOTE: For BMC interface, limitation constrain is size of RECV_MAX_PAYLOAD_SIZE.
#define IPMI_CHASSIS_CTL_I2C_BUFF_LEN 48 // Max len of I2C buffer.

struct chassis_ctl_req {
    // Standard part of the response
    uint8_t ctl_code;
    union
    {
        struct
        {
            uint16_t addr;
            uint16_t val;
        } fpga; // FPGA SPI register read/write request.
        struct
        {
            uint32_t addr;
            uint32_t val;
        } amba; // FPGA AMBA register read/write request.
        struct
        {
            uint8_t port;
            uint8_t addr;
            uint8_t rx_len;
            uint8_t tx_len;
            uint8_t tx_buff[IPMI_CHASSIS_CTL_I2C_BUFF_LEN];
        } i2c; // I2C request.
        struct
        {
            uint8_t board;
        } power; ///< Power management request.
    } operation;
} __attribute__ ((packed));

//NOTE: Standard IPMI chassis (mc) control codes are reserved from 0x0 to 0x5.
#define IPMI_CHASSIS_CTL_FLASH_ARM  0x8
#define IPMI_CHASSIS_CTL_FLASH_FPGA 0x9

#define IPMI_CHASSIS_CTL_PCI_RESET_OFF 0xA
#define IPMI_CHASSIS_CTL_PCI_RESET_ON  0xB

#define IPMI_CHASSIS_CTL_JTAG_SEL_LOCAL 0xC
#define IPMI_CHASSIS_CTL_JTAG_SEL_REMOTE  0xD

//TODO: Move this functionality to IPMI i2c commands.
#define IPMI_CHASSIS_CTL_I2C 0x10
struct chassis_ctl_i2c_rsp {
    int8_t sts;
    uint8_t rx_len;
    uint8_t rx_buff[IPMI_CHASSIS_CTL_I2C_BUFF_LEN];
} __attribute__ ((packed));

#define IPMI_CHASSIS_CTL_BOOT_FPGA 0x11
#define IPMI_CHASSIS_CTL_PCI_RESET 0x12

#define IPMI_CHASSIS_CTL_FPGA_INIT_OFF  0x13
#define IPMI_CHASSIS_CTL_FPGA_INIT_CLR  0x14

#define IPMI_CHASSIS_CTL_REDLED_OFF  0x15

#define IPMI_CHASSIS_CTL_START_HEALTH  0x16

#define IPMI_CHASSIS_CTL_JTAG_ENABLE    0x17
#define IPMI_CHASSIS_CTL_JTAG_DISABLE   0x18

#define IPMI_CHASSIS_CTL_JTAG_SEL_ARM 0x19
#define IPMI_CHASSIS_CTL_JTAG_SEL_FPGA 0x1A

#define IPMI_CHASSIS_CTL_PROG_OFF   0x1B
#define IPMI_CHASSIS_CTL_PROG_ON    0x1C

/** IPMI Post-Power notification command.
 * This command is send by ICB board to its satellites when COMExpress PCIe clock is running, so
 * that satellite boards can perform initializations that require PCIe clock (e.g. PCIe reset, write data
 * to some PCIe (HW identification) registers , ...).
 */
#define IPMI_CHASSIS_CTL_POSTPOWER  0x1D
/** Response structure to IPMI_CHASSIS_CTL_POSTPOWER IPMI request. */
struct chassis_ctl_postpower_rsp {
    uint16_t status1; ///< Status of operation. If neq 0, indicates failure.
    uint16_t status2; ///< Additional information regarding status of post-power initialization. E.g., in case of HW initialization failure it will contain address in FLASH where failure was hit.
} __attribute__ ((packed));

#define IPMI_CHASSIS_CTL_FPGA_SPI_READ  0xDB
struct chassis_ctl_fpgaread_rsp {
    int8_t sts;
    uint16_t value;
} __attribute__ ((packed));

#define IPMI_CHASSIS_CTL_FPGA_SPI_WRITE 0xDC

#define IPMI_CHASSIS_CTL_FPGA_SPI_DEBUG 0xBD

#define IPMI_CHASSIS_CTL_FPGA_AMBA_READ  0xAB
struct chassis_ctl_ambaread_rsp {
    int8_t sts;
    uint32_t value;
} __attribute__ ((packed));

#define IPMI_CHASSIS_CTL_FPGA_AMBA_WRITE 0xAC

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Extended IPMI Storage commands.

/**
 * Platform B storage IPMI extension command for HW identification/initialization.
 */
#define IPMI_CMD_HWSTORAGE          0x70

/**
 * Base board HW identification/initialization IPMI request identifier.
 */
#define IPMI_HWSTORAGE_BASEBOARD_IDENT   0

/**
 * Subsystem board HW identification/initialization IPMI request identifier.
 */
#define IPMI_HWSTORAGE_SUBSYSBOARD_IDENT 1

/**
 * IPMI HW identification/initialization open operation command.
 */
#define IPMI_HWSTORAGE_CMD_INIT     0

/**
 * IPMI HW identification/initialization close operation command.
 */
#define IPMI_HWSTORAGE_CMD_FLUSH    1

/**
 * IPMI HW identification/initialization read operation command.
 */
#define IPMI_HWSTORAGE_CMD_READ     2

/**
 * IPMI HW identification/initialization write operation command.
 */
#define IPMI_HWSTORAGE_CMD_WRITE    3

/**
 * IPMI HW identification/initialization erase operation command.
 */
#define IPMI_HWSTORAGE_CMD_ERASE    4

/**
 * IPMI HW initialization command.
 */
#define IPMI_HWSTORAGE_CMD_HWINIT   5

/**
 * IPMI Post power HW initialization command.
 * @note: Valid only for base board.
 */
#define IPMI_HWSTORAGE_CMD_POSTHWINIT   6

/**
 * HW Identification/initialization IPMI write command data buffer length.
 * @note For BMC interface: Constrain for this buffer size is SEND_MAX_PAYLOAD_SIZE,
 * currently defined as 59 bytes.
 */
#define IPMI_HWSTORAGE_WRITEDATA_LEN 54

/**
 * Read data from HW identification/initialization storage structure
 */
struct storage_hwstorage_read{
    uint16_t offset; ///< Offset into HW identification/initialization structure.
    int8_t len; ///< Number of bytes in data buffer to read.
} __attribute__ ((packed));
/**
 * Write data to HW identification/initialization storage structure
 */
struct storage_hwstorage_write{
    uint16_t offset; ///< Offset into HW identification/initialization structure.
    int8_t len; ///< Number of bytes in data buffer to write.
    uint8_t data[IPMI_HWSTORAGE_WRITEDATA_LEN]; ///< Buffer with write data bytes.
} __attribute__ ((packed));

/**
 * HW Identification/initialization IPMI request structure.
 */
struct storage_hwstorage_req {
    // Standard part of the response
    uint8_t hwIdent; ///< Base or subsystem board identifier.
    uint8_t cmd; ///< Command to execute for HW identification/initialization.
    /**
     * Union container for HW identification/initialization commands.
     */
    union
    {
        struct storage_hwstorage_read read;
        struct storage_hwstorage_write write;
    } command;
} __attribute__ ((packed));

typedef struct storage_hwstorage_req storage_hwstorage_req_t;

/**
 * HW Identification/initialization IPMI read command data buffer length.
 * @note For BMC interface: Constrain for this buffer size is RECV_MAX_PAYLOAD_SIZE,
 * currently defined as 59 bytes.
 */
#define IPMI_HWSTORAGE_READATA_LEN 59

/**
 * HW Identification and initialization IPMI read request response.
 */
struct storage_hwstorage_read_rsp {
    uint8_t data[IPMI_HWSTORAGE_READATA_LEN]; ///< Buffer with read data bytes.
} __attribute__ ((packed));

typedef struct storage_hwstorage_read_rsp storage_hwstorage_read_rsp_t;

/**
 * HW Identification and initialization IPMI erase request response.
 */
struct storage_hwstorage_erase_rsp {
    uint16_t bytes_erased; ///< Number of erased bytes. Size of location where HW Identification and initialization data are stored.
} __attribute__ ((packed));

typedef struct storage_hwstorage_erase_rsp storage_hwstorage_erase_rsp_t;

/**
 * HW Identification and initialization IPMI flush request response.
 */
struct storage_hwstorage_flush_rsp {
    uint16_t bytes_written; ///< Number of written bytes.
} __attribute__ ((packed));

typedef struct storage_hwstorage_flush_rsp storage_hwstorage_flush_rsp_t;

/**
 * HW initialization command request response.
 */
struct storage_hwstorage_hwinit_rsp {
    uint16_t status; ///< Return status. Non zero value indicates failure.
    uint16_t opcount; ///< Offset inside HW initialization data where unsuccessful status was hit.
} __attribute__ ((packed));

typedef struct storage_hwstorage_hwinit_rsp storage_hwstorage_hwinit_rsp_t;

/** Max length of serial identification string. */
#define HWIDENT_SERIALNO_LEN        32

/** Max length of customer identification string. */
#define HWIDENT_CUSTOMERID_LEN      4

/** Max length of history record data. */
#define HWIDENT_HISTORYRECORD_LEN   2048

/**
 * Structure that holds common HW identification data.
 */
struct hwidentification
{
    uint8_t appId; ///< Platform B application identifier.
    uint8_t subsysId; ///< Platform B application specific subsystem identifier.
    uint8_t hwRevision; ///< HW revision number;
    char    customerId[HWIDENT_CUSTOMERID_LEN]; ///< Unique customer identification string (0-terminated).
    char    serialNo[HWIDENT_SERIALNO_LEN]; ///< Platform B component serial identification string (0-terminated).
    char    historyRecords[HWIDENT_HISTORYRECORD_LEN]; ///< Platform B component history record string (0-terminated).
} __attribute__ ((packed));

typedef struct hwidentification hwidentification_t;

/** Subsystem board present. */
#define HWIDENT_SUBSYSTEM_PRESENT   0x01

/**
 * Structure that holds base base board HW identification data.
 */
struct board_hwidentification
{
    struct hwidentification common; ///< Common HW identification data.
    uint8_t startupOrder; ///< Zero based board startup order.
    uint8_t subsysPresent; ///< Subsystem board present (if eq to HWIDENT_SUBSYSTEM_PRESENT).
} __attribute__ ((packed));

typedef struct board_hwidentification board_hwident_t;

/**
 * Structure that holds subsystem base board HW identification data.
 * @note: This data is usually located in subsystem board FLASH.
 */
struct subsys_hwidentification
{
    struct hwidentification common; ///< Common HW identification data.
} __attribute__ ((packed));

typedef struct subsys_hwidentification subsys_hwident_t;

/** Max length of HW initialization sequence. */
#define HWINIT_SEQUENCE_LEN 3072

/** Mask for max delay in interpreter code. */
#define HWINIT_MAX_DELAY_MASK 0x7F ///< Will result in max 127 milli second delay.
/** Mask for max retry in interpreter code. */
#define HWINIT_MAX_RETRY_MASK 0x1F ///< Will result in max 31 retry.

/** HW initialization interpreter language instruction codes. */
/**
 * NOP No operation.
 * Mnemonic: NOP
 */
#define HWINIT_OPCODE_NOP               0x00
#define HWINIT_OPCODE_NOP1              0xFF

/**
 * A = A & <16-bit operand>.
 * Mnemonic: ANDA <16-bit operand>;
 */
#define HWINIT_OPCODE_AND_ACCA          0x01

/**
 * A = A | <16-bit operand>.
 * Mnemonic: ORA <16-bit operand>;
 */
#define HWINIT_OPCODE_OR_ACCA           0x02

/**
 * A = ~A.
 * Mnemonic: NOTA;
 */
#define HWINIT_OPCODE_NOT_ACCA          0x03

/**
 * Load accumulator A with <16-bit operand> value.
 * A = <16-bit operand>
 * Mnemonic: WRA <16-bit operand>;
 */
#define HWINIT_OPCODE_WRITE_ACCA        0x04

/**
 * Load accumulator B with <16-bit operand> value.
 * B = <16-bit operand>
 * Mnemonic: WRB <16-bit operand>;
 */
#define HWINIT_OPCODE_WRITE_ACCB        0x05

/**
 * Delay execution for <16-bit operand> milli seconds.
 * @note Max is 127 msec.
 * Mnemonic: DLY <16-bit operand>;
 */
#define HWINIT_OPCODE_DELAY             0x06

/**
 * Read value of FPGA <16-bit operand> address register to accumulator A.
 * A = FPGA(<16-bit operand>).
 * Mnemonic: FRDA <16-bit operand>;
 */
#define HWINIT_OPCODE_FPGAREAD_ACCA     0x07

/**
 * Write value of accumulator A to FPGA <16-bit operand> address register.
 * FPGA(<16-bit operand>) = A.
 * Mnemonic: FWRA <16-bit operand>;
 */
#define HWINIT_OPCODE_FPGAWRITE_ACCA    0x08

/**
 * Write value of <16-bit operand2> to FPGA <16-bit operand1> address register.
 * FPGA(<16-bit operand1>) = <16-bit operand2>.
 * Mnemonic: FDWR <16-bit operand1> <16-bit operand2>;
 */
#define HWINIT_OPCODE_FPGAWRITE_DIRECT  0x09

/**
 * Wait until bits are set in FPGA <16-bit operand1> address register as defined by <16-bit operand2> value.
 * Wait will be repeated until condition is satisfied or until value defined in accumulator B (number of retry) is
 * decremented to 0. Each repeat will wait for milli seconds defined in accumulator A.
 * @note Delay is downcast to max 127 and number of retry is downcast to max 15.
 * WHILE ((FPGA(<16-bit operand1>) & <16-bit operand2>) != <16-bit operand2>).
 * Mnemonic: FWBS <16-bit operand1> <16-bit operand2>;
 */
#define HWINIT_OPCODE_FPGAWAIT_BITSET   0x0A

/**
 * Wait until bits are clear in FPGA <16-bit operand1> address register as defined by <16-bit operand2> value.
 * Wait will be repeated until condition is satisfied or until value defined in accumulator B (number of retry) is
 * decremented to 0. Each repeat will wait for milli seconds defined in accumulator A.
 * @note Delay is downcast to max 127 and number of retry is downcast to max 15.
 * WHILE ((FPGA(<16-bit operand1>) & <16-bit operand2>) != 0)
 * Mnemonic: FWBC <16-bit operand1> <16-bit operand2>;
 */
#define HWINIT_OPCODE_FPGAWAIT_BITCLEAR 0x0B

/**
 * Read value of AMBA <32-bit operand> address register to accumulator AB.
 * AB = AMBA(<32-bit operand>).
 * Mnemonic: ARDAB <32-bit operand>;
 */
#define HWINIT_OPCODE_AMBAREAD_ACCAB    0x0C

/**
 * Write value of accumulator AB to AMBA <32-bit operand> address register.
 * AMBA(<32-bit operand>) = AB.
 * Mnemonic: AWRAB <32-bit operand>;
 */
#define HWINIT_OPCODE_AMBAWRITE_ACCAB   0x0D

/**
 * Write value of <32-bit operand> to AMBA address defined by accumulator AB.
 * AMBA(AB) = <32-bit operand>.
 * Mnemonic: AWRADR <32-bit operand>;
 */
#define HWINIT_OPCODE_AMBAWRITE_ACCABADDR 0x0E

/**
 * Write value of <32-bit operand2> to AMBA <32-bit operand1> address register.
 * AMBA(<32-bit operand1>) = <32-bit operand2>.
 * Mnemonic: ADWR <32-bit operand1> <32-bit operand2>;
 */
#define HWINIT_OPCODE_AMBAWRITE_DIRECT  0x0F

/**
 * Macro expansion: This macro will create sequence of native HW initialization
 * instructions that will use FPGA HW SPI control registers to program HW chips.
 * HW chip selected by <16-bit operand2> will be programmed with bit-flipped
 * value of <32-bit operand1>. SPI busy bit is defined by <16-bit operand3>.
 * Mnemonic HWSPIWFLIP_m <32-bit operand1> <16-bit operand2> <16-bit operand3>;
 * note@ Consult FPGA memory map registers for proper parameter settings.
 */
#define HWINIT_MACRO_HWSPIWFLIP_m  -1

/**
 * Macro expansion: This macro will create sequence of native HW initialization
 * instructions that will use FPGA HW SPI control registers to program HW chips.
 * HW chip selected by <16-bit operand2> will be programmed with value
 * of <32-bit operand1>. SPI busy bit is defined by <16-bit operand3>.
 * Mnemonic HWSPIW_m <32-bit operand1> <16-bit operand2> <16-bit operand3>;
 * note@ Consult FPGA memory map registers for proper parameter settings.
 */
#define HWINIT_MACRO_HWSPIW_m      -1

/**
 * Macro expansion: This macro will create sequence of native HW initialization
 * instructions that will use FPGA HW AMBA control registers to program VCXO 
 * frequency on Silabs 57X chip. Chip configuration values are set with value
 * of <64-bit operand1>.
 */
#define HWINIT_MACRO_HWSI57XFREQ_m -1

/**
 * Structure that holds board HW initialization data.
 */
struct hwinintialization
{
    uint32_t initCodeSize; ///< Number of bytes of initialization code in hwinintialization::initCode buffer (e.g. sizeof initCode).
    uint8_t initCode[HWINIT_SEQUENCE_LEN]; ///< Base or subsystem board HW initialization sequence.
} __attribute__ ((packed));

typedef struct hwinintialization hwinit_t;

#define MAC_ADDRESS_LENGTH 6 ///< MAC address length in bytes

/**
 * ICB1B base board specific configuration data.
 */
struct icb_baseboard
{
    uint8_t mnmgtEthMAC[MAC_ADDRESS_LENGTH]; ///< ARM Ethernet controller MAC address.
    uint8_t ignore_satellites;               ///< If bit is cleared, then satellite on bit position will be ignored.
} __attribute__ ((packed));

typedef struct icb_baseboard icb_baseboard_t;

/**
 * PCDB1 base board specific configuration data.
 */
struct pcdb1_baseboard
{
    uint16_t usbIdProduct; ///< USB ipProduct identification. Just for testing!
    uint32_t dummyVal; ///< Just for testing!
} __attribute__ ((packed));

typedef struct pcdb1_baseboard pcdb1_baseboard_t;

/**
 * AMC board specific configuration data.
 */
struct amc_baseboard
{
    struct {
        uint32_t slots[eCrateNum]; ///< crate slots compatibility
    } compatibility;
} __attribute__ ((packed));

typedef struct amc_baseboard amc_baseboard_t;

/**
 * Union space reservation for board specific HW configuration data.
 */
#define HWCONFIG_BOARD_SPECIFIC_SIZE    256

/** Base and subsystem board configuration structure version number. */
#define HWCONFIG_STRUCTVER    1
/** Board configuration structure magic number. */
#define HWCONFIG_MAGIC        0xC0F6B0AD
/** Max length of structure revision identification string. */
#define HWCONFIG_REVISION_LEN    128

/**
 * Header structure
 */
typedef struct cfg_header {
    uint32_t crcCode; ///< Calculated CRC code over data in this structure, without summing crcCode.
    uint32_t magicCode; ///< Structure magic code, used to have something to avoid all 0x00 or 0xFF CRC calculation problem.
    uint8_t  structVersion; ///< Version of this structure defined in HWCONFIG_STRUCTVER.
    char     revisionStrg[HWCONFIG_REVISION_LEN]; ///< Revision identification string  (0-terminated).
    uint32_t timeCreate; ///< Time when this structure data were created.
    uint32_t timeModify; ///< Time when this structure data was modified.
} __attribute__ ((packed)) cfg_header_t;

/**
 * Subsystem board structure that holds configuration data.
 * @note This structure is entity that is stored in HW FLASH for subsystem board.
 * @note IPMI interface commands are read and write members of this structure.
 */
struct subsysboard_configuration
{
    cfg_header_t header;     ///< record header
    struct subsys_hwidentification hwident; ///< HW identification data.
    struct hwinintialization hwinit; ///< HW initialization data.
} __attribute__ ((packed));

typedef struct subsysboard_configuration subsysboard_config_t;

/**
 * Base board structure that holds configuration data.
 * @note This structure is entity that is stored in HW FLASH for subsystem board.
 * @note IPMI interface commands are read and write members of this structure.
 */
struct baseboard_configuration
{
    cfg_header_t header;    ///< record header
    uint32_t fpgaImageReadTimeout; ///< FPGA boot timeout counter. Each counter decrement means FUNC_LOOP_MSECTIMEOUT delay in time domain.
    uint8_t boardType; ///< Type of base board for std.device_id field member.
    /** base board specific configuration data. */
    union {
        icb_baseboard_t icb;       ///< Place holder for ICB1B board specific data.
        pcdb1_baseboard_t pcdb1;   ///< Place holder for PCDB1 board specific data.
        amc_baseboard_t amc;       ///< Place holder for AMC board specific data.
        uint8_t reserved[HWCONFIG_BOARD_SPECIFIC_SIZE]; ///< Union space allocation.
    } board_type; ///< Base board specific data union member selector.
    struct board_hwidentification hwident; ///< HW identification data.
    struct hwinintialization hwinit; ///< HW initialization data.
    struct hwinintialization posthwinit; ///< Post power HW initialization data.
} __attribute__ ((packed));

typedef struct baseboard_configuration baseboard_config_t;

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// end of packing alignment for IPMI data structures
#ifdef CROSSWORKS
#pragma pack(pop)
#endif
//----------------------------------------------------------------------------

#endif /*IPMI_ITECH_TYPES_H*/

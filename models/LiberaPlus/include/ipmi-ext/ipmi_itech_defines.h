/*
 * Copyright (c) 2010 Instrumentation Technologies.
 * All Rights Reserved.
 *
 * $Id: ipmi_itech_defines.h 20071 2014-01-28 08:28:45Z crt.valentincic $
 */

#ifndef IPMI_ITECH_DEFINES_H
#define IPMI_ITECH_DEFINES_H

//----------------------------------------------------------------------------
/**
 *  Private enterprise numbers
 */
#define IANA_ITECH_PIE 38565 ///< Instrumentation technologies PIE
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// USB definitions

/**
 * USB devices definitions
 * @warning These constants need to be kept in sync with values in
 * custom ipmitool (bmc_usb.c).
 */
#define BMC_USB_VENDOR        0x1237    ///< vendor id
#define BMC_USB_PRODUCT_ICB   0xabcd    ///< icb usb produc id
#define BMC_USB_PRODUCT_SAT   0x0100    ///< satellite usb produc id offset

//----------------------------------------------------------------------------

/**
 * Definition values for the board std.device_id field.
 */
#define BMC_BOARD_ICB_ID            0x2C  ///< Interconnection board revision B.
#define BMC_BOARD_ADC_GDBRD_ID      0xAD  ///< ADC board.
#define BMC_BOARD_ADC_LLRF_ID       0xA0  ///< ADC LLRF board.
#define BMC_BOARD_ADC_SPH_ID        0xA1  ///< ADC SPH board.
#define BMC_BOARD_VM_ID             0xEC  ///< Vector Modulator board.
#define BMC_BOARD_TCM_ID            0x2E  ///< Timing board revision B.
#define BMC_BOARD_RAF_ID            0xAF  ///< RAF1 board.
#define BMC_BOARD_RAF_SPE_ID        0xAE  ///< RAF_SPE1 board.
#define BMC_BOARD_GDX_ID            0x7D  ///< GDX digital board.
#define BMC_BOARD_TIM_ID            0x3E  ///< TIM1 digital board.
#define BMC_BOARD_S5C_ID            0x5C  ///< spectra board
#define BMC_BOARD_SPE_ID            0x5D  ///< spectra DPP3 board
#define BMC_BOARD_EVRX_ID           0xEB  ///< EVRX digital board.
#define BMC_BOARD_SER_ID            0x5E  ///< rs485 serial board.
#define BMC_BOARD_GDBRD_ID          0xDB  ///< Generic digital Platform B board.
#define BMC_BOARD_UNKNOWN_ID        0xFF  ///< Board ID is unknown.
#define BMC_BOARD_INVALID_ID        0x00  ///< Reserved Board ID (invalid).

/**
 * Definitions for values of the board ipm_devid_ext_rsp::state field.
 */
#define BMC_BOARD_UNINITIALIZED_STATE    0x0    ///< Board is in uninitialized state.
#define BMC_BOARD_HW_INITIALIZED_STATE   0x1    ///< Board has HW initialized.
#define BMC_BOARD_FPGA_BOOTING           0x2    ///< FPGA is booting 
#define BMC_BOARD_FPGA_INITIALIZED_STATE 0x3    ///< Board successfully started FPGA and performing after FPGA boot initialization.
#define BMC_BOARD_FPGA_RUNNING_STATE     0x4    ///< Board finished after FPGA boot initializations and is in full functional state.
#define BMC_BOARD_FPGA_BOOT_TIMEOUT      0x5    ///< FPGA boot failed.
#define BMC_BOARD_DEFECTIVE_STATE        0xFF   ///< State is defective.

/**
 * Definitions for bit values of the board ipm_devid_ext_rsp::status field.
 */
#define BMC_BOARD_REDLEDON_BIT_STATUS    (1 << 0) ///< Bit set indicates that RED LED on board is on (lighted).
#define BMC_BOARD_POWEROFF_BIT_STATUS    (1 << 1) ///< Bit set indicates that board is power off.
#define BMC_BOARD_HWIDENT_BIT_STATUS     (1 << 2) ///< Bit set indicates that HW configuration is wrong (not matching).
#define BMC_BOARD_FLASHTOFPGA_BIT_STATUS (1 << 3) ///< Bit set indicates that flash switch is switched to fpga
#define BMC_BOARD_PCIE_CLK_BIT_STATUS    (1 << 4) ///< Bit set indicates that PCIe clock is missing

/**
 * Definitions for bit values of the board ipm_devid_ext_rsp::info_flags field.
 */
#define BMC_BOARD_SUBSYSPRESENT_BIT_INFO   (1 << 0) ///< Bit set indicates that subsystem board is present.


// Definition of the IPMI header structure constants
#define ITECH_HPM_OEM_DATA_LENGTH           0x0

// I-tech HPM components
#define HPM_COMPONENTE_0_FPGA_FLASH 0x01

//----------------------------------------------------------------------------
typedef enum {
     SENSOR_TYPE_RESERVED       = 0x00,
     SENSOR_TYPE_TEMPERATURE    = 0x01,
     SENSOR_TYPE_VOLTAGE        = 0x02,
     SENSOR_TYPE_CURRENT        = 0x03,
     SENSOR_TYPE_FAN            = 0x04,
     SENSOR_TYPE_CHASSIS_INTR   = 0x05,
     SENSOR_TYPE_SEC_VIOLATION  = 0x06,
     SENSOR_TYPE_PROCESSOR      = 0x07,
     SENSOR_TYPE_POWER_SUPPLY   = 0x08,
     SENSOR_TYPE_POWER_UNIT     = 0x09,
     SENSOR_TYPE_COOLING_DEVICE = 0x0A,
     SENSOR_TYPE_UNIT_SENSOR    = 0x0B,
     SENSOR_TYPE_MEMORY         = 0x0C
} sdr_sensor_type_t;
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// i-tech default temperature thresholds
// B = 0
#define DEF_TEMP_THRESHOLD_B0_HNR  85
#define DEF_TEMP_THRESHOLD_B0_HC   80 
#define DEF_TEMP_THRESHOLD_B0_HNC  70 
#define DEF_TEMP_THRESHOLD_B0_LNR  0 
#define DEF_TEMP_THRESHOLD_B0_LC   5 
#define DEF_TEMP_THRESHOLD_B0_LNC  10
// B=-20
#define DEF_TEMP_THRESHOLD_Bm20_HNR 105
#define DEF_TEMP_THRESHOLD_Bm20_HC  100 
#define DEF_TEMP_THRESHOLD_Bm20_HNC 90 
#define DEF_TEMP_THRESHOLD_Bm20_LNR 20 
#define DEF_TEMP_THRESHOLD_Bm20_LC  25 
#define DEF_TEMP_THRESHOLD_Bm20_LNC 35
//----------------------------------------------------------------------------

// SEL event record type: 0x00 - 0xBF standard type
#define SEL_STANDARD_RECORD         0x02
#define SEL_STANDARD_TYPE_LAST      0xBF
#define TYPE_CODE_SENSOR_SPECIFIC   0x6f ///< Standard SEL sensor direction and type.
#define OEM_SENSOR_TYPE             0xC0 ///< Standard SEL event sensor type.

// SEL event record type: 0xC0 - 0xDF OEM time stamped
#define SEL_OEMTS_RECORD            0xC2
#define SEL_OEM_TS_TYPE_LAST        0xDF

// SEL event record type: 0xE0 - 0xFF OEM non time stamped
#define SEL_OEMNOTS_SPEC_RECORD     0xE2
#define SEL_OEMNOTS_SPEC_TYPE_LAST  0xFF
//----------------------------------------------------------------------------

/**
 * Platform B application identifier. Each new instrument should be added to the
 * end of the list.
 */
typedef enum {
     appId_undefined = 0, ///< Undefined application Id.
     appId_LLRF = 1,      ///< Low Level RF application Id.
     appId_EBPM = 2,      ///< Brilliance+ application Id.
     appId_SPH = 3,       ///< Single Pass H application Id.
     appId_Spectra = 4,   ///< Spectra Id.
     appId_SPE = 5,       ///< Single Pass E application Id.
     appId_invalid = 0xFF ///< Invalid Id. @note FLASH erase value.
} applicationId_e;

/**
 * Platform B application subsystem board identifier.
 * Actual values are defined by the applications.
 *
 * @see fw_base.xsd and fw_app_defines.xsd for each application
 */
typedef unsigned int    appId_subsysId_t;

#define appId_none      (appId_undefined)
#define appId_RFInvalid (0xFF)


/**
 * Platform B list of current HW revisions for all base and subsystem boards.
 */
typedef enum {
    hwIdent_hwrevision_unknown = 0, ///< Undefined HW revision.
    hwIdent_hwrevision_A, ///< HW revision A.
    hwIdent_hwrevision_B, ///< HW revision B.
    hwIdent_hwrevision_C, ///< HW revision C.
    hwIdent_hwrevision_D, ///< HW revision D.
    hwIdent_hwrevision_E, ///< HW revision E.
    hwIdent_hwrevision_F, ///< HW revision F.
    hwIdent_hwrevision_G, ///< HW revision G.
    hwIdent_hwrevision_invalid = 0xFF ///< Invalid HW revision. @note FLASH erased value.
} hwRevisions_e;

/**
 * Platform B ICB box power state.
 */
typedef enum
{
    eBoxPowerOff = 0, /** ICB in power off state. */
    eBoxPowerOffPending, /** ICB in transition to power off state. */
    eBoxPowerOn /** ICB in power on state. */
} icbPowerState_e;

/**
 * Platform B ICB box power button actions.
 */
typedef enum {
    icbPowerButtonAction_None = 0, ///< No ICB power button action.
    icbPowerButtonAction_On, ///< Power ON ICB button action.
    icbPowerButtonAction_Off, ///< Power OFF ICB button action.
    icbPowerButtonAction_Cycle ///< Power cycle (off->on) ICB action.
} icbPowerButtonAction_e;

/**
 * Crate types
 */
typedef enum
{
    ePlatformB = 0, ///< platform B
    eMicroTCA,      ///< micro TCA std. platform
    eCrateNum
} crate_t;

#define PLATFORM_B_SLOTS_NUM 8
#define UTCA_SLOTS_NUM       12

/**
 * SEL storage location detection
 */
typedef enum
{
    eSelStorageLocFlash  = 0, ///< in flash together with FPGA image
    eSelStorageLocEeprom      ///< in eeprom, used only for SEL

} sel_storage_loc_t;

#endif /*IPMI_ITECH_DEFINES_H*/

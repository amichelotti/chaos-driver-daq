/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: types.h 20071 2014-01-28 08:28:45Z crt.valentincic $
 */

#ifndef BMC_TYPES_H
#define BMC_TYPES_H

#include <cstring>
#include <vector>
#include <bitset>

extern "C" {
    #include "ipmi-ext/ipmi_itech_defines.h"
    #include "ipmi-ext/ipmi_itech_types.h"

    struct ipm_devid_ext_rsp;
    typedef struct ipm_devid_ext_rsp ipm_devid_ext_rsp_t;

}

#include "bmc/uri.h"

#if 0
#warning "Replace with consts".
#define IPMI_LAN_TIMEOUT    2
#define IPMI_LAN_RETRY      4
#define IPMI_LAN_PORT       0x26f
#define IPMI_LAN_CHANNEL_E  0x0e

// uTca definitions
#define IPMI_BMC_UTCA_SLOT_MCH      0xc2
#define IPMI_BMC_UTCA_SLOT_AMC      0xc1
#define IPMI_BMC_UTCA_SLOT_PWD      0x0a
#define IPMI_BMC_UTCA_SLOT_COOL     0x1e

#define IPMI_BMC_UTCA_SLOT_MCH_NUMOFSLOT    0x2
#define IPMI_BMC_UTCA_SLOT_AMD_NUMOFSLOT    0xc

#define IPMI_BMC_UTCA_SLOT_POS_OFFSET       0x60
#endif

typedef volatile char FPGAMem_t;

namespace bmc {

	/**
	 * @addtogroup bmc_general Common helper Classes and functions
	 * @{
	 */

    /**
     * Firmware type
     * @note this value is set in firmware at compile time
     * and shouldn't be changed during run time!
     */
    enum FirmwareType_e {
        // only valid values for this enum are from 0 to 255 !!
        eFwInvalid      = BMC_BOARD_INVALID_ID,
        eFwICB          = BMC_BOARD_ICB_ID,
        eFwADC_GDBRD    = BMC_BOARD_ADC_GDBRD_ID,
        eFwGDBRD        = BMC_BOARD_GDBRD_ID,
        eFwVM           = BMC_BOARD_VM_ID,
        eFwTCM          = BMC_BOARD_TCM_ID,
        eFwUnknown      = BMC_BOARD_UNKNOWN_ID
    };

    /**
     * Board type
     * @note this value can be set/overwritten when setting board configuration
     */
    enum BoardType_e {
        // only valid values for this enum are from 0 to 255 !!

        eICB            = BMC_BOARD_ICB_ID,

        eVM             = BMC_BOARD_VM_ID,
        eTCM            = BMC_BOARD_TCM_ID,

        eADC_GDBRD      = BMC_BOARD_ADC_GDBRD_ID,
        eADC_LLRF       = BMC_BOARD_ADC_LLRF_ID,
        eADC_SPH        = BMC_BOARD_ADC_SPH_ID,

        eGDBRD          = BMC_BOARD_GDBRD_ID,
        eTIM            = BMC_BOARD_TIM_ID,
        eRAF            = BMC_BOARD_RAF_ID,
        eRAF_SPE        = BMC_BOARD_RAF_SPE_ID,
        eGDX            = BMC_BOARD_GDX_ID,
        eS5C            = BMC_BOARD_S5C_ID,
        eSPE            = BMC_BOARD_SPE_ID,
        eEVRX           = BMC_BOARD_EVRX_ID,
        eSER			= BMC_BOARD_SER_ID,

        // --
        // Boards ids above are 8-bit I-Tech AMC board IDs.
        // IDs that follow are used for non-I-Tech boards.
        //
        eNonAmcBoard= 0xF2, // This particular value is not used at all

        eOperatingSystem,
        eUTcaEntity,

        eBoardUnknown   = BMC_BOARD_UNKNOWN_ID // BoardInfo not yet queried
    };

    /**
     *  Board status
     */
    enum BoardStatus_e {
        // only valid values for this enum are from 0 to 255 !!
        eBoardDefective        = BMC_BOARD_DEFECTIVE_STATE, // Can't be contacted via Connection
        eBoardUninitialized    = BMC_BOARD_UNINITIALIZED_STATE,
        eBoardHWIinitialized   = BMC_BOARD_HW_INITIALIZED_STATE,
        eBoardFPGABooting      = BMC_BOARD_FPGA_BOOTING,
        eBoardFPGAInitialized  = BMC_BOARD_FPGA_INITIALIZED_STATE,
        eBoardRunning          = BMC_BOARD_FPGA_RUNNING_STATE,
        eBoardFPGATimeout      = BMC_BOARD_FPGA_BOOT_TIMEOUT
    };

    /**
     *  Board power status
     */
    enum BoardPowerStatus_e {
        eBoardPowerOff=0,
        eBoardPowerMgmt=1,
        eBoardPowerMain=2
    };

    /**
     *  Red led power status
     */
    enum RedLedStatus_e {
        eRedLedUnknown=0,
        eRedLedOn,
        eRedLedOff
    };

    /**
     *  Sensor type
     */
    enum SensorType_e {
        eSTUnknown,
        eSTFan,
        eSTTemp,
        eSTVoltage,
        eSTCurrent,
        eSTOtherAnalog,
        eSTDiscrete,
        eSTPower,
        eSTEnergy,
        eSTMemory,
        eSTCpu,
        eSTMcDeviceLocator,
        eSTHotSwapHandle
    };

    /**
     *  Sensor groups
     */
    enum SensorGroup_e {
        eIpmiSensorGroup = 0,
        eOsSensorGroup,
        eFpgaSensorGroup,
        // add new here
        eEndOfSensorGroupList
    };

    /**
     *  Fpga peripherial devices
     */
    enum FpgaPeriphDev_e {
        eADT7301 = 0,
        eAD7787AnalogMux
        // add new here
    };

    /**
     *  Power commands
     */
    enum Power_e {
        ePowerDown = 0,
        ePowerUp,
        ePowerCycle
    };

    /**
     *  Board (base & subsystem)
     */
    enum Board_e {
        eBaseBoard = IPMI_HWSTORAGE_BASEBOARD_IDENT,
        eSubsysBoard = IPMI_HWSTORAGE_SUBSYSBOARD_IDENT
    };

    /**
     * JTAG type
     */
    enum JTAG_e {
        eJTAGSelSlot    = 0,
        eJTAGTrigSet,
        eJTAGTrigClear,
        eJTAGSelLocal   = IPMI_CHASSIS_CTL_JTAG_SEL_LOCAL,
        eJTAGSelRemote  = IPMI_CHASSIS_CTL_JTAG_SEL_REMOTE,
        eJTAGEnable     = IPMI_CHASSIS_CTL_JTAG_ENABLE,
        eJTAGDisable    = IPMI_CHASSIS_CTL_JTAG_DISABLE,
        eJTAGSelArm     = IPMI_CHASSIS_CTL_JTAG_SEL_ARM,
        eJTAGSelFpga    = IPMI_CHASSIS_CTL_JTAG_SEL_FPGA
    };

    /**
     * Data stream access type as defined by the FPGA. This type is used
     * in the Linux kernel module to create device file names.
     * @see Board::CreateDeviceName.
     */
    enum DataStreamAccessType_e {
        DSOnDemand = 0,
        DSStream = 1,
        DSEvent = 2,
        DSHealth = 3,
        DSMonitor = 128
    };

    enum SensorLimit_e {
        eOK, eLowNR, eLowC, eLowNC, eHighNC, eHighC, eHighNR, eNA
    };

    class Sensor;
    typedef std::vector<Sensor*> Sensors;

    struct BoardBriefStatusMap{
        std::bitset<32> boardPresentBitmask;
        std::bitset<32> boardMgmtPwrBitmask;
        std::bitset<32> boardMainPwrBitmask;
    };

    // Board configuration structure holds all configuration data for board
    struct BoardConfiguration {

        BoardConfiguration()
        : boardType(eBoardUnknown), firmwareType(eFwInvalid)
        {
            memset(&baseBoard, 0, sizeof(baseBoard));
            memset(&subsysBoard, 0, sizeof(subsysBoard));
        }

        baseboard_config_t      baseBoard;
        subsysboard_config_t    subsysBoard;
        BoardType_e             boardType;
        FirmwareType_e          firmwareType;
        bmc::Uri                boardUri;
    };

    // A subset of above complete structure
    struct HwIdentification {
        bool                valid;
        applicationId_e     applicationId;
        appId_subsysId_t    subsysId;
        int                 hwRevision;
        std::string         customerId;
        std::string         serialNo;
        std::string         cfgRevision;
    };
    /**@}*/

} // namespace

#endif /*  */

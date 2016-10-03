/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: board.h 19990 2014-01-07 09:35:55Z damijan.skvarc $
 */

#ifndef BMC_BOARD_H
#define BMC_BOARD_H

#include <memory>
#include <string>
#include <vector>

#include "istd/system.h"
#include "istd/time.h"
#include "istd/stl_helpers.h"
#include "istd/any_reference.h"
#include "bmc/types.h"
#include "bmc/sensor_id.h"
#include "bmc/hpm_image.h"
#include "bmc/uri.h"
#include "bmc/spi_bus_controller_fpga_amba.h"
#include "bmc/spi_device_serial_flash.h"

namespace bmc {

    class Connection;
    class ConnectionCallback;
    class SystemEventLog;
    class SerialFlash;
    class Sensor;

    extern std::string c_defaultBoardName;

    // FPGA features
    const uint32_t c_fpgaFeatImageUpgrade = (0x1);

    struct DeviceMapping_t {
        inline DeviceMapping_t() : addr(NULL), size(-1), fd(istd::c_invalidFd) {};

        FPGAMem_t   *addr;
        ssize_t      size;
        int          fd;
        bool         IsValid() const {
            return ((addr != NULL) && (size!=-1) && (fd!=istd::c_invalidFd)) ? true : false;
        }
    };

	/**
	 * @addtogroup bmc_board
	 * @{
	 */
    class Board {
        public:

            const Uri&          GetUri() const;
            const std::string&  GetName() const;
            std::string         GetShortName() const;
            std::string         GetShortId() const;

            BoardType_e         GetType() const;
            FirmwareType_e      GetFirmwareType() const;

            BoardStatus_e       GetStatus() const;

            /*
             *  standard device id (info)
             */
            int                 GetDeviceId() const;
            int                 GetDeviceRevision() const;
            int                 GetMajorVer() const;
            int                 GetMinorVer() const;
            int                 GetIpmiVersion() const;
            int                 GetDeviceSupport() const;
            int                 GetManufacturerId() const;
            int                 GetProductId() const;
            int                 GetFwRevision() const;
            int                 GetFpgaRevision() const;

            RedLedStatus_e      GetRedLighStatus() const;

            BoardPowerStatus_e  ReadPowerStatus();
            BoardPowerStatus_e  GetPowerStatus() const;

            /** On-board FLASH chip identification:
             * Upper byte is manufacturer ID,
             * lower byte is chip ID of tha manufacturer.
             * @retval 0x0000 Inaccessible FLASH chip
             * @retval 0xFFFF Inaccessible FLASH chip
             */
            SystemEventLog*     GetSel() const;

            SensorIds           GetSensorsIds();

            // GetAllSensors?
            Sensor*             GetSensor(const SensorId& a_id);

            std::string         GetDevicePrefix() const;
            std::string         CreateDeviceName(
                                    DataStreamAccessType_e a_atype,
                                    int a_streamNum = 0) const;

            HwIdentification    GetHardwareId();
            HwIdentification    GetSubsysHardwareId();

            /** Queries all information about the board again. */
            void                Update();
            void                ShutDown();
            void                PowerCycle();
            void                PowerUp();
            void                PowerDown();
            void                FpgaReboot();

            void                SetPortStateUsb(bool a_state);

            void FpgaSpiWrite(uint16_t a_addr, uint16_t a_data) const;
            chassis_ctl_fpgaread_rsp FpgaSpiRead(uint16_t a_addr) const;
            void FpgaAmbaWrite(uint32_t a_addr, uint32_t a_data) const;
            chassis_ctl_ambaread_rsp FpgaAmbaRead(uint32_t a_addr) const;

            // Time methods
            void SetTime(const istd::Time& a_time);
            istd::Time GetTime() const;

            /** Reads FPGA image from the board */
            HpmImage ReadImage() const;

            /** Write FPGA image on the board flash memory */
            void WriteImage(const HpmImage& a_img);
            void WriteImage(const RawImage& a_img);

            void SetConnectionCallback(ConnectionCallback *a_callback);

            bmc::BoardConfiguration ReadConfiguration();
            void WriteConfiguration(bmc::BoardConfiguration& a_bCfg);
            storage_hwstorage_hwinit_rsp InitHwInit(bmc::Board_e a_board);
            void JtagCmd(bmc::JTAG_e a_jtagCmd, int a_boardId = 0);

            void MapDevice(DeviceMapping_t &a_device_mapping) const throw (istd::Exception);
            void UnmapDevice(DeviceMapping_t &a_device_mapping) const;

            bool IsFeatureSupported(const DeviceMapping_t &a_devMap, uint32_t a_feature) const;

        private:
            explicit Board(
                const Uri&          a_url,
                BoardPowerStatus_e  a_powerStatus,
                const std::string&  a_name);

            virtual ~Board();

            void                ExecuteCmdOverIcb(unsigned int a_cmd, istd::AnyReference a_par);
            void                Power(bmc::Power_e a_power);
            Connection*         GetConnection();

            void SendImagePcie(
                SerialFlash*       a_serialFlash,
                const ImageData&   a_data) const;

            bool WriteImagePcie(const ImageData& a_img);
            void WriteImageData(const ImageData& a_img);

            void AddFpgaSensors(size_t& a_sensorCount);

            const Uri           m_uri; ///< Address of the board
            BoardPowerStatus_e  m_powerStatus;
            std::string         m_name; ///< Name, used to display the board in UI
            std::string         m_shortName;

            mutable std::unique_ptr<ipm_devid_ext_rsp_t> m_info;
            std::string         m_devStr;
            Connection*         m_connection;
            Sensors             m_sensors;
            SystemEventLog*     m_sel;

            std::vector<BusControllerPtr> m_pCtrl;
            std::vector<BusDevicePtr> m_pDevice;

            friend Board* CreateBoard(const Uri&, BoardPowerStatus_e, const std::string&);
            friend void DeleteBoard(Board*);

            friend class Sensor;
            friend class SystemEventLog;
            friend struct istd::DeleteMapElement;
    };
    /**@}*/

}


#endif /* BMC_BOARD_H */

/*
 * Copyright (c) 2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: spi_device_serial_flash.h 19990 2014-01-07 09:35:55Z damijan.skvarc $
 */

#ifndef SPI_DEVICE_SERIAL_FLASH_H_
#define SPI_DEVICE_SERIAL_FLASH_H_

#include "spi_device.h"
#include "devices/serial_flash.h"

#define SKIP_ADDR                           0xFFFFFFFF

#define RETRY_SLEEP_US                      1
#define RETRY_IN_PROGRESS_ERASE_CNT         2000000
#define RETRY_IN_PROGRESS_FLASH_DETECT_CNT  10000
#define RETRY_IN_PROGRESS_CNT               100000

/*-----------------------------------------------------------------------------*/
namespace bmc {

    struct tFlashWriteInfo {
        uint32_t flashWriteAddr;
        uint32_t flashSector;
        uint32_t flashBytesWritten;
    };

    /* class declaration */
    class SerialFlash : public SpiDevice
    {
    public:
        explicit SerialFlash(
            BusController &a_controller,
            const std::string &a_name,
            BusDeviceId a_id, bool a_writeOnly);

        virtual ~SerialFlash();

        bool    AutoincWriteInit(void);
        bool    AutoincWrite(const uint8_t* a_page);
        bool    AutoincWriteFinish(const uint8_t* a_page, uint32_t a_len);

        bool    GetId(tFlashId * a_flashId);

    private:
        const tStorageDevice * m_ptrFlashCfg;
        tFlashWriteInfo m_flashWriteInfo;
        bool            m_autoinc_enable;

        void    SetLen(uint16_t a_val);

        bool    TransferData(uint8_t a_opCode, uint32_t a_addr,
                             const uint8_t * a_wBuf, uint32_t a_wLen,
                             uint8_t * a_rBuf, uint32_t a_rLen);

        bool    ReadPage(uint32_t a_addr, uint8_t * a_buf);
        bool    WritePage(uint32_t a_addr, const uint8_t * a_buf);

        bool    WriteDataAtAddr(uint32_t a_addr, const uint8_t * a_wBuf, uint32_t a_wLen);
        bool    ReadDataAtAddr(uint32_t a_addr, uint8_t * a_rBuf, uint32_t a_rLen);

        bool    WritePageAtAddr(uint32_t a_addr, const uint8_t * a_wBuf);
        bool    ReadPageAtAddr(uint32_t a_addr, uint8_t * a_rBuf);


        bool    SectorErase(uint16_t a_sectorNum);
        bool    ConfigInit(void);
        bool    ConfigIsInit(void);

        uint8_t GetStatus(void);
        void    SetStatus(uint8_t a_status);
        bool    IsWriteInProcess(void);
        bool    IsReady(uint32_t a_timeout);
        void    WriteEnable(bool a_enable);
        bool    IsWriteEnabled(void);
        bool    IsReadyForWritting(uint32_t a_timeout);

    };
}


#endif /* SPI_DEVICE_SERIAL_FLASH_H_ */

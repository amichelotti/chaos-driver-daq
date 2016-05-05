/*
 * Copyright (c) 2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: spi_bus_controller_fpga_amba.h 19990 2014-01-07 09:35:55Z damijan.skvarc $
 */

#ifndef SPI_BUS_CONTROLLER_FPGA_AMBA_H_
#define SPI_BUS_CONTROLLER_FPGA_AMBA_H_

#include "spi_bus_controller.h"


#define CLK_PRESCALE_OFFS        0
#define CLK_PRESCALE_SIZE        5
#define CLK_PRESCALE_MASK        ((1<<CLK_PRESCALE_SIZE)-1)

#define TRANSFER_LEN_OFFS        8
#define TRANSFER_LEN_SIZE        6
#define TRANSFER_LEN_MASK       ((1<<TRANSFER_LEN_SIZE)-1)

#define CLK_POLARITY_OFFS        16
#define CLK_POLARITY_MASK        (1<<CLK_POLARITY_OFFS)

#define CLK_TRANSMIT_EDGE_OFFS   17
#define CLK_TRANSMIT_EDGE_MASK   (1<<CLK_TRANSMIT_EDGE_OFFS)

#define CLK_SAMPLE_EDGE_OFFS     18
#define CLK_SAMPLE_EDGE_MASK     (1<<CLK_SAMPLE_EDGE_OFFS)

#define TRANSMIT_LSB_FIRST_OFFS  19
#define TRANSMIT_LSB_FIRST_MASK  (1<<TRANSMIT_LSB_FIRST_OFFS)

#define LOCK_TRANSFER_OFFS       20
#define LOCK_TRANSFER_MASK       (1<<LOCK_TRANSFER_OFFS)

#define BUSY_FLAG_OFFS           21
#define BUSY_FLAG_MASK           (1<<BUSY_FLAG_OFFS)


/*---------------------------------------------------------------------------------------------*/
/* fpga spi bus controller memory map */

namespace bmc {

    struct tSpiBusController {
        /* offset + 0x00 */ uint64_t control;
        /* offset + 0x08 */ uint64_t chip_select;
        /* offset + 0x10 */ uint64_t data;
        /* offset + 0x18 */ uint64_t output_user_data;
        /* offset + 0x20 */ uint64_t input_user_data;
    };

    class  SpiBusControllerFpgaAmba;
    typedef std::shared_ptr<SpiBusControllerFpgaAmba>  SpiBusControllerFpgaAmbaPtr;

    class  SpiBusControllerFpgaAmba : public SpiBusController {

    public:
        SpiBusControllerFpgaAmba(const std::string &a_name, uint32_t a_capacity, bool a_cs_active_high, volatile tSpiBusController *a_reg);
        virtual ~SpiBusControllerFpgaAmba();

        static size_t Size()    { return sizeof(tSpiBusController); }

    protected:
        virtual void  Write(uint8_t a_reg, uint64_t a_data);
        virtual void  Read(uint8_t a_reg, uint64_t &a_data);
        virtual void  SetBusParams(const bmc::BusParamsPtr &a_params);
        virtual void  SelectDevice(const BusDevice &a_device, tDevSelectMode_e a_select);
        virtual bool  ToggleChipSelect() { return m_toggleCS; }

        virtual bool  IsIdle();
        virtual bool  IsAcknowledged();

        virtual bool  OnSetOutput(uint32_t  a_val);
        virtual bool  OnGetOutput(uint32_t &a_val);

    private:

        /* interface to FPGA PCI memory */
        volatile tSpiBusController  *m_reg;
        bool                         m_toggleCS;
    };
}
#endif /* SPI_BUS_CONTROLLER_FPGA_AMBA_H_ */

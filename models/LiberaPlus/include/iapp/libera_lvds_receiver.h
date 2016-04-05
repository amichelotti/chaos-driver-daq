/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * LVDS Receiver FPGA object implementation.
 *
 * $Id: libera_lvds_receiver.h 14606 2011-12-21 12:43:41Z matej.kenda $
 */

#ifndef LIBERA_LVDS_RECEIVER_H
#define LIBERA_LVDS_RECEIVER_H

#include <vector>
#include "declarations.h"
#include "libera_board.h"
#include "libera_fpga_object.h"

namespace iapp {

    struct tLvdsRxSlot {
        /* 0x00 */ 
        struct {
            uint64_t checkMode  :  1;
            uint64_t reserved0  :  7;
            uint64_t autoDelay  :  1;
            uint64_t reserved1  : 22;
            uint64_t failure    :  1;
            uint64_t reserved2  : 32;
        } control;
        /* 0x08 */ union {
            struct { /* Read registers */
                uint64_t delay    :  6;
                uint64_t reserved : 58;

                bool operator==(const uint64_t data) volatile {
                    return (delay == data);
                };
                uint64_t operator&(const uint64_t data) volatile {
                    return (delay & data);
                };
            } delay;
            struct { /* Write registers */
                /* Delay Increment & Enable MUST be written simulatenously */
                uint64_t delayCtrl :  3; /* DelayInc, DelayEn & DelayRst */
                uint64_t bitslip  :  9;
                uint64_t reserved : 52;
            } serdesCtrl;
        };
        /* 0x10 */ uint64_t checkDataWord0;
        /* 0x18 */ uint64_t checkDataWord1;
    };

    struct tLvdsRx {
        tLvdsRxSlot slot[4];
    };

    class LiberaLvdsReceiver : public LiberaFpgaObject {
    public:
        enum LvdsRxVersion { eLvdsVersion2 = 0,
                             eLvdsVersion3 = 1 };

    private:
        static const size_t c_lvdsRxNumRetries   = 1000;
        static const size_t c_lvdsRxEnableOffset = 0x4000;

        static const size_t c_lvdsRxDelayIncBit = 1;
        static const size_t c_lvdsRxDelayEnBit = 2;
        static const size_t c_lvdsRxDelayRstBit = 4;
    public:
        LiberaLvdsReceiver(LiberaBoard &a_board, FPGAMem_t *a_reg,
                           std::string a_name);
        virtual ~LiberaLvdsReceiver();

        /* LVDS Rx has enable set at offset + c_lvdsRxEnableOffset so we have to
         * return this size.
         */
        static size_t Size() { return (c_lvdsRxEnableOffset+sizeof(uint64_t)); }

        bool GetWindow(size_t a_slot);
        bool SetBitslip(size_t a_slot, const uint64_t a_bitslipWord);

        bool Enable(size_t a_slot);
        bool Disable(size_t a_slot);

        void SetNumRetries(long a_numRetries) { m_numRetries = a_numRetries; }

    protected:
        virtual bool        OnQueryChild(const LiberaObject &a_child);

        int SlotToRxPort(size_t a_slot);

    private:
        volatile tLvdsRx  *m_reg;
        /* enable is located at &m_reg + c_lvdsRxEnableOffset */
        volatile uint64_t *m_enable; 
        int                m_numRetries;
    };

    
    typedef std::vector<LiberaLvdsReceiver*> LiberaLvdsReceivers;

} //namespace


#endif // LIBERA_LVDS_RECEIVER_H


/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * LVDS Transmitter FPGA object implementation.
 *
 * $Id: libera_lvds_transmitter.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef LIBERA_LVDS_TRANSMITTER_H
#define LIBERA_LVDS_TRANSMITTER_H

#include <vector>
#include "declarations.h"
#include "libera_board.h"
#include "libera_fpga_object.h"

namespace iapp {
    struct tLvdsTx {
        /* 0x00 */ struct {
            uint64_t training : 1;
            uint64_t reserved : 63;
        } control;
        /* 0x08 */ uint64_t trainWord0;
        /* 0x10 */ uint64_t trainWord1;
    };

    class LiberaLvdsTransmitter : public LiberaFpgaObject
    {
    public:
        static const uint64_t c_lvdsTrainWord0  = 0x123456789ULL;
        static const uint64_t c_lvdsTrainWord1  = 0xedcba9876ULL;
        static const uint64_t c_lvdsBitslipWord = 0x888888888ULL;

        enum LvdsTxModeType { eLvdsTxNormal = 0,
                              eLvdsTxTraining = 1 };

    public:
        LiberaLvdsTransmitter(LiberaBoard &a_board, FPGAMem_t *a_reg,
                              std::string a_name);
        virtual ~LiberaLvdsTransmitter();

        static size_t       Size()       { return sizeof(tLvdsTx); }

        bool SetMode(LvdsTxModeType a_mode);
        bool SetTrainWords(uint64_t a_word0, uint64_t a_word1);

        int  GetSlot() { return m_slot; };
    protected:
        virtual bool        OnQueryChild(const LiberaObject &a_child);

    private:
        volatile tLvdsTx *m_reg;
        int               m_slot;
    };

    
    typedef std::vector<LiberaLvdsTransmitter*> LiberaLvdsTransmitters;

} //namespace


#endif // LIBERA_LVDS_TRANSMITTER_H

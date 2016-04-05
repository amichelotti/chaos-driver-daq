/*
 * Copyright (c) 2008-2009 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: libera_board.h 19990 2014-01-07 09:35:55Z damijan.skvarc $
 */

#ifndef LIBERA_BOARD_H
#define LIBERA_BOARD_H

#include "istd/trace.h"
#include "istd/system.h"
#include "istd/exception.h"
#include "ireg/declarations.h"
#include "bmc/types.h"
#include "bmc/uri.h"
#include "bmc/board.h"

#include "declarations.h"
#include "libera_object.h"
#include "libera_fpga_object.h"
#include "identification_block.h"

namespace iapp {

    class LiberaApplication;

    class LiberaBoard : public LiberaObject
    {
    public:
        const bmc::Uri&      Uri() const;
        const bmc::Board*    Board() const;
        bmc::BoardType_e     GetType() const;
        void                 FlushRegisters(volatile void * a_fpga_reg);
        LiberaApplication&   GetApplication();
        IdentificationBlock* IdBlock() { return m_id_block; };

        /* callback functions, called by LocalTimingSync during synchronization */
        virtual void         OnAnnounceSync() {};
        virtual void         OnAdjustedTimestamp(int64_t a_diff_lmt, uint64_t a_start_lmt) {};

    protected:

        template<class FPGA_OBJ, typename ...ARGS>
            FPGA_OBJ* RegisterObject(
                size_t a_offset,
                const std::string &a_name,
                ARGS&& ...a_args)
            {
                // Template parameter must be derived from LiberaFpgaObject
                static_assert(
                    std::is_base_of<LiberaFpgaObject, FPGA_OBJ>::value,
                    "FPGA_OBJ is not derived from LiberaFpgaObject");

                istd_FTRC();

                if (FPGA_OBJ::Size() == 0) {
                    throw istd::Exception("Invalid FPGA memory interface");
                }

                FPGAMem_t *pMem = MapRegisters(a_offset, FPGA_OBJ::Size());
                if (pMem == NULL) {
                    throw istd::Exception(
                            "Failure while mapping to FPGA memory registers");
                }
                FPGA_OBJ *p = iapp::Create<FPGA_OBJ>(*this, pMem, a_name,
                                               std::forward<ARGS>(a_args)...);

                return p;
            }

    protected:

        LiberaBoard(LiberaApplication &a_application, const bmc::Board* a_board);
        virtual ~LiberaBoard();

        /**
         * Maps a certain register region residing on specific offset to
         * a pointer to mapped memory pertaining to this board.
         * @param a_offset Offset of register region from base in bytes
         * @param a_size   Size of register region from base in bytes
         * @return pointer inside mapped memory
         * @retval NULL register section can't be mapped
         */
        FPGAMem_t*     MapRegisters(ssize_t a_offset, ssize_t a_size);
        virtual bool   OnQueryChild(const LiberaObject &a_child);

        virtual void   MapDevice(bmc::DeviceMapping_t &a_device_mapping) const;
        virtual void   UnmapDevice(bmc:: DeviceMapping_t &a_device_mapping) const;
        void           UnmapDevice();

        LiberaApplication     &m_application;

    private:
        const bmc::Board*      m_board;
        bmc::DeviceMapping_t   m_device_mapping;
        IdentificationBlock   *m_id_block;
    };

    typedef std::vector<LiberaBoard*> LiberaBoards;

}


#endif // LIBERA_BOARD_H

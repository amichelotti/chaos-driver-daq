/*
 * Copyright (c) 2010 Instrumentation Technologies
 * All Rights Reserved.
 *
 * $Id: tim_id_specific.h 15635 2012-04-05 13:35:15Z matej.kenda $
 */


#ifndef TIM_ID_SPECIFIC_H_
#define TIM_ID_SPECIFIC_H_

#include "iapp/libera_fpga_object.h"
#include "iapp/libera_board.h"

struct tTimId {
/* 0x000 */    const uint32_t  f_adc;           ///< ADC sampling frequency
               const uint32_t  reserved0;
};

class TimIdSpecific : public iapp::LiberaFpgaObject
{
public:
    TimIdSpecific(iapp::LiberaBoard &a_board, FPGAMem_t *a_reg, const std::string &a_name);
    virtual ~TimIdSpecific();

    static size_t  Size() { return sizeof(tTimId); }

    uint32_t GetADCFrequency() const;

protected:
    virtual void OnRegistryAdd(ireg::TreeNodePtr &parent);

private:
    volatile tTimId *m_reg;

    uint32_t m_adc_freq;
};

#endif /* TIM_ID_SPECIFIC_H_ */

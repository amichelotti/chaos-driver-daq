/*
 * Copyright (c) 2011 Instrumentation Technologies
 * All Rights Reserved.
 *
 * Silabs Si5338 i2c device - bmc.
 *
 * $Id: i2c_device_SI5338.h 17623 2012-09-05 07:58:09Z damijan.skvarc $
 */

#ifndef SI5338_H
#define SI5338_H

#include "i2c_device.h"

/*-----------------------------------------------------------------------------*/

/* I2C Si5338 chip address. */
#define SI5338_I2C_ADDRESS        0x70

#define SI5338_REG_AREA_SIZE_IN_BYTES 351

namespace bmc {

// If a mask is 0x00, all bits in the associated register are
// reserved and must remain unchanged. If the mask is 0xFF,
// all the bits in the register can be changed.
const uint8_t c_regMask[SI5338_REG_AREA_SIZE_IN_BYTES]={
        0x00, // 0
        0x00, // 1
        0x00, // 2
        0x00, // 3
        0x00, // 4
        0x00, // 5
        0x1D, // 6
        0x00, // 7
        0x00, // 8
        0x00, // 9
        0x00, // 10
        0x00, // 11
        0x00, // 12
        0x00, // 13
        0x00, // 14
        0x00, // 15
        0x00, // 16
        0x00, // 17
        0x00, // 18
        0x00, // 19
        0x00, // 20
        0x00, // 21
        0x00, // 22
        0x00, // 23
        0x00, // 24
        0x00, // 25
        0x00, // 26
        0x80, // 27
        0xFF, // 28
        0xFF, // 29
        0xFF, // 30
        0xFF, // 31
        0xFF, // 32
        0xFF, // 33
        0xFF, // 34
        0xFF, // 35
        0x1F, // 36
        0x1F, // 37
        0x1F, // 38
        0x1F, // 39
        0xFF, // 40
        0x7F, // 41
        0x3F, // 42
        0x00, // 43
        0x00, // 44
        0xFF, // 45
        0xFF, // 46
        0x3F, // 47
        0xFF, // 48
        0xFF, // 49
        0xFF, // 50
        0xFF, // 51
        0x7F, // 52
        0xFF, // 53
        0xFF, // 54
        0xFF, // 55
        0xFF, // 56 uint8_t&, co
        0xFF, // 57
        0xFF, // 58
        0xFF, // 59
        0xFF, // 60
        0xFF, // 61
        0x3F, // 62
        0x7F, // 63
        0xFF, // 64
        0xFF, // 65
        0xFF, // 66
        0xFF, // 67
        0xFF, // 68
        0xFF, // 69
        0xFF, // 70
        0xFF, // 71
        0xFF, // 72
        0x3F, // 73
        0x7F, // 74
        0xFF, // 75
        0xFF, // 76
        0xFF, // 77
        0xFF, // 78
        0xFF, // 79
        0xFF, // 80
        0xFF, // 81
        0xFF, // 82
        0xFF, // 83
        0x3F, // 84
        0x7F, // 85
        0xFF, // 86
        0xFF, // 87
        0xFF, // 88
        0xFF, // 89
        0xFF, // 90
        0xFF, // 91
        0xFF, // 92
        0xFF, // 93
        0xFF, // 94
        0x3F, // 95
        0x00, // 96
        0xFF, // 97
        0xFF, // 98
        0xFF, // 99
        0xFF, // 100
        0xFF, // 101
        0xFF, // 102
        0xFF, // 103
        0xFF, // 104
        0xFF, // 105
        0xBF, // 106
        0xFF, // 107
        0x7F, // 108
        0xFF, // 109
        0xFF, // 110
        0xFF, // 111
        0x7F, // 112
        0xFF, // 113
        0xFF, // 114
        0xFF, // 115
        0xFF, // 116
        0xFF, // 117
        0xFF, // 118
        0xFF, // 119
        0xFF, // 120
        0xFF, // 121
        0xFF, // 122
        0xFF, // 123
        0xFF, // 124
        0xFF, // 125
        0xFF, // 126
        0xFF, // 127
        0xFF, // 128
        0x0F, // 129
        0x0F, // 130
        0xFF, // 131
        0xFF, // 132
        0xFF, // 133
        0xFF, // 134
        0xFF, // 135
        0xFF, // 136
        0xFF, // 137
        0xFF, // 138
        0xFF, // 139
        0xFF, // 140
        0xFF, // 141
        0xFF, // 142
        0xFF, // 143
        0xFF, // 144
        0x00, // 145
        0x00, // 146
        0x00, // 147
        0x00, // 148
        0x00, // 149
        0x00, // 150
        0x00, // 151
        0xFF, // 152
        0xFF, // 153
        0xFF, // 154
        0xFF, // 155
        0xFF, // 156
        0xFF, // 157
        0x0F, // 158
        0x0F, // 159
        0xFF, // 160
        0xFF, // 161
        0xFF, // 162
        0xFF, // 163
        0xFF, // 164
        0xFF, // 165
        0xFF, // 166
        0xFF, // 167
        0xFF, // 168
        0xFF, // 169
        0xFF, // 170
        0xFF, // 171
        0xFF, // 172
        0xFF, // 173
        0xFF, // 174
        0xFF, // 175
        0xFF, // 176
        0xFF, // 177
        0xFF, // 178
        0xFF, // 179
        0xFF, // 180
        0x0F, // 181
        0xFF, // 182
        0xFF, // 183
        0xFF, // 184
        0xFF, // 185
        0xFF, // 186
        0xFF, // 187
        0xFF, // 188
        0xFF, // 189
        0xFF, // 190
        0xFF, // 191
        0xFF, // 192
        0xFF, // 193
        0xFF, // 194
        0xFF, // 195
        0xFF, // 196
        0xFF, // 197
        0xFF, // 198
        0xFF, // 199
        0xFF, // 200
        0xFF, // 201
        0xFF, // 202
        0x0F, // 203
        0xFF, // 204
        0xFF, // 205
        0xFF, // 206
        0xFF, // 207
        0xFF, // 208
        0xFF, // 209
        0xFF, // 210
        0xFF, // 211
        0xFF, // 212
        0xFF, // 213
        0xFF, // 214
        0xFF, // 215
        0xFF, // 216
        0xFF, // 217
        0x00, // 218
        0x00, // 219
        0x00, // 220
        0x00, // 221
        0x00, // 222
        0x00, // 223
        0x00, // 224
        0x00, // 225
        0x04, // 226
        0x00, // 227
        0x00, // 228
        0x00, // 229
        0xFF, // 230
        0x00, // 231
        0x00, // 232
        0x00, // 233
        0x00, // 234
        0x00, // 235
        0x00, // 236
        0x00, // 237
        0x00, // 238
        0x00, // 239
        0x00, // 240
        0xFF, // 241
        0x02, // 242
        0x00, // 243
        0x00, // 244
        0x00, // 245
        0xFF, // 246
        0x00, // 247
        0x00, // 248
        0x00, // 249
        0x00, // 250
        0x00, // 251
        0x00, // 252
        0x00, // 253
        0x00, // 254
        0xFF, // 255
        0x00, // 256
        0x00, // 257
        0x00, // 258
        0x00, // 259
        0x00, // 260
        0x00, // 261
        0x00, // 262
        0x00, // 263
        0x00, // 264
        0x00, // 265
        0x00, // 266
        0x00, // 267
        0x00, // 268
        0x00, // 269
        0x00, // 270
        0x00, // 271
        0x00, // 272
        0x00, // 273
        0x00, // 274
        0x00, // 275
        0x00, // 276
        0x00, // 277
        0x00, // 278
        0x00, // 279
        0x00, // 280
        0x00, // 281
        0x00, // 282
        0x00, // 283
        0x00, // 284
        0x00, // 285
        0x00, // 286
        0xFF, // 287
        0xFF, // 288
        0xFF, // 289
        0xFF, // 290
        0xFF, // 291
        0xFF, // 292
        0xFF, // 293
        0xFF, // 294
        0xFF, // 295
        0xFF, // 296
        0xFF, // 297
        0xFF, // 298
        0x0F, // 299
        0x00, // 300
        0x00, // 301
        0x00, // 302
        0xFF, // 303
        0xFF, // 304
        0xFF, // 305
        0xFF, // 306
        0xFF, // 307
        0xFF, // 308
        0xFF, // 340
        0xFF, // 341
        0xFF, // 342
        0xFF, // 343
        0xFF, // 344
        0xFF, // 345
        0xFF, // 346
        0x0F, // 347
        0x00, // 348
        0x00, // 349
        0x00, // 350

};

typedef struct _tRegMap {
    // chip revision
    struct {
        uint8_t revID     : 3;
        uint8_t reserved  : 5;
    } reg0;
    // - unused
    uint8_t reg1To5[5];
    // Mask bits for LOS_CLKIN,LOS_FB, LOL, SYS_CAL
    struct {
        uint8_t sysCalMask   : 1;
        uint8_t reserved1    : 1;
        uint8_t losClkinMask : 1;
        uint8_t losFdbkMask  : 1;
        uint8_t pllLolMask   : 1;
        uint8_t reserved     : 3;
    } reg6;
    // - unused
    uint8_t reg7To26[20];
    // i2c configuration
    struct {
        uint8_t i2cAddr   : 7;
        uint8_t i2cIp8Sel : 1;
    } reg27;
    // Reference clock reg.
    struct {
        uint8_t xtalFreq  : 2;
        uint8_t p1DivIn   : 3;
        uint8_t p2DivIn   : 1;
        uint8_t reserved   : 2;
    } reg28;
#define DIVFBCLK    0x3
#define DIVREFCLK   0x2
    struct {
        uint8_t p1Div     : 3;
        uint8_t p1DivIn   : 2;
        uint8_t pfdInRef  : 3;
    } reg29;
    struct {
        uint8_t p2Div     : 3;
        uint8_t p2DivIn   : 2;
        uint8_t pfdInFb   : 3;
    } reg30;
    // Output clock select
#define RxDIV_IN_Mx 0x6
    struct {
        uint8_t multiSynthPD  : 1;
        uint8_t rxDivAndClkPD : 1;
        uint8_t rxDiv         : 3;
        uint8_t rxDivIn       : 3;
    } reg31To34[4];
    // Output drivers config
#define DRVx_VDD0_3V3       0
    struct {
        uint8_t drv0VddO  : 2;
        uint8_t drv1VddO  : 2;
        uint8_t drv2VddO  : 2;
        uint8_t drv3VddO  : 2;
    } reg35;
#define DRVx_FMT_LVPECL    0x4
    struct {
        uint8_t drvxFmt   : 3;
        uint8_t reserved  : 5;
    } reg36To39[4];
#define DRVx_TRIM_LVPECL    0xf
    struct {
        uint8_t  drv0Trim   : 5;
        uint8_t  drv1TrimL3 : 3;
    } reg40;
    struct {
        uint8_t  drv1TrimH2 : 2;
        uint8_t  drv2Trim   : 5;
        uint8_t  reserved   : 1;
    } reg41;
    struct {
        uint8_t  drv3Trim  : 5;
        uint8_t  reserved  : 3;
    } reg42;
    // -
    uint8_t reg43To44[2];
    // fcal override
    uint8_t reg45;
    uint8_t reg46;
    struct {
        uint8_t  fcal     : 2;
        uint8_t  reserved : 6;
    } reg47;
    // PLL Configuration
#define PLL_KPHI_BIT_LEN 7
    struct {
        uint8_t pllKphi   : PLL_KPHI_BIT_LEN;
        uint8_t reserved  : 1;
    } reg48;
#define VCO_GAIN_BIT_LEN 3
#define RSEL_BIT_LEN 2
#define BWSEL_BIT_LEN 2
    struct {
        uint8_t bwSel      : BWSEL_BIT_LEN;
        uint8_t rSel       : RSEL_BIT_LEN;
        uint8_t vcoGain    : VCO_GAIN_BIT_LEN;
        uint8_t fcalOvrdEn : 1;
    } reg49;
#define MSCAL_BIT_LEN 6
    struct {
        uint8_t msCal     : MSCAL_BIT_LEN;
        uint8_t reserved  : 2;
    } reg50;
#define MSPEC_BIT_LEN 3
    struct {
        uint8_t msPec     : MSPEC_BIT_LEN;
        uint8_t reserved  : 5;
    } reg51;
    // -
    uint8_t reg52;
    // MultiSynth Registers
#define DIV_MS_P1_BIT_LEN 18
#define DIV_MS_P2_BIT_LEN 30
#define DIV_MS_P3_BIT_LEN 30

#define MSX_OFFSET_IN_BYTES 11
#define MSX_DATA_LENGTH_IN_BYTES 11
    uint8_t reg53To107[55];

    // -
    uint8_t reg108To217[110];

    // notice: def. config is used here:
    // Freq. Inc/Dec
    // Init. phase offset & phase step size
    // Spread spectrum config.

    // status bits for LOS_CLKIN,LOS_FB, LOL, SYS_CAL
    struct {
        uint8_t sysCalInProc : 1;
        uint8_t reserved1    : 1;
        uint8_t losClkin     : 1;
        uint8_t losFdbk      : 1;
        uint8_t pllLosOfLock : 1;
        uint8_t reserved     : 3;
    } reg218;
    // -
    uint8_t reg219To229[11];
    // Output driver enable
    struct {
        uint8_t  oeb0      : 1;
        uint8_t  oeb1      : 1;
        uint8_t  oeb2      : 1;
        uint8_t  oeb3      : 1;
        uint8_t  oebAll    : 1;
        uint8_t  reserved  : 3;
    } reg230;
    // -
    uint8_t reg231To234[4];
    // fcal (read only)
    uint8_t reg235;
    uint8_t reg236;
    struct {
        uint8_t  fcal     : 2;
        uint8_t  reserved : 6;
    } reg237;
    // -
    uint8_t reg238To340[3];
    // prevent LOL status from asserting
    struct {
        uint8_t  reserved  : 7;
        uint8_t  disLol    : 1;
    } reg241;
    uint8_t reg242To345[4];
    // soft reset
    struct {
        uint8_t  reserved1 : 1;
        uint8_t  softReset : 1;
        uint8_t  reserved  : 6;
    } reg246;
    // -
    uint8_t reg247To254[8];
    // page select
#define PAGE_SEL_ADDR 255
    struct {
        uint8_t  pageSel : 1;
        uint8_t  reserved : 7;
    } reg255;
    // -
    uint8_t reg256To349[94];
    // last reg.
    uint8_t reg350;
} tSi5338RegMap;

typedef struct _tSi5338RegArea {
    union{
        uint8_t data[SI5338_REG_AREA_SIZE_IN_BYTES];
        tSi5338RegMap reg;
    };
} tSi5338RegArea;

// MultiSynth
typedef enum _tSi5338MsX_e {
    eSi5338Ms0=0,
    eSi5338Ms1,
    eSi5338Ms2,
    eSi5338Ms3,
    eSi5338MsN,
    eSi5338MsNum
} tSi5338MsX_e;

typedef enum _tSi5338MsPx_e {
    eSi5338MsP1=0,
    eSi5338MsP2,
    eSi5338MsP3
} tSi5338MsPx_e;

// Output drivers config
typedef enum _tSi5338Output_e {
    eSi5338Output0=0,
    eSi5338Output1,
    eSi5338Output2,
    eSi5338Output3,
    eSi5338OutputNum
} tSi5338Output_e;

// Reference clock
typedef enum _tSi5338ClkIn_e {
    eSi5338ClkIn1=0,
    eSi5338ClkIn2
} tSi5338ClkIn_e;

// Clock status
typedef enum _tSi5338ClkStatus_e {
    eSi5338PllLossOfLock=0,
    eSi5338LossOfSigIn2,
    eSi5338LossOfSigIn1,
    eSi5338CalInProg,
    eSi5338AllAlarms
} tSi5338ClkStatus_e;

// mem. page select
typedef enum _tSi5338PageSel_e {
    eSi5338Page0=0,
    eSi5338Page1,
    eSi5338UnknownState
} tSi5338PageSel_e;

// clock status
typedef enum _tSi5338ClkPresent_e {
    eSi5338NoClkPresent=0,
    eSi5338ClkPresent
} tSi5338ClkPresent_e;


// pll status
typedef enum _tSi5338PllLock_e {
    eSi5338PllLocked=0,
    eSi5338PllNotLocked
} tSi5338PllLock_e;

// device status
typedef enum _tSi5338DeviceStatus_e {
    eSi5338Stop=0,
    eSi5338Start
} tSi5338DeviceStatus_e;


static_assert(sizeof(tSi5338RegMap) == SI5338_REG_AREA_SIZE_IN_BYTES, "Si5338 register map size mismatch");


/*-----------------------------------------------------------------------------*/


    class Si5338 : public I2cDevice
    {
    public:
        Si5338(BusController  &a_controller,
                std::string   a_name,
                BusDeviceId   a_id,
                uint8_t       a_device_address,
                uint8_t       a_bus_address);
        virtual ~Si5338();

        static const uint8_t c_i2cAddr;

    protected:

    private:
        tSi5338RegArea m_regArea;
        tSi5338RegMap * regMapPtr;
        tSi5338PageSel_e m_pageSel;
        tSi5338DeviceStatus_e m_devState;

    private:
        // basic i2c read & write functions
        void Si5338Write(uint16_t a_regAddr, uint8_t * a_data);
        void Si5338Read(uint16_t a_regAddr, uint8_t * a_data);
        void Si5338ReadModifiedWrite(uint16_t a_regAddr,
                                     uint8_t a_writeData,
                                     uint8_t a_dataMask);
        // device mem. space access functions
        void Si5338PageSelect(uint16_t a_regAddr);
        void Si5338GetReg(uint16_t a_regAddr);
        void Si5338SetReg(uint16_t a_regAddr);
        void Si5338GetRegInRange(uint16_t a_firstRegAddr, uint16_t a_lastRegAddr);
        void Si5338SetRegInRange(uint16_t a_firstRegAddr, uint16_t a_lastRegAddr);

        // init. func.
        void Si5338RefClkInit();
        void Si5338OutputDriverInit();

        // output driver ctrl.
        void Si5338OutputEnableAll();
        void Si5338OutputDisableAll();

        // multisynth
        void Si5338MultiSynthSet(tSi5338MsX_e a_msX, tSi5338MsPx_e a_p, uint32_t a_val);
        uint32_t Si5338MultiSynthGet(tSi5338MsX_e a_msX, tSi5338MsPx_e a_p);

        // output divider
        void SetRxDivider(tSi5338Output_e a_sel, uint8_t a_val);
        uint8_t GetRxDivider(tSi5338Output_e a_sel);

        // update procedure func.
        void Si5338LolPreventFromAsserting(bool a_val);
        void Si5338OverrideFreqCalForVco(bool a_val);
        void Si5338SoftReset();
        void Si5338CopyFcalValuesToActiveRegs();

        // status func.
        bool Si5338ClockStatusGet(tSi5338ClkStatus_e a_clkStatus);

    public:
        // main
        void Si5338StopDevice();
        void Si5338StartDevice();
        void Si5338DevStateSet(tSi5338DeviceStatus_e a_state);

        /* input clock configuration nodes */
        bool _SetInputClkSel(const tSi5338ClkIn_e& a_val);
        bool _GetInputClkSel(tSi5338ClkIn_e& a_val);
        bool _SetInputDividerP1(const uint32_t& a_val);
        bool _GetInputDividerP1(uint32_t& a_val);
        bool _SetInputDividerP2(const uint32_t& a_val);
        bool _GetInputDividerP2(uint32_t& a_val);

        /* pll configuration nodes */
        bool _SetPllKphi(const uint32_t& a_val);
        bool _GetPllKphi(uint32_t& a_val);
        bool _SetVcoGain(const uint32_t& a_val);
        bool _GetVcoGain(uint32_t& a_val);
        bool _SetRSel(const uint32_t& a_val);
        bool _GetRSel(uint32_t& a_val);
        bool _SetBwSel(const uint32_t& a_val);
        bool _GetBwSel(uint32_t& a_val);
        bool _SetMCal(const uint32_t& a_val);
        bool _GetMCal(uint32_t& a_val);
        bool _SetMsPec(const uint32_t& a_val);
        bool _GetMsPec(uint32_t& a_val);

        // multisynth
        bool SetMultiSynthMsPX(const uint32_t* a_values, const size_t a_pos, const size_t a_size,
                                        tSi5338MsPx_e a_p);
        size_t GetMultiSynthMsPX(uint32_t* a_values, const size_t a_pos, const size_t a_size,
                                        tSi5338MsPx_e a_p);
        bool _SetMultiSynthMsP1(const uint32_t* a_values, const size_t a_pos, const size_t a_size);
        size_t _GetMultiSynthMsP1(uint32_t* a_values, const size_t a_pos, const size_t a_size);
        bool _SetMultiSynthMsP2(const uint32_t* a_values, const size_t a_pos, const size_t a_size);
        size_t _GetMultiSynthMsP2(uint32_t* a_values, const size_t a_pos, const size_t a_size);
        bool _SetMultiSynthMsP3(const uint32_t* a_values, const size_t a_pos, const size_t a_size);
        size_t _GetMultiSynthMsP3(uint32_t* a_values, const size_t a_pos, const size_t a_size);

        /* output dividers */
        bool  _SetRxDivider(const uint32_t* a_values, const size_t a_pos, const size_t a_size);
        size_t _GetRxDivider(uint32_t* a_values, const size_t a_pos, const size_t a_size);

        /* status nodes */
        bool _GetDeviceState(tSi5338DeviceStatus_e& a_val);
        bool _GetIn1ClkStatus(tSi5338ClkPresent_e& a_val);
        bool _GetIn2ClkStatus(tSi5338ClkPresent_e& a_val);
        bool _GetPllStatus(tSi5338PllLock_e& a_val);
    };

}

#endif // GDX_SI5338_H

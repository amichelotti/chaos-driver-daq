// $Id: ebpp.h 2454 2009-03-04 14:47:44Z tomaz.beltram $

//! \file ebpp.h
//! Electron Beam Position Processor specific definitions.

#if !defined(_EBPP_H)
#define _EBPP_H

//defines for interlock status register checks
#define INTERLOCK_XPOSITION(_status) (_status & 0x01)
#define INTERLOCK_YPOSITION(_status) (_status & 0x02)
#define INTERLOCK_ATT_LIMIT(_status) (_status & 0x04)
#define INTERLOCK_ADC_OVERFLOW(_status) (_status & 0x08)
#define INTERLOCK_ADC_NOT_FILTERED(_status) (_status & 0x10)

// Get defines for interfacing with LPLLD.
#include "lplld.h"

/** Libera EBPP Slow Acquisition (SA) sample. */
typedef struct {
	// Amplitudes
	int Va, Vb, Vc, Vd;
	// Sum Va + Vb + Vc + Vd
	int Sum;
	// Quadropole signal
	int Q;
	// Horizontal beam position
	int X;
	// Vertical beam position
	int Y;
	// Horiz. and vert. correction factors from the FA Application
	int Cx, Cy;
	// 6 values reserved for future use
	int reserved[6];
}
CSPI_SA_ATOM;

/** Libera EBPP Data on Demand (DD) raw sample. */
typedef struct {
	int cosVa, sinVa;
	int cosVb, sinVb;
	int cosVc, sinVc;
	int cosVd, sinVd;
}
CSPI_DD_RAWATOM;

/** Libera EBPP Data on Demand (DD) sample. */
typedef struct {
	// Amplitudes
	int Va, Vb, Vc, Vd;
	// Horiz. and vert. beam position
	int X, Y;
	// Quadropole signal
	int Q;
	// Sum Va + Vb + Vc + Vd
	int Sum;
}
CSPI_DD_ATOM;

/** Libera EBPP ADC data sample. */
typedef struct {
	short chD;
	short chC;
	short chB;
	short chA;
}
CSPI_ADC_ATOM;

/** Libera EBPP ADC CW data sample. */
typedef struct {
	short chD;
	short chC;
	short chB;
	short chA;
	int X, Y;
	int Sum;
	int Qa, Qb, Qc, Qd;
}
CSPI_ADC_CW_ATOM;

/** Libera EBPP ADC SP data sample. */
typedef struct {
	size_t trigger;
	int threshold;
	size_t n_before;
	size_t n_after;
	int X, Y, Sum;
}
CSPI_ADC_SP_ATOM;

/** Libera EBPP average sum data sample. */
typedef struct {
	unsigned long avesum;
}
CSPI_AVERAGE_ATOM;

//--------------------------------------------------------------------------

/** DSC Compensation parameters. */
struct DSC_COMPPARAMS
{
	float ampl[16][4];
	float phase[16][4];
	int status;
};

/** Environment parameters or attributes. */
#pragma pack(4)
struct tagCSPI_ENVPARAMS
{
	CSPI_ENVPARAMS_BASE;

	int Kx, Ky;
	int Xoffset, Yoffset, Qoffset;

	int switches;	// Analog board switch mode. See CSPI_SWITCHMODE.
	int gain;		// Analog board gain (dBm).

	int agc;		// AGC mode. See CSPI_AGCMODE.
	int dsc;		// DSC mode. See CSPI_DSCMODE.

	/** Interlock parameters. */
	struct {
		/// Interlock mode. See CSPI_INTERLOCKMODE.
		int mode;
		/// Interlock limits.
		int Xlow, Xhigh, Ylow, Yhigh;
		/// Interlock overflow limit (ADC count).
		int overflow_limit;
		/// Interlock overflow duration (ADC clock periods).
		int overflow_dur;
		/// Gain limit (dBm) for gain-dependant interlock.
		int gain_limit;
	} ilk;

	/// Interlock status. Write this value will reset status.
	int ilk_status;
	/// Post mortem buffer offset (+/-).
	int PMoffset;
	/// Post mortem decimation (1 or 64).
	int PMdec;

	int trig_delay;        // Hardware Trigger Delay

	int external_switching; // Switching source.
	int switching_delay;   // Switching delay.

	int ddc_maflength;     // Moving Average Filter Length
	int ddc_mafdelay;      // Moving Average Filter Delay

	int notch1[LIBERA_CFG_NOTCH_MAX];
	int notch2[LIBERA_CFG_NOTCH_MAX];
	int polyphase_fir[LIBERA_CFG_FIR_MAX];

	/** LPLLD runtime parameters. Write only. */
	int mtvcxoffs; // RF-VCXO detuning offset (*40 Hz). Environment configuration : OffsetTune
	int mtncoshft; // NCO frequency shift flag. Environment configuration : CompTune
	int mtphsoffs; // VCXO phase offset. Environment configuration : PhaseOffs
	int mtunlcktr; // MT unlock threshold.  Environment configuration : MTUnlock
	int mtsyncin; // Synchronization input. Environment configuration : SyncIn
	int stunlcktr; // ST unlock threshold.  Environment configuration : STUnlock

	/** LPLLD status. Read only. */
	pll_status_t pll_status; // PLL MT (mt_stat) and ST (st_stat) status structures.

	/** Second max value of ADC data rate - read only */
	int max_adc;

	/** Average of the sum values between 2 triggers - read only */
	int average_sum;

	/** Post-mortem triggering */
	struct {
		int mode;
		int min_x_limit;
		int max_x_limit;
		int min_y_limit;
		int max_y_limit;
		int overflow_limit;
		int overflow_dur;
	} pm;

	/** Spike removal */
	struct {
		int enable;           // Enabled in FPGA
		int cspi_enable;      // Enabled in CSPI
		int averaging_stop;
		int average_window;
		int start;
		int window;
	} sr;

	/** Single pass */
	struct {
		int threshold;
		int n_before;
		int n_after;
	} sp;

	double frev;	// frev
};
#pragma pack()

/** Libera EBPP specific environment bitflags. */
#define CSPI_ENV_KX                CUSTOM_ENV_BIT(0)
#define CSPI_ENV_KY                CUSTOM_ENV_BIT(1)
#define CSPI_ENV_XOFFSET           CUSTOM_ENV_BIT(2)
#define CSPI_ENV_YOFFSET           CUSTOM_ENV_BIT(3)
#define CSPI_ENV_QOFFSET           CUSTOM_ENV_BIT(4)
#define CSPI_ENV_SWITCH            CUSTOM_ENV_BIT(5)
#define CSPI_ENV_GAIN              CUSTOM_ENV_BIT(6)
#define CSPI_ENV_AGC               CUSTOM_ENV_BIT(7)
#define CSPI_ENV_DSC               CUSTOM_ENV_BIT(8)
#define CSPI_ENV_ILK               CUSTOM_ENV_BIT(9)
#define CSPI_ENV_ILKSTATUS         CUSTOM_ENV_BIT(10)
#define CSPI_ENV_PMOFFSET          CUSTOM_ENV_BIT(11)
#define CSPI_ENV_TRIGDELAY         CUSTOM_ENV_BIT(12)
#define CSPI_ENV_EXTSWITCH         CUSTOM_ENV_BIT(13)
#define CSPI_ENV_SWDELAY           CUSTOM_ENV_BIT(14)
#define CSPI_ENV_NOTCH1            CUSTOM_ENV_BIT(15)
#define CSPI_ENV_NOTCH2            CUSTOM_ENV_BIT(16)
#define CSPI_ENV_POLYPHASE_FIR     CUSTOM_ENV_BIT(17)
#define CSPI_ENV_DDC_MAFLENGTH     CUSTOM_ENV_BIT(18)
#define CSPI_ENV_DDC_MAFDELAY      CUSTOM_ENV_BIT(19)
#define CSPI_ENV_MTVCXOFFS         CUSTOM_ENV_BIT(20)
#define CSPI_ENV_MTNCOSHFT         CUSTOM_ENV_BIT(21)
#define CSPI_ENV_MTPHSOFFS         CUSTOM_ENV_BIT(22)
#define CSPI_ENV_MTUNLCKTR         CUSTOM_ENV_BIT(23)
#define CSPI_ENV_MTSYNCIN          CUSTOM_ENV_BIT(24)
#define CSPI_ENV_STUNLCKTR         CUSTOM_ENV_BIT(25)
#define CSPI_ENV_LPLLDSTAT         CUSTOM_ENV_BIT(26)
#define CSPI_ENV_MAX_ADC           CUSTOM_ENV_BIT(27)
#define CSPI_ENV_AVERAGE_SUM       CUSTOM_ENV_BIT(28)
#define CSPI_ENV_PM                CUSTOM_ENV_BIT(29)
#define CSPI_ENV_SR                CUSTOM_ENV_BIT(30)
#define CSPI_ENV_SP                CUSTOM_ENV_BIT(31)
#define CSPI_ENV_FREV              CUSTOM_ENV_BIT(32)
#define CSPI_ENV_PMDEC             CUSTOM_ENV_BIT(33)

/** Available switch modes. */
typedef enum {
	/** Enable switching. */
	CSPI_SWITCH_AUTO	= 0xff,

	/** Enable direct connection (no crossover).
		Final value for DSC depends on HW type.
		Brilliance = 15, Electron = 3. */
	CSPI_SWITCH_DIRECT	= 0x100,

	/** DIRECT switch position value for Brillinace */
	CSPI_SWITCH_DIRECT_BRILLIANCE = 0x0F,

	/** DIRECT switch position value for NOT Brillinace, e.g. Electron */
	CSPI_SWITCH_DIRECT_NOTBRILLIANCE = 0x03,

	/** Minimal switch position value  */
	CSPI_SWITCH_MIN         = 0x00,

	/** Maximal switch position value  */
	CSPI_SWITCH_MAX         = 0x0f,
}
CSPI_SWITCHMODE;

/** Available AGC modes. */
typedef enum {
	/** Manual gain control. */
	CSPI_AGC_MANUAL = 0,
	/** Enable AGC. */
	CSPI_AGC_AUTO,
}
CSPI_AGCMODE;

/** Available DSC modes. */
typedef enum {
	/** Disable DSC. Keep current DSC coefficients. */
	CSPI_DSC_OFF = 0,
	/** Disable DSC. Apply unitiy DSC coefficients. */
	CSPI_DSC_UNITY,
	/** Enable signal conditioning with DSC daemon in AUTO mode. */	
	CSPI_DSC_AUTO,
	/** Save current DSC coefficients onto FLASH /opt/dsc/lastgood.dat. */
	CSPI_DSC_SAVE_LASTGOOD,
	/** Reset currently learned coefficients to unity value. */
	CSPI_DSC_RESET_COEFF,
}
CSPI_DSCMODE;

/** Available interlock modes. */
typedef enum {
	/** Disable interlock. */
	CSPI_ILK_DISABLE = 0,
	/** Enable interlock. */
	CSPI_ILK_ENABLE = 1,
	/** Enable gain-dependant interlock. */
	CSPI_ILK_ENABLE_GAINDEP = 3,
}
CSPI_ILKMODE;


/** Event specific values for event CSPI_EVENT_INTERLOCK. */
typedef enum {
	/** IL: position X out of limit. */
	CSPI_INTERLOCK_X    = LIBERA_INTERLOCK_X,

	/** IL: position Y out of limit. */
	CSPI_INTERLOCK_Y    = LIBERA_INTERLOCK_Y,

	/** IL: Attenuators set higher than predefined value. */
	CSPI_INTERLOCK_ATTN = LIBERA_INTERLOCK_ATTN,

	/** IL: ADC Overflow  (filtered). */
	CSPI_INTERLOCK_ADCF = LIBERA_INTERLOCK_ADCF,

	/** IL: ADC Overflow  (not filtered). */
	CSPI_INTERLOCK_ADC = LIBERA_INTERLOCK_ADC,
} CSPI_ILKCAUSE;

//--------------------------------------------------------------------------

/** Derived from CSPI_CONPARAMS to handle EBPP specific
 *  parameters or attributes.
 */

#pragma pack(4) // because of uint64_t event_mask in CSPI_CONPARAMS_BASE
typedef struct {
	/** Common connection parameters. */
	CSPI_CONPARAMS_BASE;

	/** DD decimation factor. */
	size_t dec;

	/** SA non-blocking mode. */
	size_t nonblock;
}
CSPI_CONPARAMS_EBPP;
#pragma pack()

//--------------------------------------------------------------------------

/** Bit flags corresponding to the CSPI_CONPARAMS_EBPP structure.
 *  See CSPI_CONPARAMS_EBPP structure for descriptions.
 */
typedef enum {
	CSPI_CON_DEC        = CUSTOM_CON_BIT(0),
	CSPI_CON_SANONBLOCK = CUSTOM_CON_BIT(1),
}
CSPI_CONFLAGS_EBPP;

/** Backward compatibility */
typedef CSPI_CONFLAGS_EBPP  CSPI_CONFLAGS_DD;
/** Backward compatibility */
typedef CSPI_CONPARAMS_EBPP  CSPI_CONPARAMS_DD;

//--------------------------------------------------------------------------

/** Communication Controller parameters available via cspi_get/setparam_fa()
 */

#define CSPI_ENVPARAMS_DCC_OFFSET 0xc00 // offset into FAI block
#define CSPI_ENVPARAMS_DCC_LINKS 4

// register offsets in the FAI block
#define CSPI_ENVPARAMS_DCC_CONTROL     0x2000
#define CSPI_ENVPARAMS_DCC_EXT_TRIGGER 0x2004
#define CSPI_ENVPARAMS_DCC_INTERFACE   0x2008

typedef struct {
	unsigned int firmware;
	unsigned int status;
	unsigned int link_partner[CSPI_ENVPARAMS_DCC_LINKS];
	unsigned int link_up;
	unsigned int timeframe;
	unsigned int hard_error [CSPI_ENVPARAMS_DCC_LINKS];
	unsigned int soft_error [CSPI_ENVPARAMS_DCC_LINKS];
	unsigned int frame_error[CSPI_ENVPARAMS_DCC_LINKS];
	unsigned int received   [CSPI_ENVPARAMS_DCC_LINKS];
	unsigned int transmitted[CSPI_ENVPARAMS_DCC_LINKS];
	unsigned int fod_process_time;
	unsigned int bpm_count;
}
CSPI_ENVPARAMS_DCC;

//--------------------------------------------------------------------------

#endif	// _EBPP_H

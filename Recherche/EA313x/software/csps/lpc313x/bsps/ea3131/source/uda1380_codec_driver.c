/***********************************************************************
 * $Id:: uda1380_codec_driver.c 2087 2009-07-27 16:35:19Z wellsk       $
 *
 * Project: UDA1380 CODEC read/write driver
 *
 * Description:
 *     Driver to read and write CODEC registers
 *
 ***********************************************************************
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * products. This software is supplied "AS IS" without any warranties.
 * NXP Semiconductors assumes no responsibility or liability for the
 * use of the software, conveys no license or title under any patent,
 * copyright, or mask work right to the product. NXP Semiconductors
 * reserves the right to make changes in the software without
 * notification. NXP Semiconductors also make no representation or
 * warranty that such application will be suitable for the specified
 * use without further testing or modification.
 **********************************************************************/

#include "lpc_types.h"
#include "lpc313x_chip.h"
#include "lpc313x_mstr_i2c_driver.h"
#include "uda1380_codec_driver.h"
#include "uda1380.h"

/***********************************************************************
 * Driver data
 **********************************************************************/

#define USEDRIVER

typedef struct {
	UNS_16 cached_regs[0x29];
	INT_32 i2cdev;
	UNS_8 i2caddr;
} UDA1380_DAT_T;
static UDA1380_DAT_T drvdata;

static const UNS_16 pbsetup[] = {
		0x2, 0x8100,
		0x0, 0x312,
		0x1, 0x0,
		0x2, 0x8100,
		0x0, 0x312,
		0x1, 0x0,
		0x2, 0x8100,
		0x0, 0x312,
		0x0, 0x302,
		0x10, 0x0,
		0x11, 0xff00,
		0x12, 0x0,
		0x13, 0x4800,
		0x14, 0x0,
		0x0, 0x312,
		0x2, 0x8100,
		0x2, 0x8500,
		0x2, 0xa500,
		0x2, 0xa500,
		0x0, 0x302,
		0x13, 0x800,
		0x0, 0x312,
		0xFF
	};

static const UNS_16 pbdown[] = {
		0x0, 0x302,
		0x0, 0x312,
		0x2, 0x2500,
		0x0, 0x12,
		0x2, 0x2500,
		0x2, 0x500,
		0x2, 0x100,
		0x2, 0x100,
		0xff
	};

/***********************************************************************
 * Location functions
 **********************************************************************/

#ifdef USEDRIVER
static INT_32 i2chandle;

#else
#define pI2C1Regs                   ((I2C_REGS_T*) I2C1_BASE)
#define I2C_CLK_DIV_12MHZ           (60)

void i2c1_reg_init(void)
{
  pI2C1Regs->ckh = I2C_CLK_DIV_12MHZ;
  pI2C1Regs->ckl = I2C_CLK_DIV_12MHZ;

  pI2C1Regs->ctl = I2C_CTL_SOFT_RESET;  //Issue soft reset of the block
}

void i2c_clear_fifo(void) {
  UNS_32 status = 0;

  while ((status & (1 << 9)) == 0)
  {
    status = pI2C1Regs->fifo;
    status = pI2C1Regs->sts;
  }
}

UNS_16 i2c1_reg_read(UNS_8 reg_addr)
{
	UNS_16 val16 = 0;
  UNS_32 status = 0;

    i2c_clear_fifo();

  pI2C1Regs->fifo = (UNS_32) drvdata.i2caddr | I2C_TXFF_START_CND | 0;
  pI2C1Regs->fifo = (UNS_32) reg_addr;
  pI2C1Regs->fifo = (UNS_32) drvdata.i2caddr | I2C_TXFF_START_CND | 1;
  pI2C1Regs->fifo = 0xFF;
  pI2C1Regs->fifo = 0xFF | I2C_TXFF_STOP_CND;

  while ((status & (I2C_STS_NAI | I2C_STS_TDI)) == 0)
  {
    status = pI2C1Regs->sts;
  }

  if (status & I2C_STS_TDI)
  {
    // Clear the TDI status
    pI2C1Regs->sts = I2C_STS_TDI;

    // Toss 3 dummy bytes
	val16 = (UNS_16) pI2C1Regs->fifo;
	val16 = (UNS_16) pI2C1Regs->fifo;
	val16 = (UNS_16) pI2C1Regs->fifo;

    // Read out the register value
	val16 = (UNS_16) pI2C1Regs->fifo;
	val16 = (val16 << 8) | (UNS_16) pI2C1Regs->fifo;
  }

	return val16;
}

void i2c_reg_write(UNS_8 reg_addr, UNS_16 reg_value)
{
  UNS_32   status = 0;

  i2c_clear_fifo();

  pI2C1Regs->fifo = (UNS_32) drvdata.i2caddr | I2C_TXFF_START_CND | 0;
  pI2C1Regs->fifo = (UNS_32) reg_addr;
  pI2C1Regs->fifo = (UNS_32) ((reg_value >> 8) & 0xFF);
  pI2C1Regs->fifo = (UNS_32) (reg_value & 0xFF) | I2C_TXFF_STOP_CND;

  while ((status & (I2C_STS_NAI | I2C_STS_TDI)) == 0)
  {
    status = pI2C1Regs->sts;
  }

  if (status & I2C_STS_TDI)
  {
    // Clear the TDI status
    pI2C1Regs->sts = I2C_STS_TDI;
  }
}
#endif

/***********************************************************************
 * Global functions
 **********************************************************************/

/***********************************************************************
 *
 * Function: uda1380_driver_init
 *
 * Purpose: Initializes driver and data
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     i2cdev  : Handle to an opened I2C device
 *     i2caddr : I2C address of the UDA1380 CODEC
 *
 * Outputs: None
 *
 * Returns: 0 if the UDA1380 was detected, otherwise <0
 *
 * Notes: None
 *
 **********************************************************************/
INT_32 uda1380_driver_init(INT_32 i2cdev, UNS_8 i2caddr) {
	drvdata.i2cdev = i2cdev;
	drvdata.i2caddr = i2caddr << 1;

#ifdef USEDRIVER
	i2chandle = i2c_mstr_open((void *) I2C1, 0);

#else
	i2c1_reg_init();
#endif

	/* Attempt to reset UDA1380 CODEC */
	uda1380_reset();

	return _NO_ERROR;
}

/***********************************************************************
 *
 * Function: uda1380_shutdown
 *
 * Purpose: Shutdown UDA1380
 *
 * Processing:
 *     See function.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void uda1380_shutdown(void) {
	uda1380_enable_audio_out(0);
	uda1380_enable_audio_in(0);
}

/***********************************************************************
 *
 * Function: uda1380_reset
 *
 * Purpose: Reset the CODEC
 *
 * Processing:
 *     See function.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void uda1380_reset(void) {
#ifdef USEDRIVER
	I2C_MTXRX_SETUP_T xfer;
	UNS_32 txdata[8], rxdata[8];
	UNS_32 status;

	txdata[0] = drvdata.i2caddr | I2C_TXFF_START_CND | 0;
	txdata[1] = UDA1380_REG_L3;
	txdata[2] = 0;
	txdata[3] = 0 | I2C_TXFF_STOP_CND;
	xfer.tx_data = txdata;
	xfer.rx_data = rxdata;
	xfer.tx_length = 4;
	xfer.clock_rate = 100000;
	xfer.cb = NULL;

	if (i2c_mstr_ioctl(i2chandle, I2C_MSTR_TRANSFER, (INT_32) &xfer) ==
		_NO_ERROR) {
			status = i2c_mstr_ioctl(i2chandle, I2C_MSTR_GET_STATUS, 0);
			while ((status & (I2C_MSTR_STATUS_ARBF |
				I2C_MSTR_STATUS_NOACKF | I2C_MSTR_STATUS_DONE)) == 0) {
					status = i2c_mstr_ioctl(i2chandle,
						I2C_MSTR_GET_STATUS, 0);
			}
	}

#else
	i2c_reg_write(UDA1380_REG_L3, 0);
#endif
	drvdata.cached_regs[UDA1380_REG_EVALCLK] = 0x0502;
	drvdata.cached_regs[UDA1380_REG_I2S] = 0x0000;
	drvdata.cached_regs[UDA1380_REG_PWRCTRL] = 0x0000;
	drvdata.cached_regs[UDA1380_REG_ANAMIX] = 0x3f3f;
	drvdata.cached_regs[UDA1380_REG_HEADAMP] = 0x0202;
	drvdata.cached_regs[UDA1380_REG_MSTRVOL] = 0x0000;
	drvdata.cached_regs[UDA1380_REG_MIXVOL] = 0xff00;
	drvdata.cached_regs[UDA1380_REG_MODEBBT] = 0x0000;
	drvdata.cached_regs[UDA1380_REG_MSTRMUTE] = 0x4800;
	drvdata.cached_regs[UDA1380_REG_MIXSDO] = 0x0000;
	drvdata.cached_regs[UDA1380_REG_DECVOL] = 0x0000;
	drvdata.cached_regs[UDA1380_REG_PGA] = 0x8000;
	drvdata.cached_regs[UDA1380_REG_ADC] = 0x0002;
	drvdata.cached_regs[UDA1380_REG_AGC] = 0x0000;
}

/***********************************************************************
 *
 * Function: uda1380_write
 *
 * Purpose: Write data to a UDA1380 CODEC register
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     reg  : register to write
 *     data : data to write
 *
 * Outputs: None
 *
 * Returns: Value written to register
 *
 * Notes: None
 *
 **********************************************************************/
UNS_16 uda1380_write(UNS_8 reg, UNS_16 data) {
#ifdef USEDRIVER
	UNS_32 rxdata[8], txdata[8];
	I2C_MTXRX_SETUP_T xfer;
	UNS_32 status;

	if (reg == UDA1380_REG_L3) {
		uda1380_reset();
	}
	else if (data != drvdata.cached_regs[reg]) {
		drvdata.cached_regs[reg] = data;

		txdata[0] = drvdata.i2caddr | I2C_TXFF_START_CND | 0;
		txdata[1] = reg;
		txdata[2] = ((UNS_32) (data & 0xFF00) >> 8);
		txdata[3] = I2C_TXFF_STOP_CND | ((UNS_32) data & 0x00FF);
		xfer.tx_data = txdata;
		xfer.rx_data = rxdata;
		xfer.tx_length = 4;
		xfer.clock_rate = 100000;
		xfer.cb = NULL;

		if (i2c_mstr_ioctl(i2chandle, I2C_MSTR_TRANSFER, (INT_32) &xfer) ==
			_NO_ERROR) {
				status = i2c_mstr_ioctl(i2chandle, I2C_MSTR_GET_STATUS, 0);
				while ((status & (I2C_MSTR_STATUS_ARBF |
					I2C_MSTR_STATUS_NOACKF | I2C_MSTR_STATUS_DONE)) == 0) {
						status = i2c_mstr_ioctl(i2chandle,
							I2C_MSTR_GET_STATUS, 0);
			}
		}
	}

#else
	i2c_reg_write(reg, data);
#endif

	return data;
}

/***********************************************************************
 *
 * Function: uda1380_set
 *
 * Purpose: Set only selected bits in a UDA1380 CODEC register
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     reg  : register to write
 *     setmask : Bits to enable
 *
 * Outputs: None
 *
 * Returns: Value written to register
 *
 * Notes: None
 *
 **********************************************************************/
UNS_16 uda1380_set(UNS_8 reg, UNS_16 setmask) {
	return uda1380_write(reg, (drvdata.cached_regs[reg] | setmask));
}

/***********************************************************************
 *
 * Function: uda1380_clear
 *
 * Purpose: Clear only selected bits in a UDA1380 CODEC register
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     reg  : register to write
 *     clearmask : Bits to disable
 *
 * Outputs: None
 *
 * Returns: Value written to register
 *
 * Notes: None
 *
 **********************************************************************/
UNS_16 uda1380_clear(UNS_8 reg, UNS_16 clearmask) {
	return uda1380_write(reg, (drvdata.cached_regs[reg] & ~clearmask));
}

/***********************************************************************
 *
 * Function: uda1380_read
 *
 * Purpose: Read a CODEC register
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     reg  : register to read
 *
 * Outputs: None
 *
 * Returns: Value read from register
 *
 * Notes: None
 *
 **********************************************************************/
UNS_16 uda1380_read(UNS_8 reg) {
	UNS_16 data;
#ifdef USEDRIVER
	UNS_32  rxdata[8], txdata[8];
	I2C_MTXRX_SETUP_T xfer;
	UNS_32 status;

	if ((reg == UDA1380_REG_HEADPHONE) || (reg == UDA1380_REG_DEC)) {
		data = drvdata.cached_regs[reg];
	}
	else if (data != drvdata.cached_regs[reg]) {
		drvdata.cached_regs[reg] = data;

		txdata[0] = drvdata.i2caddr | I2C_TXFF_START_CND | 0;
		txdata[1] = reg;
		txdata[2] = I2C_TXFF_START_CND | ((UNS_32) data & 0x00FF);
		txdata[3] = 0xff;
		txdata[4] = 0xff | I2C_TXFF_STOP_CND;
		xfer.tx_data = txdata;
		xfer.rx_data = rxdata;
		xfer.tx_length = 5;
		xfer.clock_rate = 100000;
		xfer.cb = NULL;

		if (i2c_mstr_ioctl(i2chandle, I2C_MSTR_TRANSFER, (INT_32) &xfer) ==
			_NO_ERROR) {
				status = i2c_mstr_ioctl(i2chandle, I2C_MSTR_GET_STATUS, 0);
				while ((status & (I2C_MSTR_STATUS_ARBF |
					I2C_MSTR_STATUS_NOACKF | I2C_MSTR_STATUS_DONE)) == 0) {
						status = i2c_mstr_ioctl(i2chandle,
							I2C_MSTR_GET_STATUS, 0);
			}
		}

		data = (UNS_16) rxdata[1] | (((UNS_16) rxdata[0]) << 8);
	}

#else
	if ((reg == UDA1380_REG_HEADPHONE) || (reg == UDA1380_REG_DEC)) {
		data = drvdata.cached_regs[reg];
	}
	else {
		data = i2c1_reg_read(reg);
	}
#endif

	return data;
}

/***********************************************************************
 *
 * Function: uda1380_mute
 *
 * Purpose: Mute or unmute audio output
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     mute : !0 to muye, 0 to unmute
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void uda1380_mute(INT_32 mute) {
	if (mute) {
		uda1380_set(UDA1380_REG_MSTRMUTE, (1 << 14) | (1 << 11) | (1 << 3));
	}
	else {
		uda1380_clear(UDA1380_REG_MSTRMUTE, (1 << 14) | (1 << 11) | (1 << 3));
	}
}

/***********************************************************************
 *
 * Function: uda1380_set_boost
 *
 * Purpose: Set bass and treble boot levels
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     bass   : Bass boost value, 0 (off) to 15 (+24db)
 *     treble : Treble boost value, 0 (off) to 3 (+6db)
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void uda1380_set_boost(UNS_8 bass, UNS_8 treble) {
	bass = bass & 0x7;
	treble = treble & 0x3;
	uda1380_write(UDA1380_REG_MSTRMUTE, (0xc000 | bass | (bass << 8) |
		(treble << 4) | (treble << 12)));
}

/***********************************************************************
 *
 * Function: uda1380_set_volume
 *
 * Purpose: Set volume level
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     left_vol  : Left volume value, 0 (max) to 252 (min)
 *     right_vol : Right volume value, 0 (max) to 252 (min)
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void uda1380_set_volume(UNS_8 left_vol, UNS_8 right_vol) {
	uda1380_write(UDA1380_REG_MSTRVOL, (left_vol | (right_vol << 8)));
}

/***********************************************************************
 *
 * Function: uda1380_enable_audio_out
 *
 * Purpose: Enable or disable audio output
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *    enable : !0 to enable audio output, 0 to disable
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void uda1380_enable_audio_out(INT_32 enable) {
	int i = 0;

	if (enable) {
		while (pbsetup[i] != 0xff) {
			uda1380_write((UNS_8) pbsetup[i], pbsetup[i + 1]);
			i += 2;
		}
	}
	else {
		while (pbdown[i] != 0xff) {
			uda1380_write((UNS_8) pbdown[i], pbdown[i + 1]);
			i += 2;
		}
	}
}

/***********************************************************************
 *
 * Function: uda1380_enable_audio_in
 *
 * Purpose: Enable or disable audio input
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *    enable : !0 to enable audio input, 0 to disable
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void uda1380_enable_audio_in(INT_32 enable) {
	/* Develope me! */
}

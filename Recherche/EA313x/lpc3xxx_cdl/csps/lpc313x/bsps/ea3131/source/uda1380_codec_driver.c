/***********************************************************************
 * $Id:: uda1380_codec_driver.c 6651 2011-03-02 06:51:24Z ing03005     $
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

typedef struct
{
  UNS_16 cached_regs[0x29];
  INT_32 i2cdev;
  UNS_8 i2caddr;
} UDA1380_DAT_T;
static UDA1380_DAT_T drvdata;

struct uda1380_setup
{
  UNS_8 reg;
  UNS_16 val;
};

static const struct uda1380_setup playback_enable[] =
{
  {0x01, 0x0000},
  {0x00, 0x0300},
  {0x02, 0x0100},
  {0x02, 0x0500},
  {0x02, 0x2500},
  {0x13, 0x0800},
  {0x14, 0x0000},
};

static const struct uda1380_setup playback_disable[] =
{
  {0x14, 0x0080},
  {0x00, 0x0300},
  {0x00, 0x0300},
  {0x00, 0x0000},
};

static const struct uda1380_setup recording_enable[] =
{
  {0x1, 0x0},
  {0x0, 0xc00},
  {0x2, 0x100},
  {0x2, 0x112},
  {0x2, 0x117},
  {0x2, 0x117},
  {0x0, 0xc00},
  {0x13, 0x0},
  {0x00, 0xc00},
  {0x11, 0x3c00},
  {0x22, 0xe},
  {0x23, 0x5},
};

static const struct uda1380_setup pb_rec_enable[] =
{
  {0x1, 0x0},
  {0x0, 0xF00},
  {0x2, 0x100},
  {0x2, 0x512},
  {0x2, 0x2517},
  {0x2, 0x2517},
  {0x0, 0xF00},
  {0x13, 0x800},
  {0x00, 0xF00},
  {0x11, 0x3c00},
  {0x22, 0xe},
  {0x23, 0x5},
  {0x14, 0x00},
};

static const struct uda1380_setup pb_rec_disable[] =
{
  {0x14, 0x0080},
  {0x00, 0x0300},
  {0x00, 0x0300},
  {0x00, 0x0000},
  {0x00, 0x0000},
  {0x02, 0x0117},
  {0x02, 0x0112},
  {0x02, 0x0100},
  {0x02, 0x0100},
};

static const struct uda1380_setup recording_disable[] =
{
  {0x00, 0x0000},
  {0x02, 0x0117},
  {0x02, 0x0112},
  {0x02, 0x0100},
  {0x02, 0x0100},
};

/***********************************************************************
 * Location functions
 **********************************************************************/

#ifdef USEDRIVER
static INT_32 i2chandle;

static void uda1380_prepare_buf(UNS_8 *buf, UNS_8 reg, UNS_16 val)
{
  buf[0] = reg;
  buf[1] = (val >> 8) & 0xFF;
  buf[2] = val & 0xFF;
}

#define I2C_DEV_RD(buf,len) i2c_dev_xfer(buf,len,1)
#define I2C_DEV_WR(buf,len) i2c_dev_xfer(buf,len,0)
static INT_32 i2c_dev_xfer(UNS_8 *buf, UNS_32 len, UNS_32 read)
{
  int i;
  UNS_32 status = 0;
  I2C_MTXRX_SETUP_T xfer;
  UNS_32 wdata[8], rdata[8];
  wdata[0] = drvdata.i2caddr | I2C_TXFF_START_CND | read;
  for (i = 0; i < len; i ++)
  {
    wdata[i+1] = (UNS_32) buf[i];
  }
  wdata[i] |= I2C_TXFF_STOP_CND;
  xfer.tx_data = wdata;
  xfer.rx_data = rdata;
  xfer.tx_length = len + 1;
  xfer.clock_rate = 100000;
  xfer.cb = NULL;

  if (i2c_mstr_ioctl(i2chandle, I2C_MSTR_TRANSFER, (INT_32) &xfer) ==
      _NO_ERROR)
  {
    status = i2c_mstr_ioctl(i2chandle, I2C_MSTR_GET_STATUS, 0);
    while ((status & (I2C_MSTR_STATUS_ARBF |
                      I2C_MSTR_STATUS_NOACKF | I2C_MSTR_STATUS_DONE)) == 0)
    {
      status = i2c_mstr_ioctl(i2chandle,
                              I2C_MSTR_GET_STATUS, 0);
    }
  }
  for (i = 0; read && i < len; i ++)
  {
    buf[i] = (UNS_8)(rdata[i] & 0xFF);
  }
  return status == I2C_MSTR_STATUS_DONE;
}
#else
#define pI2C1Regs                   ((I2C_REGS_T*) I2C1_BASE)
#define I2C_CLK_DIV_12MHZ           (60)

void i2c1_reg_init(void)
{
  pI2C1Regs->ckh = I2C_CLK_DIV_12MHZ;
  pI2C1Regs->ckl = I2C_CLK_DIV_12MHZ;

  pI2C1Regs->ctl = I2C_CTL_SOFT_RESET;  //Issue soft reset of the block
}

void i2c_clear_fifo(void)
{
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
  pI2C1Regs->fifo = (UNS_32)((reg_value >> 8) & 0xFF);
  pI2C1Regs->fifo = (UNS_32)(reg_value & 0xFF) | I2C_TXFF_STOP_CND;

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
INT_32 uda1380_driver_init(INT_32 i2cdev, UNS_8 i2caddr)
{
  drvdata.i2cdev = i2cdev;
  drvdata.i2caddr = i2caddr << 1;

#ifdef USEDRIVER
  i2chandle = i2c_mstr_open((void *) I2C1, 0);

#else
  i2c1_reg_init();
#endif

  /* Attempt to reset UDA1380 CODEC */
  uda1380_reset();

  uda1380_write(UDA1380_REG_PWRCTRL, 0x0100);
  uda1380_write(UDA1380_REG_EVALCLK, 0x0000);

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
void uda1380_shutdown(void)
{
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
void uda1380_reset(void)
{
#ifdef USEDRIVER
  UNS_8 data[8];
  INT_32 status;

  uda1380_prepare_buf(data, UDA1380_REG_L3, 0);
  status = I2C_DEV_WR(data, 3);

  /* I2C xfer has failed */
  if (!status)
    return;
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
UNS_16 uda1380_write(UNS_8 reg, UNS_16 data)
{
#ifdef USEDRIVER
  UNS_8 i2cdata[8];
  INT_32 status;

  if (reg == UDA1380_REG_L3)
  {
    uda1380_reset();
    return data;
  }

  uda1380_prepare_buf(i2cdata, reg, data);
  status = I2C_DEV_WR(i2cdata, 3);
  if (!status || (status = uda1380_read(reg)) < 0)
    return ~data;
  if (data == (UNS_16)status)
  {
    drvdata.cached_regs[reg] = data;
    return data;
  }
  return ~data;
#else
  i2c_reg_write(reg, data);

  return data;
#endif

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
UNS_16 uda1380_set(UNS_8 reg, UNS_16 setmask)
{
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
UNS_16 uda1380_clear(UNS_8 reg, UNS_16 clearmask)
{
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
INT_32 uda1380_read(UNS_8 reg)
{
  INT_32 status;

#ifndef USEDRIVER
  UNS_16 data = 0;
#endif

#ifdef USEDRIVER
  UNS_8  i2cdata[2];
  status = I2C_DEV_WR(&reg, 1);
  if (!status)
    return -1;
  status = I2C_DEV_RD(i2cdata, 2);
  if (!status)
    return -1;
  return (i2cdata[0] << 8) | (i2cdata[1] & 0xFF);
#else
  if ((reg == UDA1380_REG_HEADPHONE) || (reg == UDA1380_REG_DEC))
  {
    data = drvdata.cached_regs[reg];
  }
  else
  {
    data = i2c1_reg_read(reg);
  }

  return data;
#endif


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
void uda1380_mute(INT_32 mute)
{
  if (mute)
  {
    uda1380_set(UDA1380_REG_MSTRMUTE, (1 << 14) | (1 << 11) | (1 << 3));
  }
  else
  {
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
void uda1380_set_boost(UNS_8 bass, UNS_8 treble)
{
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
void uda1380_set_volume(UNS_8 left_vol, UNS_8 right_vol)
{
  uda1380_write(UDA1380_REG_MSTRVOL, (left_vol | (right_vol << 8)));
}

/***********************************************************************
 *
 * Function: uda1380_setup_data
 *
 * Purpose: Writes the set of registers and their values to Codec
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *         us -> Pointer to array of uda1380_setup struct
 *         cnt -> Number of items in the @us array
 *
 * Outputs: None
 *
 * Returns: Number of failed writes
 *
 * Notes: None
 *
 **********************************************************************/
static int uda1380_setup_data(const struct uda1380_setup *us, int cnt)
{
  int error = 0, i;
  for (i = 0; i < cnt; i ++)
  {
    error += uda1380_write(us->reg, us->val) != us->val;
    us++;
  }
  return error;
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
int uda1380_enable_audio_out(INT_32 enable)
{
  const struct uda1380_setup *us = enable ? playback_enable : playback_disable;
  int cnt = enable ? sizeof(playback_enable) : sizeof(playback_disable);
  cnt /= sizeof(*us);
  return uda1380_setup_data(us, cnt);
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
int uda1380_enable_audio_in(INT_32 enable)
{
  const struct uda1380_setup *us = enable ? recording_enable : recording_disable;
  int cnt = enable ? sizeof(recording_enable) : sizeof(recording_disable);
  cnt /= sizeof(*us);
  return uda1380_setup_data(us, cnt);
}

/***********************************************************************
 *
 * Function: uda1380_enable_audio
 *
 * Purpose: Enable or disable audio input and output
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
int uda1380_enable_audio(INT_32 enable)
{
  const struct uda1380_setup *us = enable ? pb_rec_enable : pb_rec_disable;
  int cnt = enable ? sizeof(pb_rec_enable) : sizeof(pb_rec_disable);
  cnt /= sizeof(*us);
  return uda1380_setup_data(us, cnt);
}

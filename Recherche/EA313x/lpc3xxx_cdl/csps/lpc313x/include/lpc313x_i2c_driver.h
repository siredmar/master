/***********************************************************************
 * $Id:: lpc313x_i2c_driver.h 2026 2009-07-22 22:56:32Z wellsk         $
 *
 * Project: LPC313x I2C driver
 *
 * Description:
 *     This file contains driver support for the LPC313x I2C.
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
 *********************************************************************/

#ifndef LPC313X_I2C_DRIVER_H
#define LPC313X_I2C_DRIVER_H

#include "lpc313x_i2c.h"

#ifdef __cplusplus
extern "C"
{
#endif

/***********************************************************************
 * I2C interrupt handlers
 **********************************************************************/
void i2c0_user_interrupt(void);     /* default I2C0 interrupt handler */
void i2c1_user_interrupt(void);     /* default I2C1 interrupt handler */
void i2c_mtx_handler(INT_32);       /* I2C master transmitter handler */
void i2c_mrx_handler(INT_32);       /* I2C master receiver handler */
void i2c_stx_handler(INT_32);       /* I2C slave transmitter handler */
void i2c_srx_handler(INT_32);       /* I2C slave receiver handler */
void i2c_mtxrx_handler(INT_32);     /* I2C master tx/rx handler */
void i2c_srxtx_handler(INT_32);     /* I2C slave rx/tx handler */

/***********************************************************************
 * I2C device configuration commands (IOCTL commands and arguments)
 **********************************************************************/

/* I2C device commands (IOCTL commands) */
typedef enum
{
  I2C_SETUP,                      /* I2C setup */
  I2C_MASTER_TX,                  /* I2C master Tx setup */
  I2C_MASTER_RX,                  /* I2C master Rx setup */
  I2C_SLAVE_TX,                   /* I2C slave Tx setup */
  I2C_SLAVE_RX,                   /* I2C slave Rx setup */
  I2C_MASTER_TXRX,                /* I2C master Tx then Rx */
  I2C_SLAVE_RXTX                  /* I2C slave Rx then Tx */
} I2C_IOCTL_CMD_T;

/* I2C master activity options */
typedef enum
{
  I2C_M_WRITE = 0,
  I2C_M_READ
} I2CM_ACTIVITY_OPT;

/* I2C address mode options */
typedef enum
{
  ADDR7BIT = 0,
  ADDR10BIT
} I2C_ADDR_OPT;

/* I2C rate options */
typedef enum
{
  I2C_RATE_ABSOLUTE = 0,
  I2C_RATE_RELATIVE
} I2C_RATE_OPT;

/* structure containing I2C channel setup arguments */
typedef struct
{
  I2C_ADDR_OPT  addr_mode;
  UNS_32  sl_addr;
  I2C_RATE_OPT rate_option;
  UNS_32  rate;
  UNS_32  low_phase;
  UNS_32  high_phase;
} I2C_SETUP_T;

/* I2C device configuration structure type */
typedef struct
{
  I2C_REGS_T  *regptr;    /* Pointer to I2C registers */
  UNS_32      txrx_setup; /* Transmission setup */
  I2C_SETUP_T *ip_setup;  /* I2C setup parameters */
  PFV         handler;    /* Transmission interrupt handler */
  BOOL_32     init;       /* Device initialized flag */
} I2C_CFG_T;

/* setup status values */
#define I2C_SETUP_STATUS_ARBF   _BIT(8)
#define I2C_SETUP_STATUS_NOACKF _BIT(9)
#define I2C_SETUP_STATUS_DONE   _BIT(10)

/* structure containing I2C channel master Tx setup arguments */
typedef struct
{
  I2C_ADDR_OPT    addr_mode;
  UNS_32          sl_addr;
  UNS_8*          tx_data;
  UNS_32          tx_length;
  UNS_32          tx_count;
  UNS_32          retransmissions_max;
  UNS_32          retransmissions_count;
  volatile UNS_32 status;
} I2C_MTX_SETUP_T;

/* structure containing I2C channel master Rx setup arguments */
typedef struct
{
  I2C_ADDR_OPT    addr_mode;
  UNS_32          sl_addr;
  UNS_8*          rx_data;
  UNS_32          rx_length;
  UNS_32          rx_count;
  UNS_32          tx_count;
  UNS_32          retransmissions_max;
  UNS_32          retransmissions_count;
  volatile UNS_32 status;
} I2C_MRX_SETUP_T;

/* structure containing I2C channel slave Tx setup arguments */
typedef struct
{
  UNS_8*          tx_data;
  UNS_32          tx_length;
  UNS_32          tx_count;
  volatile UNS_32 status;
} I2C_STX_SETUP_T;

/* structure containing I2C channel slave Rx setup arguments */
typedef struct
{
  UNS_8*          rx_data;
  UNS_32          rx_length;
  UNS_32          rx_count;
  volatile UNS_32 status;
} I2C_SRX_SETUP_T;

/* structure containing I2C channel master Tx/Rx setup arguments */
typedef struct
{
  I2C_ADDR_OPT    addr_mode;
  UNS_32          sl_addr;
  UNS_8*          tx_data;
  UNS_32          tx_length;
  UNS_32          tx_count;
  UNS_8*          rx_data;
  UNS_32          rx_length;
  UNS_32          rx_count;
  UNS_32          retransmissions_max;
  UNS_32          retransmissions_count;
  volatile UNS_32 status;
} I2C_MTXRX_SETUP_T;

/***********************************************************************
 * I2C driver API functions
 **********************************************************************/

/* Open a I2C */
INT_32 i2c_open(void *ipbase, INT_32 arg);

/* Close the I2C */
STATUS i2c_close(INT_32 devid);

/* I2C configuration block */
STATUS i2c_ioctl(INT_32 devid,
                 INT_32 cmd,
                 INT_32 arg);

/* I2C read function (stub only) */
INT_32 i2c_read(INT_32 devid,
                void *buffer,
                INT_32 max_bytes);

/* I2C write function (stub only) */
INT_32 i2c_write(INT_32 devid,
                 void *buffer,
                 INT_32 n_bytes);

/***********************************************************************
 * Other I2C driver functions
 **********************************************************************/
void  enable_i2c_irq_int(UNS_32);
void disable_i2c_irq_int(UNS_32);

#ifdef __cplusplus
}
#endif

#endif /* LPC313X_I2C_DRIVER_H */

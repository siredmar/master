/***********************************************************************
 * $Id:: lpc313x_spi_driver.h 1180 2008-09-11 19:32:40Z pdurgesh       $
 *
 * Project: LPC313X SPI driver
 *
 * Description:
 *     This file contains driver support for the SPI module on the
 *     LPC313X
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

#ifndef LPC313X_SPI_DRIVER_H
#define LPC313X_SPI_DRIVER_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "lpc313x_spi.h"

/***********************************************************************
 * SPI device driver enums
 **********************************************************************/

/* SPI slave ids */
typedef enum
{
  SPI_SLAVE1_ID = 0,
  SPI_DEFAULT_ID = SPI_SLAVE1_ID,
  SPI_SLAVE2_ID,
  SPI_SLAVE3_ID,
  SPI_INVALID_ID = 0xFF
} SPI_SLAVE_ID_T;

/* SPI transfer mode */
typedef enum
{
  SPI_MODE0,
  SPI_MODE1,
  SPI_MODE2,
  SPI_MODE3,
  SSI_MODE
} SPI_TRANS_MODE_T;

/* SPI device commands (IOCTL commands) */
typedef enum
{
  /* Enable or disable the SPI, use arg = 0 to disable, arg = 1
     to enable */
  SPI_ENABLE,
  /* selects the current slave for upcoming transactions */
  SPI_SET_ACTIVE_SLAVE,
  /* Setup the SPI controller, use arg as a pointer to type
     SPI_CONFIG_T */
  SPI_CONFIG,
  /* Setup the slave settings, use arg as a pointer to type
     SPI_SLAVE_CONFIG_T */
  SPI_SLAVE_CONFIG,
  /* Enable or disable loopback mode, use arg as '1' to enable or
     '0' to disable */
  SPI_ENABLE_LOOPB,
  /* Slave output disable, use '1' to disable, or '0' for normal
     operation */
  SPI_SO_DISABLE,
  /* Flush RX FIFO */
  SPI_FLUSH_RX_FIFO,
  /* Flush TX FIFO */
  SPI_FLUSH_TX_FIFO,
  /* Setup callbacks, use arg as a pointer to type SPI_CB_T */
  SPI_SET_CALLBACKS,
  /* Clear SPI interrupts, use arg as a OR'ed value of
     SPI_IIR_RORRIS or SPI_IIR_RTRIS */
  SPI_CLEAR_INTS,
  /* Get a SPI status, use an argument type of SPI_IOCTL_STS_T
     as the argument to return the correct status */
  SPI_GET_STATUS
} SPI_IOCTL_CMD_T;

/* SPI device arguments for SPI_GET_STATUS command (IOCTL arguments) */
typedef enum
{
  /* main status of SPI block */
  SPI_FIFO_ST,
  /* Returns SPI clock rate for slave0 in Hz */
  SPI_SLV1_CLOCK_ST,
  /* Returns SPI clock rate for slave0 in Hz */
  SPI_SLV2_CLOCK_ST,
  /* Returns SPI clock rate for slave0 in Hz */
  SPI_SLV3_CLOCK_ST,
  /* Returns SPI clock rate in Hz. Returns value set for slave0. */
  SPI_CLOCK_ST,
  /* Returns masked pending interrupts (a combination of
     SPI_IIR_RORRIS, SPI_IIR_RTRIS, SPI_IIR_RXRIS, and
     SPI_IIR_TXRIS) */
  SPI_PENDING_INTS_ST,
  /* Returns raw pending interrupts (a combination of SPI_IIR_RORRIS,
     SPI_IIR_RTRIS, SPI_IIR_RXRIS, and SPI_IIR_TXRIS) */
  SPI_RAW_INTS_ST
} SPI_IOCTL_STS_T;


/***********************************************************************
 * SPI device configuration commands (IOCTL commands and arguments)
 **********************************************************************/

/* Structure for setting up SPI parameters */
typedef struct
{
  /* slave index */
  SPI_SLAVE_ID_T sid;
  /* Number of data bits, must be between 4 and 16 */
  UNS_32 databits;
  /* Number of words required in sequential multi-slave transfer mode */
  UNS_32 words;
  /* Transfer mode  */
  SPI_TRANS_MODE_T mode;
  /* Flag used to set cs polarity high between frames */
  BOOL_32 cs_high;
  /* Post & pre transfer delay */
  UNS_32 pp_delay;
  /* inter transfer delay */
  UNS_32 inter_delay;
  /* Serial clock rate */
  UNS_32 clk;
} SPI_SLAVE_CONFIG_T;

/* Structure for setting up SPI parameters */
typedef struct
{
  /* Master/slave mode, use TRUE for master mode */
  BOOL_32 master_mode;
  /* inter-slave delay */
  UNS_32 inter_slave_delay;
  /* slave configuartions */
  SPI_SLAVE_CONFIG_T slave[3];
} SPI_CONFIG_T;

/* Callbacks for SPI */
typedef struct
{
  /* Transmit callback when more data is needed */
  PFV txcb;
  /* Receive callback (FIFO, FIFO timeout, and RX overtrun) */
  PFV rxcb;
} SPI_CB_T;

/***********************************************************************
 * SPI driver API functions
 **********************************************************************/

/* Open the SPI */
INT_32 spi_open(void *ipbase,
                INT_32 arg);

/* Close the SPI */
STATUS spi_close(INT_32 devid);

/* SPI configuration block */
STATUS spi_ioctl(INT_32 devid,
                 INT_32 cmd,
                 INT_32 arg);

/* SPI write function - the buffer must be aligned on a 16-bit
   boundary if the data size is 9 bits or more */
INT_32 spi_write(INT_32 devid,
                 void *buffer,
                 INT_32 n_fifo);

/* SPI read function - the buffer must be aligned on a 16-bit
   boundary if the data size is 9 bits or more */
INT_32 spi_read(INT_32 devid,
                void *buffer,
                INT_32 max_fifo);

/* SPI0 interrupt handler */
void spi0_int(void);

/* SPI1 interrupt handler */
void spi1_int(void);

#ifdef __cplusplus
}
#endif

#endif /* LPC313X_SPI_DRIVER_H */

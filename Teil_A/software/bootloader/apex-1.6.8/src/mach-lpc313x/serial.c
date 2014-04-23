/* serial.c

   written by Durgesh Pattamatta
   1 Nov 2004

   Copyright (C) 2008 NXP Semiconductors

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------
   This implementation assumes the UART clock is sourced by 12MHz FFAST_IN
   clock in CGU. 

*/

#include <driver.h>
#include <service.h>
#include <apex.h>		/* printf, only */
#include "lpc313x.h"
#include "lpc313x_cgu.h"


//#include <debug_ll.h>
#if (CONFIG_OSC_FREQ == 24000000)
#define UART_BRD_115200		(13)
#define UART_FDR_115200		((1 << 4) | (0 << 0))
#else
#define UART_BRD_115200		(6)
#define UART_FDR_115200		((12 << 4) | (1 << 0))
#endif

#define UART_RX_ERR (UART_LSR_FIFORX_ERR | UART_LSR_FR | UART_LSR_PE | UART_LSR_OE)

extern struct driver_d* console_driver;

static struct driver_d lpx313x_serial_driver;

void lpx313x_serial_init (void)
{
  /* enable UART clocks */
  cgu_clk_en_dis(CGU_SB_UART_APB_CLK_ID, 1);
  cgu_clk_en_dis(CGU_SB_UART_U_CLK_ID, 1);

  /* enable access to the Divisor latch registers */
  UART_LCR  = UART_LCR_DIVLATCH_EN;          
  /* set LSByte of Divisor */
  UART_DLL  = UART_LOAD_DLL(UART_BRD_115200); 
  /* set MSByte of Divisor */
  UART_DLM  = UART_LOAD_DLM(UART_BRD_115200); 
  /* disable acces to Divisor latch registers, set 8bit data, 1 stop, no parity */
  UART_LCR  = UART_LCR_WLEN_8BITS;          
  /* configure fractional divider */
  UART_FDR  = UART_FDR_115200;			
  /* disable modem control stuff */
  UART_MCR  = 0x00;          
  /* enable fifo. clear Tx and Rx fifos and holding registers. */
  UART_FCR  = UART_FCR_FIFO_EN | UART_FCR_TXFIFO_FLUSH | UART_FCR_RXFIFO_FLUSH;
  /* disable Tx and Rx interrupts */          
  UART_IE  = 0x00;          

  if (console_driver == 0)
    console_driver = &lpx313x_serial_driver;

//  lpx313x_serial_driver.write (NULL, "serial console initialized\r\n", 28);
//  printf ("Really, it is\n");
}

void lpx313x_serial_release (void)
{
	/* flush serial output */
  while (!(UART_LSR & UART_LSR_TEMT))
    ;
}

ssize_t lpx313x_serial_poll (struct descriptor_d* d, size_t cb)
{
  return (UART_LSR & UART_LSR_RDR);
}

ssize_t lpx313x_serial_read (struct descriptor_d* d, void* pv, size_t cb)
{
  ssize_t cRead = 0;
  unsigned char* pb;
  for (pb = (unsigned char*) pv; cb--; ++pb) {

    while (!(UART_LSR & (UART_LSR_RDR | UART_RX_ERR)))
      ;				/* block until character is available */

    if (UART_LSR & UART_RX_ERR)
      return -1;		/* -ESERIAL */
    
    *pb = UART_FIFO;
    ++cRead;
  }

  return cRead;
}

ssize_t lpx313x_serial_write (struct descriptor_d* d,
			      const void* pv, size_t cb)
{
  ssize_t cWrote = 0;
  int n_bytes = 0;
  const unsigned char* pb = pv;
  for (pb = (unsigned char*) pv; cb--; ++pb) {

	  /* wait until FIFO is empty */
	  if (n_bytes == 0) {
	  
      while (!(UART_LSR & UART_LSR_TEMT))
        ;
       /* FIFO is empty so we can write upto 32 bytes */ 
       n_bytes = 32;
     }

    UART_FIFO = *pb;
    --n_bytes;
    ++cWrote;
  }

  /* wait until FIFO is empty */
  while (!(UART_LSR & UART_LSR_TEMT))
    ;

  return cWrote;
}

static __driver_0 struct driver_d lpx313x_serial_driver = {
  .name = "serial-lpx313x",
  .description = "lpx313x serial driver",
  .flags = DRIVER_SERIAL | DRIVER_CONSOLE,
  .read = lpx313x_serial_read,
  .write = lpx313x_serial_write,
  .poll = lpx313x_serial_poll,
};

static __service_3 struct service_d lpx313x_serial_service = {
  .init = lpx313x_serial_init,
  .release = lpx313x_serial_release,
};

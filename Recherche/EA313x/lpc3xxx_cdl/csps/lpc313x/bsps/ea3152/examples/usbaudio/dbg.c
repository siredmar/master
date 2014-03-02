/***********************************************************************
 * $Id:: dbg.c 6636 2011-03-01 04:26:56Z ing02124                     $
 *
 * Project: USB Debug functions
 *
 * Description:
 *     USB debug function used for debugging.
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
#include <stdio.h>
#include <string.h>

#include "lpc313x_uart_driver.h"
#include "dbg.h"

static INT_32 uartdev;

char buftmp[512];

/***********************************************************************
 *
 * Function: uart_init
 *
 * Purpose: Initialise UART
 *
 * Processing:
 *     See function.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 **********************************************************************/
void uart_init(void)
{
  uartdev = uart_open((void *) UART_BASE, 0);
}

#ifdef DEBUG
static struct dbg_buffer
{
  volatile int head;
  volatile int tail;
  volatile int need_flush;
  volatile int full;
  volatile int empty;
  char buffer[DBUF_SZ];
}dbgobj, *dbgbuf = &dbgobj;

/***********************************************************************
 *
 * Function: dbgbuf_init
 *
 * Purpose: Debug Buffer Init function
 *
 * Processing:
 *     See function.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 **********************************************************************/
void dbgbuf_init(void)
{
  uart_init();
  dbgbuf->empty = 1;
}

/***********************************************************************
 *
 * Function: dbgbuf_flush
 *
 * Purpose: Flush Debug Buffer function
 *
 * Processing:
 *     See function.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 **********************************************************************/
void dbgbuf_flush(void)
{
  int h = dbgbuf->head;
  int t = dbgbuf->tail;
  int f = dbgbuf->full;
  int wrap = h < t;
  int end = wrap ? DBUF_SZ : h + f;
  if (dbgbuf->empty)
    return ;

  uart_string_write(buftmp, 0);
  uart_string_write(&dbgbuf->buffer[t], end - t);
  if (wrap)
  {
    uart_string_write(buftmp, 0);
    uart_string_write(dbgbuf->buffer, h + f);
  }
  dbgbuf->tail = h;
  dbgbuf->need_flush = 0;
  dbgbuf->empty = dbgbuf->tail == dbgbuf->head;
  dbgbuf->full = 0;
}

/***********************************************************************
 *
 * Function: dbgbuf_write
 *
 * Purpose: Write into Debug Buffer function
 *
 * Processing:
 *     See function.
 *
 * Parameters: @buff: Data buffer
 *             @len: Size of data
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 **********************************************************************/
void dbgbuf_write(const char * buff, UNS_32 len)
{
  int t = dbgbuf->tail;
  int i, f;
  if (!len || dbgbuf->full)
    return;
  dbgbuf->empty = 0;

  for (i = dbgbuf->head; len--; buff++)
  {
    dbgbuf->buffer[i++] = *buff;
    if (i >= DBUF_SZ) i = 0;

    if (i != t) continue;
    dbgbuf->need_flush = 1;
    dbgbuf->head = i ? i - 1 : DBUF_SZ - 1;
    dbgbuf->full = 1;
    return ;
  }

  dbgbuf->head = i;
  f = (i + (i < t) * DBUF_SZ) - t;
  if (f >= (DBUF_SZ * FLUSH_SZ) / 100) dbgbuf->need_flush = 1;
}

/***********************************************************************
 *
 * Function: dbgbuf_puts
 *
 * Purpose: Write into Debug Buffer function
 *
 * Processing:
 *     See function.
 *
 * Parameters: @buff: Data buffer
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 **********************************************************************/
void dbgbuf_puts(const char * buff)
{
  dbgbuf_write(buff, strlen(buff));
}

/***********************************************************************
 *
 * Function: dump_addr_range
 *
 * Purpose: Dump contents of Address range
 *
 * Processing:
 *     See function.
 *
 * Parameters: @start: Start address
 *             @end: End address
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 **********************************************************************/
void dump_addr_range(uint32_t *start, uint32_t *end)
{
  int i;
  int n = (uint32_t)start, s = (n & 0xF) / 4;
  for (i = 0; s && i < s; i++)
  {
    if (!i)
    {
      sprintf(buftmp, "\r\n 0x%08x : ", n & ~0xF);
      dbgbuf_puts(buftmp);
    }
    dbgbuf_puts("           ");
  }
  for (i = 0; i <= end - start; i++)
  {
    if (!((s + i) & 3))
    {
      sprintf(buftmp, "\r\n 0x%08x : ", (unsigned int) &start[i]);
      dbgbuf_puts(buftmp);
    }
    sprintf(buftmp, "0x%08x ", (unsigned int) start[i]);
    dbgbuf_puts(buftmp);
  }
  dbgbuf_puts("\r\n");
}

/***********************************************************************
 *
 * Function: dbgbuf_needs_flush
 *
 * Purpose: Check if debug buffer needs to be flushed
 *
 * Processing:
 *     See function.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 **********************************************************************/
int dbgbuf_needs_flush(void)
{
  return dbgbuf->need_flush;
}
#endif /* DEBUG */

/***********************************************************************
 *
 * Function: uart_string_write
 *
 * Purpose: UART string output helper function
 *
 * Processing:
 *     See function.
 *
 * Parameters: @pbuff - Pointer to char buffer
 *             @len   - Length of buffer
 *
 * Outputs: Character stored in @pbuff printed to uart
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 ***********************************************************************/
void uart_string_write(const char *pbuff, UNS_32 len)
{
  UNS_32 written;
  UNS_32 devid = uartdev;

  if (devid != 0)
  {
    if (len == 0)
      len = strlen(pbuff);

    while (len > 0)
    {
      written = uart_write(devid, (UNS_8 *)pbuff, len);
      len -= written;
      pbuff += written;
    }
  }
}

/***********************************************************************
 *
 * Function: uart_puts
 *
 * Purpose: UART string write function
 *
 * Processing:
 *     See function.
 *
 * Parameters: @str - Pointer to char buffer
 *
 * Outputs: Character stored in @pbuff printed to uart
 *
 * Returns: None
 *
 * Notes: None
 *
 ***********************************************************************/
void uart_puts(const char *str)
{
  uart_string_write(str, 0);
}

/***********************************************************************
 *
 * Function: uart_getchar
 *
 * Purpose: UART read character function
 *
 * Processing:
 *     See function.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: Character or -1 in case of error
 *
 * Notes: None
 *
 ***********************************************************************/
int uart_getchar(void)
{
  UNS_8 ch;
  if (uart_read(uartdev, &ch, 1) > 0)
    return (int)ch;
  else
    return -1;
}

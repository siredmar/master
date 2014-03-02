/***********************************************************************
 *
 *
 * Project: LPC313x debug routines
 *
 * Description:
 *     This example provides various routines to print strings to
 *     UART or debug buffer (available only if DEBUG is defined).
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
#include <string.h>

#include "lpc313x_uart_driver.h"

#define DEBUG
#include "lpc313x_debug.h"

static INT_32 uartdev;

char buftmp[512];

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
 * Function: dbgbuf_flush
 *
 * Purpose: Flush the content of debug buffer to UART
 *
 * Processing:
 *     See function.
 *
 * Parameters: None
 *
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 ***********************************************************************/
void dbgbuf_flush(void)
{
  int h = dbgbuf->head;
  int t = dbgbuf->tail;
  int f = dbgbuf->full;
  int wrap = h < t;
  int end = wrap ? DBUF_SZ : h + f;
  if (dbgbuf->empty)
    return ;

  lpc313x_uart_string_write(buftmp, 0);
  lpc313x_uart_string_write(&dbgbuf->buffer[t], end - t);
  if (wrap)
  {
    lpc313x_uart_string_write(buftmp, 0);
    lpc313x_uart_string_write(dbgbuf->buffer, h + f);
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
 * Purpose: Writes a string data to debug buffer
 *
 * Processing:
 *     See function.
 *
 * Parameters: @buff - Contains string to be written
 *             @len  - Length of the string
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 ***********************************************************************/
void dbgbuf_write(const char * buff, UNS_32 len)
{
  int t = dbgbuf->tail;
  int i, f;

  if (!dbgbuf->full && !dbgbuf->empty &&
      (dbgbuf->head == dbgbuf->tail))
  {
    dbgbuf->empty = 1;
  }
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
 * Purpose: Writes a NUL terminated ASCII string to debug buffer
 *
 * Processing:
 *     See function.
 *
 * Parameters: @buff - Contains string to be written
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 ***********************************************************************/
void dbgbuf_puts(const char * buff)
{
  dbgbuf_write(buff, strlen(buff));
}

/***********************************************************************
 *
 * Function: dump_addr_range
 *
 * Purpose: Dumps word data available in a range of memory
 *
 * Processing:
 *     See function.
 *
 * Parameters: @start - Start address of memory region
 *             @end   - End address of memory region
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 ***********************************************************************/
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
 * Purpose: To check if the debug buffer is almost full
 *
 * Processing:
 *     See function.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: 1 - Is the buffer is almost full
 *          0 - Buffer has room for more data
 *
 * Notes: None
 *
 ***********************************************************************/
int dbgbuf_needs_flush(void)
{
  return dbgbuf->need_flush;
}
#endif /* DEBUG */

/***********************************************************************
 *
 * Function: lpc313x_uart_string_write
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
void lpc313x_uart_string_write(const char *pbuff, UNS_32 len)
{
  UNS_32 written;
  UNS_32 devid;

  uartdev = uartdev ? uartdev : uart_open((void *) DEBUG_UART, 0);
  devid = uartdev;

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
 * Purpose: Writes a NUL terminated ASCII string to UART
 *
 * Processing:
 *     See function.
 *
 * Parameters: @str - Contains string to be written
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 ***********************************************************************/
void uart_puts(const char *str)
{
  lpc313x_uart_string_write(str, 0);
}

/***********************************************************************
 *
 * Function: uart_getchar
 *
 * Purpose: Reads a single character from UART
 *
 * Processing:
 *     See function.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: -1 - If no characters available from UART
 *        : int converted ASCII value of received character
 *
 * Notes: This is a non blocking routine.
 *
 ***********************************************************************/
int uart_getchar(void)
{
  UNS_8 ch;
  uartdev = uartdev ? uartdev : uart_open((void *) DEBUG_UART, 0);
  if (uart_read(uartdev, &ch, 1) > 0)
    return (int)ch;
  else
    return -1;
}

/***********************************************************************
 *
 * Function: uart_gets
 *
 * Purpose: Reads a single character from UART
 *
 * Processing:
 *     See function.
 *
 * Parameters: @str - Pointer to string
 *             @len - Length of the string array
 *
 * Outputs: None
 *
 * Returns: Length of received string
 *
 *
 * Notes: This is a non blocking routine.
 *
 ***********************************************************************/
int uart_gets(char * str, int len)
{
  int i;
  char val;
  for (i = 0; i < len - 1; i++)
  {
    int ch;
    while ((ch = uart_getchar()) == -1);
    switch (ch)
    {
      case '\r':
        uart_puts("\r\n");
        str[i] = 0;
        return i;

      case '\n':
        if (!i)
        {
          /* Stray Line feed from previous input */
          i --;
          continue;
        }
        break;

      case '\b':
        /* Erase 1 byte from console */
        if (i)
        {
          uart_puts("\033[1D \033[1D");
          i -= 2;
        }
        else
        {
          i --;
        }
        continue;

      case '\003': /* ^C */
        uart_puts("^C");
        return 0;

      default:
        /* We need to echo the typed char */
        val = (char) ch;
        lpc313x_uart_string_write(&val, 1);
    }
    str[i] = (char) ch;
  }
  str[i] = 0;
  return i;
}

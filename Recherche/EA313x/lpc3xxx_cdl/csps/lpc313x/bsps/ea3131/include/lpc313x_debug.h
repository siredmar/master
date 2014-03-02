/***********************************************************************
 * $Id:: lpc313x_debug.h 6405 2011-02-07 04:30:58Z ing03005                $
 *
 * Project: LPC313x Debug interfaces
 *
 * Description:
 *     LPC313x Debug aid routines
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
#ifndef _LPC313X_DEBUG_H
#define _LPC313X_DEBUG_H

#define DBUF_SZ    4096
#define FLUSH_SZ   70 /* in % */
#define DEBUG_UART UART_BASE

extern char buftmp[512];
#ifdef __cplusplus
extern "C"
{
#endif
void lpc313x_uart_string_write(const char *pbuff, UNS_32 len);
void uart_puts(const char *str);
int uart_getchar(void);
int uart_gets(char * str, int len);
#ifdef DEBUG
void dbgbuf_puts(const char *str);
void dbgbuf_flush(void);
int dbgbuf_needs_flush(void);
void dump_addr_range(uint32_t *start, uint32_t *end);
#else
#define dbgbuf_needs_flush() 0
#define dbgbuf_puts(x)
#define dbgbuf_flush()
#define dump_addr_range(x,y)
#endif

/* Kept here to avoid including stdio.h */
int sprintf(char *, const char *, ...);
#ifdef __cplusplus
}
#endif

#endif /* _LPC313X_DEBUG_H */

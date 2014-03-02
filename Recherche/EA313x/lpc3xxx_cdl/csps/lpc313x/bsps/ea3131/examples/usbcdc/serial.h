/***********************************************************************
 * $Id:: serial.h 6288 2011-01-28 10:23:45Z ing02124                     $
 *
 * Project: USB serial driver include file
 *
 * Description:
 *     A simple USB serial driver.
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

/*----------------------------------------------------------------------------
 Serial interface related prototypes
 *---------------------------------------------------------------------------*/
extern void  ser_OpenPort  (void);
extern void  ser_ClosePort (void); 
extern void  ser_InitPort  (unsigned long baudrate, unsigned int databits, unsigned int parity, unsigned int stopbits);
extern void  ser_AvailChar (int *availChar);
extern int   ser_Write     (const char *buffer, int *length);
extern int   ser_Read      (char *buffer, const int *length);
extern void  ser_LineState (unsigned short *lineState);


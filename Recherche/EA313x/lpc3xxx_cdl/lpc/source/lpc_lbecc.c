/***********************************************************************
 * $Id:: lpc_lbecc.c 1290 2008-10-30 22:41:55Z wellsk                  $
 *
 * Project: NAND software ECC generation and correction
 *
 * Description:
 *     Generates, checks, and corrects 512 byte ECCs.
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

#include "lpc_lbecc.h"

/***********************************************************************
* Local data and types
**********************************************************************/

/* Generated parity lookup tables used in software mode */
static unsigned char P1Otab[256];
static unsigned char P1Etab[256];
static unsigned char P2Otab[256];
static unsigned char P2Etab[256];
static unsigned char P4Otab[256];
static unsigned char P4Etab[256];
static unsigned char PBytetab[256];

/* Bit isolation macro for table generation */
#define ISOLATEBIT(x, y) (unsigned char) ((y >> x) & 0x1)

/***********************************************************************
* Private functions
**********************************************************************/

/***********************************************************************
 *
 * Function: lpc_eccGetBits
 *
 * Purpose: Determine number of active bits in the XOR'd value
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *  v : Value to check the number of bits
 *
 * Outputs: None
 *
 * Returns: The number of active bits in a 32-bit word.
 *
 * Notes: None
 *
 **********************************************************************/
static UNS_8 lpc_eccGetBits(UNS_32 v)
{
  UNS_8 i, count;

  if (v == 0)
  {
    return 0;
  }

  count = 0;
  for (i = 0; i < 32; i++)
  {
    if ((v & (0x1 << i)) != 0)
    {
      count++;
    }
  }

  return count;
}

/***********************************************************************
 *
 * Function: lpc_eccGenParityBit4
 *
 * Purpose: Generate XOR parity bit for 4 bits
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *  b0 : Bit 0
 *  b1 : Bit 1
 *  b2 : Bit 2
 *  b3 : Bit 3
 *
 * Outputs: None
 *
 * Returns: The XOR'ed bit value of the 4 bits
 *
 * Notes: None
 *
 **********************************************************************/
static UNS_8 lpc_eccGenParityBit4(UNS_8 b0,
                                  UNS_8 b1,
                                  UNS_8 b2,
                                  UNS_8 b3)
{
  return b0 ^ b1 ^ b2 ^ b3;
}

/***********************************************************************
 *
 * Function: lpc_eccGenParityBit8
 *
 * Purpose: Generate XOR parity bit for 8 bits
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *  data : 8-bit data value
 *
 * Outputs: None
 *
 * Returns: The XOR'ed bit value of the 8 bits
 *
 * Notes: None
 *
 **********************************************************************/
static UNS_8 lpc_eccGenParityBit8(UNS_8 data)
{
  int idx;
  UNS_8 par = 0;

  for (idx = 0; idx < 8; idx++)
  {
    par = par ^(data & 0x1);
    data = data >> 1;
  }

  return par;
}

/***********************************************************************
* Public functions
**********************************************************************/

/***********************************************************************
 *
 * Function: lpc_eccinittables
 *
 * Purpose: Generate parity lookup tables for software mode
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
void lpc_eccinittables(void)
{
  INT_32 idx;

  /* All tables */
  for (idx = 0; idx < 256; idx++)
  {
    P1Otab[idx] = lpc_eccGenParityBit4(ISOLATEBIT(7, idx), ISOLATEBIT(5, idx),
                                       ISOLATEBIT(3, idx), ISOLATEBIT(1, idx));
    P1Etab[idx] = lpc_eccGenParityBit4(ISOLATEBIT(6, idx), ISOLATEBIT(4, idx),
                                       ISOLATEBIT(2, idx), ISOLATEBIT(0, idx));
    P2Otab[idx] = lpc_eccGenParityBit4(ISOLATEBIT(7, idx), ISOLATEBIT(6, idx),
                                       ISOLATEBIT(3, idx), ISOLATEBIT(2, idx));
    P2Etab[idx] = lpc_eccGenParityBit4(ISOLATEBIT(5, idx), ISOLATEBIT(4, idx),
                                       ISOLATEBIT(1, idx), ISOLATEBIT(0, idx));
    P4Otab[idx] = lpc_eccGenParityBit4(ISOLATEBIT(7, idx), ISOLATEBIT(6, idx),
                                       ISOLATEBIT(5, idx), ISOLATEBIT(4, idx));
    P4Etab[idx] = lpc_eccGenParityBit4(ISOLATEBIT(3, idx), ISOLATEBIT(2, idx),
                                       ISOLATEBIT(1, idx), ISOLATEBIT(0, idx));
    PBytetab[idx] = lpc_eccGenParityBit8((UNS_8) idx);
  }
}

/***********************************************************************
 *
 * Function: lpc_eccgenerate512
 *
 * Purpose: Generate a software ECC for a 512 byte block of data
 *
 * Processing:
 *     See function.
 *
 * Parameters: TBD
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
UNS_32 lpc_eccgenerate512(LPC_ECC512 eccbuf,
                          UNS_8 *datbuf)
{
  /* Correct parity generation algorithm */
  INT_32 idx;
  UNS_8 P1O, P1E, P2O, P2E, P4O, P4E, P8O, P8E, P16O, P16E, P32O, P32E;
  UNS_8 P64O, P64E, P128O, P128E, P256O, P256E, P512O, P512E;
  UNS_8 P1024O, P1024E, P2048O, P2048E;

  /* All parity bits initially 0 */
  P1O = P1E = P2O = P2E = P4O = P4E = P8O = P8E = P16O = P16E = 0;
  P32O = P32E = P64O = P64E = P128O = P128E = P256O = P256E = 0;
  P512O = P512E = P1024O = P1024E = P2048O = P2048E = 0;

  for (idx = 0; idx < 512; idx++)
  {
    P1O = P1O ^ P1Otab[datbuf[idx]];
    P1E = P1E ^ P1Etab[datbuf[idx]];
    P2O = P2O ^ P2Otab[datbuf[idx]];
    P2E = P2E ^ P2Etab[datbuf[idx]];
    P4O = P4O ^ P4Otab[datbuf[idx]];
    P4E = P4E ^ P4Etab[datbuf[idx]];

    if ((idx & 0x1) == 0)
    {
      P8E = P8E ^ PBytetab[datbuf[idx]];
    }
    else
    {
      P8O = P8O ^ PBytetab[datbuf[idx]];
    }

    if ((idx & 0x2) == 0)
    {
      P16E = P16E ^ PBytetab[datbuf[idx]];
    }
    else
    {
      P16O = P16O ^ PBytetab[datbuf[idx]];
    }

    if ((idx & 0x4) == 0)
    {
      P32E = P32E ^ PBytetab[datbuf[idx]];
    }
    else
    {
      P32O = P32O ^ PBytetab[datbuf[idx]];
    }

    if ((idx & 0x8) == 0)
    {
      P64E = P64E ^ PBytetab[datbuf[idx]];
    }
    else
    {
      P64O = P64O ^ PBytetab[datbuf[idx]];
    }

    if ((idx & 0x10) == 0)
    {
      P128E = P128E ^ PBytetab[datbuf[idx]];
    }
    else
    {
      P128O = P128O ^ PBytetab[datbuf[idx]];
    }

    if ((idx & 0x20) == 0)
    {
      P256E = P256E ^ PBytetab[datbuf[idx]];
    }
    else
    {
      P256O = P256O ^ PBytetab[datbuf[idx]];
    }

    if ((idx & 0x40) == 0)
    {
      P512E = P512E ^ PBytetab[datbuf[idx]];
    }
    else
    {
      P512O = P512O ^ PBytetab[datbuf[idx]];
    }

    if ((idx & 0x80) == 0)
    {
      P1024E = P1024E ^ PBytetab[datbuf[idx]];
    }
    else
    {
      P1024O = P1024O ^ PBytetab[datbuf[idx]];
    }

    if ((idx & 0x100) == 0)
    {
      P2048E = P2048E ^ PBytetab[datbuf[idx]];
    }
    else
    {
      P2048O = P2048O ^ PBytetab[datbuf[idx]];
    }
  }

  *eccbuf = (P2048E << 11) | (P1024E << 10) | (P512E << 9) |
            (P256E << 8) | (P128E << 7) | (P64E << 6) | (P32E << 5) |
            (P16E << 4) | (P8E << 3) | (P4E << 2) | (P2E << 1) |
            (P1E << 0);
  eccbuf++;
  *eccbuf = (P2048O << 11) | (P1024O << 10) | (P512O << 9) |
            (P256O << 8) | (P128O << 7) | (P64O << 6) | (P32O << 5) |
            (P16O << 4) | (P8O << 3) | (P4O << 2) | (P2O << 1) |
            (P1O << 0);

  return 1;
}

/***********************************************************************
 *
 * Function: lpc_eccCheckAndCorrect
 *
 * Purpose: Check and correct ECC and data
 *
 * Processing:
 *     See function.
 *
 * Parameters: TBD
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
LPC_ECC_ERROR_T lpc_eccCheckAndCorrect(LPC_ECC512 eccgood,
                                       LPC_ECC512 eccerr,
                                       UNS_8 *buf)
{
  UNS_16 pe, po, errbit, erridx;
  UNS_32 m;
  LPC_ECC_ERROR_T eccerror;

  pe = *eccgood ^ *eccerr;
  po = *(eccgood + 1) ^ *(eccerr + 1);
  m = ((UNS_32) pe << 16) | (UNS_32) po;

  /* There are 4 possible types for results for ECC check. If
     the check returns 0, the computed and stored ECC values
     are the same and the data has no errors. If the checks
     returns 1, then the data is good and the ECC is incorrect
     and the data has no errors. If the check returns 12, then
     the ECC is good and the data is bad and the data needs
     error corrections. All other results of check are
     uncorrectable errors. */
  switch (lpc_eccGetBits(m))
  {
    case 0:
      eccerror = LPC_ECC_NOERR;
      break;

    case 1:
      /* ECC area error, data is valid */
      eccerror = LPC_ECC_AREA_ERR;
      break;

    case 12:
      /* ECC is good, data area has an error, so correct */
      /* Get bit position of error */
      errbit = (po & 0x7);
      erridx = ((po >> 3) & 0x1FF);

      buf[erridx] = buf[erridx] ^(0x1 << errbit);
      eccerror = LPC_ECC_CORRECTED;
      break;

    default:
      /* All other cases are not correctable */
      eccerror = LPC_ECC_NOTCORRECTABLE;
      break;
  }

  return eccerror;
}

/* region-checksum.h

   written by Marc Singer
   22 Aug 2008

   Copyright (C) 2008 Marc Singer

   -----------
   DESCRIPTION
   -----------

*/

#if !defined (__REGION_CHECKSUM_H__)
#    define   __REGION_CHECKSUM_H__

/* ----- Includes */

/* ----- Types */

/* ----- Globals */

/* ----- Prototypes */

enum {
  regionChecksumSpinner	= (1<<0),
  regionChecksumLength	= (1<<1), /* Add non-zero length bytes, LSB first */
  regionChecksumLSB     = (1<<2), /* UBOOT compatible CRC algorithm */
};

int region_checksum (size_t cbCheck, struct descriptor_d* din, unsigned flags,
                     unsigned long* crc_result);


#endif  /* __REGION_CHECKSUM_H__ */

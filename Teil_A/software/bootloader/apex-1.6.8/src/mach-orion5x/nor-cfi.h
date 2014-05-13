/* nor-cfi.h

   written by Marc Singer
   2 Nov 2008

   Copyright (C) 2008 Marc Singer

   -----------
   DESCRIPTION
   -----------

   o NOR_BUS_WIDTH.  For some reason as yet unidentified, the NOR boot
     flash on the Orion based NAS boxes is configured with an
     eight-bit data bus.  The part has 16 bit word size, so the query
     strings will represent as 'Q \0 'R' \0 'Y' instead of three
     adjacent ASCII characters.  The CFI driver handles this when it
     is told that the NOR_BUS_WIDTH is 8.

fff80000: ffff 0000 0000 ffff 0000 0000 0000 0000    ................
fff80010: ffff ffff ffff 0000 0000 ffff 0000 0000    ................
fff80020: 0051 0052 0059 0002 0000 0040 0000 0000    Q.R.Y.....@.....
fff80030: 0000 0000 0000 0027 0036 0000 0000 0004    ......'.6.......
fff80040: 0000 000a 0000 0005 0000 0004 0000 0013    ................
fff80050: 0002 0000 0000 0000 0004 0000 0000 0040    ..............@.
fff80060: 0000 0001 0000 0020 0000 0000 0000 c080    ...... .........
fff80070: 0000 0006 0000 0000 0001 ffff ffff ffff    ................
fff80080: 0050 0052 0049 0031 0030 0000 0002 0001    P.R.I.1.0.......
fff80090: 0001 0004 0000 0000 0000 ffff ffff ffff    ................
fff800a0: 0000 ffff ffff 0000 0000 0000 ffff ffff    ................
fff800b0: ffff ffff ffff ffff ffff ffff ffff ffff    ................
fff800c0: ffff ffff 0000 0000 ffff ffff 0000 ffff    ................
fff800d0: 0000 0000 ffff ffff ffff ffff 0000 ffff    ................
fff800e0: ffff ffff ffff 0000 ffff ffff ffff ffff    ................
fff800f0: ffff ffff 0055 ffff 0055 ffff ffff 0000    ....U...U.......
fff80100: ffff 0000 0000 ffff 0000 0000 0000 0000    ................
fff80110: ffff ffff ffff 0000 0000 ffff 0000 0000    ................

*/

#if !defined (__NOR_CFI_H__)
#    define   __NOR_CFI_H__

/* ----- Includes */

#include <config.h>
#include <mach/hardware.h>

/* ----- Constants */

#if !defined (NOR_WIDTH)
# define NOR_WIDTH		(16)
#endif
#if !defined (NOR_BUS_WIDTH)
# define NOR_BUS_WIDTH		(8)
#endif
#if !defined (NOR_0_PHYS)
# define NOR_0_PHYS		(0xfff80000)
#endif

#endif  /* __NOR_CFI_H__ */

/* drv-emac.c

   written by Marc Singer
   16 Nov 2004

   Copyright (C) 2004 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   Driver for the embedded lh79524 Ethernet MAC and Altima AC101L PHY,
   if present.


   MAC EEPROM
   ----------

   Let me propose a format for the eeprom data.

   Byte     0      1      2    2+n-1   2+n
	+------+------+------+------+------+
	| 0x94 |  OP  |    DATA[n]  |  OP  | ...
	+------+------+------+------+------+

   The first byte is 0x94 representing 79524, 9 for the series, and 4
   for the model.  Following this is a list of fields, each starting
   with a byte describing the field and a sequence of data bytes
   determined by the type.

     OP     Length (n)    Description
    ----    ----------    -----------

    0x01       0x6        Primary MAC address in network byte order
    0x08       0x1        Speed negotiation:  0, auto; 1, 10Mbps; 2, 100Mpbs
    0x09       0x1        Duplex negotiation: 0, auto; 1, half;   2, full
    0xff                  End of fields

   I recommend that we standardize on emitting the OPs in ascending
   order.  It shouldn't matter, but it does make it easy for the parser
   code to detect and read the MAC address.

   Only the MAC address should be required.  All other parameters
   default to AUTO when there is no OP.  This means that a minimal
   configuration for a default MAC address of 01:23:45:67:89:ab would
   look like this.

      +------+------+------+------+------+------+------+------+------+
      | 0x94 | 0x01 | 0x01 | 0x23 | 0x45 | 0x67 | 0x89 | 0xab | 0xff |
      +------+------+------+------+------+------+------+------+------+


   Setup Sequence
   --------------

   0) CPLD clear PHY power-down
   1) IOCON muxctl1, muxctl23, muxctl24 (to 1's)
   2) RCPC ahbclkctrl
   3) EMAC specaddr1_bottom, specaddr1_top
   4) EMAC rxqptr, rxstatus, buffer initializatoin
   5) EMAC txqptr, txstatus, (buffer initialization not really necessary)
   6) EMAC netconfig
   7) EMAC netctrl
   8) Read EMAC irqstatus for LINK_INTR


  PHY Mamagement
  --------------

  It is possible to poll for the PHY management operation to complete.
  There is an interrupt bit as well as a status bit.

  Auto MDI/MDI-X
  --------------

  The altima PHY supports automatic detection of the MDI/MDI-X mode of
  the twisted-pair.  According to Broadcom, this feature requires
  appropriate magnetics.  As of the Rev J SDK board, LPD doesn't
  provide the proper circuit to automatically detect the TP mode.
  This, the feature is disabled.

  CONFIG_CMD_EMAC_LH79524
  -----------------------

  It is possible to disable the EMAC commands altogether.  If this is
  done, the MAC address cannot be set from the APEX command line nor
  can the MAC address be written to EEPROM.  However, the device will
  still be initialized and the MAC address read from EEPROM if it is
  already there.

  EMAC_NETCONFIG_CPYFRM & Promiscuous Mode
  ----------------------------------------

  The receive code is written to discard frames that don't have either
  the broadcast or the specific address bits set.  By default, the
  EMAC has the CPYFRM bit set which means that all frames will be
  received.  It should all work OK, except for the fact that the CPU
  isn't fast enough to receive all frames on the wire on an unswitched
  segment.  So, what happens is the EMAC fails to receive anything
  interesting, the buffer fill, and it spends most of it's time
  discarding uninteresting frames.

  The good thing is that once CPYFRM is cleared, the MAC appears to
  work OK on the unswitched link.  TFTP works as does RARP.

  One side effect of disabling the CPYFRM bit is that there seems to
  be an unresolved error in the receiver code where the receiver can
  lock up.

*/


#include <apex.h>
#include <config.h>
#include <driver.h>
#include <service.h>
#include <command.h>
#include <error.h>
#include <linux/string.h>
#include "hardware.h"
#include <linux/kernel.h>
#include <asm/mmu.h>

#define USE_DISABLE_AUTOMDI_MDIX /* Disable auto-mdi/mdix detection */
#define USE_PHY_RESET		/* Reset the PHY after detection  */
//#define USE_DISABLE_100MB

#if defined CONFIG_CMD_EMAC_LH79524_DIAG
# define USE_DIAG		/* Enables diagnostic code */
# define TALK 0
#endif
//#define TALK 1

#if defined (TALK)
#define PRINTF(f...)		printf (f)
#else
#define PRINTF(f...)		do {} while (0)
#define TALK 0
#endif

#if TALK > 2
#define PRINTF3(f...)		printf (f)
#else
#define PRINTF3(f...)		do {} while (0)
#endif

#if TALK > 0
# define DBG(l,f...)		if (l <= TALK) printf (f);
#else
# define DBG(l,f...)		do {} while (0)
#endif

#if TALK > 0
# define PRINT_PKT(p,l)		dump ((void*) p, l, 0)
#else
# define PRINT_PKT(p,l)		do {} while (0)
#endif

#define PHY_CONTROL	0
#define PHY_STATUS	1
#define PHY_ID1		2
#define PHY_ID2		3

#define PHY_CONTROL_RESET		(1<<15)
#define PHY_CONTROL_LOOPBACK		(1<<14)
#define PHY_CONTROL_POWERDOWN		(1<<11)
#define PHY_CONTROL_ANEN_ENABLE		(1<<12)
#define PHY_CONTROL_RESTART_ANEN	(1<<9)
#define PHY_STATUS_ANEN_COMPLETE	(1<<5)
#define PHY_STATUS_LINK			(1<<2)
#define PHY_STATUS_100FULL		(1<<14)
#define PHY_STATUS_100HALF		(1<<13)
#define PHY_STATUS_10FULL		(1<<12)
#define PHY_STATUS_10HALF		(1<<11)

static int phy_address;
static unsigned long phy_id;	/* ID read from PHY */


#define  C_TX_BUFFER	2
#define CB_TX_BUFFER	1536
#define  C_RX_BUFFER	24
#define CB_RX_BUFFER	128

#define RX_QUEUE_LENGTH	 C_RX_BUFFER
#define RX_BUFFER_SIZE	 CB_RX_BUFFER
#define TX_QUEUE_LENGTH	 C_TX_BUFFER
#define TX_BUFFER_SIZE	 CB_TX_BUFFER

static long* rgl_rx_descriptor;
static long* rgl_tx_descriptor;
static char* rgbRxBuffer;
static char* rgbTxBuffer;

static int head_rx;		/* Index of next receive buffer */
static int head_tx;
static int tail_tx;		/* Index of next available transmit buffer */

#define RX0()		rgl_rx_descriptor[head_rx*2]
#define RX1()		rgl_rx_descriptor[head_rx*2 + 1]
#define RX0_(i)		rgl_rx_descriptor[((head_rx\
					      + i + RX_QUEUE_LENGTH)\
					      %RX_QUEUE_LENGTH)*2]
#define RX1_(i)		rgl_rx_descriptor[((head_rx\
					      + i + RX_QUEUE_LENGTH)\
					      %RX_QUEUE_LENGTH)*2 + 1]
#define RX0__(i)	rgl_rx_descriptor[((i)%RX_QUEUE_LENGTH)*2]
#define RX1__(i)	rgl_rx_descriptor[((i)%RX_QUEUE_LENGTH)*2 + 1]

#define TXH0()		rgl_tx_descriptor[head_tx*2]
#define TXH1()		rgl_tx_descriptor[head_tx*2 + 1]
#define TXT0()		rgl_tx_descriptor[tail_tx*2]
#define TXT1()		rgl_tx_descriptor[tail_tx*2 + 1]

#define TX1_USED	(1<<31)
#define TX1_WRAP	(1<<30)
#define TX1_RETRYLIMIT	(1<<29)
#define TX1_UNDERRUN	(1<<28)
#define TX1_BUFEX	(1<<27)
#define TX1_LAST	(1<<15)

#define RX0_USED	(1<<0)
#define RX0_WRAP	(1<<1)
#define RX1_START	(1<<14)
#define RX1_END		(1<<15)
#define RX1_BROADCAST	(1<<31)
#define RX1_SPEC1	(1<<26)

#if defined (CONFIG_ETHERNET)
extern char host_mac_address[];
#else
static char host_mac_address[6];
#endif

#if defined (USE_DIAG)

struct ethernet_header {
  unsigned char dest[6];
  unsigned char src[6];
  unsigned short protocol;
};

#if 0
  /* This should be (adequately) defined in a header. */
static void msleep (int ms)
{
  unsigned long time = timer_read ();

  do {
  } while (timer_delta (time, timer_read ()) < ms);
}
#endif

#endif

static int emac_phy_read (int phy_address, int phy_register)
{
  unsigned short result;

  //  PRINTF3 ("emac_phy_read %d %d\n", phy_address, phy_register);

  EMAC_NETCTL |= EMAC_NETCTL_MANGEEN;
  EMAC_PHYMAINT
    = (1<<30)|(2<<28)
    | ((phy_address  & 0x1f)<<23)
    | ((phy_register & 0x1f)<<18)
    | (2<<16);

  while ((EMAC_NETSTATUS & (1<<2)) == 0)
    ;

  result = EMAC_PHYMAINT & 0xffff;

  EMAC_NETCTL &= ~EMAC_NETCTL_MANGEEN;

  //  PRINTF3 ("emac_phy_read => 0x%x\n", result);

  return result;
}

static void emac_phy_write (int phy_address, int phy_register, int phy_data)
{
  EMAC_NETCTL |= EMAC_NETCTL_MANGEEN;
  EMAC_PHYMAINT
    = (1<<30)|(1<<28)
    |((phy_address & 0x1f)<<23)
    |((phy_register & 0x1f)<<18)
    |(2<<16)
    |(phy_data & 0xffff);

  while ((EMAC_NETSTATUS & (1<<2)) == 0)
    ;

  EMAC_NETCTL &= ~EMAC_NETCTL_MANGEEN;
}

#if defined (USE_PHY_RESET)
static void emac_phy_reset (int phy_address)
{

  PRINTF ("emac: phy_reset\n");
  emac_phy_write (phy_address, 0,
		  PHY_CONTROL_RESET
		  | emac_phy_read (phy_address, 0));
  while (emac_phy_read (phy_address, 0) & PHY_CONTROL_RESET)
    ;
}
#else
# define emac_phy_reset(v) do { } while (0)
#endif

static void emac_phy_configure (int phy_address)
{
  PRINTF ("emac: phy_configure\n");

#if defined USE_DISABLE_AUTOMDI_MDIX
  if (phy_id == 0x00225521){
    unsigned long l = emac_phy_read (phy_address, 23);
    emac_phy_write (phy_address, 23,
		    (l & ~((1<<7)|(1<<6)))|(1<<7)); /* Force MDI. */
  }
#endif

#if defined USE_DISABLE_100MB
  {
    unsigned long l = emac_phy_read (phy_address, 0);
    emac_phy_write (phy_address, 0, l & ~(1<<13));
  }
#endif
}


/* emac_phy_detect

   scans for a valid PHY attached to the EMAC.

*/

static void emac_phy_detect (void)
{
				/* Scan PHYs, #1 first, #0 last */
  for (phy_address = 1; phy_address < 33; ++phy_address) {
    unsigned int id1;
    unsigned int id2;
    id1 = emac_phy_read (phy_address & 0x1f, PHY_ID1);
    id2 = emac_phy_read (phy_address & 0x1f, PHY_ID2);
//    PRINTF ("%s: %d %d %d\n", __FUNCTION__, phy_address, id1, id2);
    if (   id1 != 0x0000 && id1 != 0xffff && id1 != 0x8000
	&& id2 != 0x0000 && id2 != 0xffff && id2 != 0x8000) {
      phy_id = (id1 << 16) | id2;
      phy_address &= 0x1f;
      PRINTF ("emac: phy_detect 0x%x  0x%lx\n", phy_address, phy_id);
      break;
    }
  }

  emac_phy_reset (phy_address);
  emac_phy_configure (phy_address);
}


/* emac_read_mac

   pulls a MAC address from the MAC EEPROM if there is one.  The
   format of this EEPROM data is described in the comment above.

*/

static int emac_read_mac (char rgbResult[6])
{
  struct descriptor_d d;
  char rgb[8];
  int result = 0;

  if (   parse_descriptor ("mac:0#8", &d)
      || open_descriptor (&d)) {
    printf ("ethernet mac eeprom unavailable\n");
    return -1;			/* No driver */
  }
  if (d.driver->read (&d, rgb, 8) == 8
      && rgb[0] == 0x94
      && rgb[1] == 0x01)
    memcpy (rgbResult, rgb + 2, 6);
  else
    result = -1;

  close_descriptor (&d);

  return result;
}


/* emac_setup

   performs the high-level configuration of the Ethernet interface,
   buffer management and preparation for IO.

*/

static void emac_setup (void)
{
  int i;

  for (i = 0; i < C_RX_BUFFER; ++i) {
    rgl_rx_descriptor[i*2]    = ((unsigned long)(rgbRxBuffer + i*CB_RX_BUFFER)
			      & ~3)
      | ((i == C_RX_BUFFER - 1) ? (1<<1) : 0);
    rgl_rx_descriptor[i*2 + 1] = 0;
  }

  for (i = 0; i < C_TX_BUFFER; ++i) {
    rgl_tx_descriptor[i*2]     = (unsigned long)(rgbTxBuffer + i*CB_TX_BUFFER);
    rgl_tx_descriptor[i*2 + 1] = (1<<31)|(1<<15)
      | ((i == C_TX_BUFFER - 1) ? (1<<30) : 0);
  }

  PRINTF ("emac: setup rgl_rx_descriptor %p\n", rgl_rx_descriptor);

  EMAC_RXBQP = (unsigned long) rgl_rx_descriptor;
  EMAC_RXSTATUS &=
    ~(  EMAC_RXSTATUS_RXCOVERRUN | EMAC_RXSTATUS_FRMREC
      | EMAC_RXSTATUS_BUFNOTAVAIL);

  PRINTF ("emac: setup rgl_tx_descriptor %p\n", rgl_tx_descriptor);
  EMAC_TXBQP = (unsigned long) rgl_tx_descriptor;
  EMAC_TXSTATUS
    = EMAC_TXSTATUS_TXUNDER | EMAC_TXSTATUS_TXCOMPLETE | EMAC_TXSTATUS_BUFEX;

  MASK_AND_SET (EMAC_NETCONFIG, 3<<10, EMAC_NETCONFIG_DIV32);
  EMAC_NETCONFIG |= 0
    //    | EMAC_NETCONFIG_FULLDUPLEX
    //    | EMAC_NETCONFIG_100MB
//    | EMAC_NETCONFIG_CPYFRM
    | EMAC_NETCONFIG_DISCARDFCS
    | EMAC_NETCONFIG_RECBYTE
    | EMAC_NETCONFIG_LENGTHCHK
    ;
  EMAC_NETCONFIG &= ~(EMAC_NETCONFIG_CPYFRM);

//  printf ("netconfig %x\n", EMAC_NETCONFIG);
//  EMAC_NETCONFIG |= EMAC_NETCONFIG_RECBYTE;
  EMAC_NETCTL |= 0
    | EMAC_NETCTL_RXEN
    | EMAC_NETCTL_TXEN
    | EMAC_NETCTL_CLRSTAT
    ;
}

void emac_init (void)
{
  PRINTF ("emac: init\n");

	/* Allocate buffers */
  rgl_rx_descriptor = alloc_uncached (2*C_RX_BUFFER*sizeof (long), 16);
  rgl_tx_descriptor = alloc_uncached (2*C_TX_BUFFER*sizeof (long), 16);
  rgbRxBuffer	    = alloc_uncached (CB_RX_BUFFER*C_RX_BUFFER, 128);
  rgbTxBuffer	    = alloc_uncached (CB_TX_BUFFER*C_RX_BUFFER, 128);

	/* Hardware setup -- necessary for the PHY to be detected. */
  MASK_AND_SET (IOCON_MUXCTL1,
		(3<<8)|(3<<6)|(3<<4),
		(1<<8)|(1<<6)|(1<<4));
  MASK_AND_SET (IOCON_RESCTL1,
		(3<<8)|(3<<6)|(3<<4),
		(1<<8)|(1<<6)|(1<<4));
  MASK_AND_SET (IOCON_MUXCTL23,
		(3<<14)|(3<<12)|(3<<10)|(3<<8)|(3<<6)|(3<<4)|(3<<2)|(3<<0),
		(1<<14)|(1<<12)|(1<<10)|(1<<8)|(1<<6)|(1<<4)|(1<<2)|(1<<0));
  MASK_AND_SET (IOCON_RESCTL23,
		(3<<14)|(3<<12)|(3<<10)|(3<<8)|(3<<6)|(3<<4)|(3<<2)|(3<<0),
		(1<<14)|(1<<12)|(1<<10)|(1<<8)|(1<<6)|(1<<4)|(1<<2)|(1<<0));
  MASK_AND_SET (IOCON_MUXCTL24,
		(3<<12)|(3<<10)|(3<<8)|(3<<6)|(3<<4)|(3<<2)|(3<<0),
		(1<<12)|(1<<10)|(1<<8)|(1<<6)|(1<<4)|(1<<2)|(1<<0));
  MASK_AND_SET (IOCON_RESCTL24,
		(3<<12)|(3<<10)|(3<<8)|(3<<6)|(3<<4)|(3<<2)|(3<<0),
		(1<<12)|(1<<10)|(1<<8)|(1<<6)|(1<<4)|(1<<2)|(1<<0));

//  PRINTF ("CPLD_CONTROL %x => %x\n", CPLD_CONTROL,
//	  CPLD_CONTROL | CPLD_CONTROL_WRLAN_ENABLE);

  RCPC_CTRL       |=  RCPC_CTRL_UNLOCK;
  RCPC_AHBCLKCTRL &= ~(1<<2);
  RCPC_CTRL	  &= ~RCPC_CTRL_UNLOCK;

  if (!emac_read_mac (host_mac_address)) {
    EMAC_SPECAD1BOT
      = (host_mac_address[3]<<24)
      | (host_mac_address[2]<<16)
      | (host_mac_address[1]<<8)
      | (host_mac_address[0]<<0);
    EMAC_SPECAD1TOP
      = (host_mac_address[5]<<8)
      | (host_mac_address[4]<<0);
#if defined (TALK)
    PRINTF ("emac: mac address\n");
    dump (host_mac_address, 6, 0);
#endif
  }

#if defined (CPLD_CONTROL_WRLAN_ENABLE)
  CPLD_CONTROL |= CPLD_CONTROL_WRLAN_ENABLE;
#endif

  emac_phy_detect ();		/* Detect and perform special setups */

  emac_setup ();		/* Prepare EMAC for IO */
}

void static emac_release (void)
{
  /* We *must* disable the Ethernet MAC before passing control the the
     next piece or software or we may be vulnerable to spurrious DMA
     transfers on the part of the EMAC.  The kernel driver will enable
     the clock and it will set the EMAC control register when it
     initializes the device.  */

  EMAC_NETCTL = 0;
  EMAC_RXBQP = 0;
  EMAC_TXBQP = 0;

  RCPC_CTRL       |=  RCPC_CTRL_UNLOCK;
  RCPC_AHBCLKCTRL |=  (1<<2);
  RCPC_CTRL	  &= ~RCPC_CTRL_UNLOCK;

#if defined (CPLD_CONTROL_WRLAN_ENABLE)
  CPLD_CONTROL &= ~CPLD_CONTROL_WRLAN_ENABLE;
#endif
}

#if !defined (CONFIG_SMALL)
static void emac_report (void)
{
  if (phy_id) {
    unsigned long bot = EMAC_SPECAD1BOT;
    unsigned long top = EMAC_SPECAD1TOP;

    printf ("  emac:   phy_addr %d  phy_id 0x%lx"
	    "  mac_addr %02x:%02x:%02x:%02x:%02x:%02x\n",
	    phy_address, phy_id,
	    (unsigned char) (bot >>  0),
	    (unsigned char) (bot >>  8),
	    (unsigned char) (bot >> 16),
	    (unsigned char) (bot >> 24),
	    (unsigned char) (top >>  0),
	    (unsigned char) (top >>  8)
	    );
  }
}
#endif

#if defined (USE_DIAG)

static void show_tx_flags (unsigned long l)
{
  if (l & (1<<31))
    printf (" used");
  if (l & (1<<30))
    printf (" wrap");
  if (l & (1<<29))
    printf (" retries");
  if (l & (1<<28))
    printf (" under");
  if (l & (1<<27))
    printf (" exhaust");
}

static void show_rx_flags (unsigned long l0, unsigned long l1)
{
  if (l0 & RX0_USED)
    printf (" used");
  if (l0 & RX0_WRAP)
    printf (" wrap");
  if (l1 & RX1_BROADCAST)
    printf (" bcast");
  if (l1 & RX1_SPEC1)
    printf (" spec1");
  if (l1 & RX1_START)
    printf (" start");
  if (l1 & RX1_END)
    printf (" end");
}
#endif


#if defined (CONFIG_CMD_EMAC_LH79524)

static int cmd_emac (int argc, const char** argv)
{
  int result = 0;

  if (argc == 1) {
#if defined (USE_DIAG)
#if defined (TALK)
    {
      unsigned i;
      struct regs {
	int reg;
	char label[5];
      };
      static struct regs __rodata rgRegs[] = {
	{  0, "ctrl" },
	{  1, "stat" },
	{  2, "id1" },
	{  3, "id2" },
	{  4, "anadv" },
	{  5, "anpar" },
	{  6, "anexp" },
	{  7, "anpag" },

#if 1
	/* Altima */
	{ 16, "btic" },
	{ 17, "intr" },
	{ 18, "diag" },
	{ 19, "loop" },
	{ 20, "cable" },
	{ 21, "rxer" },
	{ 22, "power" },
	{ 23, "oper" },
	{ 24, "rxcrc" },
	{ 28|(1<<8)|(0<<12), "mode" },
	{ 29|(1<<8)|(0<<12), "test" },
	{ 29|(1<<8)|(1<<12), "blnk" },
	{ 30|(1<<8)|(1<<12), "l0s1" },
	{ 31|(1<<8)|(1<<12), "l0s2" },
	{ 29|(1<<8)|(2<<12), "l1s1" },
	{ 30|(1<<8)|(2<<12), "l1s2" },
	{ 31|(1<<8)|(2<<12), "l2s1" },
	{ 29|(1<<8)|(3<<12), "l2s2" },
	{ 30|(1<<8)|(3<<12), "l3s1" },
	{ 31|(1<<8)|(3<<12), "l3s2" },
#else
	/* NatSemi */
	{ 16, "phsts" },
	{ 20, "fcscr" },
	{ 21, "recr" },
	{ 22, "pcsr" },
	{ 25, "phctl" },
	{ 26, "100sr" },
	{ 27, "cdtst" },
#endif
      };
#if 1
#define WIDE 5
#define ROWS (sizeof (rgRegs)/sizeof (rgRegs[0]) + WIDE - 1)/WIDE
#define COUNT ROWS*WIDE
      for (i = 0; i < COUNT; ++i) {
	int index = (i%WIDE)*ROWS + i/WIDE;
	if (index >= sizeof (rgRegs)/sizeof (rgRegs[0]))
	  continue;
	if (i && (i % WIDE) == 0)
	  printf ("\n");
	if (rgRegs[index].reg & (1<<8))
	  emac_phy_write (phy_address, 28,
			  (emac_phy_read (phy_address, 28) & 0x0fff)
			  | (rgRegs[index].reg & 0xf000));
	printf ("%5s-%-2d %04x  ",
		rgRegs[index].label, rgRegs[index].reg & 0xff,
		emac_phy_read (phy_address, rgRegs[index].reg & 0xff));
      }
#else
      for (i = 0; i < 2*(((sizeof (rgRegs)/sizeof (rgRegs[0])) + 0x7)&~7);
	   ++i) {
	unsigned index = (i & 7) | ((i >> 1) & ~7);
	if (index >= sizeof (rgRegs)/sizeof (rgRegs[0]))
	  continue;
	//	printf ("i %d (%x) index %d (%x)\n", i, i, index, index);
	if (i & (1<<3)) {
	  if (i && (index & 0x7) == 0)
	    printf ("\n");
	  if (rgRegs[index].reg & (1<<8))
	    emac_phy_write (phy_address, 28,
			    (emac_phy_read (phy_address, 28) & 0x0fff)
			    | (rgRegs[index].reg & 0xf000));
	  printf ("%04x    ", emac_phy_read (phy_address,
					     rgRegs[index].reg & 0xff));
	}
	else {
	  if (i && (index & 0x7) == 0)
	    printf ("\n\n");
	  printf ("%s-%-2d ", rgRegs[index].label, rgRegs[index].reg & 0xff);
	}
      }
#endif

      printf ("\n");
    }
    {
      unsigned long l;
      int i;
      l = emac_phy_read (phy_address, 0);
      PRINTF ("phy_control 0x%lx\n", l);
      l = emac_phy_read (phy_address, 1);
      PRINTF ("phy_status 0x%lx", l);
      if (l&PHY_STATUS_ANEN_COMPLETE)
	PRINTF (" anen_complete");
      if (l&PHY_STATUS_LINK)
	PRINTF (" link");
      if (l&PHY_STATUS_100FULL)
	PRINTF (" cap100F");
      if (l&PHY_STATUS_100HALF)
	PRINTF (" cap100H");
      if (l&PHY_STATUS_10FULL)
	PRINTF (" cap10F");
      if (l&PHY_STATUS_10HALF)
	PRINTF (" cap10H");
      printf ("\n");
      //      l = emac_phy_read (phy_address, 4);
      //      PRINTF ("phy_advertisement 0x%lx\n", l);
      l = emac_phy_read (phy_address, 5);
      PRINTF ("phy_partner 0x%lx", l);
      if (l & (1<<9))
	PRINTF ("  100Base-T4");
      if (l & (1<<8))
	PRINTF ("  100Base-TX-full");
      if (l & (1<<7))
	PRINTF ("  100Base-TX");
      if (l & (1<<6))
	PRINTF ("  10Base-T-full");
      if (l & (1<<5))
	PRINTF ("  10Base-T");
      PRINTF ("\n");
      l = emac_phy_read (phy_address, 6);
      PRINTF ("phy_autoneg_expansion 0x%lx", l);
      if (l & (1<<4))
	PRINTF (" parallel-fault");
      if (l & (1<<1))
	PRINTF (" page-received");
      if (l & (1<<0))
	PRINTF (" partner-ANEN-able");
      PRINTF ("\n");
      //      l = emac_phy_read (phy_address, 7);
      //      PRINTF ("phy_autoneg_nextpage 0x%lx\n", l);
      //      l = emac_phy_read (phy_address, 16);
      //      PRINTF ("phy_bt_interrupt_level 0x%lx\n", l);
      l = emac_phy_read (phy_address, 17);
      PRINTF ("phy_interrupt_control 0x%lx", l);
      if (l & (1<<4))
	PRINTF (" parallel_fault");
      if (l & (1<<2))
	PRINTF (" link_not_ok");
      if (l & (1<<0))
	PRINTF (" anen_complete");
      PRINTF ("\n");
      l = emac_phy_read (phy_address, 18);
      PRINTF ("phy_diagnostic 0x%lx", l);
      if (l & (1<<12))
	PRINTF (" force 100TX");
      if (l & (1<<13))
	PRINTF (" force 10BT");
      PRINTF ("\n");
      //      l = emac_phy_read (phy_address, 19);
      //      PRINTF ("phy_power_loopback 0x%lx\n", l);
      //      l = emac_phy_read (phy_address, 20);
      //      PRINTF ("phy_cable 0x%lx\n", l);
      //      l = emac_phy_read (phy_address, 21);
      //      PRINTF ("phy_rxerr 0x%lx\n", l);
      //      l = emac_phy_read (phy_address, 22);
      //      PRINTF ("phy_power_mgmt 0x%lx\n", l);
      //      l = emac_phy_read (phy_address, 23);
      //      PRINTF ("phy_op_mode 0x%lx\n", l);
      //      l = emac_phy_read (phy_address, 24);
      //      PRINTF ("phy_last_crc 0x%lx\n", l);

      PRINTF ("emac_netctl 0x%lx\n", EMAC_NETCTL);
      PRINTF ("emac_netconfig 0x%lx\n", EMAC_NETCONFIG);
//      PRINTF ("emac_netstatus 0x%lx\n", EMAC_NETSTATUS);
      PRINTF ("emac_txstatus 0x%lx\n", EMAC_TXSTATUS);
      PRINTF ("emac_rxstatus 0x%lx\n", EMAC_RXSTATUS);
      PRINTF ("emac_txbqp 0x%lx\n", EMAC_TXBQP);
      PRINTF ("emac_rxbqp 0x%lx\n", EMAC_RXBQP);

      for (i = 0; i < TX_QUEUE_LENGTH; ++i) {
	printf ("emac:tx%02d & %p (0x%08lx 0x%08lx)",
		i, &rgl_tx_descriptor[i*2],
		rgl_tx_descriptor[i*2 + 0],
		rgl_tx_descriptor[i*2 + 1]);
	show_tx_flags (rgl_tx_descriptor[i*2 + 1]);
	if (i == head_tx)
	  printf (" HEAD");
	if (i == tail_tx)
	  printf (" TAIL");
	if (i == ((void*)EMAC_TXBQP - (void*) rgl_tx_descriptor)/8)
	  printf  (" *");
	printf ("\n");
      }
      for (i = 0; i < RX_QUEUE_LENGTH; ++i) {
	printf ("emac:rx%02d & %p (0x%08lx 0x%08lx)",
		i, &rgl_rx_descriptor[i*2],
		rgl_rx_descriptor[i*2 + 0], rgl_rx_descriptor[i*2 + 1]);
	show_rx_flags (rgl_rx_descriptor[i*2 + 0], rgl_rx_descriptor[i*2 + 1]);
	if (i == head_rx)
	  printf (" HEAD");
	if (i == ((void*)EMAC_RXBQP - (void*) rgl_rx_descriptor)/8)
	  printf  (" *");
	printf ("\n");
      }
    }
#endif
#endif
  }
  else {
	/* Set mac address */
    if (strcmp (argv[1], "mac") == 0) {
      char rgb[6];
      if (argc != 3)
	return ERROR_PARAM;

      {
	int i;
	char* pb = (char*) argv[2];
	for (i = 0; i < 6; ++i) {
	  if (!*pb)
	    ERROR_RETURN (ERROR_PARAM, "mac address too short");;
	  rgb[i] = simple_strtoul (pb, &pb, 16);
	  if (*pb)
	    ++pb;
	}
	if (*pb)
	  ERROR_RETURN (ERROR_PARAM, "mac address too long");;
      }

      memcpy (host_mac_address, rgb, 6); /* For networking layer */
      EMAC_SPECAD1BOT
	= (rgb[3]<<24)|(rgb[2]<<16)|(rgb[1]<<8)|(rgb[0]<<0);
      EMAC_SPECAD1TOP
	= (rgb[5]<<8)|(rgb[4]<<0);
      printf ("emac: mac address\n");
      dump (rgb, 6, 0);
    }

	/* Save mac address */
    if (strcmp (argv[1], "save") == 0) {
      unsigned char rgb[9];
      unsigned long bot = EMAC_SPECAD1BOT;
      unsigned long top = EMAC_SPECAD1TOP;

      rgb[0] = 0x94;		/* Signature */
      rgb[1] = 0x01;		/* OP: MAC address */
      rgb[8] = 0xff;		/* OP: end */

      rgb[2] = bot >>  0;
      rgb[3] = bot >>  8;
      rgb[4] = bot >> 16;
      rgb[5] = bot >> 24;
      rgb[6] = top >>  0;
      rgb[7] = top >>  8;

      {
	struct descriptor_d d;
	static const char sz[] = "mac:0#9";
	if (   !parse_descriptor (sz, &d)
	    && !open_descriptor (&d)) {
	  d.driver->erase (&d, 9);
	  d.driver->seek (&d, 0, SEEK_SET);
	  if (d.driver->write (&d, rgb, 9) != 9)
	    result = ERROR_RESULT (ERROR_FAILURE,
				   "unable to write mac address");;
	  close_descriptor (&d);
	}
	else
	  ERROR_RETURN (ERROR_NODRIVER,
			"mac eeprom driver unavailable");;
      }
    }
  }

  return result;
}

	/* Work-around for gcc-2.95 */
#if defined (USE_DIAG)
# define _USE_DIAG(s) s
#else
# define _USE_DIAG(s)
#endif

static __command struct command_d c_emac = {
  .command = "emac",
  .func = cmd_emac,
  COMMAND_DESCRIPTION ("manage ethernet MAC address")
  COMMAND_HELP(
"emac [SUBCOMMAND [PARAMETER]]\n"
"  Commands for the Ethernet MAC and PHY devices.\n"
_USE_DIAG(
"  Without a SUBCOMMAND, it displays diagnostics about the EMAC\n"
"    and PHY devices.  This information is for debugging the hardware.\n"
"  clear - reset the EMAC.\n"
"  anen  - restart auto negotiation.\n"
"  send  - send a test packet.\n"
"  loop  - enable loopback mode.\n"
"  force - force power-up and restart auto-negotiation.\n"
)
"  mac   - set the MAC address to PARAMETER.\n"
"    PARAMETER has the form XX:XX:XX:XX:XX:XX where each X is a\n"
"    hexadecimal digit.  Be aware that MAC addresses must be unique for\n"
"    proper operation of the network.  This command may be added to the\n"
"    startup commands to set the MAC address at boot-time.\n"
"  save  - saves the MAC address to the mac: EEPROM device.\n"
"    A saved MAC address will be used to automatically configure the MAC\n"
"    at startup.  For this feature to work, there must be a mac: driver.\n"
"  e.g.  emac mac 01:23:45:67:89:ab         # Never use this MAC address\n"
"        emac save\n"
  )
};

#endif				/* CONFIG_CMD_EMAC_LH79524 */


static int eth_open (struct descriptor_d* d)
{
  return 0;			/* No problems */
}


static void eth_clean_rx_queue (void)
{
  int i;
  //  int c = 0;
  EMAC_NETCTL &= ~EMAC_NETCTL_RXEN;

  i = ((void*) EMAC_RXBQP - (void*) rgl_rx_descriptor)/8;

  //  printf ("!");

#if 0
  printf ("\n");
  for (i = 0; i < RX_QUEUE_LENGTH; ++i) {
    printf ("emac:rx%02d & %p (0x%08lx 0x%08lx)",
	    i, &rgl_rx_descriptor[i*2],
	    rgl_rx_descriptor[i*2 + 0], rgl_rx_descriptor[i*2 + 1]);
    show_rx_flags (rgl_rx_descriptor[i*2 + 0], rgl_rx_descriptor[i*2 + 1]);
    if (i == head_rx)
      printf (" HEAD");
    if (i == ((void*)EMAC_RXBQP - (void*) rgl_rx_descriptor)/8)
      printf  (" *");
    printf ("\n");
  }
#endif

  if (RX0_ (-1) & RX0_USED)
    DBG(0, "+++ receive queue full on clean\n");

  if (!(RX0__ (i) & RX0_USED)) {
    DBG(0, "  *** used bit false alarm #%d\n", i);
    goto done;
  }
  do {
    DBG(0, "  *** clean rx queue #%d %8lx %8lx\n",
	 i, RX0__ (i), RX1__ (i));
    rgl_rx_descriptor[i*2 + 1] = 0;
    rgl_rx_descriptor[i*2]    &= ~RX0_USED;
//    ++c;
    i = (i + 1) % RX_QUEUE_LENGTH;
  } while (i != head_rx && (rgl_rx_descriptor[i*2] & RX0_USED));

 done:
//  printf ("(%d)", c);

#if 0
  if (c == 0) {
    printf ("\n");
    for (i = 0; i < RX_QUEUE_LENGTH; ++i) {
      printf ("emac:rx%02d & %p (0x%08lx 0x%08lx)",
	      i, &rgl_rx_descriptor[i*2],
	      rgl_rx_descriptor[i*2 + 0], rgl_rx_descriptor[i*2 + 1]);
      show_rx_flags (rgl_rx_descriptor[i*2 + 0], rgl_rx_descriptor[i*2 + 1]);
      if (i == head_rx)
	printf (" HEAD");
      if (i == ((void*)EMAC_RXBQP - (void*) rgl_rx_descriptor)/8)
	printf  (" *");
      printf ("\n");
    }
  }
#endif

  EMAC_RXSTATUS = EMAC_RXSTATUS_BUFNOTAVAIL;
  EMAC_NETCTL |= EMAC_NETCTL_RXEN;
}

static inline void eth_clean_tx_queue (void)
{
  int state = 0;

  DBG (2, "clean [%lx %lx] #%d  txstatus 0x%x\n",
       TXH0 (), TXH1 (),
       head_tx, EMAC_TXSTATUS);

  while (state >= 0) {
    switch (state) {
    case 0:
      if (   (TXH1 () & 0x7ff)
	  && (TXH1 () & TX1_USED))
	++state;
      else {
	--state;
	break;
      }
      /* fall through */
    case 1:
      if (TXH1 () & TX1_LAST)
	--state;
      /* Mark free */
      TXH1 () = (TXH1 () & TX1_WRAP) | TX1_USED;
    }
    if (state >= 0)
      head_tx = (head_tx + 1) % TX_QUEUE_LENGTH;
  }
}

static ssize_t eth_read (struct descriptor_d* d, void* pv, size_t cb)
{
  int c;
  int i;
  int frame_len = 0;
  unsigned long status = EMAC_RXSTATUS;
  EMAC_RXSTATUS = 0xff;

  //  if (!(status & EMAC_RXSTATUS_FRMREC))
  //    return 0;

  if (status & EMAC_RXSTATUS_BUFNOTAVAIL)
    eth_clean_rx_queue ();

 restart:
  if (!(RX0 () & RX0_USED))	/* End of receive */
    return 0;

  DBG (2,"%s: frame received (%d)\n", __FUNCTION__, head_rx);

  if (RX0_ (-1) & RX0_USED)
    DBG (1, "+++ receive queue full\n");

  /* Short-circuit on corrupt receive, USED but not START */
  if ((RX1 () & RX1_START) == 0) {
  cleanup:
    do {
      DBG (1,"cleanup %d @%p %lx %lx\n", head_rx, &RX0 (), RX0 (), RX1 ());
      RX1 () = 0;
      RX0 () &= ~RX0_USED;
      DBG (1,"   %lx %lx\n", RX0 (), RX1 ());
      head_rx = (head_rx + 1) % RX_QUEUE_LENGTH;
    } while (   (RX0 () & RX0_USED)
	     && (RX1 () & RX1_START) == 0);
    goto restart;
//    return 0;			/* Nothing to receive at the moment */
  }

  EMAC_RXSTATUS = EMAC_RXSTATUS_FRMREC;

	/* Determine length of frame */
  c = 0;
  for (i = head_rx; 1; i = (i + 1) %RX_QUEUE_LENGTH) {
    if ((rgl_rx_descriptor[i*2] & RX0_USED) == 0)
      return 0;     /* as yet, incompleted receive */
    DBG (3, "  #%d(%d) %lx %lx\n", i, head_rx,
	 rgl_rx_descriptor[i*2], rgl_rx_descriptor[i*2 + 1]);
    if ((rgl_rx_descriptor[i*2 + 1] & RX1_START)
	&& (i != head_rx || c))
      goto cleanup;		/* incomplete frame */
    ++c;
    if ((rgl_rx_descriptor[i*2 + 1] & RX1_END) == 0)
      continue;
    frame_len +=  rgl_rx_descriptor[i*2 + 1] & 0x7ff;
    if (!(rgl_rx_descriptor[i*2 + 1] & (RX1_BROADCAST | RX1_SPEC1)))
      goto cleanup;		/* bogus frame */
    break;
  }

  DBG (3, "rxcomplete RXBQP  0x%lx #%d (%d) - %lx %lx\n"
       "  len 0x%x (%d) %d buf\n",
       EMAC_RXBQP,
       ((void*) EMAC_RXBQP - (void*) rgl_rx_descriptor)/8,
       head_rx,
       RX0 (), RX1 (),
       frame_len, frame_len, c);

  if (frame_len == 0)
    return 0;	/* Return immediately if no frame */

  {
    char* pb = pv;
    size_t cbRead = 0;
    c = 0;
    while (cbRead < frame_len) {
      size_t cbCopy = cb;
      ++c;
      if (cbCopy > RX_BUFFER_SIZE)
	cbCopy = RX_BUFFER_SIZE;
//      DBG (3, "  #%d %lx %lx  len 0x%x (%d)\n",
//	   head_rx, RX0 (), RX1 (), cb, cbCopy);
      memcpy (pb, &rgbRxBuffer[head_rx*RX_BUFFER_SIZE], cbCopy);
      cb -= cbCopy;
      pb += cbCopy;
      cbRead += cbCopy;
      RX1 () = 0;
      RX0 () &= ~RX0_USED;
      head_rx = (head_rx + 1) % RX_QUEUE_LENGTH;
    }
    DBG(2, "  %d buf -> %d\n", c, head_rx);
    PRINT_PKT (pv, frame_len);
  }

  /* *** FIXME: account errors? */
  EMAC_RXSTATUS
    = EMAC_RXSTATUS_RXCOVERRUN
    | EMAC_RXSTATUS_FRMREC;

  return frame_len;
}

ssize_t eth_write (struct descriptor_d* d, const void* pv, size_t cb)
{
  int need;
  int available;

 wait:
  eth_clean_tx_queue ();

  need = (cb + TX_BUFFER_SIZE - 1)/TX_BUFFER_SIZE;
  available = head_tx - tail_tx;
  if (available < 0 || (TXT1 () & TX1_USED) != 0)
    available += TX_QUEUE_LENGTH;

//  printf ("%s: %p %d %d/%d\n", __FUNCTION__, pv, cb, need, available);

  if (available < need)
    goto wait;			/* Loop until there is a free buffer */

  if (cb > CB_TX_BUFFER)
    cb = CB_TX_BUFFER;

  /* *** FIXME: we're assuming that we need only one buffer */
  memcpy ((void*) TXT0 (), pv, cb);
  TXT1 () = (TXT1 () & TX1_WRAP) | TX1_LAST | (cb & 0x7ff);

  PRINT_PKT (rgl_tx_descriptor[0], cb);

  tail_tx = (tail_tx + 1) % TX_QUEUE_LENGTH;

  EMAC_TXSTATUS = 0xff; // EMAC_TXSTATUS_TXCOMPLETE;

  EMAC_NETCTL	|= EMAC_NETCTL_STARTTX;

  /* We don't wait for the transmit to complete because we check for
     available buffer space when we transmit.  In this way, we get
     some overlap in the IO. */

  //  while (!(EMAC_TXSTATUS & EMAC_TXSTATUS_TXCOMPLETE))
  //    ;

  return cb;
}


static __driver_4 struct driver_d eth_driver = {
  .name = "eth-lh7952x",
  .description = "lh7952x Ethernet driver",
  .flags = DRIVER_NET,
  .open = eth_open,
  .close = close_helper,
  .read = eth_read,
  .write = eth_write,
};

static __service_6 struct service_d lh7952x_emac_service = {
  .init = emac_init,
  .release = emac_release,
#if !defined (CONFIG_SMALL)
  .report = emac_report,
#endif
};

#if 0
void eth_diag (int mode)
{
  int i;
//  long rgl[2*C_RX_BUFFER];

  switch (mode) {
  default:
  case 0:
    for (i = 0; i < RX_QUEUE_LENGTH; ++i) {
      printf ("emac:rx%02d & %p (0x%08lx 0x%08lx)",
	      i, &rgl_rx_descriptor[i*2],
	      rgl_rx_descriptor[i*2 + 0], rgl_rx_descriptor[i*2 + 1]);
      show_rx_flags (rgl_rx_descriptor[i*2 + 0], rgl_rx_descriptor[i*2 + 1]);
      if (i == head_rx)
	printf (" HEAD");
      if (i == ((void*)EMAC_RXBQP - (void*) rgl_rx_descriptor)/8)
	printf  (" *");
      printf ("\n");
    }
    break;
  case 1:
    i = ((void*) EMAC_RXBQP - (void*) rgl_rx_descriptor)/8;
    printf ("emac:rx%02d & %p (0x%08lx 0x%08lx)",
	    i, &rgl_rx_descriptor[i*2],
	    rgl_rx_descriptor[i*2 + 0], rgl_rx_descriptor[i*2 + 1]);
    show_rx_flags (rgl_rx_descriptor[i*2 + 0], rgl_rx_descriptor[i*2 + 1]);
    if (i == head_rx)
      printf (" HEAD");
    if (i == ((void*)EMAC_RXBQP - (void*) rgl_rx_descriptor)/8)
      printf  (" *");
    printf ("\n");
    break;
  }
}
#endif

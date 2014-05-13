/* drv-dm9000.c

   written by Marc Singer
   26 May 2006

   Copyright (C) 2006 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   o Optimization. This code is somewhat wasteful.  We should be able
     to recover some space in the loader through use herein.

   o Multiple dm9000's.  If multiple DM9000 support is enabled in the
     boot loader and only one is stuffed on the target and it is the
     second of the two, you may need to use the -1 switch when setting
     the MAC address even though APEX doesn't really care.

   o Failed probe.  In some cases, it may be desirable to report that
     a probe failed and show the data read from the dm9000 when the
     chip isn't recognized.  Presently, this requires a recompile.

   o RX packet errors.  (1<<8) FIFO error; (1<<9) CRC error;
     (1<<15) length error.

   o Reading ISR in read() function.  It seems to be necessary that we
     read the ISR register in the read() function or we won't be able
     to receive anything.  Not sure why.

        / *I/O mode * /
        db->io_mode = ior(db, DM9000_ISR) >> 6; / *ISR bit7:6 keeps I/O mode * /

        / *GPIO0 on pre-activate PHY * /
        iow(db, DM9000_GPR, 0); / *REG_1F bit0 activate phyxcer * /
        iow(db, DM9000_GPCR, GPCR_GEP_CNTL);    / *Let GPIO0 output * /
        iow(db, DM9000_GPR, 0); / *Enable PHY * /

        / *Program operating register * /
        iow(db, DM9000_TCR, 0);         / *TX Polling clear * /
        iow(db, DM9000_BPTR, 0x3f);     / *Less 3Kb, 200us * /
        iow(db, DM9000_FCR, 0xff);      / *Flow Control * /
        iow(db, DM9000_SMCR, 0);        / *Special Mode * /
        / *clear TX status * /
        iow(db, DM9000_NSR, NSR_WAKEST | NSR_TX2END | NSR_TX1END);
        iow(db, DM9000_ISR, ISR_CLR_STATUS); / *Clear interrupt status * /

        / *Set address filter table * /
        dm9000_hash_table(dev);

        / *Activate DM9000 * /
        iow(db, DM9000_RCR, RCR_DIS_LONG | RCR_DIS_CRC | RCR_RXEN);
        / * Enable TX/RX interrupt mask * /
        iow(db, DM9000_IMR, IMR_PAR | IMR_PTM | IMR_PRM);




*/

#include <apex.h>
#include <config.h>
#include <driver.h>
#include <service.h>
#include <command.h>
#include <error.h>
#include <linux/string.h>
#include <asm/reg.h>
#include <linux/kernel.h>

#include <dm9000.h>
#include <mach/drv-dm9000.h>

//#define TALK 1
#include <talk.h>

#if !defined (C_DM)
# define C_DM	(1)
#endif

#if DM_WIDTH != 16
# error "DM9000 driver only supports 16 bit bus"
#endif

#if !defined DM_IO_DELAY
# define DM_IO_DELAY
#endif

#define DRIVER_NAME		 "eth-dm9000"

#define PHY_CONTROL	0
#define PHY_STATUS	1
#define PHY_ID1		2
#define PHY_ID2		3
#define PHY_ANEG_ADV	4
#define PHY_ANEG_CAP	5

#define PHY_CONTROL_RESET		(1<<15)
#define PHY_CONTROL_LOOPBACK		(1<<14)
#define PHY_CONTROL_POWERDOWN		(1<<11)
#define PHY_CONTROL_ANEN_ENABLE		(1<<12)
#define PHY_CONTROL_MII_DISABLE		(1<<10)
#define PHY_CONTROL_RESTART_ANEN	(1<<9)
#define PHY_STATUS_ANEN_COMPLETE	(1<<5)
#define PHY_STATUS_100FULL		(1<<14)
#define PHY_STATUS_100HALF		(1<<13)
#define PHY_STATUS_10FULL		(1<<12)
#define PHY_STATUS_10HALF		(1<<11)
#define PHY_STATUS_LINK			(1<<2)

struct dm9000 {
  int present;
  unsigned short vendor;
  unsigned short product;
  unsigned short chip;
  u16 rgs_eeprom[64/sizeof (u16)]; /* Data copied from the eeprom */
  volatile u16* index;
  volatile u16* data;
  const char* name;
  volatile int tx_count;
};

static struct dm9000 dm9000[C_DM];
#if C_DM > 1
static int g_dm9000_default;	/* Default dm9000 for commands  */
#else
# define g_dm9000_default 0
#endif
static int g_dm9000_present;

static void write_reg (int dm, int index, u16 value)
{
  DBG (3, "%s: [%d] %x <- %x\n", __FUNCTION__, dm, index, value);
  *dm9000[dm].index = index;
  DM_IO_DELAY;
  *dm9000[dm].data  = value;
}

static u16 read_reg (int dm, int index)
{
  int value;
  *dm9000[dm].index = index;
  DM_IO_DELAY;
  value =  *dm9000[dm].data;
  DBG (3, "%s: [%d] %x -> %x\n", __FUNCTION__, dm, index, value);
  return value;
}

static u16 read_eeprom (int dm, int index)
{
  u16 v;

  write_reg (dm, DM9000_EPAR, index);
  write_reg (dm, DM9000_EPCR, EPCR_ERPRR);
  mdelay (10);	/* according to the datasheet 200us should be enough,
		   but it doesn't work */
  write_reg (dm, DM9000_EPCR, 0x0);

  v = read_reg (dm, DM9000_EPDRL) & 0xff;
  v |= (read_reg (dm, DM9000_EPDRH) & 0xff) << 8;
  return v;
}

static void write_eeprom (int dm, int index, u16 value)
{
  write_reg (dm, DM9000_EPAR, index);
  write_reg (dm, DM9000_EPDRH, (value >> 8) & 0xff);
  write_reg (dm, DM9000_EPDRL,  value       & 0xff);
  write_reg (dm, DM9000_EPCR, EPCR_WEP | EPCR_ERPRW);
  mdelay (10);
  write_reg (dm, DM9000_EPCR, 0);
}

static u16 read_phy (int dm, int index)
{
  u16 v;

  write_reg (dm, DM9000_EPAR, index | (1<<6));
  write_reg (dm, DM9000_EPCR, EPCR_ERPRR | EPCR_EPOS);
  mdelay (10);	/* according to the datasheet 200us should be enough,
		   but it doesn't work */
  write_reg (dm, DM9000_EPCR, EPCR_EPOS);

  v = read_reg (dm, DM9000_EPDRL) & 0xff;
  v |= (read_reg (dm, DM9000_EPDRH) & 0xff) << 8;
  return v;
}

#if 0
static void write_phy (int dm, int index, u16 value)
{
  write_reg (dm, DM9000_EPAR, index | (1<<6));
  write_reg (dm, DM9000_EPDRL,  value       & 0xff);
  write_reg (dm, DM9000_EPDRH, (value >> 8) & 0xff);
  write_reg (dm, DM9000_EPCR, EPCR_ERPRW | EPCR_EPOS);
  mdelay (10);
  write_reg (dm, DM9000_EPCR, EPCR_EPOS);
}
#endif

static void dm9000_read_eeprom (int dm)
{
  int i;
  for (i = 0; i < sizeof (dm9000[dm].rgs_eeprom)/sizeof (u16); ++i)
    dm9000[dm].rgs_eeprom[i] = read_eeprom (dm, i);
}


#if defined (CONFIG_CMD_ETH_DM9000)

void dm9000_dump (char* rgb)
{
  printf ("NCR %s %s\n",
          (rgb[DM9000_NCR] & NCR_FDX) ? "FDX" : "HDX",
          (rgb[DM9000_NCR] & NCR_WAKEEN) ? "WAKE" : "!wake"
          );
  printf ("NSR %s %s %s %s %s %s\n",
          (rgb[DM9000_NSR] & NSR_RXOV) ? "RXOV" : "!rxov",
          (rgb[DM9000_NSR] & NSR_TX1END) ? "TX1END" : "",
          (rgb[DM9000_NSR] & NSR_TX2END) ? "TX2END" : "",
          (rgb[DM9000_NSR] & NSR_WAKEST) ? "WAKEST" : "",
          (rgb[DM9000_NSR] & NSR_LINKST) ? "LINK" : "!link",
          (rgb[DM9000_NSR] & NSR_SPEED) ? "100Mbps" : "10Mbps"
          );
  printf ("ISR %s %s %s %s %s %s\n",
          (rgb[DM9000_ISR] & ISR_PRS) ? "RCV" : "!rcv",
          (rgb[DM9000_ISR] & ISR_PTS) ? "TXC" : "!txc",
          (rgb[DM9000_ISR] & ISR_ROS) ? "ROV" : "!rov",
          (rgb[DM9000_ISR] & ISR_ROOS) ? "ROOC" : "!rooc",
          (rgb[DM9000_ISR] & ISR_UDRUN) ? "TUN" : "!tun",
          (rgb[DM9000_ISR] & ISR_LINKCHG) ? "LINK" : "!link"
          );
  printf ("IMR %s %s %s %s %s %s\n",
          (rgb[DM9000_IMR] & IMR_PRM) ? "RCV" : "!rcv",
          (rgb[DM9000_IMR] & IMR_PTM) ? "TXC" : "!txc",
          (rgb[DM9000_IMR] & IMR_ROM) ? "ROV" : "!rov",
          (rgb[DM9000_IMR] & IMR_ROOM) ? "ROOC" : "!rooc",
          (rgb[DM9000_IMR] & IMR_UDRUNM) ? "TUN" : "!tun",
          (rgb[DM9000_IMR] & IMR_LINKCHGM) ? "LINK" : "!link"
          );
}


  /* *** FIXME: this shouldn't be in the command conditional */
#if defined (CONFIG_ETHERNET)
extern char host_mac_address[]; 
#else
static char host_mac_address[6];
#endif

static int cmd_eth (int argc, const char** argv)
{
  int result = 0;
  int dm = g_dm9000_default;

  if (argc > 1 && argv[1][0] == '-') {
    switch (argv[1][1]) {
    default:
    case '0':
      break;
#if C_DM > 0
    case '1':
      dm = 1;
      break;
#endif
    }
    --argc;
    ++argv;
  }

  if (!dm9000[dm].present)
    ERROR_RETURN (ERROR_FAILURE, "dm9000 device not present");

  if (argc == 1) {
    printf ("dm9000[%d]: vendor 0x%x  product 0x%x  chip 0x%x\n",
	    dm, dm9000[dm].vendor, dm9000[dm].product, dm9000[dm].chip);
    printf ("  0x%02x 0x%02x 0x%02x 0x%02x 0x%04x\n",
            read_reg (dm, DM9000_VIDH), read_reg (dm, DM9000_VIDL),
            read_reg (dm, DM9000_PIDH), read_reg (dm, DM9000_PIDL),
            read_reg (dm, DM9000_CHIPR));
  }
  else {
	/* Set mac address */
    if (PARTIAL_MATCH (argv[1], "m", "ac") == 0) {
      char __aligned rgb[6];
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
      /* we should set mac address for non-eeprom systems */
      write_reg (dm, DM9000_PAR, host_mac_address[0]);
      write_reg (dm, DM9000_PAR+1, host_mac_address[1]);
      write_reg (dm, DM9000_PAR+2, host_mac_address[2]);
      write_reg (dm, DM9000_PAR+3, host_mac_address[3]);
      write_reg (dm, DM9000_PAR+4, host_mac_address[4]);
      write_reg (dm, DM9000_PAR+5, host_mac_address[5]);
      /* enable broadcast packets */
      write_reg (dm, DM9000_MAR+5, 0x80);

    }

    if (PARTIAL_MATCH (argv[1], "s", "ave") == 0) {
      u16 rgs[3];
      rgs[0] = host_mac_address[0] | (host_mac_address[1] << 8);
      rgs[1] = host_mac_address[2] | (host_mac_address[3] << 8);
      rgs[2] = host_mac_address[4] | (host_mac_address[5] << 8);
      write_eeprom (dm, 0, rgs[0]);
      write_eeprom (dm, 1, rgs[1]);
      write_eeprom (dm, 2, rgs[2]);
      dm9000_read_eeprom (dm);
    }

    if (PARTIAL_MATCH (argv[1], "rea", "d-eeprom") == 0) {
      dm9000_read_eeprom (dm);
      dump ((void*) dm9000[dm].rgs_eeprom, sizeof (dm9000[dm].rgs_eeprom), 0);
    }

    if (PARTIAL_MATCH (argv[1], "reg", "") == 0) {
      if (argc == 2) {
        int reg;
        char rgb[256];
        for (reg = 0; reg < sizeof (rgb); ++reg)
          rgb[reg] = (unsigned char) read_reg (dm, reg);
        dumpw (rgb, sizeof (rgb), 0, 0);
        dm9000_dump (rgb);
      }
      else {
        int reg = simple_strtoul (argv[2], NULL, 0);
        unsigned char value = read_reg (dm, reg);
        printf ("reg 0x%x -> 0x%x (%d)\n", reg, value, value);
      }
    }

    if (PARTIAL_MATCH (argv[1], "w", "rite") == 0) {
      int reg = simple_strtoul (argv[2], NULL, 0);
      unsigned char value = simple_strtoul (argv[3], NULL, 0);
      write_reg (dm, reg, value);
      printf ("reg 0x%x <- 0x%x (%d)\n", reg, value, value);
    }

    if (PARTIAL_MATCH (argv[1], "phy", "") == 0) {
      if (argc == 2) {
        int reg;
        uint16_t rgw[20];
        for (reg = 0; reg < sizeof (rgw); ++reg)
          rgw[reg] = read_phy (dm, reg);
        dumpw ((char*) rgw, sizeof (rgw), 0, 2);
      }
#if 0
      else {
        int reg = simple_strtoul (argv[2], NULL, 0);
        unsigned char value = read_reg (dm, reg);
        printf ("reg 0x%x -> 0x%x (%d)\n", reg, value, value);
      }
#endif
    }

  }

  return result;
}

static __command struct command_d c_eth = {
  .command = "eth",
  .func = cmd_eth,
  COMMAND_DESCRIPTION ("dm9000 diagnostics")
  COMMAND_HELP(
"eth [-#] [SUBCOMMAND [PARAMETER]]\n"
"  Commands for the Ethernet MAC and PHY devices.\n"
"  Without a SUBCOMMAND, it displays info about the chip\n"
"    This information is for debugging the hardware.\n"
"  -#          - select interface number # (0..N-1)\n"
//"  clear - reset the EMAC.\n"
//"  anen  - restart auto negotiation.\n"
//"  send  - send a test packet.\n"
//"  loop  - enable loopback mode.\n"
//"  force - force power-up and restart auto-negotiation.\n"
"  mac         - set the MAC address to PARAMETER.\n"
"    PARAMETER has the form XX:XX:XX:XX:XX:XX where each X is a\n"
"    hexadecimal digit.  Be aware that MAC addresses must be unique for\n"
"    proper operation of the network.  This command may be added to the\n"
"    startup commands to set the MAC address at boot-time.\n"
"  save        - saves the MAC address to the mac: EEPROM device.\n"
"    A saved MAC address will be used to automatically configure the MAC\n"
"    at startup.  For this feature to work, there must be a mac: driver.\n"
"  read-eeprom - reads and dumps the contents of the EEPROM.\n"
"  reg [IDX]   - show dm9000 register IDX or all registers\n"
"  write IDX VAL - write dm9000 register IDX with value VAL\n"
"  e.g.  eth mac 01:23:45:67:89:ab         # Never use this MAC address\n"
"        eth save\n"
  )
};

#endif

static void dm9000_init (void)
{
  int dm;

  ENTRY (1);

#if C_DM > 1
  g_dm9000_default = -1;
#endif

  for (dm = 0; dm < C_DM; ++dm) {

    DBG (2, "%s: %d\n", __FUNCTION__, dm);

    switch (dm) {
    case 0:
      dm9000[dm].index = &__REG16 (DM_PHYS_INDEX);
      dm9000[dm].data  = &__REG16 (DM_PHYS_DATA);
# if defined (DM_NAME)
      dm9000[dm].name = DM_NAME;
# endif
      break;
#if C_DM > 1
    case 1:
      dm9000[dm].index = &__REG16 (DM1_PHYS_INDEX);
      dm9000[dm].data  = &__REG16 (DM1_PHYS_DATA);
# if defined (DM1_NAME)
      dm9000[dm].name = DM1_NAME;
# endif
      break;
#endif
    default:
      continue;
    }

#if C_DM > 1
    if (g_dm9000_default == -1)	/* Default interface for commands */
      g_dm9000_default = dm;
#endif

    DBG (2, "%s: [%d] index %p  data %p\n", __FUNCTION__, dm,
	 dm9000[dm].index, dm9000[dm].data);

    dm9000[dm].vendor  = read_reg (dm, DM9000_VIDL)
      | (read_reg (dm, DM9000_VIDH) << 8);
    dm9000[dm].product = read_reg (dm, DM9000_PIDL)
      | (read_reg (dm, DM9000_PIDH) << 8);
    dm9000[dm].chip    = read_reg (dm, DM9000_CHIPR);

    DBG (2, "%s: [%d] vendor %x  product %x  chip %x\n", __FUNCTION__, dm,
	 dm9000[dm].vendor, dm9000[dm].product, dm9000[dm].chip);

    if (dm9000[dm].vendor != 0xa46 || dm9000[dm].product != 0x9000)
      continue;

    dm9000[dm].present = 1;
    g_dm9000_present = 1;

    dm9000[dm].rgs_eeprom[0] = read_eeprom (dm, 0);
    dm9000[dm].rgs_eeprom[1] = read_eeprom (dm, 1);
    dm9000[dm].rgs_eeprom[2] = read_eeprom (dm, 2);

	/* Only the first interface can participate in the network stack */
    if (dm == 0) {
      host_mac_address[0] =  dm9000[dm].rgs_eeprom[0]       & 0xff;
      host_mac_address[1] = (dm9000[dm].rgs_eeprom[0] >> 8) & 0xff;
      host_mac_address[2] =  dm9000[dm].rgs_eeprom[1]       & 0xff;
      host_mac_address[3] = (dm9000[dm].rgs_eeprom[1] >> 8) & 0xff;
      host_mac_address[4] =  dm9000[dm].rgs_eeprom[2]       & 0xff;
      host_mac_address[5] = (dm9000[dm].rgs_eeprom[2] >> 8) & 0xff;
    }
  }

  write_reg (g_dm9000_default, DM9000_GPR, 0); /* Power-up PHY */
#if 0
  udelay (200);
  {
    uint16_t v = read_phy (0, PHY_CONTROL);
    DBG (1, "phy starts out %x\n", v);
    write_phy (0, PHY_CONTROL, 0);
    DBG (1, "phy now at %x\n", read_phy (0, PHY_CONTROL));
    v &= ~(PHY_CONTROL_POWERDOWN | PHY_CONTROL_MII_DISABLE);
    v |= PHY_CONTROL_RESET
      | PHY_CONTROL_ANEN_ENABLE | PHY_CONTROL_RESTART_ANEN;
    write_phy (0, PHY_CONTROL, v);
    while ((v = read_phy (0, PHY_CONTROL)) & PHY_CONTROL_RESET)
      ;
  }
#endif

  /* *** FIXME: None of these seem to be necessary */
  write_reg (g_dm9000_default, DM9000_TCR, 0);
  write_reg (g_dm9000_default, DM9000_BPTR, 0x3f);
  write_reg (g_dm9000_default, DM9000_FCR, 0xff);
  write_reg (g_dm9000_default, DM9000_SMCR, 0);
  write_reg (g_dm9000_default,
             DM9000_NSR, NSR_WAKEST | NSR_TX2END | NSR_TX1END);
  write_reg (g_dm9000_default, DM9000_ISR, ISR_CLR_STATUS);

  write_reg (g_dm9000_default, DM9000_RCR,
             RCR_DIS_LONG | RCR_DIS_CRC | RCR_RXEN);

  dm9000[g_dm9000_default].tx_count = 0; /* Clear count of pending transmits */
}


#if !defined (CONFIG_SMALL)
static void dm9000_report (void)
{
  int dm;

  if (!g_dm9000_present)
    return;

  printf ("  dm9000: host_mac_addr %02x:%02x:%02x:%02x:%02x:%02x\n",
	  host_mac_address[0], host_mac_address[1],
	  host_mac_address[2], host_mac_address[3],
	  host_mac_address[4], host_mac_address[5]);

  for (dm = 0; dm < C_DM; ++dm) {
    if (dm9000[dm].present) {
      printf ("  dm9000: [%d]"
//	      " phy_addr %d  phy_id 0x%lx"
	      "  mac_addr %02x:%02x:%02x:%02x:%02x:%02x"
	      " (0x%x 0x%x 0x%x) %s\n",
	      dm,
//	      -1, (unsigned long)-1,
	      (dm9000[dm].rgs_eeprom[0]) & 0xff,
	      (dm9000[dm].rgs_eeprom[0] >> 8) & 0xff,
	      (dm9000[dm].rgs_eeprom[1]) & 0xff,
	      (dm9000[dm].rgs_eeprom[1] >> 8) & 0xff,
	      (dm9000[dm].rgs_eeprom[2]) & 0xff,
	      (dm9000[dm].rgs_eeprom[2] >> 8) & 0xff,
	      dm9000[dm].vendor, dm9000[dm].product, dm9000[dm].chip,
	      dm9000[dm].name ? dm9000[dm].name : "");
      printf ("          %d bit\n",
              (read_reg (dm, DM9000_ISR) & (1<<6)) ? 8 : 16);
    }
  }
}
#endif


static int dm9000_open (struct descriptor_d* d)
{
  write_reg (g_dm9000_default, DM9000_RCR, RCR_RXEN); /* Receive enable */
  write_reg (g_dm9000_default, DM9000_IMR, IMR_PAR);  /* Auto increment */

  /* FIXME: Make sure we're init'd */

  return 0;
}

/** Check for transmit completion and decrement the tx_count
    accordingly.  The return value is zero if a packet can be loaded
    into a transmit buffer. */
static int dm9000_check_tx (void)
{
  uint16_t status;
  if (dm9000[g_dm9000_default].tx_count == 0)
    return 0;

  status = read_reg (g_dm9000_default, DM9000_NSR);
  status &= (1<<3)|(1<<2);
  if (status) {
    write_reg (g_dm9000_default, DM9000_NSR, status);

    if (status & (1<<2))
      --dm9000[g_dm9000_default].tx_count;
    if (status & (1<<3))
      --dm9000[g_dm9000_default].tx_count;
  }

  return dm9000[g_dm9000_default].tx_count < 2 ? 0 : 1;
}


/** Read packet from the DM9000 into system memory.  The header for
    the packet is a 0x01, STATUS, LENGTH_LOW, LENGTH_HIGH.

*/

static ssize_t dm9000_read (struct descriptor_d* d, void* pv, size_t cb)
{
//  uint16_t isr = read_reg (g_dm9000_default, DM9000_ISR);
  uint16_t status;
  size_t length;
  char* rgb = (char*) pv;
  int i;

  dm9000_check_tx ();           /* Just because we can */

//  if (!(isr & ISR_PRS))
//    return 0;
  read_reg  (g_dm9000_default, DM9000_ISR);
  write_reg (g_dm9000_default, DM9000_ISR, ISR_PRS); /* Clear interrupt */

  status = read_reg (g_dm9000_default, DM9000_MRCMDX); /* Dummy read */
  status = read_reg (g_dm9000_default, DM9000_MRCMDX);
  //printf ("packet received, status 0x%x\n", status);

  if ((status & 0xff) != 1) {     /* Return if the receive buffer not ready */
//    printf ("early termination on receive\n");
    return 0;
  }

  status = read_reg (g_dm9000_default, DM9000_MRCMD);
  length = read_reg (g_dm9000_default, DM9000_MRCMD);

  for (i = 0; i < length; i += 2) {
    uint16_t v = read_reg (g_dm9000_default, DM9000_MRCMD);
    if (i + 2 > cb)
      continue;
    rgb[i] = v & 0xff;
    rgb[i + 1] = (v >> 8) & 0xff;
  }

  PRINTF ("Received %d bytes (status 0x%x)\n", length, status);
#if defined (TALK) && TALK > 1
  dumpw (rgb, length, 0, 0);
#endif

  return length;
}

static ssize_t dm9000_write (struct descriptor_d* d, const void* pv, size_t cb)
{
  uint8_t* rgb = (uint8_t*) pv;
  int i;
//  int check = dm9000_check_tx ();

  ENTRY (0);

//  if (check)
//    printf ("packet transmit blocked\n");
  while (dm9000_check_tx ())
    ;
//  if (check)
//    printf ("packet transmit complete\n");
  read_reg (g_dm9000_default, DM9000_ISR);           /* Dummy read */
  write_reg (g_dm9000_default, DM9000_ISR, ISR_PTS); /* Clear interrupt */

  ++dm9000[g_dm9000_default].tx_count;

  *dm9000[g_dm9000_default].index = DM9000_MWCMD;
  DM_IO_DELAY;
  for (i = 0; i < cb; i += 2) {
    uint16_t v = rgb[i] | (rgb[i + 1] << 8);
    *dm9000[g_dm9000_default].data = v;
    DM_IO_DELAY;
  }

	/* Tell DM9000 the size of the packet */
  write_reg (g_dm9000_default, DM9000_TXPLL, cb & 0xff);
  write_reg (g_dm9000_default, DM9000_TXPLH, (cb >> 8) & 0xff);

	/* Initiate transfer */
  write_reg (g_dm9000_default, DM9000_TCR, TCR_TXREQ);

  return 0;
}

static __driver_4 struct driver_d dm9000_driver = {
  .name = DRIVER_NAME,
  .description = "Davicom DM9000 Ethernet driver",
  .flags = DRIVER_NET,
  .open = dm9000_open,
  .close = close_helper,
  .read = dm9000_read,
  .write = dm9000_write,
};

static __service_6 struct service_d dm9000_service = {
  .init = dm9000_init,
  //  .release = dm9000_release,
#if !defined (CONFIG_SMALL)
  .report = dm9000_report,
#endif
};

/* drv-npe.c

   written by Marc Singer
   30 Dec 2006

   Copyright (C) 2006 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   IXP4xx NPE Ethernet Driver

   NOTES
   =====

   Dependencies
   ------------

   The npe_mh code communicates with the NPE by sending messages.  The
   npe_mac code uses it to set the MAC address, for example.  The only
   client of the npe_mh calls is the mac.

   Unknowns
   --------

   o Would be nice to know the meaning of the other bits in the NPE
     status register.  We can tell that the NPE is running and that's
     all.

*/

#include <config.h>
#include <mach/hardware.h>
#include <mach/drv-npe.h>
#include <linux/types.h>
#include <linux/string.h>
#include <asm/byteorder.h>
#include <apex.h>
#include <driver.h>
#include <service.h>
#include <network.h>

struct npe_plat_data {
  const char* name;
  int data_size;
  int inst_size;
  int id;         /* Node ID */
};

struct mac_plat_info {
  int npe_id;     /* Node ID of the NPE for this port */
  int port_id;    /* Port ID for NPE-B @ ixp465 */
  int eth_id;     /* Physical ID */
  int phy_id;     /* ID of the connected PHY (PCB/platform dependent) */
  int rxq_id;     /* Queue ID of the RX-free q*/
  int txq_id;     /* Where to push the outgoing packets */
};

struct npe_info {
  u32 addr;			/* Physical IO base address */
};

struct mac_info {
  u32 addr;
  int npe_id;
  int phy_id;
  int eth_id;
  int rxq_id;
  int txq_id;
//  struct resource *res;
//  struct device *npe_dev;
//  struct qm_qmgr *qmgr;
//  struct qm_queue *rxq;
  //  struct qm_queue *txq;
//  u32 irqflags;
//  struct net_device_stats stat;
//  struct mii_if_info mii;
//  struct work_struct mdio_thread;
//  int rxq_pkt;
  //  int unloading;
//  struct mac_plat_info *plat;
};

struct npe_info npe[3];
struct mac_info mac[2];

static inline void npe_reg_write (struct npe_info* npe, u32 reg, u32 val)
{
  __REG (npe->addr + reg) = val;
}

static inline u32 npe_reg_read (struct npe_info* npe, u32 reg)
{
  return __REG (npe->addr + reg);
}

static inline u32 npe_status (struct npe_info* npe)
{
  return npe_reg_read (npe, IX_NPEDL_REG_OFFSET_EXCTL);
}

/* ixNpeDlNpeMgrCommandIssue */
static inline void npe_write_exctl (struct npe_info* npe, u32 cmd)
{
  npe_reg_write(npe, IX_NPEDL_REG_OFFSET_EXCTL, cmd);
}
/* ixNpeDlNpeMgrWriteCommandIssue */
static inline void
npe_write_cmd(struct npe_info* npe, u32 addr, u32 data, int cmd)
{
	npe_reg_write(npe, IX_NPEDL_REG_OFFSET_EXDATA, data);
	npe_reg_write(npe, IX_NPEDL_REG_OFFSET_EXAD, addr);
	npe_reg_write(npe, IX_NPEDL_REG_OFFSET_EXCTL, cmd);
}
/* ixNpeDlNpeMgrReadCommandIssue */
static inline u32
npe_read_cmd(struct npe_info* npe, u32 addr, int cmd)
{
	npe_reg_write(npe, IX_NPEDL_REG_OFFSET_EXAD, addr);
	npe_reg_write(npe, IX_NPEDL_REG_OFFSET_EXCTL, cmd);
	/* Intel reads the data twice - so do we... */
	npe_reg_read(npe, IX_NPEDL_REG_OFFSET_EXDATA);
	return npe_reg_read(npe, IX_NPEDL_REG_OFFSET_EXDATA);
}

/* ixNpeDlNpeMgrExecAccRegWrite */
static inline void npe_write_ecs_reg(struct npe_info* npe, u32 addr, u32 data)
{
	npe_write_cmd(npe, addr, data, IX_NPEDL_EXCTL_CMD_WR_ECS_REG);
}
/* ixNpeDlNpeMgrExecAccRegRead */
static inline u32 npe_read_ecs_reg(struct npe_info* npe, u32 addr)
{
	return npe_read_cmd(npe, addr, IX_NPEDL_EXCTL_CMD_RD_ECS_REG);
}

#define NPE_MH_RETRIES			200

#define NPE_MH_CMD			0
#define NPE_MH_PORT			1
#define NPE_MH_MAC			2

#define IX_ETHNPE_EDB_SETPORTADDRESS	(0x01)
#define IX_ETHNPE_FW_SETFIREWALLMODE	(0x0E)
#define IX_ETHNPE_VLAN_SETRXQOSENTRY	(0x0B)
#define IX_ETHNPE_SETLOOPBACK_MODE	(0x12)

struct npe_mh_msg {
  union {
    u8 rgb[8];
    u32 rgw[2];
  };				/* u ? */
};

#define logical_id(mp) (((mp)->npe_id << 4) | ((mp)->port_id & 0xf))

static inline void mac_write_reg (struct mac_info* mac, int offset, u32 val)
{
//  printf ("%s: %x + %x <- %x\n", __FUNCTION__, mac->addr, offset, val);
  __REG (mac->addr + (offset<<2)) = val;
}

static inline u32 mac_read_reg (struct mac_info* mac, int offset)
{
  u32 v = __REG (mac->addr + (offset<<2));

//  printf ("%s: %x + %x -> %x\n", __FUNCTION__, mac->addr, offset, v);
  return v;
//  return __REG (mac->addr + offset);
}

static inline void mac_set_regbit (struct mac_info* mac, int offset, u32 bit)
{
  mac_write_reg (mac, offset, mac_read_reg (mac, offset) | bit);
}
static inline void mac_reset_regbit (struct mac_info* mac, int offset, u32 bit)
{
  mac_write_reg (mac, offset, mac_read_reg (mac, offset) & ~bit);
}

static inline void mac_mdio_cmd_write(struct mac_info* mac, u32 cmd)
{
  int i;
//  printf ("%s: 0x%x\n", __FUNCTION__, cmd);
  for (i = 0; i < 4; ++i, cmd >>= 8)
    mac_write_reg (mac, MAC_MDIO_CMD + i, cmd & 0xff);
}

#define mac_mdio_cmd_read(mac)		mac_mdio_read ((mac), MAC_MDIO_CMD)
#define mac_mdio_status_read(mac)	mac_mdio_read ((mac), MAC_MDIO_STS)
static inline u32 mac_mdio_read (struct mac_info* mac, int offset)
{
  int i;
  u32 v;

  for (i = 0, v = 0; i < 4; ++i)
    v |= (mac_read_reg (mac, offset + i) & 0xff) << (i*8);

  return v;
}

static inline u32 mdio_cmd (int phy_addr, int phy_reg)
{
  return (phy_addr << MII_ADDR_SHL) | (phy_reg << MII_REG_SHL) | MII_GO;
}

static int send_npe_mh_msg (struct npe_info* npe, struct npe_mh_msg* msg)
{
  int retries;
  u32 rgwSend[2];

  rgwSend[0] = be32_to_cpu (msg->rgw[0]);
  rgwSend[1] = be32_to_cpu (msg->rgw[1]);

  if (npe_reg_read (npe, IX_NPEDL_REG_OFFSET_STAT) & IX_NPEMH_NPE_STAT_IFNE)
    return -1;

  npe_reg_write (npe, IX_NPEDL_REG_OFFSET_FIFO, rgwSend[0]);

  for (retries = NPE_MH_RETRIES; retries--; )
    if (npe_reg_read (npe, IX_NPEDL_REG_OFFSET_STAT) & IX_NPEMH_NPE_STAT_IFNF)
      break;
  if (retries < 0)		/* Timeout */
    return -1;

  npe_reg_write (npe, IX_NPEDL_REG_OFFSET_FIFO, rgwSend[1]);

  for (retries = NPE_MH_RETRIES; retries--; )
    if ((npe_reg_read (npe, IX_NPEDL_REG_OFFSET_STAT)
	 & IX_NPEMH_NPE_STAT_OFNE))
      break;

  if (retries < 0) {
    printf ("npe_mh: waiting for output FIFO NotEmpty failed\n");
    return -1;
  }

  {
    u32 rgwRecv[2];
    int i;
    for (rgwRecv[0] = rgwRecv[1] = 0, i = 0;
	 npe_reg_read (npe, IX_NPEDL_REG_OFFSET_STAT) & IX_NPEMH_NPE_STAT_OFNE;
	 )
      rgwRecv [(i++)&1] = npe_reg_read (npe, IX_NPEDL_REG_OFFSET_FIFO);

    if (   rgwRecv[0] != rgwSend[0]
	|| rgwRecv[1] != rgwSend[1])
      printf ("npe_mh: unexpected answer: send %08x:%08x Ret %08x:%08x\n",
	      rgwSend[0], rgwSend[1], rgwRecv[0], rgwRecv[1]);
  }

  return 0;
}

int npe_mh_disable_firewall (struct npe_info* npe, struct mac_plat_info* mp)
{
  struct npe_mh_msg msg;

  memset (&msg, 0, sizeof (msg));
  msg.rgb[NPE_MH_CMD] = IX_ETHNPE_FW_SETFIREWALLMODE;
  msg.rgb[NPE_MH_PORT] = logical_id (mp);

  return send_npe_mh_msg (npe, &msg);
}

int npe_mh_npe_loopback_mode (struct npe_info* npe, struct mac_plat_info* mp,
			      int enable)
{
  struct npe_mh_msg msg;

  memset (&msg, 0, sizeof (msg));
  msg.rgb[NPE_MH_CMD] = IX_ETHNPE_SETLOOPBACK_MODE;
  msg.rgb[NPE_MH_PORT] = logical_id (mp);
  msg.rgb[3] = enable ? 1 : 0;

  return send_npe_mh_msg (npe, &msg);
}

int npe_mh_setportaddr (struct npe_info* npe, struct mac_plat_info* mp,
			u8* mac_address)
{
  struct npe_mh_msg msg;

  msg.rgb[NPE_MH_CMD] = IX_ETHNPE_EDB_SETPORTADDRESS;
  msg.rgb[NPE_MH_PORT] = mp->eth_id;
  memcpy (msg.rgb + NPE_MH_MAC, mac_address, 6);

  return send_npe_mh_msg (npe, &msg);
}

int npe_mh_set_rxqid (struct npe_info* npe, struct mac_plat_info* mp, int qid)
{
  struct npe_mh_msg msg;
  int i;

  memset (&msg, 0, sizeof (msg));
  msg.rgb[NPE_MH_CMD] = IX_ETHNPE_VLAN_SETRXQOSENTRY;
  msg.rgb[NPE_MH_PORT] = logical_id (mp);
  msg.rgb[5] = qid | 0x80;
  msg.rgb[7] = qid<<4;
  for (i = 0; i < 8; ++i) {
    int result;
    msg.rgb[3] = i;
    if ((result = send_npe_mh_msg (npe, &msg)))
      return result;
  }
  return 0;
}

int phy_read (struct mac_info* mac, int phy_address, int phy_register)
{
  u32 v = mdio_cmd (phy_address, phy_register);
  int retries;

  mac_mdio_cmd_write (mac, v);
  for (retries = 100; retries--; )
    if (!((v = mac_mdio_cmd_read (mac)) & MII_GO))
      break;
  if (retries < 0) {
    printf ("%s: PHY[%d] access failed 0x%x\n", __FUNCTION__, phy_address, v);
    return 0;
  }

  v = mac_mdio_status_read (mac);
  if (v & MII_READ_FAIL) {
    printf ("%s: PHY[%d] unresponsive 0x%x\n", __FUNCTION__, phy_address, v);
    return 0;
  }

  return v;
}

void phy_write (struct mac_info* mac, int phy_address, int phy_register, int v)
{
  int retries;

  v |= mdio_cmd (phy_address, phy_register) | MII_WRITE ;

  mac_mdio_cmd_write (mac, v);
  for (retries = 100; retries--; )
    if (!((v = mac_mdio_cmd_read (mac)) & MII_GO))
      break;
  if (retries < 0) {
    printf ("%s: PHY[%d] access failed\n", __FUNCTION__, phy_address);
    return;
  }

  return;
}

void npe_init (void)
{
  npe[0].addr = NPEA_PHYS;
  npe[1].addr = NPEB_PHYS;
  npe[2].addr = NPEC_PHYS;

  mac[0].addr = ETH_PHYS;
  mac[0].npe_id = ETH_NPE_ID;
  mac[0].phy_id = ETH_PHY_ID;
  mac[0].eth_id = ETH_ETH_ID;
  mac[0].rxq_id = ETH_RXQ_ID;
  mac[0].txq_id = ETH_TXQ_ID;

  printf ("npe: init\n");
}

void npe_release (void)
{
}

void npe_report (void)
{
  int i;
  for (i = 0; i < 3; ++i) {
    u32 v = npe_status (&npe[i]);
    printf ("  %s    npe%c%s (0x%x)\n",
	    i ? "    " : "npe:", 'a' + i,
	    (v & IX_NPEDL_EXCTL_STATUS_RUN) ? " running" : "", v);
  }

  for (i = 0; i < 1; ++i)
    printf ("          eth%c 0x%x 0x%x 0x%x 0x%x\n",
	    'a' + i,
	    phy_read (&mac[i], mac[i].phy_id, PHY_CONTROL),
	    phy_read (&mac[i], mac[i].phy_id, PHY_STATUS),
	    phy_read (&mac[i], mac[i].phy_id, PHY_ID1),
	    phy_read (&mac[i], mac[i].phy_id, PHY_ID2));

//  printf ("  npe:    phy_addr %d  phy_id 0x%lx"
//	  "  mac_addr %02x:%02x:%02x:%02x:%02x:%02x\n",
//	  phy_address, phy_id,
}

static __driver_4 struct driver_d eth_driver = {
  .name = "eth-npe-ix4xx",
  .description = "ixp4xx NPE Ethernet driver",
  .flags = DRIVER_NET,
  //  .open = eth_open,
//  .close = close_helper,
//  .read = eth_read,
//  .write = eth_write,
};

static __service_6 struct service_d ixp4xx_npe_emac_service = {
  .init = npe_init,
  .release = npe_release,
#if !defined (CONFIG_SMALL)
  .report = npe_report,
#endif
};

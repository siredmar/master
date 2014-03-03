/*
 * Cirrus Logic CS8900A Ethernet
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
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

#include <mach/drv-cs8900.h>

#undef DEBUG

#define DRIVER_NAME		 "eth-cs8900"

#define  C_RX_BUFFER		 4
#define CB_RX_BUFFER		 (1536 + 6)

#if defined (CONFIG_ETHERNET)
extern char host_mac_address[6];
#endif

static int initialized;
static unsigned long phy_id;	/* ID read from PHY */
static int head_rx;		/* Next buffer to accept a receive packet */
static int count_rx;		/* Number of received packets buffered */
static char __xbss(ethernet) rgbRxBuffer[C_RX_BUFFER * CB_RX_BUFFER];

/* 16 bit aligned registers, 16 bit wide */
#define CS8900_BUS16_0  *(volatile u8 *)(CS8900_BASE+0x00)
#define CS8900_BUS16_1  *(volatile u8 *)(CS8900_BASE+0x01)

#if !defined (CS8900_IO_OVERRIDE)
#define CS_REG16    __REG16
#endif /* CS8900_IO_OVERRIDE */
#define CS8900_RxDATA   CS_REG16(CS8900_BASE+0x00)
#define CS8900_TxDATA   __REG16(CS8900_BASE+0x00)
#define CS8900_TxCMD    __REG16(CS8900_BASE+0x04)
#define CS8900_TxLEN    __REG16(CS8900_BASE+0x06)
#define CS8900_ISQ      CS_REG16(CS8900_BASE+0x08)
#define CS8900_PPTR     __REG16(CS8900_BASE+0x0a)
#define CS8900_PDATA    CS_REG16(CS8900_BASE+0x0c)
#define CS8900_PDATA_W  __REG16(CS8900_BASE+0x0c)
#define ISQ_RxEvent     0x04
#define ISQ_TxEvent     0x08
#define ISQ_BufEvent    0x0C
#define ISQ_RxMissEvent 0x10
#define ISQ_TxColEvent  0x12
#define ISQ_EventMask   0x3F
/* packet page register offsets */
/* bus interface registers */
#define PP_ChipID              0x0000	/* Chip identifier - must be 0x630E */
#define PP_ChipRev             0x0002	/* Chip revision, model codes */
#define PP_IntReg              0x0022	/* Interrupt configuration */
#define PP_IntReg_IRQ0         0x0000	/* Use INTR0 pin */
#define PP_IntReg_IRQ1         0x0001	/* Use INTR1 pin */
#define PP_IntReg_IRQ2         0x0002	/* Use INTR2 pin */
#define PP_IntReg_IRQ3         0x0003	/* Use INTR3 pin */
/* status and control registers */
#define PP_RxCFG     0x0102	/* Receiver configuration */
#define PP_RxCFG_Skip1         0x0040	/* Skip (i.e. discard) current frame */
#define PP_RxCFG_Stream        0x0080	/* Enable streaming mode */
#define PP_RxCFG_RxOK          0x0100	/* RxOK interrupt enable */
#define PP_RxCFG_RxDMAonly     0x0200	/* Use RxDMA for all frames */
#define PP_RxCFG_AutoRxDMA     0x0400	/* Select RxDMA automatically */
#define PP_RxCFG_BufferCRC     0x0800	/* Include CRC characters in frame */
#define PP_RxCFG_CRC           0x1000	/* Enable interrupt on CRC error */
#define PP_RxCFG_RUNT          0x2000	/* Enable interrupt on RUNT frames */
#define PP_RxCFG_EXTRA         0x4000	/* Enable interrupt on frames with extra data */
#define PP_RxCTL     0x0104	/* Receiver control */
#define PP_RxCTL_IAHash        0x0040	/* Accept frames that match hash */
#define PP_RxCTL_Promiscuous   0x0080	/* Accept any frame */
#define PP_RxCTL_RxOK          0x0100	/* Accept well formed frames */
#define PP_RxCTL_Multicast     0x0200	/* Accept multicast frames */
#define PP_RxCTL_IA            0x0400	/* Accept frame that matches IA */
#define PP_RxCTL_Broadcast     0x0800	/* Accept broadcast frames */
#define PP_RxCTL_CRC           0x1000	/* Accept frames with bad CRC */
#define PP_RxCTL_RUNT          0x2000	/* Accept runt frames */
#define PP_RxCTL_EXTRA         0x4000	/* Accept frames that are too long */
#define PP_TxCFG     0x0106	/* Transmit configuration */
#define PP_TxCFG_CRS           0x0040	/* Enable interrupt on loss of carrier */
#define PP_TxCFG_SQE           0x0080	/* Enable interrupt on Signal Quality Error */
#define PP_TxCFG_TxOK          0x0100	/* Enable interrupt on successful xmits */
#define PP_TxCFG_Late          0x0200	/* Enable interrupt on "out of window" */
#define PP_TxCFG_Jabber        0x0400	/* Enable interrupt on jabber detect */
#define PP_TxCFG_Collision     0x0800	/* Enable interrupt if collision */
#define PP_TxCFG_16Collisions  0x8000	/* Enable interrupt if > 16 collisions */
#define PP_TxCmd     0x0108	/* Transmit command status */
#define PP_TxCmd_TxStart_5     0x0000	/* Start after 5 bytes in buffer */
#define PP_TxCmd_TxStart_381   0x0040	/* Start after 381 bytes in buffer */
#define PP_TxCmd_TxStart_1021  0x0080	/* Start after 1021 bytes in buffer */
#define PP_TxCmd_TxStart_Full  0x00C0	/* Start after all bytes loaded */
#define PP_TxCmd_Force         0x0100	/* Discard any pending packets */
#define PP_TxCmd_OneCollision  0x0200	/* Abort after a single collision */
#define PP_TxCmd_NoCRC         0x1000	/* Do not add CRC */
#define PP_TxCmd_NoPad         0x2000	/* Do not pad short packets */
#define PP_BufCFG    0x010A	/* Buffer configuration */
#define PP_BufCFG_SWI          0x0040	/* Force interrupt via software */
#define PP_BufCFG_RxDMA        0x0080	/* Enable interrupt on Rx DMA */
#define PP_BufCFG_TxRDY        0x0100	/* Enable interrupt when ready for Tx */
#define PP_BufCFG_TxUE         0x0200	/* Enable interrupt in Tx underrun */
#define PP_BufCFG_RxMiss       0x0400	/* Enable interrupt on missed Rx packets */
#define PP_BufCFG_Rx128        0x0800	/* Enable Rx interrupt after 128 bytes */
#define PP_BufCFG_TxCol        0x1000	/* Enable int on Tx collision ctr overflow */
#define PP_BufCFG_Miss         0x2000	/* Enable int on Rx miss ctr overflow */
#define PP_BufCFG_RxDest       0x8000	/* Enable int on Rx dest addr match */
#define PP_LineCTL   0x0112	/* Line control */
#define PP_LineCTL_Rx          0x0040	/* Enable receiver */
#define PP_LineCTL_Tx          0x0080	/* Enable transmitter */
#define PP_LineCTL_AUIonly     0x0100	/* AUI interface only */
#define PP_LineCTL_AutoAUI10BT 0x0200	/* Autodetect AUI or 10BaseT interface */
#define PP_LineCTL_ModBackoffE 0x0800	/* Enable modified backoff algorithm */
#define PP_LineCTL_PolarityDis 0x1000	/* Disable Rx polarity autodetect */
#define PP_LineCTL_2partDefDis 0x2000	/* Disable two-part defferal */
#define PP_LineCTL_LoRxSquelch 0x4000	/* Reduce receiver squelch threshold */
#define PP_SelfCTL   0x0114	/* Chip self control */
#define PP_SelfCTL_Reset       0x0040	/* Self-clearing reset */
#define PP_SelfCTL_SWSuspend   40x0100	/* Initiate suspend mode */
#define PP_SelfCTL_HWSleepE    0x0200	/* Enable SLEEP input */
#define PP_SelfCTL_HWStandbyE  0x0400	/* Enable standby mode */
#define PP_SelfCTL_HC0E        0x1000	/* use HCB0 for LINK LED */
#define PP_SelfCTL_HC1E        0x2000	/* use HCB1 for BSTATUS LED */
#define PP_SelfCTL_HCB0        0x4000	/* control LINK LED if HC0E set */
#define PP_SelfCTL_HCB1        0x8000	/* control BSTATUS LED if HC1E set */
#define PP_BusCTL    0x0116	/* Bus control */
#define PP_BusCTL_ResetRxDMA   0x0040	/* Reset RxDMA pointer */
#define PP_BusCTL_DMAextend    0x0100	/* Extend DMA cycle */
#define PP_BusCTL_UseSA        0x0200	/* Assert MEMCS16 on address decode */
#define PP_BusCTL_MemoryE      0x0400	/* Enable memory mode */
#define PP_BusCTL_DMAburst     0x0800	/* Limit DMA access burst */
#define PP_BusCTL_IOCHRDYE     0x1000	/* Set IOCHRDY high impedence */
#define PP_BusCTL_RxDMAsize    0x2000	/* Set DMA buffer size 64KB */
#define PP_BusCTL_EnableIRQ    0x8000	/* Generate interrupt on interrupt event */
#define PP_TestCTL   0x0118	/* Test control */
#define PP_TestCTL_DisableLT   0x0080	/* Disable link status */
#define PP_TestCTL_ENDECloop   0x0200	/* Internal loopback */
#define PP_TestCTL_AUIloop     0x0400	/* AUI loopback */
#define PP_TestCTL_DisBackoff  0x0800	/* Disable backoff algorithm */
#define PP_TestCTL_FDX         0x4000	/* Enable full duplex mode */
#define PP_ISQ       0x0120	/* Interrupt Status Queue */
#define PP_RER       0x0124	/* Receive event */
#define PP_RER_IAHash          0x0040	/* Frame hash match */
#define PP_RER_Dribble         0x0080	/* Frame had 1-7 extra bits after last byte */
#define PP_RER_RxOK            0x0100	/* Frame received with no errors */
#define PP_RER_Hashed          0x0200	/* Frame address hashed OK */
#define PP_RER_IA              0x0400	/* Frame address matched IA */
#define PP_RER_Broadcast       0x0800	/* Broadcast frame */
#define PP_RER_CRC             0x1000	/* Frame had CRC error */
#define PP_RER_RUNT            0x2000	/* Runt frame */
#define PP_RER_EXTRA           0x4000	/* Frame was too long */
#define PP_TER       0x0128	/* Transmit event */
#define PP_TER_CRS             0x0040	/* Carrier lost */
#define PP_TER_SQE             0x0080	/* Signal Quality Error */
#define PP_TER_TxOK            0x0100	/* Packet sent without error */
#define PP_TER_Late            0x0200	/* Out of window */
#define PP_TER_Jabber          0x0400	/* Stuck transmit? */
#define PP_TER_NumCollisions   0x7800	/* Number of collisions */
#define PP_TER_16Collisions    0x8000	/* > 16 collisions */
#define PP_BER       0x012C	/* Buffer event */
#define PP_BER_SWint           0x0040	/* Software interrupt */
#define PP_BER_RxDMAFrame      0x0080	/* Received framed DMAed */
#define PP_BER_Rdy4Tx          0x0100	/* Ready for trsansmission */
#define PP_BER_TxUnderrun      0x0200	/* Transmit underrun */
#define PP_BER_RxMiss          0x0400	/* Received frame missed */
#define PP_BER_Rx128           0x0800	/* 128 bytes received */
#define PP_BER_RxDest          0x8000	/* Received framed passed address filter */
#define PP_RxMiss    0x0130	/*  Receiver miss counter */
#define PP_TxCol     0x0132	/*  Transmit collision counter */
#define PP_LineSTAT  0x0134	/* Line status */
#define PP_LineSTAT_LinkOK     0x0080	/* Line is connected and working */
#define PP_LineSTAT_AUI        0x0100	/* Connected via AUI */
#define PP_LineSTAT_10BT       0x0200	/* Connected via twisted pair */
#define PP_LineSTAT_Polarity   0x1000	/* Line polarity OK (10BT only) */
#define PP_LineSTAT_CRS        0x4000	/* Frame being received */
#define PP_SelfSTAT  0x0136	/* Chip self status */
#define PP_SelfSTAT_33VActive  0x0040	/* supply voltage is 3.3V */
#define PP_SelfSTAT_InitD      0x0080	/* Chip initialization complete */
#define PP_SelfSTAT_SIBSY      0x0100	/* EEPROM is busy */
#define PP_SelfSTAT_EEPROM     0x0200	/* EEPROM present */
#define PP_SelfSTAT_EEPROM_OK  0x0400	/* EEPROM checks out */
#define PP_SelfSTAT_ELPresent  0x0800	/* External address latch logic available */
#define PP_SelfSTAT_EEsize     0x1000	/* Size of EEPROM */
#define PP_BusSTAT   0x0138	/* Bus status */
#define PP_BusSTAT_TxBid       0x0080	/* Tx error */
#define PP_BusSTAT_TxRDY       0x0100	/* Ready for Tx data */
#define PP_TDR       0x013C	/* AUI Time Domain Reflectometer */
/* initiate transmit registers */
#define PP_TxCommand 0x0144	/* Tx Command */
#define PP_TxLength  0x0146	/* Tx Length */
/* address filter registers */
#define PP_LAF       0x0150	/* Logical address filter (6 bytes) */
#define PP_IA        0x0158	/* Individual address (MAC) */
/* EEPROM Kram */
#define SI_BUSY 0x0100
#define PP_EECMD 0x0040		/*  NVR Interface Command register */
#define PP_EEData 0x0042	/*  NVR Interface Data Register */
#define EEPROM_WRITE_EN		0x00F0
#define EEPROM_WRITE_DIS	0x0000
#define EEPROM_WRITE_CMD	0x0100
#define EEPROM_READ_CMD		0x0200
#define EEPROM_ERASE_CMD	0x0300
/* packet page register access functions */
static unsigned short get_reg_init_bus(int regno)
{
	/* force 16 bit busmode */
	volatile unsigned char c;
	c = CS8900_BUS16_0;
	c = CS8900_BUS16_1;
	c = CS8900_BUS16_0;
	c = CS8900_BUS16_1;
	c = CS8900_BUS16_0;

	CS8900_PPTR = regno;
	return CS8900_PDATA;
}

static unsigned short get_reg(int regno)
{
	CS8900_PPTR = regno;
	return CS8900_PDATA;
}

static void put_reg(int regno, unsigned short val)
{
	CS8900_PPTR = regno;
	CS8900_PDATA_W = val;
}

static void eth_reset(void)
{
	int tmo;
	unsigned short us;

	/* reset NIC */
	put_reg(PP_SelfCTL, get_reg(PP_SelfCTL) | PP_SelfCTL_Reset);

	/* wait for 200ms */
	udelay(200000);
	/* Wait until the chip is reset */
	tmo = timer_read();
	/* wait for 1 secs max */
	while ((((us = get_reg_init_bus(PP_SelfSTAT)) & PP_SelfSTAT_InitD) == 0)
	       && (timer_delta(tmo, timer_read()) < 1 * 1000))
		/*NOP*/;
}

static void eth_reginit(void)
{
	/* receive only error free packets addressed to this card */
	put_reg(PP_RxCTL, PP_RxCTL_IA | PP_RxCTL_Broadcast | PP_RxCTL_RxOK);
	/* do not generate any interrupts on receive operations */
	put_reg(PP_RxCFG, 0);
	/* do not generate any interrupts on transmit operations */
	put_reg(PP_TxCFG, 0);
	/* do not generate any interrupts on buffer operations */
	put_reg(PP_BufCFG, 0);
	/* enable transmitter/receiver mode */
	put_reg(PP_LineCTL, PP_LineCTL_Rx | PP_LineCTL_Tx);
}

static void cs8900_close(struct descriptor_d *d)
{
	/* disable transmitter/receiver mode */
	put_reg(PP_LineCTL, 0);

	/* "shutdown" to show ChipID or kernel wouldn't find the cs8900 ... */
	get_reg_init_bus(PP_ChipID);
	close_helper(d);
}

static void cs8900_init(void)
{
	initialized = 0;
	/* verify chip id */
	if ((phy_id = get_reg_init_bus(PP_ChipID)) != 0x630e) {
		printf("CS8900 Ethernet chip not found?! id:0x%lx\n", phy_id);
		return;
	}
	head_rx = count_rx = 0;
	initialized = 1;
	
	/****#### need to be fixed */ 
	    host_mac_address[0] = 0x00;
	host_mac_address[1] = 0x08;
	host_mac_address[2] = 0xee;
	host_mac_address[3] = 0x00;
	host_mac_address[4] = 0x80;
	host_mac_address[5] = 0x44;

	eth_reset();
	/* set the ethernet address */
	put_reg(PP_IA + 0, host_mac_address[0] | (host_mac_address[1] << 8));
	put_reg(PP_IA + 2, host_mac_address[2] | (host_mac_address[3] << 8));
	put_reg(PP_IA + 4, host_mac_address[4] | (host_mac_address[5] << 8));

	eth_reginit();
}

/* Get a data block via Ethernet */
static int eth_rx(void)
{
	int i;
	u16 rxlen;
	u16 *addr;
	u16 status;
	status = get_reg(PP_RER);

	if ((status & PP_RER_RxOK) == 0)
		return 0;

	status = CS8900_RxDATA;	/* stat */
	rxlen = CS8900_RxDATA;	/* len */

	/* printf ("receiving 0x%04x 0x%04x  head %d count %d\n",
	   status, rxlen, head_rx, count_rx);
	 */

	if (count_rx < C_RX_BUFFER) {

		addr = (u16 *) (rgbRxBuffer + head_rx * CB_RX_BUFFER + 2);
		for (i = rxlen >> 1; i > 0; i--)
			*addr++ = CS8900_RxDATA;

		if (rxlen & 1)
			*addr++ = CS8900_RxDATA;

		*(u16 *) & rgbRxBuffer[head_rx * CB_RX_BUFFER] = rxlen;
		head_rx = (head_rx + 1) % C_RX_BUFFER;
		++count_rx;
	} else {
#ifdef DEBUG
		printf("dropping receive packet\n");
		
#endif /* DEBUG */
		    /* If packet doesn't fit in buffer, skip it */ 
		    status = get_reg(PP_RxCFG);
		put_reg(PP_RxCFG, status | PP_RxCFG_Skip1);
		rxlen = 0;
	}

	return rxlen;
}

/* Send a data block via Ethernet. */
static int cs8900_send(struct descriptor_d *d, const void *packet, size_t size)
{
	u16 *addr;
	unsigned long tmo;
	unsigned short s;
	int length = (int)size;	/* this takes care of odd length pkts */

	eth_rx();		/* Receive when we can */

      retry:
	/* initiate a transmit sequence */
	CS8900_TxCMD = PP_TxCmd_TxStart_Full;
	CS8900_TxLEN = length;

	/* Test to see if the chip has allocated memory for the packet */
	if ((get_reg(PP_BusSTAT) & PP_BusSTAT_TxRDY) == 0) {
		/* Oops... this should not happen! */
#ifdef DEBUG
		printf("cs: unable to send packet; retrying...\n");
#endif
		/* Delay before accessing PHY again */
		tmo = timer_read();
		/* wait for 5 secs */
		while (timer_delta(tmo, timer_read()) < 5 * 1000) ;
		eth_reset();
		eth_reginit();
		goto retry;
	}

	/* Write the contents of the packet */
	/* assume even number of bytes */
	for (addr = (u16 *) packet; length > 0; length -= 2)
		CS8900_TxDATA = *addr++;
	
	    /* wait for transfer to succeed */
	    tmo = timer_read();
	while ((s = get_reg(PP_TER) & ~0x1F) == 0) {
		/* if more than 5 sec break */
		if (timer_delta(tmo, timer_read()) >= 5 * 1000) {
			
#ifdef DEBUG
			    printf("\ntransmission timeout%d /%d\n", tmo,
				   timer_read());
			
#endif /* DEBUG */
			    break;
	}
	}

	/* nothing */ ;
	if ((s & (PP_TER_CRS | PP_TER_TxOK)) != PP_TER_TxOK) {
#ifdef DEBUG
		printf("\ntransmission error %#x\n", s);
#endif /* DEBUG */
	} else {
		
#ifdef DEBUG
		    printf("\ntransmission good %#x len%d\n", s,
			   (u32) ((u32) addr - (u32) packet));
		
#endif /* DEBUG */
	}

	return 0;
}

static int cs8900_open(struct descriptor_d *d)
{
	return 0;		/* No problems */
}

static ssize_t cs8900_read(struct descriptor_d *d, void *pv, size_t cb)
{
	u16 length;
	int buffer;

	eth_rx();

	if (count_rx == 0)
		return 0;

	buffer = (head_rx + C_RX_BUFFER - count_rx) % C_RX_BUFFER;

	length = *(u16 *) & rgbRxBuffer[buffer * CB_RX_BUFFER];
	if (length > cb)
		length = cb;
	memcpy(pv, &rgbRxBuffer[buffer * CB_RX_BUFFER + 2], length);

#ifdef DEBUG
	printf("RX:%d\n", length);
	
#endif	/* DEBUG */
	--count_rx;
	    return length;
}
static __driver_4 struct driver_d cs8900_driver = {
	.name = DRIVER_NAME,
	.description = "CS8900 Ethernet driver",
	.flags = DRIVER_NET,
	.open = cs8900_open,
	.close = cs8900_close,
	.read = cs8900_read,
	.write = cs8900_send,
};

#if !defined (CONFIG_SMALL)
static void cs8900_report(void)
{
	if (!initialized)
		return;

	printf("  CS8900: Chip_id 0x%lx"
	       "  mac_addr %02x:%02x:%02x:%02x:%02x:%02x\n",
	       phy_id,
	       host_mac_address[0], host_mac_address[1],
	       host_mac_address[2], host_mac_address[3],
	       host_mac_address[4], host_mac_address[5]);
}
#endif /* CONFIG_SMALL */

static __service_6 struct service_d cs8900_service = {
	.init = cs8900_init,
#if !defined (CONFIG_SMALL)
	.report = cs8900_report,
#endif
};

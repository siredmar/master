/*
 * Header file for I2C support on PNX010x/4008.
 *
 * Author: Dennis Kovalev <dkovalev@ru.mvista.com>
 * adapted to linux 3.3 by Hans-Frieder Vogt <hfvogt@gmx.net>
 *
 * 2004-2006 (c) MontaVista Software, Inc.
 * 2012 (c) Jörg Gittinger
 *
 * This file is licensed under * the terms of the GNU General Public License
 * version 2. This program * is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */

#ifndef __I2C_PNX_H__
#define __I2C_PNX_H__

struct platform_device;
struct clk;

struct i2c_pnx_mif {
	struct mutex		lock;
	int			mode;		/* Interface mode */
	struct i2c_msg          *msgs;
	int                     msgs_num;
	int                     msgs_tx_idx;
	int                     msgs_rx_idx;
	u8                      *tx_buf;        /* current TX data buffer */
	int                     tx_buf_len;     /* TX data buffer length */
	u8                      *rx_buf;        /* current RX data buffer */
	int                     rx_buf_len;     /* RX data buffer length */
	volatile int            state;
	struct completion	complete;	/* I/O completion */
	int                     cmd_ret;        /* Return value */
	struct timer_list	timer;		/* Timeout */
};

struct i2c_pnx_algo_data {
	void __iomem		*ioaddr;
	struct i2c_pnx_mif	mif;
	int			last;
	struct clk		*clk;
	struct i2c_adapter	adapter;
	phys_addr_t		base;
	int			irq;
	u32			timeout;
};

#endif /* __I2C_PNX_H__ */

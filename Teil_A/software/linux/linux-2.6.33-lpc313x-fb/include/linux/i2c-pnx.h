/*
 * Header file for I2C support on PNX010x/4008.
 *
 * Author: Dennis Kovalev <dkovalev@ru.mvista.com>
 *
 * 2004-2006 (c) MontaVista Software, Inc. 
 * 2012 (c) Jörg Gittinger
 * This file is licensed under the terms of the GNU General Public License 
 * version 2. This program is licensed "as is" without any warranty of any 
 * kind, whether express or implied.
 */

#ifndef __I2C_PNX_H__
#define __I2C_PNX_H__

#include <linux/pm.h>

struct platform_device;

struct i2c_pnx_algo_data {
	u32			base;
	u32			ioaddr;
	int			irq;
};

struct i2c_pnx_data {
	int (*suspend) (struct platform_device *pdev, pm_message_t state);
	int (*resume) (struct platform_device *pdev);
	u32 (*calculate_input_freq) (struct platform_device *pdev);
	int (*set_clock_run) (struct platform_device *pdev);
	int (*set_clock_stop) (struct platform_device *pdev);
	struct i2c_adapter *adapter;
	
	struct mutex		lock;
	int			mode;		/* I2C TX read or write */
	struct i2c_msg 		*msgs;
	int			msgs_num;
	int			msgs_tx_idx;
	int			msgs_rx_idx;
	u8			*tx_buf;	/* current TX data buffer */
	int			tx_buf_len;	/* TX data buffer length */
	u8			*rx_buf;	/* current RX data buffer */
	int			rx_buf_len;	/* RX data buffer length */
	volatile int		state;
	struct completion	cmd_complete;	/* I/O completion */
	int			cmd_ret;	/* Return value */
	struct timer_list	timer;		/* Timeout */
};

#endif /* __I2C_PNX_H__ */

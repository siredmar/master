/*
 * Provides I2C support for Philips PNX010x/PNX4008 boards.
 *
 * Authors: Dennis Kovalev <dkovalev@ru.mvista.com>
 *	    Vitaly Wool <vwool@ru.mvista.com>
 *          Jörg Gittinger <sw@gittingerbox.de>
 * adapted to linux 3.3 by Hans-Frieder Vogt <hfvogt@gmx.net>
 *
 * 2004-2006 (c) MontaVista Software, Inc.
 * 2012 (c) Jörg Gittinger
 * This file is licensed under the terms of the GNU General Public License
 * version 2. This program is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */

#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <linux/delay.h>
#include <linux/i2c.h>
#include <linux/timer.h>
#include <linux/completion.h>
#include <linux/platform_device.h>
#include <linux/i2c-pnx.h>
#include <linux/io.h>
#include <linux/err.h>
#include <linux/clk.h>
#include <linux/slab.h>
#include <linux/of_i2c.h>

#define I2C_PNX_TIMEOUT_DEFAULT		100 /* msec */
#define I2C_PNX_SPEED_KHZ_DEFAULT	100
#define I2C_PNX_REGION_SIZE		0x100

/* state definitions */
#define I2C_PNX_STATE_TX_FINISHED	(1<<0)
#define I2C_PNX_STATE_ABORTED		(1<<1)
#define I2C_PNX_STATE_EXPECT_RX		(1<<2)
#define I2C_PNX_STATE_DONE		(1<<3)
#define I2C_PNX_STATE_TX_BY_XFER	(1<<4)  /* ISR must not TX */
#define I2C_PNX_STATE_ABORT_PENDING	(1<<5)
#define I2C_PNX_STATE_NAI_OCCURRED	(1<<6)

enum {
	mstatus_tdi = 0x00000001,
	mstatus_afi = 0x00000002,
	mstatus_nai = 0x00000004,
	mstatus_drmi = 0x00000008,
	mstatus_active = 0x00000020,
	mstatus_scl = 0x00000040,
	mstatus_sda = 0x00000080,
	mstatus_rff = 0x00000100,
	mstatus_rfe = 0x00000200,
	mstatus_tff = 0x00000400,
	mstatus_tfe = 0x00000800,
};

enum {
	mcntrl_tdie = 0x00000001,
	mcntrl_afie = 0x00000002,
	mcntrl_naie = 0x00000004,
	mcntrl_drmie = 0x00000008,
	mcntrl_daie = 0x00000020,
	mcntrl_rffie = 0x00000040,
	mcntrl_tffie = 0x00000080,
	mcntrl_reset = 0x00000100,
	mcntrl_cdbmode = 0x00000400,
};

enum {
	rw_bit = 1 << 0,
	start_bit = 1 << 8,
	stop_bit = 1 << 9,
};

#define I2C_REG_RX(a)	((a)->ioaddr)		/* Rx FIFO reg (RO) */
#define I2C_REG_TX(a)	((a)->ioaddr)		/* Tx FIFO reg (WO) */
#define I2C_REG_STS(a)	((a)->ioaddr + 0x04)	/* Status reg (RO) */
#define I2C_REG_CTL(a)	((a)->ioaddr + 0x08)	/* Ctl reg */
#define I2C_REG_CKL(a)	((a)->ioaddr + 0x0c)	/* Clock divider low */
#define I2C_REG_CKH(a)	((a)->ioaddr + 0x10)	/* Clock divider high */
#define I2C_REG_ADR(a)	((a)->ioaddr + 0x14)	/* I2C address */
#define I2C_REG_RFL(a)	((a)->ioaddr + 0x18)	/* Rx FIFO level (RO) */
#define I2C_REG_TFL(a)	((a)->ioaddr + 0x1c)	/* Tx FIFO level (RO) */
#define I2C_REG_RXB(a)	((a)->ioaddr + 0x20)	/* Num of bytes Rx-ed (RO) */
#define I2C_REG_TXB(a)	((a)->ioaddr + 0x24)	/* Num of bytes Tx-ed (RO) */
#define I2C_REG_TXS(a)	((a)->ioaddr + 0x28)	/* Tx slave FIFO (RO) */
#define I2C_REG_STFL(a)	((a)->ioaddr + 0x2c)	/* Tx slave FIFO level (RO) */

static inline int wait_timeout(struct i2c_pnx_algo_data *data)
{
	long timeout = data->timeout;
	while (timeout > 0 &&
			(ioread32(I2C_REG_STS(data)) & mstatus_active)) {
		mdelay(1);
		timeout--;
	}
	return (timeout <= 0);
}

static inline int wait_reset(struct i2c_pnx_algo_data *data)
{
	long timeout = data->timeout;
	while (timeout > 0 &&
			(ioread32(I2C_REG_CTL(data)) & mcntrl_reset)) {
		mdelay(1);
		timeout--;
	}
	return (timeout <= 0);
}

static inline void i2c_pnx_arm_timer(struct i2c_pnx_algo_data *algo_data)
{
	struct timer_list *timer = &algo_data->mif.timer;
	unsigned long expires = msecs_to_jiffies(algo_data->timeout);

	if (expires <= 1)
		expires = 2;

	del_timer_sync(timer);

	dev_dbg(&algo_data->adapter.dev, "Timer armed at %lu plus %lu jiffies.\n",
		jiffies, expires);

	timer->expires = jiffies + expires;
	timer->data = (unsigned long)algo_data;

	add_timer(timer);
}

static inline void i2c_pnx_unarm_timer(struct i2c_pnx_algo_data *algo_data)
{
	struct timer_list *timer = &algo_data->mif.timer;

	del_timer_sync(timer);
}

/**
 * i2c_pnx_master_tx - transmit data to slave
 * @adap:		pointer to I2C adapter structure
 *
 * Sends one byte of data to the slave
 */
static int i2c_pnx_master_tx(struct i2c_pnx_algo_data *algo_data)
{
	u32 val;

	dev_dbg(&algo_data->adapter.dev, "%s(): entering: stat = %04x.\n",
		__func__, ioread32(I2C_REG_STS(algo_data)));

	BUG_ON(algo_data->mif.tx_buf_len == 0);

	/* We still have something to talk about... */
	val = *algo_data->mif.tx_buf++;

	/* last byte of all messages? */
	if ((algo_data->mif.tx_buf_len == 1) && 
		(algo_data->mif.msgs_tx_idx == algo_data->mif.msgs_num-1))
		val |= stop_bit;

	algo_data->mif.tx_buf_len--;
	iowrite32(val, I2C_REG_TX(algo_data));

	dev_dbg(&algo_data->adapter.dev, "%s(): tx %#x [%d]\n", __func__,
		val, algo_data->mif.tx_buf_len + 1);

	return 0;
}

/**
 * i2c_pnx_master_tx_rcv - fill TX FIFO for receive
 * @adap:		pointer to I2C adapter structure
 *
 * Writes a byte to the TX FIFO required for receiving
 */
static int i2c_pnx_master_tx_rcv(struct i2c_pnx_algo_data *algo_data)
{
	unsigned int val = 0;

	dev_dbg(&algo_data->adapter.dev, "%s(): entering: stat = %04x.\n",
		__func__, ioread32(I2C_REG_STS(algo_data)));


	if ((algo_data->mif.tx_buf_len == 1)
		&& (algo_data->mif.msgs_tx_idx == algo_data->mif.msgs_num-1)) {
		/* Last byte, do not acknowledge next rcv. */
		val |= stop_bit;
		dev_dbg(&algo_data->adapter.dev, "%s(): sending stop bit.\n",
			__func__);
	}

	algo_data->mif.tx_buf_len--;
	/*
	 * Now we'll 'ask' for data:
	 * For each byte we want to receive, we must
	 * write a (dummy) byte to the Tx-FIFO.
	 */
	iowrite32(val, I2C_REG_TX(algo_data));

	return 0;
}

/**
 * i2c_pnx_master_rcv - receive data from slave
 * @adap:		pointer to I2C adapter structure
 *
 * Reads bytes from the RX FIFO
 */
static int i2c_pnx_master_rcv(struct i2c_pnx_algo_data *algo_data)
{
	unsigned int val = 0;
	int count = 16; /* max 16 bytes in order to burden isr not too much */
	u32 sts;

	dev_dbg(&algo_data->adapter.dev, "%s(): entering: stat = %04x, rx len = %d.\n",
		__func__, ioread32(I2C_REG_STS(algo_data)),
		algo_data->mif.rx_buf_len);

	BUG_ON(algo_data->mif.rx_buf_len == 0);
	/* Handle data. */
	do {
        /* READ VAL TO FIFO */
		val = ioread32(I2C_REG_RX(algo_data));
		*algo_data->mif.rx_buf++ = (u8) (val & 0xff);
		dev_dbg(&algo_data->adapter.dev, "%s(): rcv 0x%x [%d]\n", __func__, val,
			algo_data->mif.rx_buf_len);

		algo_data->mif.rx_buf_len--;
		sts = ioread32(I2C_REG_STS(algo_data));
		count--;
	} while(algo_data->mif.rx_buf_len
		&& count
		&& !(sts & mstatus_rfe));

	if (algo_data->mif.rx_buf_len == 0) {
		/* we are finished with receiving */
		algo_data->mif.state &= ~I2C_PNX_STATE_EXPECT_RX;
	}

	dev_dbg(&algo_data->adapter.dev, "%s(): exiting: stat = %04x, rx len = %d, state = %x.\n",
		__func__,
		ioread32(I2C_REG_STS(algo_data)),
		algo_data->mif.rx_buf_len,
		algo_data->mif.state);

	return 0;
}

static void
i2c_pnx_state_cmd_complete(struct i2c_pnx_algo_data *algo_data)
{
	u32 ctl;

	dev_dbg(&algo_data->adapter.dev, "%s(): entering: stat = %04x.\n",
		__func__, ioread32(I2C_REG_STS(algo_data)));
	
	/* Disable master interrupts */
	ctl = ioread32(I2C_REG_CTL(algo_data));
	ctl &= ~(mcntrl_afie | mcntrl_naie | mcntrl_rffie |
		 mcntrl_drmie | mcntrl_daie | mcntrl_tdie);
	iowrite32(ctl, I2C_REG_CTL(algo_data));

	/* Kill timer. */
	i2c_pnx_unarm_timer(algo_data);
	/*i2c_pnx->cmd_ret = 0;*/
	complete(&algo_data->mif.complete);

	dev_dbg(&algo_data->adapter.dev, "%s(): exiting: ret = %d, stat = %04x.\n",
		__func__, algo_data->mif.cmd_ret, ioread32(I2C_REG_STS(algo_data)));
}

static void
i2c_pnx_state_abort(struct i2c_pnx_algo_data *algo_data, int rc)
{
	u32 ctl;

	dev_dbg(&algo_data->adapter.dev, "%s(): abort: stat = %04x.\n",
		__func__, ioread32(I2C_REG_STS(algo_data)));

	/* Disable master interrupts */
	ctl = ioread32(I2C_REG_CTL(algo_data));
	ctl &= ~(mcntrl_afie | mcntrl_naie | mcntrl_rffie |
		 mcntrl_drmie | mcntrl_daie | mcntrl_tdie);
	/* Reset I2C engine */
	ctl |= mcntrl_reset;
	
	iowrite32(ctl, I2C_REG_CTL(algo_data));

	/* Kill timer */
	i2c_pnx_unarm_timer(algo_data);
	algo_data->mif.cmd_ret = rc;
	algo_data->mif.state |= I2C_PNX_STATE_ABORTED;
	complete(&algo_data->mif.complete);
}

static void
i2c_pnx_state_abort_with_stop(struct i2c_pnx_algo_data *algo_data, int rc)
{
	u32 ctl;

	dev_dbg(&algo_data->adapter.dev, "%s(): abort: stat = %04x.\n",
		__func__, ioread32(I2C_REG_STS(algo_data)));

	/* Write a STOP bit to TX FIFO */
	iowrite32(0x00 | stop_bit, I2C_REG_TX(algo_data));

	/* Disable master interrupts except for stop*/
	ctl = ioread32(I2C_REG_CTL(algo_data));
	ctl &= ~(mcntrl_afie | mcntrl_naie | mcntrl_rffie |
		 mcntrl_drmie | mcntrl_daie/* | mcntrl_tdie*/);
	iowrite32(ctl, I2C_REG_CTL(algo_data));

	/* set error code */
	algo_data->mif.cmd_ret = rc;
}


static int
i2c_pnx_tx_start(struct i2c_pnx_algo_data *algo_data)
{
	struct i2c_msg *pmsg;
	int rc = 0;
	u8 addr;
	u32 val;

	pmsg = &algo_data->mif.msgs[algo_data->mif.msgs_tx_idx];
	addr = pmsg->addr;

	/* Check for 7 bit slave addresses only */
	if (addr & ~0x7f) {
		dev_err(&algo_data->adapter.dev, "%s: Invalid slave address %x. "
		       "Only 7-bit addresses are supported\n",
		       algo_data->adapter.name, addr);
		return -EINVAL;
	}

	/* tx start condition */
	if (pmsg->flags & I2C_M_RD) {
		u32 ctl;
		/* for now we accept only one RD per set of messages */
		BUG_ON(algo_data->mif.rx_buf != NULL);
		if (pmsg->len == 0) {
			dev_err(&algo_data->adapter.dev, "%s: Receive buffer size is zero.\n",
					algo_data->adapter.name);
			return -EINVAL;
		}
		algo_data->mif.rx_buf = pmsg->buf;
		algo_data->mif.rx_buf_len = pmsg->len;
		algo_data->mif.mode = I2C_SMBUS_READ;
		algo_data->mif.state |= I2C_PNX_STATE_EXPECT_RX;
		/* the LPC3131 I2C engine requires TX FIFO to be fed
		 * even in RX mode. For each byte to be received
		 * TX FIFO must be fed. Therefore we set tx_buf_len
		 * as well.
		 */
		algo_data->mif.tx_buf_len = pmsg->len;

		/* Enable data available interrupt and 
		 * Receive FIFO full interrupt */
		ctl = ioread32(I2C_REG_CTL(algo_data));
		ctl |= mcntrl_rffie | mcntrl_daie;
		iowrite32(ctl, I2C_REG_CTL(algo_data));
	} else {
		algo_data->mif.tx_buf = pmsg->buf;
		algo_data->mif.tx_buf_len = pmsg->len;
		algo_data->mif.mode = I2C_SMBUS_WRITE;
	}
	dev_dbg(&algo_data->adapter.dev, "%s(): addr %02x, mode %d, %d bytes\n", __func__,
                addr,
		algo_data->mif.mode,
		pmsg->len);

	/* Write the slave address, START bit and R/W bit */
	val = (addr << 1) | start_bit | algo_data->mif.mode;
	if (pmsg->len == 0)
		val |= stop_bit;

	dev_dbg(&algo_data->adapter.dev, "%s(): sending %#x\n", __func__,
		val);

	iowrite32(val, I2C_REG_TX(algo_data));

	return rc;
}

static int
i2c_pnx_state_next_tx(struct i2c_pnx_algo_data *algo_data)
{
	int rc = 0;
	int count = 10;	/* max 10 in order not to burden isr too much */
	u32 ctl;

	do {
		dev_dbg(&algo_data->adapter.dev, "%s(): tx next byte: stat = %04x.\n",
			__func__, ioread32(I2C_REG_STS(algo_data)));

		if (algo_data->mif.tx_buf_len == 0) {
			/* no more data in current message */
			if (likely(algo_data->mif.msgs_tx_idx < algo_data->mif.msgs_num-1)) {
				/* switch to next message and send (re-)start */
				algo_data->mif.msgs_tx_idx++;
				rc = i2c_pnx_tx_start(algo_data);
			} else {
				/* end of transmission */
				dev_dbg(&algo_data->adapter.dev, "%s(): end of transmission! stat = %04x.\n",
					__func__, ioread32(I2C_REG_STS(algo_data)));
				algo_data->mif.state |= I2C_PNX_STATE_TX_FINISHED;
				/* disable Transmit FIFO Not Full Interrupt */
				ctl = ioread32(I2C_REG_CTL(algo_data));
				ctl &= ~mcntrl_tffie;
				iowrite32(ctl, I2C_REG_CTL(algo_data));
			}	
		} else {
			/* still data to transmit. */
			if (algo_data->mif.mode == I2C_SMBUS_READ) {
				rc = i2c_pnx_master_tx_rcv(algo_data);
			} else {
				rc = i2c_pnx_master_tx(algo_data);
			}
		}
		count--;
	} while(!rc 
		&& count
		&& !(ioread32(I2C_REG_STS(algo_data)) & mstatus_tff)
		&& !(algo_data->mif.state & 
			(I2C_PNX_STATE_TX_FINISHED |
			I2C_PNX_STATE_ABORTED |
			I2C_PNX_STATE_ABORT_PENDING |
			I2C_PNX_STATE_NAI_OCCURRED))); 

	return rc;
}

static irqreturn_t i2c_pnx_interrupt(int irq, void *dev_id)
{
	struct i2c_pnx_algo_data *algo_data = dev_id;
	u32 stat, rc;

	dev_dbg(&algo_data->adapter.dev,
		"%s(): mstat = %x mctrl = %x, mode = %d, state = %x\n",
		__func__,
		ioread32(I2C_REG_STS(algo_data)),
		ioread32(I2C_REG_CTL(algo_data)),
		algo_data->mif.mode,
		algo_data->mif.state);
	stat = ioread32(I2C_REG_STS(algo_data));

	/* let's see what kind of event this is */
	if (stat & mstatus_afi) {
		/* We lost arbitration in the midst of a transfer */
		i2c_pnx_state_abort(algo_data, -EIO);
	} else if (stat & mstatus_nai) {
		/* Slave did not acknowledge, generate a STOP */
		dev_dbg(&algo_data->adapter.dev,
			"%s(): Slave did not acknowledge.\n",
			__func__);

		if (algo_data->mif.state & I2C_PNX_STATE_TX_BY_XFER) {
			/* we have interrupted i2c_pnx_xfer() - do not
			 * abort at this time because we may interfere
			 * with writing to TX FIFO at i2c_pnx_xfer() */
			algo_data->mif.state |= I2C_PNX_STATE_NAI_OCCURRED;
			/* disable not-acknowledge and transaction-done interrupt */
			iowrite32(ioread32(I2C_REG_CTL(algo_data)) &
				~(mcntrl_naie | mcntrl_tdie),
				I2C_REG_CTL(algo_data));
			dev_dbg(&algo_data->adapter.dev, "%s(): Delayed abort.\n", __func__);
			goto irq_done;
		}

		if (algo_data->mif.mode == I2C_SMBUS_READ) {
			/* in case of a read we must perform a 'hard' abort. */
			i2c_pnx_state_abort(algo_data, -EREMOTEIO);
		} else {
			i2c_pnx_state_abort_with_stop(algo_data, -EREMOTEIO);
		}
		goto irq_done;
	}
	if (!(stat & mstatus_tff)) {
		/* TX FIFO not full */
		if (!(algo_data->mif.state &
			(I2C_PNX_STATE_TX_FINISHED |
			I2C_PNX_STATE_TX_BY_XFER |
			I2C_PNX_STATE_ABORT_PENDING))) {
			/* TX FIFO wants to be fed */
			rc = i2c_pnx_state_next_tx(algo_data);
			if (rc < 0) {
				i2c_pnx_state_abort(algo_data, rc);
				goto irq_done;
			}
		}
	}
	if (!(stat & mstatus_rfe)) {
		/* Valid data in RX FIFO */
		if (algo_data->mif.state & I2C_PNX_STATE_EXPECT_RX) {
			/* we expect data */
			rc = i2c_pnx_master_rcv(algo_data);
			if (rc < 0) {
				i2c_pnx_state_abort(algo_data, rc);
				goto irq_done;
			}
		}
	}

	/* Final check. Re-read status to see if 
	 * transaction is done now after receiving. */
	stat = ioread32(I2C_REG_STS(algo_data));
	if (stat & mstatus_tdi) {
		/* Transaction done */
		algo_data->mif.state |= I2C_PNX_STATE_DONE;
		dev_dbg(&algo_data->adapter.dev, "%s(): "
			"Transaction done. state = %x\n",
			__func__,
			algo_data->mif.state);
	}

	/* Check for end of transaction */
	if ((algo_data->mif.state & (I2C_PNX_STATE_TX_FINISHED |
		I2C_PNX_STATE_EXPECT_RX |
		I2C_PNX_STATE_DONE))
		== (I2C_PNX_STATE_TX_FINISHED | I2C_PNX_STATE_DONE)) {
		/* command done */
		if (algo_data->mif.state & I2C_PNX_STATE_ABORT_PENDING) {
			/* this was a delayed abort (STOP bit) */
			algo_data->mif.state &= ~I2C_PNX_STATE_ABORT_PENDING;
			algo_data->mif.state |= I2C_PNX_STATE_ABORTED;
		}

		i2c_pnx_state_cmd_complete(algo_data);
	}
irq_done:
	/* Clear TDI and AFI bits */
	stat = ioread32(I2C_REG_STS(algo_data));
	stat |= mstatus_tdi | mstatus_afi;
	if (!(algo_data->mif.state & I2C_PNX_STATE_DONE)) {
		/* We are not done yet. Do not clear a possible mstatus_tdi.
		 * Otherwise we might lose this notificaiton. */
		stat &= ~mstatus_tdi;
	}
	iowrite32(stat, I2C_REG_STS(algo_data));

	dev_dbg(&algo_data->adapter.dev,
		"%s(): exiting, stat = %x ctrl = %x, state = %x.\n",
		 __func__, ioread32(I2C_REG_STS(algo_data)),
		 ioread32(I2C_REG_CTL(algo_data)),
		algo_data->mif.state);

	return IRQ_HANDLED;
}

static void i2c_pnx_timeout(unsigned long data)
{
	struct i2c_pnx_algo_data *algo_data = (struct i2c_pnx_algo_data *)data;

	dev_err(&algo_data->adapter.dev,
		"Master timed out. stat = %04x, cntrl = %04x, state = %x. "
		"Resetting master...\n",
		ioread32(I2C_REG_STS(algo_data)),
		ioread32(I2C_REG_CTL(algo_data)),
		algo_data->mif.state);

	/* Reset master and disable interrupts */
	i2c_pnx_state_abort(algo_data, -EIO);
}

static inline void bus_reset_if_active(struct i2c_pnx_algo_data *algo_data)
{
	u32 stat;

	if ((stat = ioread32(I2C_REG_STS(algo_data))) & mstatus_active) {
		dev_err(&algo_data->adapter.dev,
			"%s: Bus is still active after xfer. Reset it...\n",
			algo_data->adapter.name);
		iowrite32(ioread32(I2C_REG_CTL(algo_data)) | mcntrl_reset,
			  I2C_REG_CTL(algo_data));
		wait_reset(algo_data);
	} else if (!(stat & mstatus_rfe) || !(stat & mstatus_tfe)) {
		/* If there is data in the fifo's after transfer,
		 * flush fifo's by reset.
		 */
		iowrite32(ioread32(I2C_REG_CTL(algo_data)) | mcntrl_reset,
			  I2C_REG_CTL(algo_data));
		wait_reset(algo_data);
	} else if (stat & mstatus_nai) {
		iowrite32(ioread32(I2C_REG_CTL(algo_data)) | mcntrl_reset,
			  I2C_REG_CTL(algo_data));
		wait_reset(algo_data);
	}
}

/**
 * i2c_pnx_xfer - generic transfer entry point
 * @adap:		pointer to I2C adapter structure
 * @msgs:		array of messages
 * @num:		number of messages
 *
 * Initiates the transfer
 */
static int
i2c_pnx_xfer(struct i2c_adapter *adap, struct i2c_msg *msgs, int num)
{
	int rc = 0;
	struct i2c_pnx_algo_data *algo_data = adap->algo_data;
	u32 stat = ioread32(I2C_REG_STS(algo_data));
	u32 ctl;

	dev_dbg(&algo_data->adapter.dev,
		"%s(): entering: %d messages, stat = %04x.\n",
		__func__, num, ioread32(I2C_REG_STS(algo_data)));

	mutex_lock(&algo_data->mif.lock);
	init_completion(&algo_data->mif.complete);

	bus_reset_if_active(algo_data);

	/* First, make sure bus is idle */
	if (wait_timeout(algo_data)) {
		/* Somebody else is monopolizing the bus */
		dev_err(&algo_data->adapter.dev, "%s: Bus busy. Slave addr = %02x, "
			"cntrl = %x, stat = %x\n",
			algo_data->adapter.name, msgs[0].addr,
			ioread32(I2C_REG_CTL(algo_data)),
			ioread32(I2C_REG_STS(algo_data)));
		mutex_unlock(&algo_data->mif.lock);
		return -EBUSY;
	} else if (ioread32(I2C_REG_STS(algo_data)) & mstatus_afi) {
		/* Sorry, we lost the bus */
		dev_err(&algo_data->adapter.dev, "%s: Arbitration failure. ",
			algo_data->adapter.name);
		mutex_unlock(&algo_data->mif.lock);
		return -EIO;
	}

	/* initialize transfers */
	algo_data->mif.msgs = msgs;
	algo_data->mif.msgs_num = num;
	algo_data->mif.msgs_tx_idx = 0;
	algo_data->mif.msgs_rx_idx = 0;
	algo_data->mif.tx_buf = NULL;
	algo_data->mif.tx_buf_len = 0;
	algo_data->mif.rx_buf = NULL;
	algo_data->mif.rx_buf_len = 0;
	algo_data->mif.cmd_ret = 0;
	algo_data->mif.state = 0;

	i2c_pnx_arm_timer(algo_data);

	/* enable transaction error and done interrupts */
	ctl = ioread32(I2C_REG_CTL(algo_data));
	ctl |= mcntrl_afie | mcntrl_naie | mcntrl_tdie;
	iowrite32(ctl, I2C_REG_CTL(algo_data));

	/* start I2C transfer */
	rc = i2c_pnx_tx_start(algo_data);

	algo_data->mif.state = I2C_PNX_STATE_TX_BY_XFER;
	/* feed TX FIFO */
	rc = i2c_pnx_state_next_tx(algo_data);
	/* from now on let ISR handle remaining TX bytes */
	algo_data->mif.state &= ~I2C_PNX_STATE_TX_BY_XFER;

	/* check if during feeding TX FIFO in this function
	 * a not-acknowledge has occurred */
	if (algo_data->mif.state & I2C_PNX_STATE_NAI_OCCURRED) {
		i2c_pnx_unarm_timer(algo_data);
		rc = -EREMOTEIO;
	}

	if (rc == 0) {
		/* check if transaction is already done */
		if ((algo_data->mif.state &
				(I2C_PNX_STATE_TX_FINISHED |
				I2C_PNX_STATE_DONE |
				I2C_PNX_STATE_EXPECT_RX))
			== (I2C_PNX_STATE_TX_FINISHED |
				I2C_PNX_STATE_DONE)) {
			/* transaction is complete */
			dev_dbg(&adap->dev, "%s(): cmd completed instantaneously, ret = %d, stat = %x\n",
				__func__, algo_data->mif.cmd_ret, ioread32(I2C_REG_STS(algo_data)));
			i2c_pnx_unarm_timer(algo_data);
			if (likely(algo_data->mif.cmd_ret == 0))
				rc = num;
			else {
				rc = algo_data->mif.cmd_ret;
				wait_reset(algo_data);
			}
			goto xfer_exit;
		}

		/* Transaction still in progress.
		 * Enable master interrupt 
		 */
		if (!(algo_data->mif.state & I2C_PNX_STATE_TX_FINISHED)) {
			ctl = ioread32(I2C_REG_CTL(algo_data));
			ctl |= mcntrl_tffie | mcntrl_drmie;
			iowrite32(ctl, I2C_REG_CTL(algo_data));
		}

		/* wait for completion */
		rc = wait_for_completion_interruptible_timeout(&algo_data->mif.complete, HZ);
		if (rc == 0) {
			dev_dbg(&adap->dev, "%s(): wait timeout stat = %x\n",
				__func__, ioread32(I2C_REG_STS(algo_data)));
		} else if (rc < 0) {
			dev_dbg(&adap->dev, "%s(): wait err rc = %d, stat = %x\n",
				__func__, rc, ioread32(I2C_REG_STS(algo_data)));
		} else {
			dev_dbg(&adap->dev, "%s(): cmd completed, ret = %d, stat = %x\n",
				__func__, algo_data->mif.cmd_ret, ioread32(I2C_REG_STS(algo_data)));
			if (likely(algo_data->mif.cmd_ret == 0))
				rc = num;
			else {
				rc = algo_data->mif.cmd_ret;
				wait_reset(algo_data);
			}
		}
	} else {
		dev_dbg(&adap->dev, "%s(): start err rc=%d, stat = %x. Resetting.\n",
			__func__, rc, ioread32(I2C_REG_STS(algo_data)));
		/* Disable master interrupts */
		ctl = ioread32(I2C_REG_CTL(algo_data));
		ctl &= ~(mcntrl_afie | mcntrl_naie | mcntrl_rffie |
			mcntrl_drmie | mcntrl_daie | mcntrl_tdie);
		/* Reset I2C engine */
		ctl |= mcntrl_reset;
		iowrite32(ctl, I2C_REG_CTL(algo_data));
		wait_reset(algo_data);
	}

xfer_exit:
	/* Clear TDI and AFI bits in case they are set. */
	stat = ioread32(I2C_REG_STS(algo_data));
	if (stat & (mstatus_tdi|mstatus_afi)) {
		dev_dbg(&algo_data->adapter.dev,
			"%s: TDI/AFI still set... clearing now.\n",
			algo_data->adapter.name);
		iowrite32(stat, I2C_REG_STS(algo_data));
	}

	mutex_unlock(&algo_data->mif.lock);

	dev_dbg(&algo_data->adapter.dev, "%s(): exiting, stat = %x\n",
		__func__, ioread32(I2C_REG_STS(algo_data)));

/*
	if (completed != num)
		return ((rc < 0) ? rc : -EREMOTEIO);
 */

	return rc;
}

static u32 i2c_pnx_func(struct i2c_adapter *adapter)
{
	return I2C_FUNC_I2C | I2C_FUNC_SMBUS_EMUL;
}

static struct i2c_algorithm pnx_algorithm = {
	.master_xfer = i2c_pnx_xfer,
	.functionality = i2c_pnx_func,
};

#ifdef CONFIG_PM
static int i2c_pnx_controller_suspend(struct platform_device *pdev,
				      pm_message_t state)
{
	struct i2c_pnx_algo_data *algo_data = platform_get_drvdata(pdev);

	clk_disable(algo_data->clk);

	return 0;
}

static int i2c_pnx_controller_resume(struct platform_device *pdev)
{
	struct i2c_pnx_algo_data *algo_data = platform_get_drvdata(pdev);

	return clk_enable(algo_data->clk);
}
#else
#define i2c_pnx_controller_suspend	NULL
#define i2c_pnx_controller_resume	NULL
#endif

static int __devinit i2c_pnx_probe(struct platform_device *pdev)
{
	unsigned long tmp;
	int ret = 0;
	struct i2c_pnx_algo_data *algo_data;
	unsigned long freq;
	struct resource *res;
	u32 speed = I2C_PNX_SPEED_KHZ_DEFAULT * 1000;
	u32 slave_addr = ~0;

	algo_data = kzalloc(sizeof(*algo_data), GFP_KERNEL);
	if (!algo_data) {
		ret = -ENOMEM;
		goto err_kzalloc;
	}

	platform_set_drvdata(pdev, algo_data);

	algo_data->adapter.dev.parent = &pdev->dev;
	algo_data->adapter.algo = &pnx_algorithm;
	algo_data->adapter.algo_data = algo_data;
	algo_data->adapter.nr = pdev->id;
	algo_data->timeout = I2C_PNX_TIMEOUT_DEFAULT;
#ifdef CONFIG_OF
	algo_data->adapter.dev.of_node = of_node_get(pdev->dev.of_node);
	if (pdev->dev.of_node) {
		of_property_read_u32(pdev->dev.of_node, "pnx,timeout",
				     &algo_data->timeout);
		of_property_read_u32(pdev->dev.of_node, "clock-frequency",
				     &speed);
		of_property_read_u32(pdev->dev.of_node, "slave-addr",
				     &slave_addr);
	}
#endif
	algo_data->clk = clk_get(&pdev->dev, NULL);
	if (IS_ERR(algo_data->clk)) {
		ret = PTR_ERR(algo_data->clk);
		goto out_drvdata;
	}

	init_timer(&algo_data->mif.timer);
	algo_data->mif.timer.function = i2c_pnx_timeout;
	algo_data->mif.timer.data = (unsigned long)algo_data;

	snprintf(algo_data->adapter.name, sizeof(algo_data->adapter.name),
		 "%s", pdev->name);

	/* Register I/O resource */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		dev_err(&pdev->dev, "Unable to get mem resource.\n");
		ret = -EBUSY;
		goto out_clkget;
	}
	if (!request_mem_region(res->start, I2C_PNX_REGION_SIZE,
				pdev->name)) {
		dev_err(&pdev->dev,
		       "I/O region 0x%08x for I2C already in use.\n",
		       res->start);
		ret = -ENOMEM;
		goto out_clkget;
	}

	algo_data->base = res->start;
	algo_data->ioaddr = ioremap(res->start, I2C_PNX_REGION_SIZE);
	if (!algo_data->ioaddr) {
		dev_err(&pdev->dev, "Couldn't ioremap I2C I/O region\n");
		ret = -ENOMEM;
		goto out_release;
	}

	ret = clk_enable(algo_data->clk);
	if (ret)
		goto out_unmap;

	if (slave_addr != ~0)
		iowrite32(slave_addr, I2C_REG_ADR(algo_data));

	freq = clk_get_rate(algo_data->clk);

	/*
	 * Clock Divisor High This value is the number of system clocks
	 * the serial clock (SCL) will be high.
	 * For example, if the system clock period is 50 ns and the maximum
	 * desired serial period is 10000 ns (100 kHz), then CLKHI would be
	 * set to 0.5*(f_sys/f_i2c)-2=0.5*(20e6/100e3)-2=98. The actual value
	 * programmed into CLKHI will vary from this slightly due to
	 * variations in the output pad's rise and fall times as well as
	 * the deglitching filter length.
	 */

	dev_dbg(&pdev->dev, "%s: input frequency %ld Hz.\n",
		algo_data->adapter.name, freq);
	tmp = (freq / speed) / 2 - 2;
	if (tmp > 0x3FF)
		tmp = 0x3FF;
	iowrite32(tmp, I2C_REG_CKH(algo_data));
	iowrite32(tmp, I2C_REG_CKL(algo_data));

	iowrite32(mcntrl_reset, I2C_REG_CTL(algo_data));
	if (wait_reset(algo_data)) {
		ret = -ENODEV;
		goto out_clock;
	}
	init_completion(&algo_data->mif.complete);
	mutex_init(&algo_data->mif.lock);

	algo_data->irq = platform_get_irq(pdev, 0);
	if (algo_data->irq < 0) {
		dev_err(&pdev->dev, "Failed to get IRQ from platform resource\n");
		goto out_irq;
	}
	ret = request_irq(algo_data->irq, i2c_pnx_interrupt,
			0, pdev->name, algo_data);
	if (ret)
		goto out_clock;

	/* Register this adapter with the I2C subsystem */
	ret = i2c_add_numbered_adapter(&algo_data->adapter);
	if (ret < 0) {
		dev_err(&pdev->dev, "I2C: Failed to add bus\n");
		goto out_irq;
	}

	of_i2c_register_devices(&algo_data->adapter);

	dev_dbg(&pdev->dev, "%s: Master at %#8x, irq %d.\n",
	       algo_data->adapter.name, res->start, algo_data->irq);

	return 0;

out_irq:
	free_irq(algo_data->irq, algo_data);
out_clock:
	clk_disable(algo_data->clk);
out_unmap:
	iounmap(algo_data->ioaddr);
out_release:
	release_mem_region(res->start, I2C_PNX_REGION_SIZE);
out_clkget:
	clk_put(algo_data->clk);
out_drvdata:
	kfree(algo_data);
err_kzalloc:
	platform_set_drvdata(pdev, NULL);
	return ret;
}

static int __devexit i2c_pnx_remove(struct platform_device *pdev)
{
	struct i2c_pnx_algo_data *algo_data = platform_get_drvdata(pdev);

	free_irq(algo_data->irq, algo_data);
	i2c_del_adapter(&algo_data->adapter);
	clk_disable(algo_data->clk);
	iounmap(algo_data->ioaddr);
	release_mem_region(algo_data->base, I2C_PNX_REGION_SIZE);
	clk_put(algo_data->clk);
	kfree(algo_data);
	platform_set_drvdata(pdev, NULL);

	return 0;
}

#ifdef CONFIG_OF
static const struct of_device_id i2c_pnx_of_match[] = {
	{ .compatible = "nxp,pnx-i2c" },
	{ },
};
MODULE_DEVICE_TABLE(of, i2c_pnx_of_match);
#endif

static struct platform_driver i2c_pnx_driver = {
	.driver = {
		.name = "pnx-i2c",
		.owner = THIS_MODULE,
#ifdef CONFIG_OF
		.of_match_table = i2c_pnx_of_match,
#endif
	},
	.probe = i2c_pnx_probe,
	.remove = __devexit_p(i2c_pnx_remove),
	.suspend = i2c_pnx_controller_suspend,
	.resume = i2c_pnx_controller_resume,
};

static int __init i2c_adap_pnx_init(void)
{
	return platform_driver_register(&i2c_pnx_driver);
}

static void __exit i2c_adap_pnx_exit(void)
{
	platform_driver_unregister(&i2c_pnx_driver);
}

MODULE_AUTHOR("Vitaly Wool, Dennis Kovalev <source@mvista.com>");
MODULE_DESCRIPTION("I2C driver for Philips IP3204-based I2C busses");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:pnx-i2c");

/* We need to make sure I2C is initialized before USB */
subsys_initcall(i2c_adap_pnx_init);
module_exit(i2c_adap_pnx_exit);

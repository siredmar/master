/*
 * Provides I2C support for Philips PNX010x/PNX4008 boards.
 *
 * Authors: Dennis Kovalev <dkovalev@ru.mvista.com>
 *	    Vitaly Wool <vwool@ru.mvista.com>
 *	    Joerg Gittinger <grmpf@gittingerbox.de>
 *
 * 2004-2006 (c) MontaVista Software, Inc. 
 * 2012 (c) Joerg Gittinger
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
#include <mach/hardware.h>
#include <mach/i2c.h>
#include <asm/irq.h>
#include <asm/uaccess.h>

#define I2C_PNX_TIMEOUT		100 /* msec */
#define I2C_PNX_SPEED_KHZ	100
#define I2C_PNX_REGION_SIZE	0x100
#define PNX_DEFAULT_FREQ	13 /* MHz */

/* state definitions */
#define I2C_PNX_STATE_TX_FINISHED	(1<<0)
#define I2C_PNX_STATE_ABORTED		(1<<1)
#define I2C_PNX_STATE_EXPECT_RX		(1<<2)
#define I2C_PNX_STATE_DONE		(1<<3)
#define I2C_PNX_STATE_TX_BY_XFER	(1<<4)	/* ISR must not TX */
#define I2C_PNX_STATE_ABORT_PENDING	(1<<5)
#define I2C_PNX_STATE_NAI_OCCURRED	(1<<6)

static inline int wait_timeout(long timeout, struct i2c_pnx_data *data)
{
	struct i2c_pnx_algo_data *algo_data = data->adapter->algo_data;
	while (timeout > 0 &&
			(ioread32(I2C_REG_STS(algo_data)) & mstatus_active)) {
		mdelay(1);
		timeout--;
	}
	return (timeout <= 0);
}

static inline int wait_reset(long timeout, struct i2c_pnx_data *i2c_pnx)
{
	struct i2c_pnx_algo_data *algo_data = i2c_pnx->adapter->algo_data;
	while (timeout > 0 &&
			(ioread32(I2C_REG_CTL(algo_data)) & mcntrl_reset)) {
		mdelay(1);
		timeout--;
	}
	return (timeout <= 0);
}

static inline void i2c_pnx_arm_timer(struct i2c_pnx_data *i2c_pnx)
{
	struct i2c_adapter *adap = i2c_pnx->adapter;
	struct timer_list *timer = &i2c_pnx->timer;
	int expires = I2C_PNX_TIMEOUT / (1000 / HZ);

	if (expires <= 1)
		expires = 2;

	del_timer_sync(timer);

	dev_dbg(&adap->dev, "Timer armed at %lu plus %u jiffies.\n",
		jiffies, expires);

	timer->expires = jiffies + expires;
	timer->data = (unsigned long)i2c_pnx;

	add_timer(timer);
}

static inline void i2c_pnx_unarm_timer(struct i2c_pnx_data *i2c_pnx)
{
	struct timer_list *timer = &i2c_pnx->timer;

	del_timer_sync(timer);
}

/**
 * i2c_pnx_master_tx - transmit data to slave
 * @adap:		pointer to I2C adapter structure
 *
 * Sends one byte of data to the slave
 */
static int i2c_pnx_master_tx(struct i2c_pnx_data *i2c_pnx)
{
	struct i2c_adapter *adap = i2c_pnx->adapter;
	struct i2c_pnx_algo_data *algo_data = adap->algo_data;
	u32 val;

	dev_dbg(&adap->dev, "%s(): entering: stat = %04x.\n",
		__func__, ioread32(I2C_REG_STS(algo_data)));

	BUG_ON(i2c_pnx->tx_buf_len == 0);

	/* We still have something to talk about... */
	val = *i2c_pnx->tx_buf++;

	/* last byte of all messages? */
	if ((i2c_pnx->tx_buf_len == 1) && 
		(i2c_pnx->msgs_tx_idx == i2c_pnx->msgs_num-1))
		val |= stop_bit;

	i2c_pnx->tx_buf_len--;
	iowrite32(val, I2C_REG_TX(algo_data));

	dev_dbg(&adap->dev, "%s(): tx %#x [%d]\n", __func__,
		val, i2c_pnx->tx_buf_len + 1);

	return 0;
}

/**
 * i2c_pnx_master_tx_rcv - fill TX FIFO for receive
 * @adap:		pointer to I2C adapter structure
 *
 * Writes a byte to the TX FIFO required for receiving
 */
static int i2c_pnx_master_tx_rcv(struct i2c_pnx_data *i2c_pnx)
{
	struct i2c_adapter *adap = i2c_pnx->adapter;
	struct i2c_pnx_algo_data *algo_data = adap->algo_data;
	unsigned int val = 0;

	dev_dbg(&adap->dev, "%s(): entering: stat = %04x.\n",
		__func__, ioread32(I2C_REG_STS(algo_data)));


	if ((i2c_pnx->tx_buf_len == 1)
		&& (i2c_pnx->msgs_tx_idx == i2c_pnx->msgs_num-1)) {
		/* Last byte, do not acknowledge next rcv. */
		val |= stop_bit;
		dev_dbg(&adap->dev, "%s(): sending stop bit.\n",
			__func__);
	}

	i2c_pnx->tx_buf_len--;
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
 * @i2c_pnx:		pointer to I2C adapter structure
 *
 * Reads bytes from the RX FIFO
 */
static int 
i2c_pnx_master_rcv(struct i2c_pnx_data *i2c_pnx)
{
	struct i2c_adapter *adap = i2c_pnx->adapter;
	struct i2c_pnx_algo_data *algo_data = adap->algo_data;
	unsigned int val = 0;
	int count = 16;	/* max 16 bytes in order to burden isr not too much */
	u32 sts;

	dev_dbg(&adap->dev, "%s(): entering: stat = %04x, rx len = %d.\n",
		__func__, 
		ioread32(I2C_REG_STS(algo_data)),
		i2c_pnx->rx_buf_len);

	BUG_ON(i2c_pnx->rx_buf_len == 0);
	/* Handle data. */
	do {
		val = ioread32(I2C_REG_RX(algo_data));
		*i2c_pnx->rx_buf++ = (u8) (val & 0xff);
		dev_dbg(&adap->dev, "%s(): rcv 0x%x [%d]\n", __func__, val,
			i2c_pnx->rx_buf_len);

		i2c_pnx->rx_buf_len--;
		sts = ioread32(I2C_REG_STS(algo_data));
		count--;
	} while(i2c_pnx->rx_buf_len 
		&& count
		&& !(sts & mstatus_rfe));

	if (i2c_pnx->rx_buf_len == 0) {
		/* we are finished with receiving */
		i2c_pnx->state &= ~I2C_PNX_STATE_EXPECT_RX;
	}

	dev_dbg(&adap->dev, "%s(): exiting: stat = %04x, rx len = %d, state = %x.\n",
		__func__, 
		ioread32(I2C_REG_STS(algo_data)),
		i2c_pnx->rx_buf_len,
		i2c_pnx->state);

	return 0;
}

static void
i2c_pnx_state_cmd_complete(struct i2c_pnx_data *i2c_pnx)
{
	struct i2c_adapter *adap = i2c_pnx->adapter;
	struct i2c_pnx_algo_data *algo_data = adap->algo_data;
	u32 ctl;

	dev_dbg(&adap->dev, "%s(): entering: stat = %04x.\n",
		__func__, ioread32(I2C_REG_STS(algo_data)));
	
	/* Disable master interrupts */
	ctl = ioread32(I2C_REG_CTL(algo_data));
	ctl &= ~(mcntrl_afie | mcntrl_naie | mcntrl_rffie |
		 mcntrl_drmie | mcntrl_daie | mcntrl_tdie);
	iowrite32(ctl, I2C_REG_CTL(algo_data));

	/* Kill timer. */
	i2c_pnx_unarm_timer(i2c_pnx);
	/*i2c_pnx->cmd_ret = 0;*/
	complete(&i2c_pnx->cmd_complete);

	dev_dbg(&adap->dev, "%s(): exiting: ret = %d, stat = %04x.\n",
		__func__, i2c_pnx->cmd_ret, ioread32(I2C_REG_STS(algo_data)));
}


static void
i2c_pnx_state_abort_without_complete(struct i2c_pnx_data *i2c_pnx, int rc)
{
	struct i2c_adapter *adap = i2c_pnx->adapter;
	struct i2c_pnx_algo_data *algo_data = adap->algo_data;
	u32 ctl;

	dev_dbg(&adap->dev, "%s(): abort: stat = %04x.\n",
		__func__, ioread32(I2C_REG_STS(algo_data)));

	/* Disable master interrupts */
	ctl = ioread32(I2C_REG_CTL(algo_data));
	ctl &= ~(mcntrl_afie | mcntrl_naie | mcntrl_rffie |
		 mcntrl_drmie | mcntrl_daie | mcntrl_tdie);
	/* Reset I2C engine */
	ctl |= mcntrl_reset;
	
	iowrite32(ctl, I2C_REG_CTL(algo_data));

	/* Kill timer */
	i2c_pnx_unarm_timer(i2c_pnx);
	i2c_pnx->cmd_ret = rc;
	i2c_pnx->state |= I2C_PNX_STATE_ABORTED;
}

static void
i2c_pnx_state_abort(struct i2c_pnx_data *i2c_pnx, int rc)
{
	i2c_pnx_state_abort_without_complete(i2c_pnx, rc);
	complete(&i2c_pnx->cmd_complete);
}

static void
i2c_pnx_state_abort_with_stop(struct i2c_pnx_data *i2c_pnx, int rc)
{
	struct i2c_adapter *adap = i2c_pnx->adapter;
	struct i2c_pnx_algo_data *algo_data = adap->algo_data;
	u32 ctl;

	dev_dbg(&adap->dev, "%s(): abort: stat = %04x.\n",
		__func__, ioread32(I2C_REG_STS(algo_data)));

	/* Write a STOP bit to TX FIFO */
	iowrite32(0x00 | stop_bit, I2C_REG_TX(algo_data));

	/* Disable master interrupts except for stop*/
	ctl = ioread32(I2C_REG_CTL(algo_data));
	ctl &= ~(mcntrl_afie | mcntrl_naie | mcntrl_rffie |
		 mcntrl_drmie | mcntrl_daie/* | mcntrl_tdie*/);
	iowrite32(ctl, I2C_REG_CTL(algo_data));

	/* set error code */
	i2c_pnx->cmd_ret = rc;
}


static int
i2c_pnx_tx_start(struct i2c_pnx_data *i2c_pnx)
{
	struct i2c_msg *pmsg;
	struct i2c_adapter *adap = i2c_pnx->adapter;
	struct i2c_pnx_algo_data *algo_data = adap->algo_data;
	int rc = 0;
	u8 addr;
	u32 val;

	pmsg = &i2c_pnx->msgs[i2c_pnx->msgs_tx_idx];
	addr = pmsg->addr;

	/* Check for 7 bit slave addresses only */
	if (addr & ~0x7f) {
		dev_err(&adap->dev, "%s: Invalid slave address %x. "
		       "Only 7-bit addresses are supported\n",
		       adap->name, addr);
		return -EINVAL;
	}

	/* tx start condition */
	if (pmsg->flags & I2C_M_RD) {
		u32 ctl;
		/* for now we accept only one RD per set of messages */
		BUG_ON(i2c_pnx->rx_buf != NULL);
		if (pmsg->len == 0) {
			dev_err(&adap->dev, "%s: Receive buffer size is zero.\n",
					adap->name);
			return -EINVAL;
		}
		i2c_pnx->rx_buf = pmsg->buf;
		i2c_pnx->rx_buf_len = pmsg->len;
		i2c_pnx->mode = I2C_SMBUS_READ;
		i2c_pnx->state |= I2C_PNX_STATE_EXPECT_RX;
		/* the LPC3131 I2C engine requires TX FIFO to be fed
		 * even in RX mode. For each byte to be received
		 * TX FIFO must be fed. Therefore we set tx_buf_len
		 * as well.
		 */
		i2c_pnx->tx_buf_len = pmsg->len;

		/* Enable data available interrupt and 
		 * Receive FIFO full interrupt */
		ctl = ioread32(I2C_REG_CTL(algo_data));
		ctl |= mcntrl_rffie | mcntrl_daie;
		iowrite32(ctl, I2C_REG_CTL(algo_data));
	} else {
		i2c_pnx->tx_buf = pmsg->buf;
		i2c_pnx->tx_buf_len = pmsg->len;
		i2c_pnx->mode = I2C_SMBUS_WRITE;
	}
	dev_dbg(&adap->dev, "%s(): mode %d, %d bytes\n", __func__,
		i2c_pnx->mode,
		pmsg->len);

	/* Write the slave address, START bit and R/W bit */
	val = (addr << 1) | start_bit | i2c_pnx->mode;
	if (pmsg->len == 0)
		val |= stop_bit;

	dev_dbg(&adap->dev, "%s(): sending %#x\n", __func__,
		val);

	iowrite32(val, I2C_REG_TX(algo_data));

	return rc;
}

static int
i2c_pnx_state_next_tx(struct i2c_pnx_data *i2c_pnx/*, int in_isr*/)
{
	struct i2c_adapter *adap = i2c_pnx->adapter;
	struct i2c_pnx_algo_data *algo_data = adap->algo_data;
	int rc = 0;
	int count = 10;	/* max 10 in order not to burden isr too much */
	u32 ctl;

	do {
		dev_dbg(&adap->dev, "%s(): tx next byte: stat = %04x.\n",
			__func__, ioread32(I2C_REG_STS(algo_data)));

		if (i2c_pnx->tx_buf_len == 0) {
			/* no more data in current message */
			if (likely(i2c_pnx->msgs_tx_idx < i2c_pnx->msgs_num-1)) {
				/* switch to next message and send (re-)start */
				i2c_pnx->msgs_tx_idx++;
				rc = i2c_pnx_tx_start(i2c_pnx);
			} else {
				/* end of transmission */
				dev_dbg(&adap->dev, "%s(): end of transmission! stat = %04x.\n",
					__func__, ioread32(I2C_REG_STS(algo_data)));
				i2c_pnx->state |= I2C_PNX_STATE_TX_FINISHED;
				/* disable Transmit FIFO Not Full Interrupt */
				ctl = ioread32(I2C_REG_CTL(algo_data));
				ctl &= ~mcntrl_tffie;
				iowrite32(ctl, I2C_REG_CTL(algo_data));
			}	
		} else {
#if 0
			/* still data to transmit. */
			ctl = ioread32(I2C_REG_CTL(algo_data));
			/* disable interrupts while we are writing to TX FIFO */
			iowrite32(ctl & ~(mcntrl_afie | mcntrl_naie | mcntrl_tdie | 
						mcntrl_rffie | mcntrl_daie | mcntrl_tffie), 
					I2C_REG_CTL(algo_data));
#endif
			if (i2c_pnx->mode == I2C_SMBUS_READ) {
#if 0
				if (!in_isr)
					/* we must block feeding the TX FIFO for the receives
					 * from i2c_pnx_start() because in case of NA interrupt
					 * an abort can interfere with feeding the FIFO
					 * in i2c_pnx_master_tx_rcv(). */
					break;
#endif
				rc = i2c_pnx_master_tx_rcv(i2c_pnx);
			} else {
				rc = i2c_pnx_master_tx(i2c_pnx);
			}
			/* enable interrupts again */
#if 0
			iowrite32(ctl, I2C_REG_CTL(algo_data));
#endif

		}
		count--;
	} while(!rc 
		&& count
		&& !(ioread32(I2C_REG_STS(algo_data)) & mstatus_tff)
		&& !(i2c_pnx->state & 
			(I2C_PNX_STATE_TX_FINISHED |
			I2C_PNX_STATE_ABORTED |
			I2C_PNX_STATE_ABORT_PENDING |
			I2C_PNX_STATE_NAI_OCCURRED))); 

	return rc;
}

static irqreturn_t i2c_pnx_interrupt(int irq, void *dev_id)
{
	u32 stat, rc;
	struct i2c_adapter *adap = dev_id;
	struct i2c_pnx_algo_data *algo_data = adap->algo_data;
	struct i2c_pnx_data *i2c_pnx = i2c_get_adapdata(adap);

	dev_dbg(&adap->dev, "%s(): mstat = %x mctrl = %x, mode = %d, state = %x\n",
		__func__,
		ioread32(I2C_REG_STS(algo_data)),
		ioread32(I2C_REG_CTL(algo_data)),
		i2c_pnx->mode,
		i2c_pnx->state);
	stat = ioread32(I2C_REG_STS(algo_data));

	/* let's see what kind of event this is */
	if (stat & mstatus_afi) {
		/* We lost arbitration in the midst of a transfer */
		i2c_pnx_state_abort(i2c_pnx, -EIO);
	} else if (stat & mstatus_nai) {
		/* Slave did not acknowledge, generate a STOP */
		dev_dbg(&adap->dev, "%s(): "
			"Slave did not acknowledge.\n",
			__func__);

		if (i2c_pnx->state & I2C_PNX_STATE_TX_BY_XFER) {
			/* we have interrupted i2c_pnx_xfer() - do not
			 * abort at this time because we may interfere
			 * with writing to TX FIFO at i2c_pnx_xfer() */
			i2c_pnx->state |= I2C_PNX_STATE_NAI_OCCURRED;
			/* disable not-acknowledge and transaction-done interrupt */
			iowrite32(ioread32(I2C_REG_CTL(algo_data)) & 
					~(mcntrl_naie | mcntrl_tdie),
					I2C_REG_CTL(algo_data));
			dev_dbg(&adap->dev, "%s(): Delayed abort.\n", __func__);
			goto irq_done;
		}

		if (i2c_pnx->mode == I2C_SMBUS_READ) {
			/* in case of a read we must perform a 'hard' abort. */
			i2c_pnx_state_abort(i2c_pnx, -EREMOTEIO);
		} else {
			i2c_pnx_state_abort_with_stop(i2c_pnx, -EREMOTEIO);
		}
		goto irq_done;
	}
	if (!(stat & mstatus_tff)) {
		/* TX FIFO not full */
		if (!(i2c_pnx->state & 
			(I2C_PNX_STATE_TX_FINISHED | 
			 I2C_PNX_STATE_TX_BY_XFER |
			 I2C_PNX_STATE_ABORT_PENDING))) {
			/* TX FIFO wants to be fed */
			rc = i2c_pnx_state_next_tx(i2c_pnx/*, 1*/);
			if (rc < 0) {
				i2c_pnx_state_abort(i2c_pnx, rc);
				goto irq_done;
			}
		}
	}
	if (!(stat & mstatus_rfe)) {
		/* Valid data in RX FIFO */
		if (i2c_pnx->state & I2C_PNX_STATE_EXPECT_RX) {
			/* we expect data */
			rc = i2c_pnx_master_rcv(i2c_pnx);
			if (rc < 0) {
				i2c_pnx_state_abort(i2c_pnx, rc);
				goto irq_done;
			}
		}
	} 

	/* Final check. Re-read status to see if 
	 * transaction is done now after receiving. */
	stat = ioread32(I2C_REG_STS(algo_data));
	if (stat & mstatus_tdi) {
		/* Transaction done */
		i2c_pnx->state |= I2C_PNX_STATE_DONE;
		dev_dbg(&adap->dev, "%s(): "
			"Transaction done. state = %x\n",
			__func__,
			i2c_pnx->state);
	}	

	/* Check for end of transaction */
	if ((i2c_pnx->state & (I2C_PNX_STATE_TX_FINISHED |
				I2C_PNX_STATE_EXPECT_RX |
				I2C_PNX_STATE_DONE))
		== (I2C_PNX_STATE_TX_FINISHED | I2C_PNX_STATE_DONE)) {
		/* command done */
		if (i2c_pnx->state & I2C_PNX_STATE_ABORT_PENDING) {
			/* this was a delayed abort (STOP bit) */
			i2c_pnx->state &= ~I2C_PNX_STATE_ABORT_PENDING;
			i2c_pnx->state |= I2C_PNX_STATE_ABORTED;
		}

		i2c_pnx_state_cmd_complete(i2c_pnx);
	}
irq_done:
	/* Clear TDI and AFI bits */
	stat = ioread32(I2C_REG_STS(algo_data));
	stat |= mstatus_tdi | mstatus_afi;
	if (!(i2c_pnx->state & I2C_PNX_STATE_DONE)) {
		/* We are not done yet. Do not clear a possible mstatus_tdi.
		 * Otherwise we might lose this notificaiton. */
		stat &= ~mstatus_tdi;
	}
	iowrite32(stat, I2C_REG_STS(algo_data));

	dev_dbg(&adap->dev, "%s(): exiting, stat = %x ctrl = %x, state = %x.\n",
		 __func__, ioread32(I2C_REG_STS(algo_data)),
		 ioread32(I2C_REG_CTL(algo_data)),
		 i2c_pnx->state);

	return IRQ_HANDLED;
}

static void i2c_pnx_timeout(unsigned long data)
{
	struct i2c_pnx_data *i2c_pnx = (struct i2c_pnx_data *)data;
	struct i2c_adapter *adap = i2c_pnx->adapter;
	struct i2c_pnx_algo_data *algo_data = adap->algo_data;

	dev_err(&adap->dev, "Master timed out. stat = %04x, ctrl = %04x, state = %x. "
	       "Resetting master...\n",
	       ioread32(I2C_REG_STS(algo_data)),
	       ioread32(I2C_REG_CTL(algo_data)),
	       i2c_pnx->state);

	/* Reset master and disable interrupts */
	i2c_pnx_state_abort(i2c_pnx, -EIO);
}

static inline void bus_reset_if_active(struct i2c_pnx_data *i2c_pnx)
{
	struct i2c_adapter *adap = i2c_pnx->adapter;
	struct i2c_pnx_algo_data *algo_data = adap->algo_data;
	u32 stat;

	if ((stat = ioread32(I2C_REG_STS(algo_data))) & mstatus_active) {
		dev_err(&adap->dev,
			"%s: Bus is still active after xfer. Reset it...\n",
		       adap->name);
		iowrite32(ioread32(I2C_REG_CTL(algo_data)) | mcntrl_reset,
			  I2C_REG_CTL(algo_data));
		wait_reset(I2C_PNX_TIMEOUT, i2c_pnx);
	} else if (!(stat & mstatus_rfe) || !(stat & mstatus_tfe)) {
		/* If there is data in the fifo's after transfer,
		 * flush fifo's by reset.
		 */
		iowrite32(ioread32(I2C_REG_CTL(algo_data)) | mcntrl_reset,
			  I2C_REG_CTL(algo_data));
		wait_reset(I2C_PNX_TIMEOUT, i2c_pnx);
	} else if (stat & mstatus_nai) {
		iowrite32(ioread32(I2C_REG_CTL(algo_data)) | mcntrl_reset,
			  I2C_REG_CTL(algo_data));
		wait_reset(I2C_PNX_TIMEOUT, i2c_pnx);
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
	struct i2c_pnx_data *i2c_pnx = i2c_get_adapdata(adap);
	u32 stat = ioread32(I2C_REG_STS(algo_data));
	u32 ctl;

	dev_dbg(&adap->dev, "%s(): entering: %d messages, stat = %04x.\n",
		__func__, num, ioread32(I2C_REG_STS(algo_data)));

	mutex_lock(&i2c_pnx->lock);

	INIT_COMPLETION(i2c_pnx->cmd_complete);

	bus_reset_if_active(i2c_pnx);

	/* First, make sure bus is idle */
	if (wait_timeout(I2C_PNX_TIMEOUT, i2c_pnx)) {
		/* Somebody else is monopolizing the bus */
		dev_err(&adap->dev, "%s: Bus busy. Slave addr = %02x, "
		       "cntrl = %x, stat = %x\n",
		       adap->name, msgs[0].addr,
		       ioread32(I2C_REG_CTL(algo_data)),
		       ioread32(I2C_REG_STS(algo_data)));
		mutex_unlock(&i2c_pnx->lock);
		return -EBUSY;
	} else if (ioread32(I2C_REG_STS(algo_data)) & mstatus_afi) {
		/* Sorry, we lost the bus */
		dev_err(&adap->dev, "%s: Arbitration failure. ",
		       adap->name);
		mutex_unlock(&i2c_pnx->lock);
		return -EIO;
	}

	/* initialize transfers */
	i2c_pnx->msgs = msgs;
	i2c_pnx->msgs_num = num;
	i2c_pnx->msgs_tx_idx = 0;
	i2c_pnx->msgs_rx_idx = 0;
	i2c_pnx->tx_buf = NULL;
	i2c_pnx->tx_buf_len = 0;
	i2c_pnx->rx_buf = NULL;
	i2c_pnx->rx_buf_len = 0;
	i2c_pnx->cmd_ret = 0;
	i2c_pnx->state = 0;

	i2c_pnx_arm_timer(i2c_pnx);

	/* enable transaction error and done interrupts */
	ctl = ioread32(I2C_REG_CTL(algo_data));
	ctl |= mcntrl_afie | mcntrl_naie | mcntrl_tdie;
	iowrite32(ctl, I2C_REG_CTL(algo_data));

	/* start I2C transfer */
	rc = i2c_pnx_tx_start(i2c_pnx);

	i2c_pnx->state |= I2C_PNX_STATE_TX_BY_XFER;
	/* feed TX FIFO */
	rc = i2c_pnx_state_next_tx(i2c_pnx/*, 0*/);
	/* from now on let ISR handle remaining TX bytes */
	i2c_pnx->state &= ~I2C_PNX_STATE_TX_BY_XFER;

	/* check if during feeding TX FIFO in this function
	 * a not-acknowledge has occurred */
	if (i2c_pnx->state & I2C_PNX_STATE_NAI_OCCURRED) {
		i2c_pnx_unarm_timer(i2c_pnx);
		rc = -EREMOTEIO;
	}

	if (rc == 0) {
		/* check if transaction is already done */
		if ((i2c_pnx->state & 
				(I2C_PNX_STATE_TX_FINISHED | 
				 I2C_PNX_STATE_DONE | 
				 I2C_PNX_STATE_EXPECT_RX))
			== (I2C_PNX_STATE_TX_FINISHED | 
				I2C_PNX_STATE_DONE)) {
			/* transaction is complete */
			dev_dbg(&adap->dev, "%s(): cmd completed instantaneously, ret = %d, stat = %x\n",
				__func__, i2c_pnx->cmd_ret, ioread32(I2C_REG_STS(algo_data)));
			i2c_pnx_unarm_timer(i2c_pnx);
			if (likely(i2c_pnx->cmd_ret == 0))
				rc = num;
			else {
				rc = i2c_pnx->cmd_ret;
				wait_reset(I2C_PNX_TIMEOUT, i2c_pnx);
			}
			goto xfer_exit;
		}

		/* Transaction still in progress.
		 * Enable master interrupt 
		 */
		if (!(i2c_pnx->state & I2C_PNX_STATE_TX_FINISHED)) {
			ctl = ioread32(I2C_REG_CTL(algo_data));
			ctl |= mcntrl_tffie | mcntrl_drmie;
			iowrite32(ctl, I2C_REG_CTL(algo_data));
		}

		/* wait for completion */
		rc = wait_for_completion_interruptible_timeout(&i2c_pnx->cmd_complete, HZ);
		if (rc == 0) {
			dev_dbg(&adap->dev, "%s(): wait timeout stat = %x\n",
				__func__, ioread32(I2C_REG_STS(algo_data)));
		} else if (rc < 0) {
			dev_dbg(&adap->dev, "%s(): wait err rc = %d, stat = %x\n",
				__func__, rc, ioread32(I2C_REG_STS(algo_data)));
		} else {
			dev_dbg(&adap->dev, "%s(): cmd completed, ret = %d, stat = %x\n",
				__func__, i2c_pnx->cmd_ret, ioread32(I2C_REG_STS(algo_data)));
			if (likely(i2c_pnx->cmd_ret == 0))
				rc = num;
			else {
				rc = i2c_pnx->cmd_ret;
				wait_reset(I2C_PNX_TIMEOUT, i2c_pnx);
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
		wait_reset(I2C_PNX_TIMEOUT, i2c_pnx);
	}

xfer_exit:
	/* Clear TDI and AFI bits in case they are set. */
	stat = ioread32(I2C_REG_STS(algo_data));
	if (stat & (mstatus_tdi|mstatus_afi)) {
		dev_dbg(&adap->dev,
			"%s: TDI/AFI still set... clearing now.\n",
		       adap->name);
		iowrite32(stat, I2C_REG_STS(algo_data));
	}

	mutex_unlock(&i2c_pnx->lock);

	dev_dbg(&adap->dev, "%s(): exiting, stat = %x, state = %x\n",
		__func__, ioread32(I2C_REG_STS(algo_data)), 
		i2c_pnx->state);

	/*if (completed != num)
		return ((rc < 0) ? rc : -EREMOTEIO);*/

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

static int i2c_pnx_controller_suspend(struct platform_device *pdev,
				      pm_message_t state)
{
	struct i2c_pnx_data *i2c_pnx = platform_get_drvdata(pdev);
	return i2c_pnx->suspend(pdev, state);
}

static int i2c_pnx_controller_resume(struct platform_device *pdev)
{
	struct i2c_pnx_data *i2c_pnx = platform_get_drvdata(pdev);
	return i2c_pnx->resume(pdev);
}

static int __devinit i2c_pnx_probe(struct platform_device *pdev)
{
	unsigned long tmp;
	int ret = 0;
	struct i2c_pnx_algo_data *algo_data;
	int freq_mhz;
	struct i2c_pnx_data *i2c_pnx = pdev->dev.platform_data;

	if (!i2c_pnx || !i2c_pnx->adapter) {
		dev_err(&pdev->dev, "%s: no platform data supplied\n",
		       __func__);
		ret = -EINVAL;
		goto out;
	}

	platform_set_drvdata(pdev, i2c_pnx);
	i2c_set_adapdata(i2c_pnx->adapter, i2c_pnx);

	if (i2c_pnx->calculate_input_freq) {
		freq_mhz = i2c_pnx->calculate_input_freq(pdev);
		dev_info(&pdev->dev, "Bus frequency is "
		       "%d MHz\n", freq_mhz);
	} else {
		freq_mhz = PNX_DEFAULT_FREQ;
		dev_info(&pdev->dev, "Setting bus frequency to default value: "
		       "%d MHz\n", freq_mhz);
	}

	i2c_pnx->adapter->algo = &pnx_algorithm;

	algo_data = i2c_pnx->adapter->algo_data;
	init_timer(&i2c_pnx->timer);
	i2c_pnx->timer.function = i2c_pnx_timeout;
	i2c_pnx->timer.data = (unsigned long)i2c_pnx;
	dev_dbg(&pdev->dev, "%s: i2c_pnx=%p.\n",
	       i2c_pnx->adapter->name, i2c_pnx);

	/* Register I/O resource */
	if (!request_mem_region(algo_data->base, I2C_PNX_REGION_SIZE,
				pdev->name)) {
		dev_err(&pdev->dev,
		       "I/O region 0x%08x for I2C already in use.\n",
		       algo_data->base);
		ret = -ENODEV;
		goto out_drvdata;
	}

	if (!(algo_data->ioaddr =
			(u32)ioremap(algo_data->base, I2C_PNX_REGION_SIZE))) {
		dev_err(&pdev->dev, "Couldn't ioremap I2C I/O region\n");
		ret = -ENOMEM;
		goto out_release;
	}

	i2c_pnx->set_clock_run(pdev);

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

	dev_dbg(&pdev->dev, "%s: input frequency %d MHz.\n",
	       i2c_pnx->adapter->name, freq_mhz);
	tmp = ((freq_mhz * 1000) / I2C_PNX_SPEED_KHZ) / 2 - 2;
	iowrite32(tmp, I2C_REG_CKH(algo_data));
	iowrite32(tmp, I2C_REG_CKL(algo_data));

	iowrite32(mcntrl_reset, I2C_REG_CTL(algo_data));
	if (wait_reset(I2C_PNX_TIMEOUT, i2c_pnx)) {
		ret = -ENODEV;
		goto out_unmap;
	}

	init_completion(&i2c_pnx->cmd_complete);
	mutex_init(&i2c_pnx->lock);

	ret = request_irq(algo_data->irq, i2c_pnx_interrupt,
			0, pdev->name, i2c_pnx->adapter);
	if (ret)
		goto out_clock;

	/* Register this adapter with the I2C subsystem */
	i2c_pnx->adapter->dev.parent = &pdev->dev;
	i2c_pnx->adapter->nr = pdev->id;
	i2c_pnx->adapter->owner = THIS_MODULE;
	i2c_pnx->adapter->class = I2C_CLASS_HWMON;
	ret = i2c_add_numbered_adapter(i2c_pnx->adapter);
	if (ret < 0) {
		dev_err(&pdev->dev, "I2C: Failed to add bus\n");
		goto out_irq;
	}

	dev_dbg(&pdev->dev, "%s: Master at %#8x, irq %d.\n",
	       i2c_pnx->adapter->name, algo_data->base, algo_data->irq);

	return 0;

out_irq:
	free_irq(algo_data->irq, i2c_pnx->adapter);
out_clock:
	i2c_pnx->set_clock_stop(pdev);
out_unmap:
	iounmap((void *)algo_data->ioaddr);
out_release:
	release_mem_region(algo_data->base, I2C_PNX_REGION_SIZE);
out_drvdata:
	platform_set_drvdata(pdev, NULL);
out:
	return ret;
}

static int __devexit i2c_pnx_remove(struct platform_device *pdev)
{
	struct i2c_pnx_data *i2c_pnx = platform_get_drvdata(pdev);
	struct i2c_adapter *adap = i2c_pnx->adapter;
	struct i2c_pnx_algo_data *algo_data = adap->algo_data;

	free_irq(algo_data->irq, i2c_pnx->adapter);
	i2c_del_adapter(adap);
	i2c_pnx->set_clock_stop(pdev);
	iounmap((void *)algo_data->ioaddr);
	release_mem_region(algo_data->base, I2C_PNX_REGION_SIZE);
	platform_set_drvdata(pdev, NULL);

	return 0;
}

static struct platform_driver i2c_pnx_driver = {
	.driver = {
		.name = "pnx-i2c",
		.owner = THIS_MODULE,
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

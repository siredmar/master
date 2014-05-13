/*
 * rtc class driver for the Maxim RV3029 chip
 *
 * Author: Eduard Tasnadi-Olescher <zed666@gmx.net>
 *
 * based on previously existing rtc class drivers especially the rtc-max6900 driver.
 *
 * 2007 (c) MontaVista, Software, Inc.  This file is licensed under
 * the terms of the GNU General Public License version 2.  This program
 * is licensed "as is" without any warranty of any kind, whether express
 * or implied.
 */

#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/bcd.h>
#include <linux/rtc.h>
#include <linux/delay.h>

#define DRV_VERSION "0.1"

#define rv3029_rtc_proc NULL 

/*
 * register indices
 */
#define RV3029_REG_SC			0x8	/* seconds      00-59 */
#define RV3029_REG_MN			0x9	/* minutes      00-59 */
#define RV3029_REG_HR			0xA	/* hours        00-23 */
#define RV3029_REG_DT			0xB	/* day of month 00-31 */
#define RV3029_REG_MO			0xD	/* month        01-12 */
#define RV3029_REG_DW			0xC	/* day of week   1-7  */
#define RV3029_REG_YR			0xE	/* year         00-99 */



static struct i2c_driver rv3029_driver;



static int rv3029_i2c_read_regs(struct i2c_client *client, u8 *buf)
{
	u8 reg_burst_read[1] = { 8 };
//	printk("Now executing rv3029_i2c_read_regs.\n");
//	printk("Preparing Message...\n");
	struct i2c_msg msgs[2] = {
		{
		 .addr = client->addr,
		 .flags = 0,	// write
		 .len = sizeof(reg_burst_read),
		 .buf = reg_burst_read}
		,
		{
		 .addr = client->addr,
		 .flags = I2C_M_RD,
		 .len = 8,
		 .buf = &buf[0]} 

	};
	int rc;

//	printk("Now calling i2c_transfer.\n");
	rc = i2c_transfer(client->adapter, msgs, ARRAY_SIZE(msgs));

	
	if (rc != ARRAY_SIZE(msgs)) {
		dev_err(&client->dev, "%s: register read failed\n", __func__);
		return -EIO;
	}
	
//	printk("i2c_transfer completed.\n");
	return 0;
}




static int rv3029_i2c_write_regs(struct i2c_client *client, u8 const *buf)
{

  
//	printk("Now executing rv3029_i2c_write_regs.\n");
//	printk("Preparing Message...\n");

		struct i2c_msg msgs[1] = {
		{
		 .addr = client->addr,
		 .flags = 0,
		 .len = 8,
		 .buf = &buf[0]} 

	};
	int rc;

//	printk("Now calling i2c_transfer.\n");
	rc = i2c_transfer(client->adapter, msgs, ARRAY_SIZE(msgs));
	
	if (rc != ARRAY_SIZE(msgs)) {
		dev_err(&client->dev, "%s: register write failed\n", __func__);
		return -EIO;
	}
	
//	printk("i2c_transfer completed.\n");
	return 0;
}




static int rv3029_i2c_read_time(struct i2c_client *client, struct rtc_time *tm)
{
	int rc;
	u8 regs[8];

//	printk("Successfully called rv3029_i2c_read_time\n");	

//	printk("Now calling rv3029_i2c_read_regs\n");
	rc = rv3029_i2c_read_regs(client, regs);
	if (rc < 0)
		return rc;
	
//	printk("Successfully called rv3029_i2c_read_regs\n");
	tm->tm_sec  = bcd2bin(regs[0]);
	tm->tm_min  = bcd2bin(regs[1]);
	tm->tm_hour = bcd2bin(regs[2] & 0x3f);
	tm->tm_mday = bcd2bin(regs[3]);
	tm->tm_mon  = bcd2bin(regs[5]);
	tm->tm_year = 100 + bcd2bin(regs[6]);
	tm->tm_wday = bcd2bin(regs[4]);

//	printk("regs successfully done\n");
	return 0;
}



static int
rv3029_i2c_set_time(struct i2c_client *client, struct rtc_time const *tm)
{
	u8 regs[8];
	int rc;

	regs[0] = 8;
	regs[1] = bin2bcd(tm->tm_sec);
	regs[2] = bin2bcd(tm->tm_min);
	regs[3] = bin2bcd(tm->tm_hour);
	regs[4] = bin2bcd(tm->tm_mday);
//	regs[5] = bin2bcd(tm->tm_mon);	
	regs[6] = bin2bcd(tm->tm_mon);
	regs[5] = bin2bcd(tm->tm_wday);
	regs[7] = bin2bcd(tm->tm_year % 100);

	
	rc = rv3029_i2c_write_regs(client, regs);
	if (rc < 0)
		return rc;

	return 0;
}

static int rv3029_rtc_read_time(struct device *dev, struct rtc_time *tm)
{
//        printk("Now calling rv3029_i2c_read_time\n");
	return rv3029_i2c_read_time(to_i2c_client(dev), tm);
}

static int rv3029_rtc_set_time(struct device *dev, struct rtc_time *tm)
{
  
//        printk("Now calling rv3029_i2c_write_time\n");
	return rv3029_i2c_set_time(to_i2c_client(dev), tm);
}




static int rv3029_remove(struct i2c_client *client)
{
	struct rtc_device *rtc = i2c_get_clientdata(client);

	if (rtc)
		rtc_device_unregister(rtc);

	return 0;
}

static const struct rtc_class_ops rv3029_rtc_ops = {
	.read_time = rv3029_rtc_read_time,
	.set_time = rv3029_rtc_set_time,
};

static int
rv3029_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct rtc_device *rtc;

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C))
		return -ENODEV;

	dev_info(&client->dev, "chip found, driver version " DRV_VERSION "\n");

	rtc = rtc_device_register(rv3029_driver.driver.name,
				  &client->dev, &rv3029_rtc_ops, THIS_MODULE);
	if (IS_ERR(rtc))
		return PTR_ERR(rtc);

	printk("Now probing the device\n");

	i2c_set_clientdata(client, rtc);
	printk("Probe Done\n");
	return 0;
}

static struct i2c_device_id rv3029_id[] = {
	{ "rv3029", 0 },
	{ }
};

static struct i2c_driver rv3029_driver = {
	.driver = {
		   .name = "rtc-rv3029",
		   },
	.probe = rv3029_probe,
	.remove = rv3029_remove,
	.id_table = rv3029_id,
};

static int __init rv3029_init(void)
{
	return i2c_add_driver(&rv3029_driver);
}

static void __exit rv3029_exit(void)
{
	i2c_del_driver(&rv3029_driver);
}

MODULE_DESCRIPTION("Micro Crystal RV3029 RTC driver");
MODULE_AUTHOR("Eduard Tasnadi-Olescher <zed666@gmx.net>");
MODULE_LICENSE("GPL");
MODULE_VERSION(DRV_VERSION);

module_init(rv3029_init);
module_exit(rv3029_exit);

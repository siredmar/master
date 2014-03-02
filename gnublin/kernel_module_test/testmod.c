#include <linux/module.h>
#define DEBUG 1
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <mach/hardware.h>
#include <mach/gpio.h>
#include <linux/timer.h>
#include <linux/platform_device.h>
#include <mach/hardware.h>
#include <mach/gpio.h>
#include <asm/io.h>


struct testmod {
   struct device *dev;
   volatile unsigned short *ctrl_io;
   volatile unsigned short *data_io;
   char *name;
};


struct timer_list timer1;
int i = 0;
int counter = 0;

void timer1_routine(unsigned long data)
{
   unsigned short int id[5];
   struct testmod *item = (struct testmod*) data;     // cast unsignled long back to struct testmod

   id[0] = 0;
   id[1] = 0;
   id[2] = 0;
   id[3] = 0;
   id[4] = 0;

   printk( KERN_ALERT "item->name: %s\n", item->name);
   if((i%2) == 1)
	{
		printk(KERN_ALERT  "GPIO is 0, BUS Command is\t%d\n", counter);
		gpio_direction_output(20, 1);    // Backlight On
		//writew(counter, item->ctrl_io);
	}
	else
	{
	   printk(KERN_ALERT  "GPIO is 1, BUS Data is\t\t%d\n", counter);
		gpio_direction_output(20, 0);   // Backlight Off
		//writew(counter, item->data_io);
	}

	counter++;

   //writew(0x00AA, item->ctrl_io); // software reset
//     writew(0x0055, item->data_io); // software reset

	writew(0x0001, item->ctrl_io); // software reset
	mdelay(200);
	writew(0x00A1, item->ctrl_io);  // read signature
	mdelay(1);
	id[0] = readw(item->data_io);
	id[1] = readw(item->data_io);
	id[2] = readw(item->data_io);
	id[3] = readw(item->data_io);
	id[4] = readw(item->data_io);

	printk(KERN_ALERT "SSD1963: signature expected:\t01 57 61 01 FF\n");
   printk(KERN_ALERT "SSD1963: signature read:\t%02X %02X %02X %02X %02X\n", id[0],id[1],id[2],id[3],id[4]);


	printk(KERN_ALERT  "Inside Timer Routine count-> %d data passed\n\n",i++);
	mod_timer(&timer1, jiffies + msecs_to_jiffies(1000));
}

int testmod_main(struct testmod *item)
{
	printk(KERN_ALERT  "gpio test driver loading... please be patient\n");

//	gpio_direction_output(19, 1);    // SSD1963 Reset High
//	mdelay(200);
//	gpio_direction_output(19, 0);    // SSD1963 Reset LOW
//	mdelay(200);
//	gpio_direction_output(19, 1);    // SSD1963 Reset High

	init_timer(&timer1);

	timer1.function = timer1_routine;
	timer1.data = (unsigned long)item;     // cast item to unsigned long because timer1.data type is unsigned long
	timer1.expires = jiffies + HZ; /* 1 second */
	add_timer(&timer1); /* Starting the timer */

	printk(KERN_ALERT "Timer Module loaded\n");
	return 0;
}



static int __init testmod_probe(struct platform_device *dev)
{
   int ret = 0;
   struct testmod *item;
   struct resource *ctrl_res;
   struct resource *data_res;
   unsigned int ctrl_res_size;
   unsigned int data_res_size;
   struct resource *ctrl_req;
   struct resource *data_req;

//   MPMC_STCONFIG0  = 0x81;
//   MPMC_STWTWEN0   = 15;    // (0 + 1) * (1/90 MHz) = 11 ns
//   MPMC_STWTOEN0   = 15;    // 0 ns
//   MPMC_STWTRD0    = 31;   // (3 + 1) * ... = 44 ns
//   MPMC_STWTPG0    = 15;    // 0 ns
//   MPMC_STWTWR0    = 15;    //
//   MPMC_STWTTURN0  = 31;   //
//
   MPMC_STCONFIG0  = 0x81;
   MPMC_STWTWEN0   = 2;
   MPMC_STWTOEN0   = 1;
   MPMC_STWTRD0    = 15;
   MPMC_STWTPG0    = 2;
   MPMC_STWTWR0    = 4;
   MPMC_STWTTURN0  = 2;

   /* enable oe toggle between consecutive reads */
//   SYS_MPMC_WTD_DEL0 = _BIT(5) | 15;

//    MPMC_STCONFIG0 = 0x81;
//    MPMC_STWTWEN0  = 0;
//    MPMC_STWTOEN0  = 0;
//    MPMC_STWTRD0   = 31;
//    MPMC_STWTPG0   = 0;
//    MPMC_STWTWR0   = 3;
//    MPMC_STWTTURN0 = 0;
//
//   SYS_MPMC_WTD_DEL0 = _BIT(5) | 31;


   dev_dbg(&dev->dev, "%s\n", __func__);

      item = kzalloc(sizeof(struct testmod), GFP_KERNEL);
      if (!item) {
         dev_err(&dev->dev, "%s: unable to kzalloc for testmod\n", __func__);
         ret = -ENOMEM;
         goto out;
      }
      item->dev = &dev->dev;
      dev_set_drvdata(&dev->dev, item);

      ctrl_res = platform_get_resource(dev, IORESOURCE_MEM, 0);
      if (!ctrl_res) {
         dev_err(&dev->dev, "%s: unable to platform_get_resource for ctrl_res\n",
            __func__);
         ret = -ENOENT;
         goto out_item;
      }
      ctrl_res_size = ctrl_res->end - ctrl_res->start + 1;
      ctrl_req = request_mem_region(ctrl_res->start, ctrl_res_size,
                     dev->name);
      if (!ctrl_req) {
         dev_err(&dev->dev, "%s: unable to request_mem_region for ctrl_req\n", __func__);
         ret = -EIO;
         goto out_item;
      }
      item->ctrl_io = ioremap(ctrl_res->start, ctrl_res_size);
      if (!item->ctrl_io) {
         ret = -EINVAL;
         dev_err(&dev->dev, "%s: unable to ioremap for ctrl_io\n", __func__);
         goto out_item;
      }

      data_res = platform_get_resource(dev, IORESOURCE_MEM, 1);
      if (!data_res) {
         dev_err(&dev->dev, "%s: unable to platform_get_resource for data_res\n", __func__);
         ret = -ENOENT;
         goto out_item;
      }
      data_res_size = data_res->end - data_res->start + 1;
      data_req = request_mem_region(data_res->start,
                     data_res_size, dev->name);
      if (!data_req) {
         dev_err(&dev->dev, "%s: unable to request_mem_region for data_req\n", __func__);
         ret = -EIO;
         goto out_item;
      }
      item->data_io = ioremap(data_res->start, data_res_size);
      if (!item->data_io) {
         ret = -EINVAL;
         dev_err(&dev->dev, "%s: unable to ioremap for data_io\n", __func__);
         goto out_item;
      }

      item->name = "testmod_str";

      dev_dbg(&dev->dev, "%s: ctrl_io=%p data_io=%p\n", __func__, item->ctrl_io, item->data_io);
      goto out;

      out_item:
      kfree(item);

      out:
      return testmod_main(item);
}



static int __devexit testmod_exit(struct platform_device *dev);

static struct platform_driver testmod_driver = {
   .driver = {
         .name = "testmod",
         .owner   = THIS_MODULE,
         },
   .probe  = testmod_probe,
   .remove = __devexit_p(testmod_exit),
};

static int __init testmod_init(void)
{
   int ret = 0;

   pr_debug("%s\n", __func__);

   ret = platform_driver_register(&testmod_driver);
   if (ret) {
      pr_err("%s: unable to platform_driver_register\n", __func__);
   }

   return ret;
}

/*
 * testmod_exit - die exit-function
 */
static int __devexit testmod_exit(struct platform_device *dev)
{
   printk(KERN_ALERT  "testmod_exit() called\n");

   platform_device_unregister(dev);       // FIXME: kill the driver... why is this not working correctly?
   del_timer_sync(&timer1);

   printk(KERN_ALERT  "Kill yourself!!\n");
   return 0;
}


module_init(testmod_init);
module_exit(testmod_exit);

//EXPORT_SYMBOL();
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Anyone <anyone@foo.invalid>");
MODULE_DESCRIPTION("Ein einfaches Beispiel");

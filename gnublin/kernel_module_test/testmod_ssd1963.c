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

#define LED_ENABLE_PIN  20
#define RESET_PIN       19
#define SCREENSIZE   130560      // 480*272

/* TFT Controler Register defines */
#define SSD1963_NOP           0x0000
#define SOFT_RESET            0x0001
#define GET_POWER_MODE        0x000A
#define GET_ADDRESS_MODE      0x000B
#define GET_PIXEL_FORMAT      0x000C
#define GET_DISPLAY_MODE      0x000D
#define GET_SIGNAL_MODE       0x000E
#define ENTER_SLEEP_MODE      0x0010
#define EXIT_SLEEP_MODE       0x0011
#define ENTER_PARTIAL_MODE    0x0012
#define ENTER_NORMAL_MODE     0x0013
#define EXIT_INVERT_MODE      0x0020
#define ENTER_INVERT_MODE     0x0021
#define SET_GAMMA_CURVE       0x0026
#define SET_DISPLAY_OFF       0x0028
#define SET_DISPLAY_ON        0x0029
#define SET_COLUMN_ADDRESS    0x002A
#define SET_PAGE_ADDRESS      0x002B
#define WRITE_MEMORY_START    0x002C
#define READ_MEMORY_START     0x002E
#define SET_PARTIAL_AERA      0x0030
#define SET_SCROLL_AREA       0x0033
#define SET_TEAR_OFF          0x0034
#define SET_TEAR_ON           0x0035
#define SET_ADDRESS_MODE      0x0036
#define SET_SCROLL_START      0x0037
#define EXIT_IDLE_MODE        0x0038
#define ENTER_IDLE_MODE       0x0039
#define SET_PIXEL_FORMAT      0x003A
#define WRITE_MEMORY_CONTINUE 0x003C
#define READ_MEMORY_CONTINUE  0x003E
#define SET_TEAR_SCNALINE     0x0044
#define GET_SCANLINE          0x0045
#define READ_DDB              0x00A1
#define SET_LCD_MODE          0x00B0
#define GET_LCD_MODE          0x00B1
#define SET_HORI_PERIOD       0x00B4
#define GET_HORI_PERIOD       0x00B5
#define SET_VERT_PERIOD       0x00B6
#define GET_VERT_PERIOD       0x00B7
#define SET_GPIO_CONF         0x00B8
#define GET_GPIO_CONF         0x00B9
#define SET_GPIO_VALUE        0x00BA
#define GET_GPIO_STATUS       0x00BB
#define SET_POST_PROC         0x00BC
#define GET_POST_PROC         0x00BD
#define SET_PWM_CONF          0x00BE
#define GET_PWM_CONF          0x00BF
#define SET_LCD_GEN0          0x00C0
#define GET_LCD_GEN0          0x00C1
#define SET_LCD_GEN1          0x00C2
#define GET_LCD_GEN1          0x00C3
#define SET_LCD_GEN2          0x00C4
#define GET_LCD_GEN2          0x00C5
#define SET_LCD_GEN3          0x00C6
#define GET_LCD_GEN3          0x00C7
#define SET_GPIO0_ROP         0x00C8
#define GET_GPIO0_ROP         0x00C9
#define SET_GPIO1_ROP         0x00CA
#define GET_GPIO1_ROP         0x00CB
#define SET_GPIO2_ROP         0x00CC
#define GET_GPIO2_ROP         0x00CD
#define SET_GPIO3_ROP         0x00CE
#define GET_GPIO3_ROP         0x00CF
#define SET_DBC_CONF          0x00D0
#define GET_DBC_CONF          0x00D1
#define SET_DBC_TH            0x00D4
#define GET_DBC_TH            0x00D5
#define SET_PLL               0x00E0
#define SET_PLL_MN            0x00E2
#define GET_PLL_MN            0x00E3
#define GET_PLL_STATUS        0x00E4
#define SET_DEEP_SLEEP        0x00E5
#define SET_LSHIFT_FREQ       0x00E6
#define GET_LSHIFT_FREQ       0x00E7
#define SET_PIXEL_DATA_INTERFACE 0x00F0
#define GET_PIXEL_DATA_INTERFACE 0x00F1

#define SCREENWIDTH                      480
#define SCREENHEIGHT                     272

#define HDP                              479        //479
#define HT                               525        //531
#define HPS                              40         //43
#define LPS                              6           //8
#define HPW                              39         //10
//
#define VDP                              271        //271
#define VT                               288        //288
#define VPS                              12         //12
#define FPS                              8          //4
#define VPW                              11         //10


struct testmod {
   struct device *dev;
   volatile unsigned short *ctrl_io;
   volatile unsigned short *data_io;
   char *name;
};


struct timer_list timer1;
int i = 0;
int counter = 0;

static void fillRect(struct testmod *item, unsigned short xs, unsigned short xe, unsigned short ys, unsigned short ye, unsigned short color)
{

   unsigned int i;
   unsigned short id[5] = {0};
   unsigned short x_width = xe - xs;
   unsigned short y_width = ye - ys;
   unsigned int area = x_width * y_width;

   writew(READ_DDB, item->ctrl_io);
     mdelay(1);
     id[0] = readw(item->data_io);
     id[1] = readw(item->data_io);
     id[2] = readw(item->data_io);
     id[3] = readw(item->data_io);
     id[4] = readw(item->data_io);


     //dev_dbg(&dev->dev, "%s: signature=%02X %02X %02X %02X %02X\n", __func__, id[0],id[1],id[2],id[3],id[4]);
     printk(KERN_ALERT "SSD1963_fillRect: signature expected:\t01 57 61 01 FF\n");
     printk(KERN_ALERT "SSD1963_fillRect: signature read:\t%02X %02X %02X %02X %02X\n", id[0],id[1],id[2],id[3],id[4]);

   writew(SET_COLUMN_ADDRESS, item->ctrl_io);
   writew((xs >> 8) & 0x00FF , item->data_io);
   writew((xs & 0x00FF), item->data_io);
   writew((xe  >> 8 ) & 0x00FF, item->data_io);
   writew((xe & 0x00FF), item->data_io);

   writew(SET_PAGE_ADDRESS, item->ctrl_io);
   writew((ys >> 8) & 0x00FF , item->data_io);
   writew((ys & 0x00FF), item->data_io);
   writew((ye  >> 8 ) & 0x00FF, item->data_io);
   writew((ye & 0x00FF), item->data_io);

   writew(WRITE_MEMORY_START, item->ctrl_io);

   for(i = 0; i < area; i++)
   {
      writew(color, item->data_io);
   }

}
void timer1_routine(unsigned long data)
{
   static unsigned short color = 0;
   struct testmod *item = (struct testmod*) data;
   fillRect(item, 100, 100, 200, 200, color);
   color = color + 100;
   mod_timer(&timer1, jiffies + msecs_to_jiffies(1000));

}

void ssd1963_init(struct testmod *item)
{
   unsigned short int id[5] = {0};

 //  struct testmod *item = (struct testmod*) data;
   gpio_direction_output(RESET_PIN, 1);
   mdelay(1);
   gpio_direction_output(RESET_PIN, 0);
   mdelay(200);
   gpio_direction_output(RESET_PIN, 1);

   writew(SOFT_RESET, item->ctrl_io);
	mdelay(200);
	writew(READ_DDB, item->ctrl_io);
	mdelay(1);
	id[0] = readw(item->data_io);
	id[1] = readw(item->data_io);
	id[2] = readw(item->data_io);
	id[3] = readw(item->data_io);
	id[4] = readw(item->data_io);


	//dev_dbg(&dev->dev, "%s: signature=%02X %02X %02X %02X %02X\n", __func__, id[0],id[1],id[2],id[3],id[4]);
	printk(KERN_ALERT "SSD1963: signature expected:\t01 57 61 01 FF\n");
   printk(KERN_ALERT "SSD1963: signature read:\t%02X %02X %02X %02X %02X\n", id[0],id[1],id[2],id[3],id[4]);


   gpio_direction_output(LED_ENABLE_PIN, 1);


      writew(SET_PLL, item->ctrl_io); // PLL config - continued
      msleep(1U);
      writew(0x0001, item->data_io);
      msleep(1U);

      writew(SET_PLL_MN, item->ctrl_io); // PLL config - continued
      msleep(1U);
      writew(0x001D, item->data_io);
      msleep(1U);
      writew(0x0002, item->data_io);
      msleep(1U);
      writew(0x0054, item->data_io);
      msleep(1000U);


      writew(SET_PLL, item->ctrl_io); // PLL config - continued
      msleep(1U);
      writew(0x0003, item->data_io);
      msleep(200U);


      writew(SOFT_RESET, item->ctrl_io);
      msleep(100U);

      writew(SET_LSHIFT_FREQ, item->ctrl_io);
      writew(0x0001, item->data_io); // LSHIFT freq
      writew(0x0070, item->data_io);
      writew(0x00A2, item->data_io);

      writew(SET_LCD_MODE, item->ctrl_io);
      writew(0x0038, item->data_io);
      writew(0x0000, item->data_io);
      writew(0x0001, item->data_io);
      writew(0x00DF, item->data_io);
      writew(0x0001, item->data_io);
      writew(0x000F, item->data_io);
      writew(0x0000, item->data_io);
      msleep(5U);

      writew(SET_HORI_PERIOD, item->ctrl_io);
      writew(((HT >> 8U) & 0x00FF), item->data_io);  // horizontal period
      writew((HT & 0x00FF), item->data_io);
      writew(((HPS >> 8U) & 0x00FF), item->data_io);
      writew((HPS & 0x00FF), item->data_io);
      writew(HPW, item->data_io);
      writew(((LPS >> 8U) & 0x00FF), item->data_io);
      writew((LPS & 0x00FF), item->data_io);
      writew(0x0000, item->data_io);

      writew(SET_VERT_PERIOD, item->ctrl_io);
      writew(((VT >> 8U) & 0x00FF), item->data_io); // vertical period
      writew((VT & 0x00FF), item->data_io);
      writew((VPS >> 8U) & 0x00FF, item->data_io);
      writew((VPS & 0x00FF), item->data_io);
      writew(VPW, item->data_io);
      writew(((FPS >> 8U) & 0x00FF), item->data_io);
      writew((FPS & 0x00FF), item->data_io);

      writew(SET_ADDRESS_MODE, item->ctrl_io);
      writew(0x0000, item->data_io);  // rotation - landscape

      writew(SET_PIXEL_DATA_INTERFACE, item->ctrl_io);
      writew(0x0003, item->data_io);  // pixel data interface 16 bit    0b011 is 16 Bit
      msleep(5U);

      writew(SET_DISPLAY_ON, item->ctrl_io); // set display on

      writew(SET_PWM_CONF, item->ctrl_io);
      writew(0x0006, item->data_io);  // pwm config
      writew(0x00F0, item->data_io);
      writew(0x0001, item->data_io);
      writew(0x00F0, item->data_io);
      writew(0x0000, item->data_io);
      writew(0x0000, item->data_io);

      writew(SET_DBC_CONF, item->ctrl_io);
      writew(0x000D, item->data_io); // pwm coffig continued

      writew(SET_GPIO_CONF, item->ctrl_io);
      writew(0x0000, item->data_io); // gpio config
      writew(0x0000, item->data_io);

      writew(SET_GPIO_VALUE, item->ctrl_io);
      writew(0x0000, item->data_io); // gpio to 0x00



}

int testmod_main(struct testmod *item)
{

   printk(KERN_ALERT  "gpio test driver loading... please be patient\n");

   //ssd1963_init(item);
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

   /* enable oe toggle between consecutive reads */
   //SYS_MPMC_WTD_DEL0 = _BIT(5) | 15;

    MPMC_STCONFIG0 = 0x81;
    MPMC_STWTWEN0  = 0;
    MPMC_STWTOEN0  = 0;
    MPMC_STWTRD0   = 31;
    MPMC_STWTPG0   = 0;
    MPMC_STWTWR0   = 15;
    MPMC_STWTTURN0 = 0;

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

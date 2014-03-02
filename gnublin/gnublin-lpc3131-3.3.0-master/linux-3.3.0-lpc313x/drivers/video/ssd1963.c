/*
* SSD1963 Framebuffer
*
* Copyright (c) 2013 Armin Schlegel <armin.schlegel@gmx.de>
* Original: Copyright (c) 2012 Jeroen Domburg <jeroen@spritesmods.com>
* Original: Copyright (c) 2009 Jean-Christian de Rivaz
*
* This is an adaption from the ili9325.c from Jeroen Domburg to work with
* the SSD1963 controller.
* Bits and pieces borrowed from the fsl-ili9325.c:
* Copyright (C) 2010-2011 Freescale Semiconductor, Inc. All Rights Reserved.
* Author: Alison Wang <b18965@freescale.com>
*         Jason Jin <Jason.jin@freescale.com>
*
* This file is subject to the terms and conditions of the GNU General Public
* License.  See the file "COPYING" in the main directory of this archive
* for more details.
*
* The Solomon Systech SSD1963 chip drive TFT screen with a resolution of 480x272.
*
* For direct I/O-mode:
*
* This driver expect the SSD1963 to be connected to a 16 bits local bus
* and to be set in the 16 bits parallel interface mode. To use it you must
* define in your board file a struct platform_device with a name set to
* "ssd1963" and a struct resource array with two IORESOURCE_MEM: the first
* for the control register; the second for the data register.
*
*/

#include <linux/kernel.h>
#define DEBUG 1
#include <linux/device.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/mm.h>
#include <linux/vmalloc.h>
#include <linux/fb.h>
#include <linux/slab.h>
#include <asm/io.h>
#include <mach/gpio.h>
#include <linux/delay.h>

/* Connection to Display
 * LPC313x        SSD1963
 * ======================
 * LPC_STCS0      CS
 * LPC_DB[0:15]   DB[0:15]
 * LPC_A15        RS
 * LPC_DQM0_NOE   RD
 * LPC_WE         R/W
 * GPIO19         RESET
 * GPIO20         LED-A
 */

#define LED_ENABLE_PIN  20
#define RESET_PIN       19


#define PIXELGROUPLEN 40
#define SCREENSIZE   130560      // 480*272

#define DATA      1
#define COMMAND   0
#define HEIGHT 272
#define WIDTH  480

#define SSD1963_LINES_PER_PAGE  32
#define SSD1963_PAGE_SIZE  (SSD1963_LINES_PER_PAGE*WIDTH*2)

/* TFT Controler Register defines */
#define SSD1963_NOP              0x00
#define SOFT_RESET               0x01
#define GET_POWER_MODE           0x0A
#define GET_ADDRESS_MODE         0x0B
#define GET_PIXEL_FORMAT         0x0C
#define GET_DISPLAY_MODE         0x0D
#define GET_SIGNAL_MODE          0x0E
#define ENTER_SLEEP_MODE         0x10
#define EXIT_SLEEP_MODE          0x11
#define ENTER_PARTIAL_MODE       0x12
#define ENTER_NORMAL_MODE        0x13
#define EXIT_INVERT_MODE         0x20
#define ENTER_INVERT_MODE        0x21
#define SET_GAMMA_CURVE          0x26
#define SET_DISPLAY_OFF          0x28
#define SET_DISPLAY_ON           0x29
#define SET_COLUMN_ADDRESS       0x2A
#define SET_PAGE_ADDRESS         0x2B
#define WRITE_MEMORY_START       0x2C
#define READ_MEMORY_START        0x2E
#define SET_PARTIAL_AERA         0x30
#define SET_SCROLL_AREA          0x33
#define SET_TEAR_OFF             0x34
#define SET_TEAR_ON              0x35
#define SET_ADDRESS_MODE         0x36
#define SET_SCROLL_START         0x37
#define EXIT_IDLE_MODE           0x38
#define ENTER_IDLE_MODE          0x39
#define SET_PIXEL_FORMAT         0x3A
#define WRITE_MEMORY_CONTINUE    0x3C
#define READ_MEMORY_CONTINUE     0x3E
#define SET_TEAR_SCNALINE        0x44
#define GET_SCANLINE             0x45
#define READ_DDB                 0xA1
#define SET_LCD_MODE             0xB0
#define GET_LCD_MODE             0xB1
#define SET_HORI_PERIOD          0xB4
#define GET_HORI_PERIOD          0xB5
#define SET_VERT_PERIOD          0xB6
#define GET_VERT_PERIOD          0xB7
#define SET_GPIO_CONF            0xB8
#define GET_GPIO_CONF            0xB9
#define SET_GPIO_VALUE           0xBA
#define GET_GPIO_STATUS          0xBB
#define SET_POST_PROC            0xBC
#define GET_POST_PROC            0xBD
#define SET_PWM_CONF             0xBE
#define GET_PWM_CONF             0xBF
#define SET_LCD_GEN0             0xC0
#define GET_LCD_GEN0             0xC1
#define SET_LCD_GEN1             0xC2
#define GET_LCD_GEN1             0xC3
#define SET_LCD_GEN2             0xC4
#define GET_LCD_GEN2             0xC5
#define SET_LCD_GEN3             0xC6
#define GET_LCD_GEN3             0xC7
#define SET_GPIO0_ROP            0xC8
#define GET_GPIO0_ROP            0xC9
#define SET_GPIO1_ROP            0xCA
#define GET_GPIO1_ROP            0xCB
#define SET_GPIO2_ROP            0xCC
#define GET_GPIO2_ROP            0xCD
#define SET_GPIO3_ROP            0xCE
#define GET_GPIO3_ROP            0xCF
#define SET_DBC_CONF             0xD0
#define GET_DBC_CONF             0xD1
#define SET_DBC_TH               0xD4
#define GET_DBC_TH               0xD5
#define SET_PLL                  0xE0
#define SET_PLL_MN               0xE2
#define GET_PLL_MN               0xE3
#define GET_PLL_STATUS           0xE4
#define SET_DEEP_SLEEP           0xE5
#define SET_LSHIFT_FREQ          0xE6
#define GET_LSHIFT_FREQ          0xE7
#define SET_PIXEL_DATA_INTERFACE 0xF0
#define GET_PIXEL_DATA_INTERFACE 0xF1

#define HDP                                 479        //479
#define HT                                  525        //531
#define HPS                                 40         //43
#define LPS                                 6           //8
#define HPW                                 39         //10
//
#define VDP                                 271        //271
#define VT                                  288        //288
#define VPS                                 12         //12
#define FPS                                 8          //4
#define VPW                                 11         //10


struct ssd1963_page {
	unsigned short x;
	unsigned short y;
	unsigned short *buffer;
	unsigned short len;
};

struct ssd1963 {
	struct device *dev;
	volatile unsigned short *ctrl_io;
	volatile unsigned short *data_io;
	struct fb_info *info;
	unsigned int pages_count;
	struct ssd1963_page *pages;
};

static inline void ssd1963_send_data(struct ssd1963 *item, unsigned short value)
{
	writew(value, item->data_io);
}

static inline void ssd1963_send_cmd(struct ssd1963 *item, unsigned char reg)
{
   unsigned short ctrl = reg & 0x00ff;

   writew(ctrl, item->ctrl_io);
}

void ssd1963_set_window(struct ssd1963 *item, unsigned short xs, unsigned short ys,
             unsigned short xe, unsigned short ye)
{
   ssd1963_send_cmd(item, SET_COLUMN_ADDRESS);
   ssd1963_send_data(item, (xs & 0xFF00) >> 8);
   ssd1963_send_data(item, (xs & 0x00FF));
   ssd1963_send_data(item, (xe & 0xFF00) >> 8);
   ssd1963_send_data(item, (xe & 0x00FF));

   ssd1963_send_cmd(item, SET_PAGE_ADDRESS);
   ssd1963_send_data(item, (ys & 0xFF00) >> 8);
   ssd1963_send_data(item, (ys & 0x00FF));
   ssd1963_send_data(item, (ye & 0xFF00) >> 8);
   ssd1963_send_data(item, (ye & 0x00FF));

   ssd1963_send_cmd(item, WRITE_MEMORY_START);
}

static void ssd1963_copy(struct ssd1963 *item, unsigned int index)
{
	unsigned short x_start;
	unsigned short y_start;
	unsigned short x_end;
	unsigned short y_end;
	unsigned short *buffer;
	unsigned int len;
	unsigned int count;

	x_start = item->pages[index].x;
	y_start = item->pages[index].y;

	/* e.g. x_end = 480 - 1 = 479 */
	x_end = WIDTH - 1;
	/* e.g. y_end = 0 + 32*480*2/480 -1 = 63 */
	y_end = y_start + SSD1963_PAGE_SIZE/WIDTH - 1;

	buffer = item->pages[index].buffer;
	len = item->pages[index].len;
	dev_dbg(item->dev,
		"%s: page[%u]: x=%3hu y=%3hu buffer=0x%p len=%3hu\n",
		__func__, index, x_start, y_start, buffer, len);



	ssd1963_set_window(item, x_start, y_start, x_end, y_end);

	for (count = 0; count < len; count++) {
		writew(buffer[count], item->data_io);
	}
}

static void ssd1963_update(struct fb_info *info, struct list_head *pagelist)
{
	struct ssd1963 *item = (struct ssd1963 *)info->par;
	struct page *page;

	list_for_each_entry(page, pagelist, lru) {
		ssd1963_copy(item, page->index);
	}
}

static void __init ssd1963_update_all(struct ssd1963 *item)
{
	unsigned short index;

	for (index = 0; index < item->pages_count; index++) {
		ssd1963_copy(item, index);
	}
}

static void __init ssd1963_setup(struct ssd1963 *item)
{
   dev_dbg(item->dev, "%s: item=0x%p\n", __func__, (void *)item);

   gpio_direction_output(LED_ENABLE_PIN, 1);


   ssd1963_send_cmd(item, SOFT_RESET);
   ssd1963_send_data(item, 0x0000);    // software reset
   msleep(10U);


   ssd1963_send_cmd(item,  SET_PLL);
   ssd1963_send_data(item, 0x0001); // PLL config - continued

   ssd1963_send_cmd(item,  SET_PLL_MN); // PLL config
   ssd1963_send_data(item, 0x002D);
   ssd1963_send_data(item, 0x0002);
   ssd1963_send_data(item, 0x0004);

   ssd1963_send_cmd(item,  SET_PLL);
   ssd1963_send_data(item, 0x0003);  // PLL config - continued
   msleep(5U);
//
//   ssd1963_send_cmd(item,  SOFT_RESET);
//   ssd1963_send_data(item, 0x0000);  // software reset
//   msleep(10U);

   ssd1963_send_cmd(item,  SET_LSHIFT_FREQ);
   ssd1963_send_data(item, 0x0000); // LSHIFT freq
   ssd1963_send_data(item, 0x00FF);
   ssd1963_send_data(item, 0x00BE);

   ssd1963_send_cmd(item,  SET_LCD_MODE);
   ssd1963_send_data(item, 0x0020);  // set lcd mode
   ssd1963_send_data(item, 0x0000);
   ssd1963_send_data(item, (((item->info->var.xres - 1) >> 8U) & 0x00FF));
   ssd1963_send_data(item, ((item->info->var.xres - 1) & 0x00FF));
   ssd1963_send_data(item, (((item->info->var.xres - 1) >> 8U) & 0x00FF));
   ssd1963_send_data(item, ((item->info->var.yres - 1) & 0x00FF));
   ssd1963_send_data(item, 0x0000);
   msleep(5U);

   ssd1963_send_cmd(item,  SET_HORI_PERIOD);
   ssd1963_send_data(item, ((HT >> 8U) & 0x00FF));  // horizontal period
   ssd1963_send_data(item, (HT & 0x00FF));
   ssd1963_send_data(item, ((HPS >> 8U) & 0x00FF));
   ssd1963_send_data(item, (HPS & 0x00FF));
   ssd1963_send_data(item, HPW);
   ssd1963_send_data(item, ((LPS >> 8U) & 0x00FF));
   ssd1963_send_data(item, (LPS & 0x00FF));
   ssd1963_send_data(item, 0x0000);

   ssd1963_send_cmd(item,  SET_VERT_PERIOD);
   ssd1963_send_data(item, ((VT >> 8U) & 0x00FF)); // vertical period
   ssd1963_send_data(item, (VT & 0x00FF));
   ssd1963_send_data(item, (VPS >> 8U) & 0x00FF);
   ssd1963_send_data(item, (VPS & 0x00FF));
   ssd1963_send_data(item, VPW);
   ssd1963_send_data(item, ((FPS >> 8U) & 0x00FF));
   ssd1963_send_data(item, (FPS & 0x00FF));

   ssd1963_send_cmd(item,  SET_ADDRESS_MODE);
   ssd1963_send_data(item, 0x0000);  // rotation - landscape

   ssd1963_send_cmd(item,  SET_PIXEL_DATA_INTERFACE);
   ssd1963_send_data(item, 0x0003);  // pixel data interface 16 bit    0b011 is 16 Bit
   msleep(5U);

   ssd1963_send_cmd(item,  SET_DISPLAY_ON); // set display on

   ssd1963_send_cmd(item,  SET_PWM_CONF);
   ssd1963_send_data(item, 0x0006);  // pwm config
   ssd1963_send_data(item, 0x00F0);
   ssd1963_send_data(item, 0x0001);
   ssd1963_send_data(item, 0x00F0);
   ssd1963_send_data(item, 0x0000);
   ssd1963_send_data(item, 0x0000);

   ssd1963_send_cmd(item,  SET_DBC_CONF);
   ssd1963_send_data(item, 0x000D); // pwm coffig continued

   ssd1963_send_cmd(item,  SET_GPIO_CONF);
   ssd1963_send_data(item, 0x0000); // gpio config
   ssd1963_send_data(item, 0x0000);

   ssd1963_send_cmd(item,  SET_GPIO_VALUE);
   ssd1963_send_data(item, 0x0000); // gpio to 0x00
}

static int __init ssd1963_video_alloc(struct ssd1963 *item)
{
	unsigned int frame_size;

	dev_dbg(item->dev, "%s: item=0x%p\n", __func__, (void *)item);

	frame_size = item->info->fix.line_length * item->info->var.yres;
	dev_dbg(item->dev, "%s: item=0x%p frame_size=%u\n",
		__func__, (void *)item, frame_size);

	item->pages_count = frame_size / SSD1963_PAGE_SIZE;
	if ((item->pages_count * SSD1963_PAGE_SIZE) < frame_size) {
		item->pages_count++;
	}
	dev_dbg(item->dev, "%s: item=0x%p pages_count=%u\n",
		__func__, (void *)item, item->pages_count);

	item->info->fix.smem_len = item->pages_count * SSD1963_PAGE_SIZE;
	item->info->fix.smem_start =
	    (unsigned long)vmalloc(item->info->fix.smem_len);
	if (!item->info->fix.smem_start) {
		dev_err(item->dev, "%s: unable to vmalloc\n", __func__);
		return -ENOMEM;
	}
	memset((void *)item->info->fix.smem_start, 0, item->info->fix.smem_len);

	return 0;
}

static void ssd1963_video_free(struct ssd1963 *item)
{
	dev_dbg(item->dev, "%s: item=0x%p\n", __func__, (void *)item);

	kfree((void *)item->info->fix.smem_start);
}

static int __init ssd1963_pages_alloc(struct ssd1963 *item)
{
	unsigned short pixels_per_page;
	unsigned short yoffset_per_page;
	unsigned short xoffset_per_page;
	unsigned short index;
	unsigned short x = 0;
	unsigned short y = 0;
	unsigned short *buffer;
	unsigned int len;

	dev_dbg(item->dev, "%s: item=0x%p\n", __func__, (void *)item);

	item->pages = kmalloc(item->pages_count * sizeof(struct ssd1963_page),
			      GFP_KERNEL);
	if (!item->pages) {
		dev_err(item->dev, "%s: unable to kmalloc for ssd1963_page\n",
			__func__);
		return -ENOMEM;
	}

	pixels_per_page = SSD1963_PAGE_SIZE / (item->info->var.bits_per_pixel / 8);
	yoffset_per_page = pixels_per_page / item->info->var.xres;
	xoffset_per_page = pixels_per_page -
	    (yoffset_per_page * item->info->var.xres);
	dev_dbg(item->dev, "%s: item=0x%p pixels_per_page=%hu "
		"yoffset_per_page=%hu xoffset_per_page=%hu\n",
		__func__, (void *)item, pixels_per_page,
		yoffset_per_page, xoffset_per_page);

	buffer = (unsigned short *)item->info->fix.smem_start;
	for (index = 0; index < item->pages_count; index++) {
		len = (item->info->var.xres * item->info->var.yres) -
		    (index * pixels_per_page);
		if (len > pixels_per_page) {
			len = pixels_per_page;
		}
		dev_dbg(item->dev,
			"%s: page[%d]: x=%3hu y=%3hu buffer=0x%p len=%3hu\n",
			__func__, index, x, y, buffer, len);
		item->pages[index].x = x;
		item->pages[index].y = y;
		item->pages[index].buffer = buffer;
		item->pages[index].len = len;

		x += xoffset_per_page;
		if (x >= item->info->var.xres) {
			y++;
			x -= item->info->var.xres;
		}
		y += yoffset_per_page;
		buffer += pixels_per_page;
	}

	return 0;
}

static void ssd1963_pages_free(struct ssd1963 *item)
{
	dev_dbg(item->dev, "%s: item=0x%p\n", __func__, (void *)item);

	kfree(item->pages);
}

static struct fb_ops ssd1963_fbops = {
	.owner        = THIS_MODULE,
	.fb_fillrect  = sys_fillrect,
	.fb_copyarea  = sys_copyarea,
	.fb_imageblit = sys_imageblit,
};

static struct fb_fix_screeninfo ssd1963_fix __initdata = {
	.id          = "SSD1963",
	.type        = FB_TYPE_PACKED_PIXELS,
	.visual      = FB_VISUAL_DIRECTCOLOR,
	.accel       = FB_ACCEL_NONE,
	.line_length = 240 * 2,
};

static struct fb_var_screeninfo ssd1963_var __initdata = {
	.xres		= 480,
	.yres		= 272,
	.xres_virtual	= 480,
	.yres_virtual	= 272,
	.bits_per_pixel	= 16,
	.red		= {6, 5, 0},
	.green		= {11, 5, 0},
	.blue		= {0, 6, 0},
	.activate	= FB_ACTIVATE_NOW,
	.height		= 480,
	.width		= 272,
	.vmode		= FB_VMODE_NONINTERLACED,
};

static struct fb_deferred_io ssd1963_defio = {
        .delay          = HZ / 50,
        .deferred_io    = &ssd1963_update,
};

static int __init ssd1963_probe(struct platform_device *dev)
{
	int ret = 0;
	struct ssd1963 *item;
	struct resource *ctrl_res;
	struct resource *data_res;
	unsigned int ctrl_res_size;
	unsigned int data_res_size;
	struct resource *ctrl_req;
	struct resource *data_req;
	unsigned short signature;
	struct fb_info *info;
	unsigned short int id[6];

	dev_dbg(&dev->dev, "%s\n", __func__);

	item = kzalloc(sizeof(struct ssd1963), GFP_KERNEL);
	if (!item) {
		dev_err(&dev->dev,
			"%s: unable to kzalloc for ssd1963\n", __func__);
		ret = -ENOMEM;
		goto out;
	}
	item->dev = &dev->dev;
	dev_set_drvdata(&dev->dev, item);

	ctrl_res = platform_get_resource(dev, IORESOURCE_MEM, 0);
	if (!ctrl_res) {
		dev_err(&dev->dev,
			"%s: unable to platform_get_resource for ctrl_res\n",
			__func__);
		ret = -ENOENT;
		goto out_item;
	}
	ctrl_res_size = ctrl_res->end - ctrl_res->start + 1;
	ctrl_req = request_mem_region(ctrl_res->start, ctrl_res_size,
				      dev->name);
	if (!ctrl_req) {
		dev_err(&dev->dev,
			"%s: unable to request_mem_region for ctrl_req\n",
			__func__);
		ret = -EIO;
		goto out_item;
	}
	item->ctrl_io = ioremap(ctrl_res->start, ctrl_res_size);
	if (!item->ctrl_io) {
		ret = -EINVAL;
		dev_err(&dev->dev,
			"%s: unable to ioremap for ctrl_io\n", __func__);
		goto out_item;
	}

	data_res = platform_get_resource(dev, IORESOURCE_MEM, 1);
	if (!data_res) {
		dev_err(&dev->dev,
			"%s: unable to platform_get_resource for data_res\n",
			__func__);
		ret = -ENOENT;
		goto out_item;
	}
	data_res_size = data_res->end - data_res->start + 1;
	data_req = request_mem_region(data_res->start,
				      data_res_size, dev->name);
	if (!data_req) {
		dev_err(&dev->dev,
			"%s: unable to request_mem_region for data_req\n",
			__func__);
		ret = -EIO;
		goto out_item;
	}
	item->data_io = ioremap(data_res->start, data_res_size);
	if (!item->data_io) {
		ret = -EINVAL;
		dev_err(&dev->dev,
			"%s: unable to ioremap for data_io\n", __func__);
		goto out_item;
	}

	dev_dbg(&dev->dev, "%s: ctrl_io=%p data_io=%p\n",
		__func__, item->ctrl_io, item->data_io);

/* Signature */
	   ssd1963_send_cmd(item, SOFT_RESET);    // software reset
	   mdelay(200);

	   ssd1963_send_cmd(item, READ_DDB);  // read signature

	   id[0] = readw(item->data_io);
	   id[1] = readw(item->data_io);
	   id[2] = readw(item->data_io);
	   id[3] = readw(item->data_io);
	   id[4] = readw(item->data_io);

	   //dev_dbg(&dev->dev, "%s: signature=%02X %02X %02X %02X %02X\n", __func__, id[0],id[1],id[2],id[3],id[4]);
	   printk(KERN_ALERT "SSD1963: reading signature expected:\t\t0x01 0x57 0x61 0x01 0xFF\n");

	   if (!(id[0]==0x01 && id[1]==0x57 && id[2]==0x61 && id[3]==0x01)) {
	      //ret = -ENODEV;
	      //dev_err(&dev->dev, "%s: unknown signature %02X %02X %02X %02X %02X\n", __func__, id[0],id[1],id[2],id[3],id[4]);
	      printk(KERN_ALERT "SSD1963: reading signature -> mismatch:\t%02X %02X %02X %02X %02X\n", id[0],id[1],id[2],id[3],id[4]);
	      goto out_item;
	   }
	   else{
	      printk(KERN_ALERT "SSD1963: reading signature -> successfully:\t%02X %02X %02X %02X %02X\n", id[0],id[1],id[2],id[3],id[4]);
	   }
	/* Signature end */

	dev_info(&dev->dev, "item=0x%p ctrl=0x%p data=0x%p\n", (void *)item,
		 (void *)ctrl_res->start, (void *)data_res->start);

	info = framebuffer_alloc(sizeof(struct ssd1963), &dev->dev);
	if (!info) {
		ret = -ENOMEM;
		dev_err(&dev->dev,
			"%s: unable to framebuffer_alloc\n", __func__);
		goto out_item;
	}
	item->info = info;
	info->par = item;
	info->fbops = &ssd1963_fbops;
	info->flags = FBINFO_FLAG_DEFAULT;
	info->fix = ssd1963_fix;
	info->var = ssd1963_var;

	ret = ssd1963_video_alloc(item);
	if (ret) {
		dev_err(&dev->dev,
			"%s: unable to ssd1963_video_alloc\n", __func__);
		goto out_info;
	}
	info->screen_base = (char __iomem *)item->info->fix.smem_start;

	ret = ssd1963_pages_alloc(item);
	if (ret < 0) {
		dev_err(&dev->dev,
			"%s: unable to ssd1963_pages_init\n", __func__);
		goto out_video;
	}
	info->fbdefio = &ssd1963_defio;
	fb_deferred_io_init(info);

	ret = register_framebuffer(info);
	if (ret < 0) {
		dev_err(&dev->dev,
			"%s: unable to register_frambuffer\n", __func__);
		goto out_pages;
	}
	ssd1963_setup(item);
	ssd1963_update_all(item);

	return ret;

out_pages:
	ssd1963_pages_free(item);
out_video:
	ssd1963_video_free(item);
out_info:
	framebuffer_release(info);
out_item:
	kfree(item);
out:
	return ret;
}

static struct platform_driver ssd1963_driver = {
	.probe = ssd1963_probe,
	.driver = {
		   .name = "ssd1963",
		   },
};

static int __init ssd1963_init(void)
{
	int ret = 0;

	pr_debug("%s\n", __func__);

	ret = platform_driver_register(&ssd1963_driver);
	if (ret) {
		pr_err("%s: unable to platform_driver_register\n", __func__);
	}

	return ret;
}

module_init(ssd1963_init);

MODULE_DESCRIPTION("SSD1963 LCD Driver");
MODULE_AUTHOR("Jean-Christian de Rivaz <jc@eclis.ch>");
MODULE_AUTHOR("Armin Schlegel <armin.schlegel@gmx.de>");
MODULE_LICENSE("GPL");

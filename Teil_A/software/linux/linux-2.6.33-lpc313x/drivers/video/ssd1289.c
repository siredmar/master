/*
 * SSD1289 Framebuffer
 *
 *
 * Copyright (c) 2014 Armin Schlegel <armin.schlegel@gmx.de>
 * Original: Copyright (c) 2009 Jean-Christian de Rivaz
 * Original: Copyright (c) 2012 Jeroen Domburg <jeroen@spritesmods.com>
 *
 * This is an adaption from the ili9325.c from Jeroen Domburg to work with
 * the SSD1289 controller.
 * Bits and pieces borrowed from the fsl-ili9325.c:
 * Copyright (C) 2010-2011 Freescale Semiconductor, Inc. All Rights Reserved.
 * Author: Alison Wang <b18965@freescale.com>
 *         Jason Jin <Jason.jin@freescale.com>
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * For direct I/O-mode:
 *
 * This driver expect the SSD1289 to be connected to a 16 bits local bus
 * and to be set in the 16 bits parallel interface mode. To use it you must
 * define in your board file a struct platform_device with a name set to
 * "ssd1289" and a struct resource array with two IORESOURCE_MEM: the first
 * for the control register; the second for the data register.
 *
 */


#include <linux/kernel.h>
//#define DEBUG 1
#include <linux/device.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/mm.h>
#include <linux/vmalloc.h>
#include <linux/fb.h>
#include <asm/io.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/uaccess.h>


#define LED_ENABLE_PIN  20
#define LED_RESET_PIN  19

#define USE_MEMCPY   0

#define SSD1289_WIDTH                      240
#define SSD1289_HEIGHT                     320

struct ssd1289_page {
   unsigned short x;
   unsigned short y;
   unsigned short *buffer;
   unsigned short *oldbuffer;
   unsigned short len;
   int must_update;
};

struct ssd1289 {
   struct device *dev;
   struct fb_info *info;
   unsigned int pages_count;
   struct ssd1289_page *pages;
   unsigned long pseudo_palette[17];
   int backlight;
   volatile unsigned short *ctrl_io;
   volatile unsigned short *data_io;
};

/* Warnig must be: HEIGHT % Lines_per_page == 0 */
#define SSD1289_LINES_PER_PAGE  16

/* Warning: do not modify this settings! */
#define SSD1289_BYTES_PER_PIXEL         2
#define SSD1289_PAGE_SIZE  (SSD1289_LINES_PER_PAGE * SSD1289_WIDTH * SSD1289_BYTES_PER_PIXEL)

inline void ssd1289_send_cmd(struct ssd1289 *item, unsigned short cmd)
{
   writew(cmd & 0xFF, item->ctrl_io );
}


inline void ssd1289_send_data(struct ssd1289 *item, unsigned short data)
{
   writew(data, item->data_io);
}

inline void ssd1289_write_reg(struct ssd1289 *item, unsigned short cmd, unsigned short data)
{
   writew(cmd & 0xFF, item->ctrl_io );
   msleep(10);
   writew(data, item->data_io);
   msleep(10);
}



void ssd1289_setWindow(struct ssd1289 *item, unsigned short xs, unsigned short ys, unsigned short xe, unsigned short ye)
{
   ssd1289_send_cmd(item, 0x0044);
   ssd1289_send_data(item, xe << 8 + xs);
   ssd1289_send_cmd(item, 0x0045);
   ssd1289_send_data(item, ys);
   ssd1289_send_cmd(item, 0x0046);
   ssd1289_send_data(item, ye);

   ssd1289_send_cmd(item, 0x004E);
   ssd1289_send_data(item, xs);
   ssd1289_send_cmd(item, 0x004F);
   ssd1289_send_data(item, ys);

   ssd1289_send_cmd(item, 0x0022);
}

void ssd1289_setPointer(struct ssd1289 *item, unsigned short x, unsigned short y)
{
   ssd1289_send_cmd(item, 0x004E);
   ssd1289_send_data(item, x);
   ssd1289_send_cmd(item, 0x004F);
   ssd1289_send_data(item, y);

   ssd1289_send_cmd(item, 0x0022);
}

void ssd1289_copy(struct ssd1289 *item, unsigned int index)
{
#define PIXELGROUPLEN 40
   unsigned short x;
   unsigned short y;
   unsigned short y_local;

   unsigned short *buffer;
   unsigned short *oldbuffer;
   unsigned int len;
   //   unsigned int count;
   unsigned short j;
   unsigned short tmpy;
   unsigned short xend;
   //int pageModified = 0;
   x = item->pages[index].x;
   y = item->pages[index].y;
   buffer = item->pages[index].buffer;
   oldbuffer = item->pages[index].oldbuffer;
   len = item->pages[index].len;
   dev_dbg(item->dev,
         "%s: page[%u]: x=%3hu y=%3hu buffer=0x%p len=%3hu\n",
         __func__, index, x, y, buffer, len);
#if USE_MEMCPY == 0
   tmpy = 0;
   xend = 0;
   for (y_local = y; y_local < y + SSD1289_LINES_PER_PAGE; y_local++)
   {
      for (x = 0; x < SSD1289_WIDTH; x++)
      {
         if (buffer[x + tmpy * SSD1289_WIDTH] != oldbuffer[x + tmpy * SSD1289_WIDTH])
         {
            if ((x + PIXELGROUPLEN) > SSD1289_WIDTH)
            {
               xend = SSD1289_WIDTH - 1;
            }
            else
               xend = x + PIXELGROUPLEN;
//            ssd1289_setPointer(item, x, y);
            //            ssd1289_setWindow(item, x, y_local, xend, y_local);
            for (j = x; j <= xend; j++)
            {
               ssd1289_setPointer(item, j, y);
               ssd1289_send_data(item, buffer[j + tmpy * SSD1289_WIDTH]);
               oldbuffer[j + tmpy * SSD1289_WIDTH] = buffer[j + tmpy * SSD1289_WIDTH];
            }
            x = xend;
         }
      }
      tmpy++;
   }
#else
   //ssd1289_setPointer(item, x, y);
   ssd1289_setWindow(item, x, SSD1289_WIDTH, y, y + SSD1289_LINES_PER_PAGE);
   memcpy(item->data_io, buffer, len * 2);
#endif
}

static void ssd1289_update_all(struct ssd1289 *item)
{
   unsigned short i;
   struct fb_deferred_io *fbdefio = item->info->fbdefio;
   for (i = 0; i < item->pages_count; i++) {
      item->pages[i].must_update = 1;
   }
   //printk(KERN_ALERT "ssd1289_update_all () occurred!\n");
   schedule_delayed_work(&item->info->deferred_work, fbdefio->delay);
}

static void ssd1289_update(struct fb_info *info,
      struct list_head *pagelist)
{
   struct ssd1289 *item = (struct ssd1289 *) info->par;
   struct page *page;
   int i;

   //We can be called because of pagefaults (mmap'ed framebuffer, pages
   //returned in *pagelist) or because of kernel activity
   //(pages[i]/must_update!=0). Add the former to the list of the latter.
   list_for_each_entry(page, pagelist, lru) {
      item->pages[page->index].must_update = 1;
   }
   //printk(KERN_ALERT "ssd1289_update() occurred!\n");
   //Copy changed pages.
   for (i = 0; i < item->pages_count; i++) {
      //ToDo: Small race here between checking and setting must_update,
      //maybe lock?
      if (item->pages[i].must_update) {
         item->pages[i].must_update = 0;
         ssd1289_copy(item, i);
      }
   }

}

typedef enum
{
   ID00 = 0,
   ID01 = (0b01 << 4),
   ID10 = (0b10 << 4),
   ID11 = (0b11 << 4)
}IDType;

typedef enum
{
   AM0 = 0,
   AM1 = (1 << 3)
}AMType;


static void __init ssd1289_setup(struct ssd1289 *item)
{
   int x;
   int y;
   printk(KERN_ALERT "ssd1289_setup() called\n");
   dev_dbg(item->dev, "%s: item=0x%p\n", __func__, (void *) item);

   gpio_direction_output(LED_RESET_PIN, 0);
   msleep(200);
   gpio_direction_output(LED_RESET_PIN, 1);
   msleep(200);

   gpio_direction_output(LED_ENABLE_PIN, 1);

   ssd1289_write_reg(item, 0x0000, 0x0001);
   msleep(100);
   ssd1289_write_reg(item, 0x0003, 0xA8A4);
   ssd1289_write_reg(item, 0x000C, 0x0000);
   ssd1289_write_reg(item, 0x000D, 0x080C);
   ssd1289_write_reg(item, 0x000E, 0x2B00);
   ssd1289_write_reg(item, 0x001E, 0x00B0);
   ssd1289_write_reg(item, 0x0001, 0x2B3F);
   ssd1289_write_reg(item, 0x0002, 0x0600);
   ssd1289_write_reg(item, 0x0010, 0x0000);
   // LANDSCAPE:
   //      ssd1289_write_reg(item, 0x11, 0x6098 );
   // LANDSCAPE_REV:
   //   ssd1289_write_reg(item, 0x11, 0x60A8 );
   // PORTRAIT_REV:
   //ssd1289_write_reg(item, 0x11, 0x6080 );
   // PORTRAIT:
   //   ssd1289_write_reg(item, 0x11, 0x6070);
   ssd1289_write_reg(item, 0x0011, 0x6000 | AM1 | ID01);
   ssd1289_write_reg(item, 0x0005, 0x0000);
   ssd1289_write_reg(item, 0x0006, 0x0000);
   ssd1289_write_reg(item, 0x0016, 0xEF1C);
   ssd1289_write_reg(item, 0x0017, 0x0003);
   ssd1289_write_reg(item, 0x0007, 0x0233);
   ssd1289_write_reg(item, 0x000B, 0x0000);
   ssd1289_write_reg(item, 0x000F, 0x0000);
   ssd1289_write_reg(item, 0x0041, 0x0000);
   ssd1289_write_reg(item, 0x0042, 0x0000);
   ssd1289_write_reg(item, 0x0048, 0x0000);
   ssd1289_write_reg(item, 0x0049, 0x013F);
   ssd1289_write_reg(item, 0x004A, 0x0000);
   ssd1289_write_reg(item, 0x004B, 0x0000);
   ssd1289_write_reg(item, 0x0044, 0xEF00);
   ssd1289_write_reg(item, 0x0045, 0x0000);
   ssd1289_write_reg(item, 0x0046, 0x013F);
   ssd1289_write_reg(item, 0x0030, 0x0707);
   ssd1289_write_reg(item, 0x0031, 0x0204);
   ssd1289_write_reg(item, 0x0032, 0x0204);
   ssd1289_write_reg(item, 0x0033, 0x0502);
   ssd1289_write_reg(item, 0x0034, 0x0507);
   ssd1289_write_reg(item, 0x0035, 0x0204);
   ssd1289_write_reg(item, 0x0036, 0x0204);
   ssd1289_write_reg(item, 0x0037, 0x0502);
   ssd1289_write_reg(item, 0x003A, 0x0302);
   ssd1289_write_reg(item, 0x003B, 0x0302);
   ssd1289_write_reg(item, 0x0023, 0x0000);
   ssd1289_write_reg(item, 0x0024, 0x0000);
   ssd1289_write_reg(item, 0x0044, (item->info->var.xres - 1) << 8);
   ssd1289_write_reg(item, 0x0045, 0x0000);
   ssd1289_write_reg(item, 0x0046, (item->info->var.yres - 1));

   //   ssd1289_write_reg(item, 0x004f, 0x0000);
   //   ssd1289_write_reg(item, 0x004e, 0x0000);
   //   ssd1289_send_cmd(item, 0x0022);

   for (y = 0; y < SSD1289_HEIGHT; y++)
   {

      for(x = 0; x < SSD1289_WIDTH; x++)
      {
         ssd1289_setPointer(item, x, y);
         ssd1289_send_data(item, 0x123);
      }
   }

      msleep(1000);
   printk(KERN_ALERT
         "ssd1289_setup() end: pages_count: %d, item->pages[x].len: %d\n\n",
         item->pages_count, item->pages[0].len);
}

//This routine will allocate the buffer for the complete framebuffer. This
//is one continuous chunk of 16-bit pixel values; userspace programs
//will write here.
static int __init ssd1289_video_alloc(struct ssd1289 *item)
{
   unsigned int frame_size;

   dev_dbg(item->dev, "%s: item=0x%p\n", __func__, (void *) item);

   frame_size = item->info->fix.line_length * item->info->var.yres;
   dev_dbg(item->dev, "%s: item=0x%p frame_size=%u\n",
         __func__, (void *) item, frame_size);

   item->pages_count = frame_size / SSD1289_PAGE_SIZE;
   if ((item->pages_count * SSD1289_PAGE_SIZE) < frame_size) {
      item->pages_count++;
   }
   dev_dbg(item->dev, "%s: item=0x%p pages_count=%u\n",
         __func__, (void *) item, item->pages_count);

   item->info->fix.smem_len = item->pages_count * SSD1289_PAGE_SIZE;
   item->info->fix.smem_start =
         (unsigned long) vmalloc(item->info->fix.smem_len);
   if (!item->info->fix.smem_start) {
      dev_err(item->dev, "%s: unable to vmalloc\n", __func__);
      return -ENOMEM;
   }
   memset((void *) item->info->fix.smem_start, 0,
         item->info->fix.smem_len);

   return 0;
}

static void ssd1289_video_free(struct ssd1289 *item)
{
   dev_dbg(item->dev, "%s: item=0x%p\n", __func__, (void *) item);

   kfree((void *) item->info->fix.smem_start);
}

//This routine will allocate a ssd1289_page struct for each vm page in the
//main framebuffer memory. Each struct will contain a pointer to the page
//start, an x- and y-offset, and the length of the pagebuffer which is in the framebuffer.
static int __init ssd1289_pages_alloc(struct ssd1289 *item)
{
   unsigned short pixels_per_page;
   unsigned short yoffset_per_page;
   unsigned short xoffset_per_page;
   unsigned int index;
   unsigned short x = 0;
   unsigned short y = 0;
   unsigned short *buffer;
   unsigned short *oldbuffer;
   unsigned int len;

   dev_dbg(item->dev, "%s: item=0x%p\n", __func__, (void *) item);

   item->pages =
         kmalloc(item->pages_count * sizeof(struct ssd1289_page),
               GFP_KERNEL);
   if (!item->pages) {
      dev_err(item->dev,
            "%s: unable to kmalloc for ssd1289_page\n",
            __func__);
      return -ENOMEM;
   }

   pixels_per_page =
         SSD1289_PAGE_SIZE / (item->info->var.bits_per_pixel / 8);
   yoffset_per_page = pixels_per_page / item->info->var.xres;
   xoffset_per_page = pixels_per_page -
         (yoffset_per_page * item->info->var.xres);
   dev_dbg(item->dev, "%s: item=0x%p pixels_per_page=%hu "
         "yoffset_per_page=%hu xoffset_per_page=%hu\n",
         __func__, (void *) item, pixels_per_page,
         yoffset_per_page, xoffset_per_page);

   oldbuffer = kmalloc(item->pages_count * pixels_per_page * 2,
         GFP_KERNEL);
   if (!oldbuffer) {
      dev_err(item->dev,
            "%s: unable to kmalloc for ssd1289_page oldbuffer\n",
            __func__);
      return -ENOMEM;
   }

   buffer = (unsigned short *) item->info->fix.smem_start;
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
      item->pages[index].oldbuffer = oldbuffer;
      item->pages[index].len = len;

      x += xoffset_per_page;
      if (x >= item->info->var.xres) {
         y++;
         x -= item->info->var.xres;
      }
      y += yoffset_per_page;
      buffer += pixels_per_page;
      oldbuffer += pixels_per_page;
   }

   return 0;
}

static void ssd1289_pages_free(struct ssd1289 *item)
{
   dev_dbg(item->dev, "%s: item=0x%p\n", __func__, (void *) item);

   kfree(item->pages);
}

static inline __u32 CNVT_TOHW(__u32 val, __u32 width)
{
   return ((val << width) + 0x7FFF - val) >> 16;
}

//This routine is needed because the console driver won't work without it.
static int ssd1289_setcolreg(unsigned regno,
      unsigned red, unsigned green, unsigned blue,
      unsigned transp, struct fb_info *info)
{
   int ret = 1;

   /*
    * If greyscale is true, then we convert the RGB value
    * to greyscale no matter what visual we are using.
    */
   if (info->var.grayscale)
      red = green = blue = (19595 * red + 38470 * green +
            7471 * blue) >> 16;
   switch (info->fix.visual) {
   case FB_VISUAL_TRUECOLOR:
      if (regno < 16) {
         u32 *pal = info->pseudo_palette;
         u32 value;

         red = CNVT_TOHW(red, info->var.red.length);
         green = CNVT_TOHW(green, info->var.green.length);
         blue = CNVT_TOHW(blue, info->var.blue.length);
         transp =
               CNVT_TOHW(transp, info->var.transp.length);

         value = (red << info->var.red.offset) |
               (green << info->var.green.offset) |
               (blue << info->var.blue.offset) |
               (transp << info->var.transp.offset);

         pal[regno] = value;
         ret = 0;
      }
      break;
   case FB_VISUAL_STATIC_PSEUDOCOLOR:
   case FB_VISUAL_PSEUDOCOLOR:
      break;
   }
   return ret;
}

static int ssd1289_blank(int blank_mode, struct fb_info *info)
{
   struct ssd1289 *item = (struct ssd1289 *) info->par;
   if (blank_mode == FB_BLANK_UNBLANK)
      item->backlight = 1;
   else
      item->backlight = 0;
   //Item->backlight won't take effect until the LCD is written to. Force that
   //by dirty'ing a page.
   item->pages[0].must_update = 1;
   //printk(KERN_ALERT "ssd1289_blank() occurred!\n");
   schedule_delayed_work(&info->deferred_work, 0);
   return 0;
}

static void ssd1289_touch(struct fb_info *info, int x, int y, int w, int h)
{
   struct fb_deferred_io *fbdefio = info->fbdefio;
   struct ssd1289 *item = (struct ssd1289 *) info->par;
   int i, ystart, yend;
   if (fbdefio) {
      //Touch the pages the y-range hits, so the deferred io will update them.
      for (i = 0; i < item->pages_count; i++) {
         ystart = item->pages[i].y;
         yend =
               item->pages[i].y +
               (item->pages[i].len / info->fix.line_length) +
               1;
         if (!((y + h) < ystart || y > yend)) {
            item->pages[i].must_update = 1;
         }
      }
      //Schedule the deferred IO to kick in after a delay.
      schedule_delayed_work(&info->deferred_work,
            fbdefio->delay);
   }
}

static void ssd1289_fillrect(struct fb_info *p,
      const struct fb_fillrect *rect)
{
   sys_fillrect(p, rect);
   ssd1289_touch(p, rect->dx, rect->dy, rect->width, rect->height);
}

static void ssd1289_imageblit(struct fb_info *p,
      const struct fb_image *image)
{
   sys_imageblit(p, image);
   ssd1289_touch(p, image->dx, image->dy, image->width,
         image->height);
}

static void ssd1289_copyarea(struct fb_info *p,
      const struct fb_copyarea *area)
{
   sys_copyarea(p, area);
   ssd1289_touch(p, area->dx, area->dy, area->width, area->height);
}

static ssize_t ssd1289_write(struct fb_info *p, const char __user * buf,
      size_t count, loff_t * ppos)
{
   ssize_t res;
   res = fb_sys_write(p, buf, count, ppos);
   ssd1289_touch(p, 0, 0, p->var.xres, p->var.yres);
   return res;
}

static struct fb_ops ssd1289_fbops = {
      .owner         = THIS_MODULE,
      .fb_read       = fb_sys_read,
      .fb_write      = ssd1289_write,
      .fb_fillrect   = ssd1289_fillrect,
      .fb_copyarea   = ssd1289_copyarea,
      .fb_imageblit  = ssd1289_imageblit,
      .fb_setcolreg  = ssd1289_setcolreg,
      .fb_blank      = ssd1289_blank,
};

static struct fb_fix_screeninfo ssd1289_fix __initdata = {
      .id            = "SSD1289",
      .type          = FB_TYPE_PACKED_PIXELS,
      .visual        = FB_VISUAL_TRUECOLOR,
      .accel         = FB_ACCEL_NONE,
      .line_length   = 240 * 2,
};

static struct fb_var_screeninfo ssd1289_var __initdata = {
      .xres = 240,
      .yres = 320,
      .xres_virtual = 240,
      .yres_virtual = 320,
      .width = 240,
      .height = 320,
      .bits_per_pixel = 16,
      .red = {11, 5, 0},
      .green = {5, 6, 0},
      .blue = {0, 5, 0},
      .activate = FB_ACTIVATE_NOW,
      .vmode = FB_VMODE_NONINTERLACED,
};

static struct fb_deferred_io ssd1289_defio = {
      .delay = HZ / 20,
      .deferred_io = &ssd1289_update,
};

static int __init ssd1289_probe(struct platform_device *dev)
{
   int ret = 0;
   struct ssd1289 *item;
   struct resource *ctrl_res;
   struct resource *data_res;
   unsigned int ctrl_res_size;
   unsigned int data_res_size;
   struct resource *ctrl_req;
   struct resource *data_req;
   unsigned short int id[6];
   struct fb_info *info;

   dev_dbg(&dev->dev, "%s\n", __func__);

   item = kzalloc(sizeof(struct ssd1289), GFP_KERNEL);
   if (!item) {
      dev_err(&dev->dev,
            "%s: unable to kzalloc for ssd1289\n", __func__);
      ret = -ENOMEM;
      goto out;
   }
   item->dev = &dev->dev;
   dev_set_drvdata(&dev->dev, item);
   item->backlight = 1;

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

   //   /* Now that we're sure that we own the memory bus... reset timings */
   //   MPMC_STCONFIG0  = 0x81;
   //   MPMC_STWTWEN0   = 0;
   //   MPMC_STWTOEN0   = 0;
   //   MPMC_STWTRD0    = 15;
   //   MPMC_STWTPG0    = 0;
   //   MPMC_STWTWR0    = 4;
   //   MPMC_STWTTURN0  = 0;

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

   dev_info(&dev->dev, "item=0x%p ctrl=0x%p data=0x%p\n", (void *)item,
         (void *)ctrl_res->start, (void *)data_res->start);

   info = framebuffer_alloc(sizeof(struct ssd1289), &dev->dev);
   if (!info) {
      ret = -ENOMEM;
      dev_err(&dev->dev,
            "%s: unable to framebuffer_alloc\n", __func__);
      goto out_item;
   }
   info->pseudo_palette = &item->pseudo_palette;
   item->info = info;
   info->par = item;
   info->dev = &dev->dev;
   info->fbops = &ssd1289_fbops;
   info->flags = FBINFO_FLAG_DEFAULT | FBINFO_VIRTFB;
   info->fix = ssd1289_fix;
   info->var = ssd1289_var;

   ret = ssd1289_video_alloc(item);
   if (ret) {
      dev_err(&dev->dev,
            "%s: unable to ssd1289_video_alloc\n", __func__);
      goto out_info;
   }
   info->screen_base = (char __iomem *) item->info->fix.smem_start;

   ret = ssd1289_pages_alloc(item);
   if (ret < 0) {
      dev_err(&dev->dev,
            "%s: unable to ssd1289_pages_init\n", __func__);
      goto out_video;
   }

   info->fbdefio = &ssd1289_defio;
   fb_deferred_io_init(info);

   ret = register_framebuffer(info);
   if (ret < 0) {
      dev_err(&dev->dev,
            "%s: unable to register_frambuffer\n", __func__);
      goto out_pages;
   }


   ssd1289_setup(item);
   ssd1289_update_all(item);

   return ret;

   out_pages:
   ssd1289_pages_free(item);
   out_video:
   ssd1289_video_free(item);
   out_info:
   framebuffer_release(info);
   out_item:
   kfree(item);
   out:
   return ret;
}


static int ssd1289_remove(struct platform_device *dev)
{
   struct fb_info *info = dev_get_drvdata(&dev->dev);
   struct ssd1289 *item = (struct ssd1289 *) info->par;
   unregister_framebuffer(info);
   ssd1289_pages_free(item);
   ssd1289_video_free(item);
   framebuffer_release(info);
   kfree(item);
   return 0;
}

#ifdef CONFIG_PM
static int ssd1289_suspend(struct platform_device *dev, pm_message_t state)
{
   //      struct fb_info *info = dev_get_drvdata(&spi->dev);
   //      struct ssd1289 *item = (struct ssd1289 *)info->par;
   /* enter into sleep mode */

   return 0;
}

static int ssd1289_resume(struct platform_device *dev)
{
   //      struct fb_info *info = dev_get_drvdata(&spi->dev);
   //      struct ssd1289 *item = (struct ssd1289 *)info->par;
   /* leave sleep mode */

   return 0;
}
#else
#define ssd1289_suspend NULL
#define ssd1289_resume NULL
#endif

static struct platform_driver ssd1289_driver = {
      .probe = ssd1289_probe,
      .remove = ssd1289_remove,
      .driver = {
            .name = "ssd1289",
      },
};

static int __init ssd1289_init(void)
{
   int ret = 0;

   pr_debug("%s\n", __func__);

   ret = platform_driver_register(&ssd1289_driver);
   if (ret) {
      pr_err("%s: unable to platform_driver_register\n",
            __func__);
   }

   return ret;
}

module_init(ssd1289_init);
module_remove(ssd1289_remove);


MODULE_DESCRIPTION("SSD1289 LCD Driver");
MODULE_AUTHOR("Jeroen Domburg <jeroen@spritesmods.com>");
MODULE_AUTHOR("Armin Schlegel <armin.schlegel@gmx.de");
MODULE_LICENSE("GPL");

/*
 * MD050SD Framebuffer
 *
 *
 * Copyright (c) 2013 Armin Schlegel <armin.schlegel@gmx.de>
 * Original: Copyright (c) 2009 Jean-Christian de Rivaz
 * Original: Copyright (c) 2012 Jeroen Domburg <jeroen@spritesmods.com>
 *
 * This is an adaption from the ili9325.c from Jeroen Domburg to work with
 * the MD050SD controller.
 * Bits and pieces borrowed from the fsl-ili9325.c:
 * Copyright (C) 2010-2011 Freescale Semiconductor, Inc. All Rights Reserved.
 * Author: Alison Wang <b18965@freescale.com>
 *         Jason Jin <Jason.jin@freescale.com>
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * The Solomon Systech MD050SD chip drive TFT screen with a resolution of 800x480.
 *
 * For direct I/O-mode:
 *
 * This driver expect the MD050SD to be connected to a 16 bits local bus
 * and to be set in the 16 bits parallel interface mode. To use it you must
 * define in your board file a struct platform_device with a name set to
 * "md050sd" and a struct resource array with two IORESOURCE_MEM: the first
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


//#define LED_ENABLE_PIN  20
#define LED_RESET_PIN  19

/* TFT Controler Register defines */
#define NOCMD                 0xFFFF
#define NOPARAM               0
#define PARAM                 1


/* TFT Controler Register defines */
#define MD050SD_SET_BACKLIGHT_PWM        (0x0001)
#define MD050SD_SET_LINE_ADDRESS_START   (0x0002)
#define MD050SD_SET_COLUMN_ADDRESS_START (0x0003)
#define MD050SD_DISPLAY_PAGE             (0x0004)
#define MD050SD_PAGE_RW                  (0x0005)
#define MD050SD_SET_LINE_ADDRESS_END     (0x0006)
#define MD050SD_SET_COLUMN_ADDRESS_END   (0x0007)
#define MD050SD_SET_SLEEP                (0x000B)
#define MD050SD_WRITE_MEMORY_START       (0x000F)

#define MD050SD_WIDTH                      800
#define MD050SD_HEIGHT                     480

struct md050sd_page {
   unsigned short x;
   unsigned short y;
   unsigned short *buffer;
   unsigned short *oldbuffer;
   unsigned short len;
   int must_update;
};

struct md050sd {
   struct device *dev;
   struct fb_info *info;
   unsigned int pages_count;
   struct md050sd_page *pages;
   unsigned long pseudo_palette[17];
   int backlight;
   volatile unsigned short *ctrl_io;
   volatile unsigned short *data_io;
};

//#define MD050SD_PAGE_SIZE  (2048*2)
#define MD050SD_LINES_PER_PAGE  32
#define MD050SD_BYTES_PER_PIXEL         2
#define MD050SD_PAGE_SIZE  (MD050SD_LINES_PER_PAGE * MD050SD_WIDTH * MD050SD_BYTES_PER_PIXEL)

//#define MD050SD_PAGE_SIZE  (800*2)



inline void md050sd_send_cmd(struct md050sd *item, unsigned short cmd)
{
   writew(cmd & 0xFF, item->ctrl_io );
}


inline void md050sd_send_data(struct md050sd *item, unsigned short data)
{
   writew(data, item->data_io);
}



//void md050sd_setDisplayRegister
//(struct md050sd *item,
//      unsigned short tft_register_ui16,
//      unsigned short tft_registerValue_ui16,
//      unsigned short tft_registerParameters_ui16)
//{
//   /* First send Command if it is != NOCMD */
//   if (tft_register_ui16 != NOCMD) {
//      md050sd_send_cmd(item, tft_register_ui16);
//   }
//   /* Then send or continue sending Parameter to tft controler if needed */
//   if (tft_registerParameters_ui16 != NOPARAM) {
//      md050sd_send_cmd(item, tft_registerValue_ui16);
//   }
//}

void md050sd_setWindow(struct md050sd *item, unsigned short xs, unsigned short ys,
      unsigned short xe, unsigned short ye)
{
   md050sd_send_cmd(item, MD050SD_SET_LINE_ADDRESS_START);
   md050sd_send_data(item, ys);
   md050sd_send_cmd(item, MD050SD_SET_COLUMN_ADDRESS_START);
   md050sd_send_data(item, xs);

   md050sd_send_cmd(item, MD050SD_SET_LINE_ADDRESS_END);
   md050sd_send_data(item, ye);
   md050sd_send_cmd(item, MD050SD_SET_COLUMN_ADDRESS_END);
   md050sd_send_data(item, xe);

   md050sd_send_cmd(item, MD050SD_WRITE_MEMORY_START);
}

//void md050sd_drawPixel(struct md050sd *item,
//      unsigned short xPosition_ui16,
//      unsigned short yPosition_ui16,
//      unsigned short rectangleColor_ui16)
//{
//   /* Set window size and position - modified for portrait view */
//   md050sd_setWindow(item, xPosition_ui16, yPosition_ui16,
//         xPosition_ui16 + 1, yPosition_ui16 + 1);
//   md050sd_send_data(item, rectangleColor_ui16);
//}

void md050sd_copy(struct md050sd *item, unsigned int index)
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

   tmpy = 0;
   xend = 0;
   for (y_local = y; y_local < y + MD050SD_LINES_PER_PAGE; y_local++) {
      for (x = 0; x < MD050SD_WIDTH; x++) {
         if (buffer[x + tmpy * MD050SD_WIDTH] != oldbuffer[x + tmpy * MD050SD_WIDTH]) {
            if ((x + PIXELGROUPLEN) > MD050SD_WIDTH) {
               xend = MD050SD_WIDTH - 1;
            } else
               xend = x + PIXELGROUPLEN;
            md050sd_setWindow(item, x, y_local, xend, y_local);
            for (j = x; j <= xend; j++) {
               md050sd_send_data(item, buffer[j + tmpy * MD050SD_WIDTH]);
               oldbuffer[j + tmpy * MD050SD_WIDTH] = buffer[j + tmpy * MD050SD_WIDTH];
            }
            x = xend;
         }
      }
      tmpy++;
   }
}

static void md050sd_update_all(struct md050sd *item)
{
   unsigned short i;
   struct fb_deferred_io *fbdefio = item->info->fbdefio;
   for (i = 0; i < item->pages_count; i++) {
      item->pages[i].must_update = 1;
   }
   //printk(KERN_ALERT "md050sd_update_all () occurred!\n");
   schedule_delayed_work(&item->info->deferred_work, fbdefio->delay);
}

static void md050sd_update(struct fb_info *info,
      struct list_head *pagelist)
{
   struct md050sd *item = (struct md050sd *) info->par;
   struct page *page;
   int i;

   //We can be called because of pagefaults (mmap'ed framebuffer, pages
   //returned in *pagelist) or because of kernel activity
   //(pages[i]/must_update!=0). Add the former to the list of the latter.
   list_for_each_entry(page, pagelist, lru) {
      item->pages[page->index].must_update = 1;
   }
   //printk(KERN_ALERT "md050sd_update() occurred!\n");
   //Copy changed pages.
   for (i = 0; i < item->pages_count; i++) {
      //ToDo: Small race here between checking and setting must_update,
      //maybe lock?
      if (item->pages[i].must_update) {
         item->pages[i].must_update = 0;
         md050sd_copy(item, i);
      }
   }

}

static void __init md050sd_setup(struct md050sd *item)
{
   int x;
   int y;
   printk(KERN_ALERT "md050sd_setup() called\n");
   dev_dbg(item->dev, "%s: item=0x%p\n", __func__, (void *) item);

   gpio_direction_output(LED_RESET_PIN, 0);
   msleep(200);
   gpio_direction_output(LED_RESET_PIN, 1);
   msleep(200);

   md050sd_setWindow(item, 0, 0, MD050SD_WIDTH - 1, MD050SD_HEIGHT - 1);
   for (x = 0; x < MD050SD_WIDTH * MD050SD_HEIGHT; x++)
      md050sd_send_data(item, 0x0000);


   //   md050sd_setWindow(item, 0, 0, MD050SD_WIDTH - 1, MD050SD_HEIGHT - 1);
   //   for (y = 0; y < item->pages_count; y++) {
   //      for (x = 0; x < item->pages[y].len; x++) {
   //         md050sd_send_data(item, item->pages[y].buffer[x]);
   //      }
   //   }

   msleep(10);
   printk(KERN_ALERT
         "md050sd_setup() end: pages_count: %d, item->pages[x].len: %d\n\n",
         item->pages_count, item->pages[0].len);
}

//This routine will allocate the buffer for the complete framebuffer. This
//is one continuous chunk of 16-bit pixel values; userspace programs
//will write here.
static int __init md050sd_video_alloc(struct md050sd *item)
{
   unsigned int frame_size;

   dev_dbg(item->dev, "%s: item=0x%p\n", __func__, (void *) item);

   frame_size = item->info->fix.line_length * item->info->var.yres;
   dev_dbg(item->dev, "%s: item=0x%p frame_size=%u\n",
         __func__, (void *) item, frame_size);

   item->pages_count = frame_size / MD050SD_PAGE_SIZE;
   if ((item->pages_count * MD050SD_PAGE_SIZE) < frame_size) {
      item->pages_count++;
   }
   dev_dbg(item->dev, "%s: item=0x%p pages_count=%u\n",
         __func__, (void *) item, item->pages_count);

   item->info->fix.smem_len = item->pages_count * MD050SD_PAGE_SIZE;
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

static void md050sd_video_free(struct md050sd *item)
{
   dev_dbg(item->dev, "%s: item=0x%p\n", __func__, (void *) item);

   kfree((void *) item->info->fix.smem_start);
}

//This routine will allocate a md050sd_page struct for each vm page in the
//main framebuffer memory. Each struct will contain a pointer to the page
//start, an x- and y-offset, and the length of the pagebuffer which is in the framebuffer.
static int __init md050sd_pages_alloc(struct md050sd *item)
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
         kmalloc(item->pages_count * sizeof(struct md050sd_page),
               GFP_KERNEL);
   if (!item->pages) {
      dev_err(item->dev,
            "%s: unable to kmalloc for md050sd_page\n",
            __func__);
      return -ENOMEM;
   }

   pixels_per_page =
         MD050SD_PAGE_SIZE / (item->info->var.bits_per_pixel / 8);
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
            "%s: unable to kmalloc for md050sd_page oldbuffer\n",
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

static void md050sd_pages_free(struct md050sd *item)
{
   dev_dbg(item->dev, "%s: item=0x%p\n", __func__, (void *) item);

   kfree(item->pages);
}

static inline __u32 CNVT_TOHW(__u32 val, __u32 width)
{
   return ((val << width) + 0x7FFF - val) >> 16;
}

//This routine is needed because the console driver won't work without it.
static int md050sd_setcolreg(unsigned regno,
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

static int md050sd_blank(int blank_mode, struct fb_info *info)
{
   struct md050sd *item = (struct md050sd *) info->par;
   if (blank_mode == FB_BLANK_UNBLANK)
      item->backlight = 1;
   else
      item->backlight = 0;
   //Item->backlight won't take effect until the LCD is written to. Force that
   //by dirty'ing a page.
   item->pages[0].must_update = 1;
   //printk(KERN_ALERT "md050sd_blank() occurred!\n");
   schedule_delayed_work(&info->deferred_work, 0);
   return 0;
}

static void md050sd_touch(struct fb_info *info, int x, int y, int w, int h)
{
   struct fb_deferred_io *fbdefio = info->fbdefio;
   struct md050sd *item = (struct md050sd *) info->par;
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

static void md050sd_fillrect(struct fb_info *p,
      const struct fb_fillrect *rect)
{
   sys_fillrect(p, rect);
   md050sd_touch(p, rect->dx, rect->dy, rect->width, rect->height);
}

static void md050sd_imageblit(struct fb_info *p,
      const struct fb_image *image)
{
   sys_imageblit(p, image);
   md050sd_touch(p, image->dx, image->dy, image->width,
         image->height);
}

static void md050sd_copyarea(struct fb_info *p,
      const struct fb_copyarea *area)
{
   sys_copyarea(p, area);
   md050sd_touch(p, area->dx, area->dy, area->width, area->height);
}

static ssize_t md050sd_write(struct fb_info *p, const char __user * buf,
      size_t count, loff_t * ppos)
{
   ssize_t res;
   res = fb_sys_write(p, buf, count, ppos);
   md050sd_touch(p, 0, 0, p->var.xres, p->var.yres);
   return res;
}

static struct fb_ops md050sd_fbops = {
      .owner         = THIS_MODULE,
      .fb_read       = fb_sys_read,
      .fb_write      = md050sd_write,
      .fb_fillrect   = md050sd_fillrect,
      .fb_copyarea   = md050sd_copyarea,
      .fb_imageblit  = md050sd_imageblit,
      .fb_setcolreg  = md050sd_setcolreg,
      .fb_blank      = md050sd_blank,
};

static struct fb_fix_screeninfo md050sd_fix __initdata = {
      .id            = "MD050SD",
      .type          = FB_TYPE_PACKED_PIXELS,
      .visual        = FB_VISUAL_TRUECOLOR,
      .accel         = FB_ACCEL_NONE,
      .line_length   = 800 * 2,
};

static struct fb_var_screeninfo md050sd_var __initdata = {
      .xres = 800,
      .yres = 480,
      .xres_virtual = 800,
      .yres_virtual = 480,
      .width = 800,
      .height = 480,
      .bits_per_pixel = 16,
      .red = {11, 5, 0},
      .green = {5, 6, 0},
      .blue = {0, 5, 0},
      .activate = FB_ACTIVATE_NOW,
      .vmode = FB_VMODE_NONINTERLACED,
};

static struct fb_deferred_io md050sd_defio = {
      .delay = HZ / 20,
      .deferred_io = &md050sd_update,
};

static int __init md050sd_probe(struct platform_device *dev)
{
   int ret = 0;
   struct md050sd *item;
   struct resource *ctrl_res;
   struct resource *data_res;
   unsigned int ctrl_res_size;
   unsigned int data_res_size;
   struct resource *ctrl_req;
   struct resource *data_req;
   unsigned short int id[6];
   struct fb_info *info;

   dev_dbg(&dev->dev, "%s\n", __func__);

   item = kzalloc(sizeof(struct md050sd), GFP_KERNEL);
   if (!item) {
      dev_err(&dev->dev,
            "%s: unable to kzalloc for md050sd\n", __func__);
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

   /* Now that we're sure that we own the memory bus... reset timings */
   MPMC_STCONFIG0  = 0x81;
   MPMC_STWTWEN0   = 13;
   MPMC_STWTOEN0   = 0;
   MPMC_STWTRD0    = 0;
   MPMC_STWTPG0    = 0;
   MPMC_STWTWR0    = 15;
   MPMC_STWTTURN0  = 0;

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


   //   md050sd_send_cmd(item, SOFT_RESET);
   //   mdelay(200);
   //   md050sd_send_cmd(item, READ_DDB);
   //   //mdelay(1);
   //
   //   id[0]=readw(item->data_io);
   //   id[1]=readw(item->data_io);
   //   id[2]=readw(item->data_io);
   //   id[3]=readw(item->data_io);
   //   id[4]=readw(item->data_io);

   //   dev_dbg(&dev->dev, "%s: signature=%02x %02x %02x %02x %02x\n", __func__, id[0],id[1],id[2],id[3],id[4]);
   //
   //   if (!(id[0]==0x01 && id[1]==0x57 && id[2]==0x61 && id[3]==0x01)) {
   //      ret = -ENODEV;
   //      dev_err(&dev->dev,
   //         "%s: unknown signature %02x %02x %02x %02x %02x\n", __func__, id[0],id[1],id[2],id[3],id[4]);
   //      goto out_item;
   //   }

   dev_info(&dev->dev, "item=0x%p ctrl=0x%p data=0x%p\n", (void *)item,
         (void *)ctrl_res->start, (void *)data_res->start);

   info = framebuffer_alloc(sizeof(struct md050sd), &dev->dev);
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
   info->fbops = &md050sd_fbops;
   info->flags = FBINFO_FLAG_DEFAULT | FBINFO_VIRTFB;
   info->fix = md050sd_fix;
   info->var = md050sd_var;

   ret = md050sd_video_alloc(item);
   if (ret) {
      dev_err(&dev->dev,
            "%s: unable to md050sd_video_alloc\n", __func__);
      goto out_info;
   }
   info->screen_base = (char __iomem *) item->info->fix.smem_start;

   ret = md050sd_pages_alloc(item);
   if (ret < 0) {
      dev_err(&dev->dev,
            "%s: unable to md050sd_pages_init\n", __func__);
      goto out_video;
   }

   info->fbdefio = &md050sd_defio;
   fb_deferred_io_init(info);

   ret = register_framebuffer(info);
   if (ret < 0) {
      dev_err(&dev->dev,
            "%s: unable to register_frambuffer\n", __func__);
      goto out_pages;
   }


   md050sd_setup(item);
   md050sd_update_all(item);

   return ret;

   out_pages:
   md050sd_pages_free(item);
   out_video:
   md050sd_video_free(item);
   out_info:
   framebuffer_release(info);
   out_item:
   kfree(item);
   out:
   return ret;
}


static int md050sd_remove(struct platform_device *dev)
{
   struct fb_info *info = dev_get_drvdata(&dev->dev);
   struct md050sd *item = (struct md050sd *) info->par;
   unregister_framebuffer(info);
   md050sd_pages_free(item);
   md050sd_video_free(item);
   framebuffer_release(info);
   kfree(item);
   return 0;
}

#ifdef CONFIG_PM
static int md050sd_suspend(struct platform_device *dev, pm_message_t state)
{
   //      struct fb_info *info = dev_get_drvdata(&spi->dev);
   //      struct md050sd *item = (struct md050sd *)info->par;
   /* enter into sleep mode */
   //      md050sd_reg_set(item, MD050SD_REG_SLEEP_MODE, 0x0001);
   return 0;
}

static int md050sd_resume(struct platform_device *dev)
{
   //      struct fb_info *info = dev_get_drvdata(&spi->dev);
   //      struct md050sd *item = (struct md050sd *)info->par;
   /* leave sleep mode */
   //      md050sd_reg_set(item, MD050SD_REG_SLEEP_MODE, 0x0000);
   return 0;
}
#else
#define md050sd_suspend NULL
#define md050sd_resume NULL
#endif

static struct platform_driver md050sd_driver = {
      .probe = md050sd_probe,
      .remove = md050sd_remove,
      .driver = {
            .name = "md050sd",
      },
};

static int __init md050sd_init(void)
{
   int ret = 0;

   pr_debug("%s\n", __func__);

   ret = platform_driver_register(&md050sd_driver);
   if (ret) {
      pr_err("%s: unable to platform_driver_register\n",
            __func__);
   }

   return ret;
}

module_init(md050sd_init);
module_remove(md050sd_remove);


MODULE_DESCRIPTION("MD050SD LCD Driver");
MODULE_AUTHOR("Jeroen Domburg <jeroen@spritesmods.com>");
MODULE_AUTHOR("Armin Schlegel <armin.schlegel@gmx.de");
MODULE_LICENSE("GPL");

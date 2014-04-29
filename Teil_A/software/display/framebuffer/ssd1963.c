/*
* SSD1963 Framebuffer
*
*
* Copyright (c) 2013 Armin Schlegel <armin.schlegel@gmx.de>
* Original: Copyright (c) 2009 Jean-Christian de Rivaz
* Original: Copyright (c) 2012 Jeroen Domburg <jeroen@spritesmods.com>
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
#include <asm/io.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/uaccess.h>


#define LED_ENABLE_PIN  20

/* TFT Controler Register defines */
#define NOCMD                 0xFFFF
#define NOPARAM               0
#define PARAM                 1


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



#define WIDTH                      480
#define HEIGHT                     272

#define HDP                        479        //479
#define HT                         525        //531
#define HPS                        40         //43
#define LPS                        6           //8
#define HPW                        39         //10
                                              //
#define VDP                        271        //271
#define VT                         288        //288
#define VPS                        12         //12
#define FPS                        8          //4
#define VPW                        11         //10


struct ssd1963_page {
   unsigned short x;
   unsigned short y;
   unsigned short *buffer;
   unsigned short *oldbuffer;
   unsigned short len;
   int must_update;
};

struct ssd1963 {
   struct device *dev;
   struct fb_info *info;
   unsigned int pages_count;
   struct ssd1963_page *pages;
   unsigned long pseudo_palette[17];
   int backlight;
   volatile unsigned short *ctrl_io;
   volatile unsigned short *data_io;
};

//#define SSD1963_PAGE_SIZE  (2048*2)
#define SSDD1963_LINES_PER_PAGE  32
#define SSD1963_PAGE_SIZE  (SSDD1963_LINES_PER_PAGE*WIDTH*2)

//#define SSD1963_PAGE_SIZE  (480*2)



inline void ssd1963_send_cmd(struct ssd1963 *item, unsigned short cmd)
{
   unsigned short ctrl = cmd & 0x00ff;
   writew(ctrl, item->ctrl_io);
}


inline void ssd1963_send_data(struct ssd1963 *item, unsigned short data)
{
   writew(data, item->data_io);
}



void ssd1963_setDisplayRegister
    (struct ssd1963 *item,
     unsigned short tft_register_ui16,
     unsigned short tft_registerValue_ui16,
     unsigned short tft_registerParameters_ui16)
{
   /* First send Command if it is != NOCMD */
   if (tft_register_ui16 != NOCMD) {
      ssd1963_send_cmd(item, tft_register_ui16);
   }
   /* Then send or continue sending Parameter to tft controler if needed */
   if (tft_registerParameters_ui16 != NOPARAM) {
      ssd1963_send_cmd(item, tft_registerValue_ui16);
   }
}

void ssd1963_setWindow(struct ssd1963 *item, unsigned short xs, unsigned short ys,
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

void ssd1963_drawPixel(struct ssd1963 *item,
                       unsigned short xPosition_ui16,
                       unsigned short yPosition_ui16,
                       unsigned short rectangleColor_ui16)
{
   /* Set window size and position - modified for portrait view */
   ssd1963_setWindow(item, xPosition_ui16, yPosition_ui16,
           xPosition_ui16 + 1, yPosition_ui16 + 1);
   ssd1963_send_data(item, rectangleColor_ui16);
}

void ssd1963_copy(struct ssd1963 *item, unsigned int index)
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
   for (y_local = y; y_local < y+SSDD1963_LINES_PER_PAGE; y_local++) {
      for (x = 0; x < WIDTH; x++) {
         if (buffer[x + tmpy * WIDTH] != oldbuffer[x + tmpy * WIDTH]) {
            if ((x + PIXELGROUPLEN) > WIDTH) {
               xend = WIDTH - 1;
            } else
               xend = x + PIXELGROUPLEN;
            ssd1963_setWindow(item, x, y_local, xend, y_local);
            for (j = x; j <= xend; j++) {
               ssd1963_send_data(item, buffer[j + tmpy * WIDTH]);
               oldbuffer[j + tmpy * WIDTH] = buffer[j + tmpy * WIDTH];
            }
            x = xend;
         }
      }
      tmpy++;
   }
}

static void ssd1963_update_all(struct ssd1963 *item)
{
   unsigned short i;
   struct fb_deferred_io *fbdefio = item->info->fbdefio;
   for (i = 0; i < item->pages_count; i++) {
      item->pages[i].must_update = 1;
   }
   //printk(KERN_ALERT "ssd1963_update_all () occurred!\n");
   schedule_delayed_work(&item->info->deferred_work, fbdefio->delay);
}

static void ssd1963_update(struct fb_info *info,
            struct list_head *pagelist)
{
   struct ssd1963 *item = (struct ssd1963 *) info->par;
   struct page *page;
   int i;
   unsigned short id[5] = {0};
   ssd1963_send_cmd(item, READ_DDB);
   mdelay(1);

   id[0]=readw(item->data_io);
   id[1]=readw(item->data_io);
   id[2]=readw(item->data_io);
   id[3]=readw(item->data_io);
   id[4]=readw(item->data_io);

   printk(KERN_ALERT "ssd1963_update signature=%02x %02x %02x %02x %02x\n",id[0],id[1],id[2],id[3],id[4]);

   //We can be called because of pagefaults (mmap'ed framebuffer, pages
   //returned in *pagelist) or because of kernel activity
   //(pages[i]/must_update!=0). Add the former to the list of the latter.
   list_for_each_entry(page, pagelist, lru) {
      item->pages[page->index].must_update = 1;
   }
   //printk(KERN_ALERT "ssd1963_update() occurred!\n");
   //Copy changed pages.
   for (i = 0; i < item->pages_count; i++) {
      //ToDo: Small race here between checking and setting must_update,
      //maybe lock?
      if (item->pages[i].must_update) {
         item->pages[i].must_update = 0;
         ssd1963_copy(item, i);
      }
   }

}



static void __init ssd1963_setup(struct ssd1963 *item)
{
   int x;
   int y;
   printk(KERN_ALERT "ssd1963_setup() called\n");
   dev_dbg(item->dev, "%s: item=0x%p\n", __func__, (void *) item);

   gpio_direction_output(LED_ENABLE_PIN, 1);

   msleep(20);

   ssd1963_setDisplayRegister(item, 0x00E2, 0x002D, PARAM); // PLL config
   ssd1963_setDisplayRegister(item, NOCMD, 0x0002, PARAM);
   ssd1963_setDisplayRegister(item, NOCMD, 0x0004, PARAM);
   msleep(6U);
   ssd1963_setDisplayRegister(item, 0x00E0, 0x0001, PARAM);
   msleep(6U);    // PLL config - continued
   ssd1963_setDisplayRegister(item, 0x00E0, 0x0003, PARAM);
   msleep(5U);    // PLL config - continued
   ssd1963_setDisplayRegister(item, 0x0001, 0x0000, NOPARAM);
   msleep(10U);      // software reset
   ssd1963_setDisplayRegister(item, 0x00E6, 0x0000, PARAM); // LSHIFT freq
   ssd1963_setDisplayRegister(item, NOCMD, 0x00FF, PARAM);
   ssd1963_setDisplayRegister(item, NOCMD, 0x00BE, PARAM);
   msleep(6U);
   ssd1963_setDisplayRegister(item, 0x00B0, 0x0020, PARAM);
   msleep(6U);    // set lcd mode
   ssd1963_setDisplayRegister(item, NOCMD, 0x0000, PARAM);
   msleep(6U);
   ssd1963_setDisplayRegister(item, NOCMD, ((HDP >> 8U) & 0x00FF), PARAM);
   msleep(6U);
   ssd1963_setDisplayRegister(item, NOCMD, (HDP & 0x00FF), PARAM);
   msleep(6U);
   ssd1963_setDisplayRegister(item, NOCMD, ((HDP >> 8U) & 0x00FF), PARAM);
   msleep(6U);
   ssd1963_setDisplayRegister(item, NOCMD, (VDP & 0x00FF), PARAM);
   msleep(6U);
   ssd1963_setDisplayRegister(item, NOCMD, 0x0000, PARAM);
   msleep(5U);
   ssd1963_setDisplayRegister(item, 0x00B4, ((HT >> 8U) & 0x00FF), PARAM);
   msleep(6U);    // horizontal period
   ssd1963_setDisplayRegister(item, NOCMD, (HT & 0x00FF), PARAM);
   msleep(6U);
   ssd1963_setDisplayRegister(item, NOCMD, ((HPS >> 8U) & 0x00FF), PARAM);
   msleep(6U);
   ssd1963_setDisplayRegister(item, NOCMD, (HPS & 0x00FF), PARAM);
   msleep(6U);
   ssd1963_setDisplayRegister(item, NOCMD, HPW, PARAM);
   msleep(6U);
   ssd1963_setDisplayRegister(item, NOCMD, ((LPS >> 8U) & 0x00FF), PARAM);
   msleep(6U);
   ssd1963_setDisplayRegister(item, NOCMD, (LPS & 0x00FF), PARAM);
   msleep(6U);
   ssd1963_setDisplayRegister(item, NOCMD, 0x0000, PARAM);
   msleep(6U);
   ssd1963_setDisplayRegister(item, 0x00B6, ((VT >> 8U) & 0x00FF), PARAM);
   msleep(6U);    // vertical period
   ssd1963_setDisplayRegister(item, NOCMD, (VT & 0x00FF), PARAM);
   msleep(6U);
   ssd1963_setDisplayRegister(item, NOCMD, (VPS >> 8U) & 0x00FF, PARAM);
   msleep(6U);
   ssd1963_setDisplayRegister(item, NOCMD, (VPS & 0x00FF), PARAM);
   msleep(6U);
   ssd1963_setDisplayRegister(item, NOCMD, VPW, PARAM);
   msleep(6U);
   ssd1963_setDisplayRegister(item, NOCMD, ((FPS >> 8U) & 0x00FF), PARAM);
   msleep(6U);
   ssd1963_setDisplayRegister(item, NOCMD, (FPS & 0x00FF), PARAM);
   msleep(6U);
   ssd1963_setDisplayRegister(item, 0x0036, 0x0000, PARAM);
   msleep(6U);    // rotation - landscape
   ssd1963_setDisplayRegister(item, 0x00F0, 0x0003, PARAM);
   msleep(5U);    // pixel data interface 16 bit    0b011 is 16 Bit
   ssd1963_setDisplayRegister(item, 0x0029, 0x0000, NOPARAM);
   msleep(6U);    // set display on
   ssd1963_setDisplayRegister(item, 0x00BE, 0x0006, PARAM);
   msleep(6U);    // pwm config
   ssd1963_setDisplayRegister(item, NOCMD, 0x00F0, PARAM);
   msleep(6U);
   ssd1963_setDisplayRegister(item, NOCMD, 0x0001, PARAM);
   msleep(6U);
   ssd1963_setDisplayRegister(item, NOCMD, 0x00F0, PARAM);
   msleep(6U);
   ssd1963_setDisplayRegister(item, NOCMD, 0x0000, PARAM);
   msleep(6U);
   ssd1963_setDisplayRegister(item, NOCMD, 0x0000, PARAM);
   msleep(6U);
   ssd1963_setDisplayRegister(item, 0x00D0, 0x000D, PARAM);
   msleep(6U);    // pwm coffig continued
   ssd1963_setDisplayRegister(item, 0x00B8, 0x0000, PARAM);
   msleep(6U);    // gpio config
   ssd1963_setDisplayRegister(item, NOCMD, 0x0000, PARAM);
   msleep(6U);
   ssd1963_setDisplayRegister(item, 0x00BA, 0x0000, PARAM);
   msleep(6U);    // gpio to 0x00

   ssd1963_setWindow(item, 0, 0, WIDTH - 1, HEIGHT - 1);
   for (x = 0; x < WIDTH * HEIGHT; x++)
      ssd1963_send_data(item, 0x0000);


   ssd1963_setWindow(item, 0, 0, WIDTH - 1, HEIGHT - 1);
   for (y = 0; y < item->pages_count; y++) {
      for (x = 0; x < item->pages[y].len; x++) {
         ssd1963_send_data(item, item->pages[y].buffer[x]);
      }
   }

   msleep(10);
   printk(KERN_ALERT
          "ssd1963_setup() end: pages_count: %d, item->pages[x].len: %d\n\n",
          item->pages_count, item->pages[0].len);
}

#undef GPIOSET


//This routine will allocate the buffer for the complete framebuffer. This
//is one continuous chunk of 16-bit pixel values; userspace programs
//will write here.
static int __init ssd1963_video_alloc(struct ssd1963 *item)
{
   unsigned int frame_size;

   dev_dbg(item->dev, "%s: item=0x%p\n", __func__, (void *) item);

   frame_size = item->info->fix.line_length * item->info->var.yres;
   dev_dbg(item->dev, "%s: item=0x%p frame_size=%u\n",
      __func__, (void *) item, frame_size);

   item->pages_count = frame_size / SSD1963_PAGE_SIZE;
   if ((item->pages_count * SSD1963_PAGE_SIZE) < frame_size) {
      item->pages_count++;
   }
   dev_dbg(item->dev, "%s: item=0x%p pages_count=%u\n",
      __func__, (void *) item, item->pages_count);

   item->info->fix.smem_len = item->pages_count * SSD1963_PAGE_SIZE;
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

static void ssd1963_video_free(struct ssd1963 *item)
{
   dev_dbg(item->dev, "%s: item=0x%p\n", __func__, (void *) item);

   kfree((void *) item->info->fix.smem_start);
}

//This routine will allocate a ssd1963_page struct for each vm page in the
//main framebuffer memory. Each struct will contain a pointer to the page
//start, an x- and y-offset, and the length of the pagebuffer which is in the framebuffer.
static int __init ssd1963_pages_alloc(struct ssd1963 *item)
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
       kmalloc(item->pages_count * sizeof(struct ssd1963_page),
          GFP_KERNEL);
   if (!item->pages) {
      dev_err(item->dev,
         "%s: unable to kmalloc for ssd1963_page\n",
         __func__);
      return -ENOMEM;
   }

   pixels_per_page =
       SSD1963_PAGE_SIZE / (item->info->var.bits_per_pixel / 8);
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
         "%s: unable to kmalloc for ssd1963_page oldbuffer\n",
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

static void ssd1963_pages_free(struct ssd1963 *item)
{
   dev_dbg(item->dev, "%s: item=0x%p\n", __func__, (void *) item);

   kfree(item->pages);
}

static inline __u32 CNVT_TOHW(__u32 val, __u32 width)
{
   return ((val << width) + 0x7FFF - val) >> 16;
}

//This routine is needed because the console driver won't work without it.
static int ssd1963_setcolreg(unsigned regno,
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

static int ssd1963_blank(int blank_mode, struct fb_info *info)
{
   struct ssd1963 *item = (struct ssd1963 *) info->par;
   if (blank_mode == FB_BLANK_UNBLANK)
      item->backlight = 1;
   else
      item->backlight = 0;
   //Item->backlight won't take effect until the LCD is written to. Force that
   //by dirty'ing a page.
   item->pages[0].must_update = 1;
   printk(KERN_ALERT "ssd1963_blank() occurred!\n");
   schedule_delayed_work(&info->deferred_work, 0);
   return 0;
}

static void ssd1963_touch(struct fb_info *info, int x, int y, int w, int h)
{
   struct fb_deferred_io *fbdefio = info->fbdefio;
   struct ssd1963 *item = (struct ssd1963 *) info->par;
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

static void ssd1963_fillrect(struct fb_info *p,
              const struct fb_fillrect *rect)
{
   sys_fillrect(p, rect);
   ssd1963_touch(p, rect->dx, rect->dy, rect->width, rect->height);
}

static void ssd1963_imageblit(struct fb_info *p,
               const struct fb_image *image)
{
   sys_imageblit(p, image);
   ssd1963_touch(p, image->dx, image->dy, image->width,
            image->height);
}

static void ssd1963_copyarea(struct fb_info *p,
              const struct fb_copyarea *area)
{
   sys_copyarea(p, area);
   ssd1963_touch(p, area->dx, area->dy, area->width, area->height);
}

static ssize_t ssd1963_write(struct fb_info *p, const char __user * buf,
              size_t count, loff_t * ppos)
{
   ssize_t res;
   res = fb_sys_write(p, buf, count, ppos);
   ssd1963_touch(p, 0, 0, p->var.xres, p->var.yres);
   return res;
}

static struct fb_ops ssd1963_fbops = {
   .owner         = THIS_MODULE,
   .fb_read       = fb_sys_read,
   .fb_write      = ssd1963_write,
   .fb_fillrect   = ssd1963_fillrect,
   .fb_copyarea   = ssd1963_copyarea,
   .fb_imageblit  = ssd1963_imageblit,
   .fb_setcolreg  = ssd1963_setcolreg,
   .fb_blank      = ssd1963_blank,
};

static struct fb_fix_screeninfo ssd1963_fix __initdata = {
   .id            = "SSD1963",
   .type          = FB_TYPE_PACKED_PIXELS,
   .visual        = FB_VISUAL_TRUECOLOR,
   .accel         = FB_ACCEL_NONE,
   .line_length   = 480 * 2,
};

static struct fb_var_screeninfo ssd1963_var __initdata = {
   .xres = 480,
   .yres = 272,
   .xres_virtual = 480,
   .yres_virtual = 272,
   .width = 480,
   .height = 272,
   .bits_per_pixel = 16,
   .red = {11, 5, 0},
   .green = {5, 6, 0},
   .blue = {0, 5, 0},
   .activate = FB_ACTIVATE_NOW,
   .vmode = FB_VMODE_NONINTERLACED,
};

static struct fb_deferred_io ssd1963_defio = {
   .delay = HZ / 50,
   .deferred_io = &ssd1963_update,
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
   unsigned short int id[6];
   struct fb_info *info;

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
// MPMC_STCONFIG0 = 0x81;
// MPMC_STWTWEN0  = 0;
// MPMC_STWTOEN0  = 0;
// MPMC_STWTRD0   = 31;
// MPMC_STWTPG0   = 0;
// MPMC_STWTWR0   = 3;
// MPMC_STWTTURN0 = 0;

   MPMC_STCONFIG0  = 0x81;
   MPMC_STWTWEN0   = 2;
   MPMC_STWTOEN0   = 1;
   MPMC_STWTRD0    = 15;
   MPMC_STWTPG0    = 2;
   MPMC_STWTWR0    = 4;
   MPMC_STWTTURN0  = 2;

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


   ssd1963_send_cmd(item, SOFT_RESET);
   mdelay(200);
   ssd1963_send_cmd(item, READ_DDB);
   //mdelay(1);

   id[0]=readw(item->data_io);
   id[1]=readw(item->data_io);
   id[2]=readw(item->data_io);
   id[3]=readw(item->data_io);
   id[4]=readw(item->data_io);

   dev_dbg(&dev->dev, "%s: signature=%02x %02x %02x %02x %02x\n", __func__, id[0],id[1],id[2],id[3],id[4]);

   if (!(id[0]==0x01 && id[1]==0x57 && id[2]==0x61 && id[3]==0x01)) {
      ret = -ENODEV;
      dev_err(&dev->dev,
         "%s: unknown signature %02x %02x %02x %02x %02x\n", __func__, id[0],id[1],id[2],id[3],id[4]);
      goto out_item;
   }

   dev_info(&dev->dev, "item=0x%p ctrl=0x%p data=0x%p\n", (void *)item,
       (void *)ctrl_res->start, (void *)data_res->start);

   info = framebuffer_alloc(sizeof(struct ssd1963), &dev->dev);
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
   info->fbops = &ssd1963_fbops;
   info->flags = FBINFO_FLAG_DEFAULT | FBINFO_VIRTFB;
   info->fix = ssd1963_fix;
   info->var = ssd1963_var;

   ret = ssd1963_video_alloc(item);
   if (ret) {
      dev_err(&dev->dev,
         "%s: unable to ssd1963_video_alloc\n", __func__);
      goto out_info;
   }
   info->screen_base = (char __iomem *) item->info->fix.smem_start;

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


static int ssd1963_remove(struct platform_device *dev)
{
   struct fb_info *info = dev_get_drvdata(&dev->dev);
   struct ssd1963 *item = (struct ssd1963 *) info->par;
   unregister_framebuffer(info);
   ssd1963_pages_free(item);
   ssd1963_video_free(item);
   framebuffer_release(info);
   kfree(item);
   return 0;
}

#ifdef CONFIG_PM
static int ssd1963_suspend(struct platform_device *dev, pm_message_t state)
{
//      struct fb_info *info = dev_get_drvdata(&spi->dev);
//      struct ssd1963 *item = (struct ssd1963 *)info->par;
   /* enter into sleep mode */
//      ssd1963_reg_set(item, SSD1963_REG_SLEEP_MODE, 0x0001);
   return 0;
}

static int ssd1963_resume(struct platform_device *dev)
{
//      struct fb_info *info = dev_get_drvdata(&spi->dev);
//      struct ssd1963 *item = (struct ssd1963 *)info->par;
   /* leave sleep mode */
//      ssd1963_reg_set(item, SSD1963_REG_SLEEP_MODE, 0x0000);
   return 0;
}
#else
#define ssd1963_suspend NULL
#define ssd1963_resume NULL
#endif

static struct platform_driver ssd1963_driver = {
   .probe = ssd1963_probe,
   .remove = ssd1963_remove,
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
      pr_err("%s: unable to platform_driver_register\n",
             __func__);
   }

   return ret;
}

module_init(ssd1963_init);
module_remove(ssd1963_remove);


MODULE_DESCRIPTION("SSD1963 LCD Driver");
MODULE_AUTHOR("Jeroen Domburg <jeroen@spritesmods.com>");
MODULE_AUTHOR("Armin Schlegel <armin.schlegel@gmx.de");
MODULE_LICENSE("GPL");

/*
 * SSD1963 Framebuffer
 *
 * Copyright (c) 2009 Jean-Christian de Rivaz
 * Copyright (c) 2010 Miguel Angel Ajo Pelayo
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * The Solomon Systech SSD1963 chip drive TFT screen up to 800x600. This
 * driver expect the SSD1963 to be connected to a 16 bits local bus and
 * to be set in the 16 bits parallel interface mode. To use it you must
 * define in your board file a struct platform_device with a name set to
 * "ssd1963" and a struct resource array with two IORESOURCE_MEM: the first
 * for the control register; the second for the data register.
 */

#include <linux/kernel.h>
#define DEBUG 1
#include <linux/device.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/mm.h>
#include <linux/vmalloc.h>
#include <linux/fb.h>
#include <linux/delay.h>
#include <asm/io.h>
#include <mach/gpio.h>

#define LED_ENABLE_PIN  20
#define RESET_PIN       19
#define SCREENSIZE   130560      // 480*272

/* TFT Controler Register defines */
#define SSD1963_NOP           0x00
#define SOFT_RESET            0x01
#define GET_POWER_MODE        0x0A
#define GET_ADDRESS_MODE      0x0B
#define GET_PIXEL_FORMAT      0x0C
#define GET_DISPLAY_MODE      0x0D
#define GET_SIGNAL_MODE       0x0E
#define ENTER_SLEEP_MODE      0x10
#define EXIT_SLEEP_MODE       0x11
#define ENTER_PARTIAL_MODE    0x12
#define ENTER_NORMAL_MODE     0x13
#define EXIT_INVERT_MODE      0x20
#define ENTER_INVERT_MODE     0x21
#define SET_GAMMA_CURVE       0x26
#define SET_DISPLAY_OFF       0x28
#define SET_DISPLAY_ON        0x29
#define SET_COLUMN_ADDRESS    0x2A
#define SET_PAGE_ADDRESS      0x2B
#define WRITE_MEMORY_START    0x2C
#define READ_MEMORY_START     0x2E
#define SET_PARTIAL_AERA      0x30
#define SET_SCROLL_AREA       0x33
#define SET_TEAR_OFF          0x34
#define SET_TEAR_ON           0x35
#define SET_ADDRESS_MODE      0x36
#define SET_SCROLL_START      0x37
#define EXIT_IDLE_MODE        0x38
#define ENTER_IDLE_MODE       0x39
#define SET_PIXEL_FORMAT      0x3A
#define WRITE_MEMORY_CONTINUE 0x3C
#define READ_MEMORY_CONTINUE  0x3E
#define SET_TEAR_SCNALINE     0x44
#define GET_SCANLINE          0x45
#define READ_DDB              0xA1
#define SET_LCD_MODE          0xB0
#define GET_LCD_MODE          0xB1
#define SET_HORI_PERIOD       0xB4
#define GET_HORI_PERIOD       0xB5
#define SET_VERT_PERIOD       0xB6
#define GET_VERT_PERIOD       0xB7
#define SET_GPIO_CONF         0xB8
#define GET_GPIO_CONF         0xB9
#define SET_GPIO_VALUE        0xBA
#define GET_GPIO_STATUS       0xBB
#define SET_POST_PROC         0xBC
#define GET_POST_PROC         0xBD
#define SET_PWM_CONF          0xBE
#define GET_PWM_CONF          0xBF
#define SET_LCD_GEN0          0xC0
#define GET_LCD_GEN0          0xC1
#define SET_LCD_GEN1          0xC2
#define GET_LCD_GEN1          0xC3
#define SET_LCD_GEN2          0xC4
#define GET_LCD_GEN2          0xC5
#define SET_LCD_GEN3          0xC6
#define GET_LCD_GEN3          0xC7
#define SET_GPIO0_ROP         0xC8
#define GET_GPIO0_ROP         0xC9
#define SET_GPIO1_ROP         0xCA
#define GET_GPIO1_ROP         0xCB
#define SET_GPIO2_ROP         0xCC
#define GET_GPIO2_ROP         0xCD
#define SET_GPIO3_ROP         0xCE
#define GET_GPIO3_ROP         0xCF
#define SET_DBC_CONF          0xD0
#define GET_DBC_CONF          0xD1
#define SET_DBC_TH            0xD4
#define GET_DBC_TH            0xD5
#define SET_PLL               0xE0
#define SET_PLL_MN            0xE2
#define GET_PLL_MN            0xE3
#define GET_PLL_STATUS        0xE4
#define SET_DEEP_SLEEP        0xE5
#define SET_LSHIFT_FREQ       0xE6
#define GET_LSHIFT_FREQ       0xE7
#define SET_PIXEL_DATA_INTERFACE 0xF0
#define GET_PIXEL_DATA_INTERFACE 0xF1



#define SCREENWIDTH                      480
#define SCREENHEIGHT                     272

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
   unsigned short *last_buffer_start;
};

static inline void ssd1963_reg_set(struct ssd1963 *item, unsigned char reg,
               unsigned short value)
{
   unsigned short ctrl = reg & 0x00ff;

   writew(ctrl, item->ctrl_io);
   writew(value, item->data_io);
}

static inline void ssd1963_send_cmd_slow(struct ssd1963 *item, unsigned char reg)
{
   unsigned short ctrl = reg & 0x00ff;

   writew(ctrl, item->ctrl_io);
   mdelay(1);

}

static inline void ssd1963_send_data_slow(struct ssd1963 *item, unsigned short value)
{
   writew(value, item->data_io);
   mdelay(1);

}

static inline unsigned short ssd1963_read_data_slow(struct ssd1963 *item)
{
   mdelay(1);
   return readw(item->data_io);


}


static inline void ssd1963_send_cmd(struct ssd1963 *item, unsigned char reg)
{
   unsigned short ctrl = reg & 0x00ff;

   writew(ctrl, item->ctrl_io);

}

static inline void ssd1963_send_data(struct ssd1963 *item, unsigned int value)
{
   writew(value, item->data_io);
}


static void ssd1963_copy(struct ssd1963 *item, unsigned int index)
{
   unsigned short x;
   unsigned short y;
   unsigned short *buffer;
   unsigned int len;
   unsigned int count;
   unsigned int diff;

   x = item->pages[index].x;
   y = item->pages[index].y;

   buffer = item->pages[index].buffer;
   len = item->pages[index].len;

   // check if we should change the chip framebuffer pointer
   if (item->last_buffer_start!=buffer)
   {

      if (x!=0)   /* The SSD1963 needs to start from X=0 to keep drawing in next lines */
      {
         diff = x*(item->info->var.bits_per_pixel / 16);
         buffer-=diff;
         len+=diff;
      }




      dev_dbg(item->dev,
         "%s: page[%u]: x=%3hu y=%3hu buffer=0x%p len=%3hu diff=%3hu\n",
         __func__, index, x, y, buffer, len,diff);

      ssd1963_send_cmd (item, SET_COLUMN_ADDRESS);
      ssd1963_send_data(item, 0);     // current x position
      ssd1963_send_data(item, 0);
      ssd1963_send_data(item, 0x01);    // end position (479)
      ssd1963_send_data(item, 0xdf);


      ssd1963_send_cmd (item, SET_PAGE_ADDRESS);
      ssd1963_send_data(item, y>>8);     // current y position
      ssd1963_send_data(item, y&0xff);
      ssd1963_send_data(item, 0x01);    // end position (271)
      ssd1963_send_data(item, 0x0f);

      ssd1963_send_cmd (item, WRITE_MEMORY_START);
   }


   memcpy(item->data_io,buffer,len*2);
   item->last_buffer_start=&buffer[len];
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

   int i;
//   gpio_direction_output(RESET_PIN, 1);
//   msleep(10U);
//   gpio_direction_output(RESET_PIN, 0);
//   msleep(100U);
//   gpio_direction_output(RESET_PIN, 1);

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


  // ssd1963_setWindow(item, 0, 0, (item->info->var.xres - 1), (item->info->var.yres - 1));
   ssd1963_send_cmd(item, SET_COLUMN_ADDRESS);
   ssd1963_send_data(item, (0 & 0xFF00) >> 8);
   ssd1963_send_data(item, (0 & 0x00FF));
   ssd1963_send_data(item, (480 & 0xFF00) >> 8);
   ssd1963_send_data(item, (480 & 0x00FF));

   ssd1963_send_cmd(item, SET_PAGE_ADDRESS);
   ssd1963_send_data(item, (0 & 0xFF00) >> 8);
   ssd1963_send_data(item, (0 & 0x00FF));
   ssd1963_send_data(item, (272 & 0xFF00) >> 8);
   ssd1963_send_data(item, (272 & 0x00FF));

   ssd1963_send_cmd(item, WRITE_MEMORY_START);

   for(i = 0; i < SCREENSIZE; i++)
   {
      ssd1963_send_data(item, 0xABCD);
   }
   ssd1963_send_cmd(item, WRITE_MEMORY_START);
}

static int __init ssd1963_video_alloc(struct ssd1963 *item)
{
   unsigned int frame_size;

   dev_dbg(item->dev, "%s: item=0x%p\n", __func__, (void *)item);

   frame_size = item->info->fix.line_length * item->info->var.yres;
   dev_dbg(item->dev, "%s: item=0x%p frame_size=%u\n",
      __func__, (void *)item, frame_size);

   item->pages_count = frame_size / PAGE_SIZE;
   if ((item->pages_count * PAGE_SIZE) < frame_size) {
      item->pages_count++;
   }
   dev_dbg(item->dev, "%s: item=0x%p pages_count=%u\n",
      __func__, (void *)item, item->pages_count);

   item->info->fix.smem_len = item->pages_count * PAGE_SIZE;
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

   pixels_per_page = PAGE_SIZE / (item->info->var.bits_per_pixel / 8);
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

   /* tell the copy routines that the LCD controller is ready to receive data
     from the start of the buffer X,Y = (0,0) */

   item->last_buffer_start=(unsigned short *)item->info->fix.smem_start;

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
   .line_length = 480 * 2,
};

static struct fb_var_screeninfo ssd1963_var __initdata = {
   .xres    = 480,
   .yres    = 272,
   .xres_virtual  = 480,
   .yres_virtual  = 272,
   .bits_per_pixel   = 16,
   .red     = {6, 5, 0},
   .green      = {11, 5, 0},
   .blue    = {0, 6, 0},
   .activate   = FB_ACTIVATE_NOW,
   .height     = 272,
   .width      = 480,
   .vmode      = FB_VMODE_NONINTERLACED,
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
      goto out_no_release;
   }
   ctrl_res_size = ctrl_res->end - ctrl_res->start + 1;
   ctrl_req = request_mem_region(ctrl_res->start, ctrl_res_size,
                  dev->name);
   if (!ctrl_req) {
      dev_err(&dev->dev,
         "%s: unable to request_mem_region for ctrl_req\n",
         __func__);
      ret = -EIO;
      goto out_no_release;
   }

      /* Now that we're sure that we own the memory bus... reset timings */
 MPMC_STCONFIG0 = 0x81;
 MPMC_STWTWEN0  = 0;
 MPMC_STWTOEN0  = 0;
 MPMC_STWTRD0   = 31;
 MPMC_STWTPG0   = 0;
 MPMC_STWTWR0   = 3;
 MPMC_STWTTURN0 = 0;

//   MPMC_STCONFIG0 = 0x81;
//   MPMC_STWTWEN0  = 31;
//   MPMC_STWTOEN0  = 31;
//   MPMC_STWTRD0   = 31;
//   MPMC_STWTPG0   = 0;
//   MPMC_STWTWR0   = 31;
//   MPMC_STWTTURN0 = 10;

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


   ssd1963_send_cmd_slow (item, SOFT_RESET);
   mdelay(200);
   ssd1963_send_cmd_slow (item, READ_DDB);
   mdelay(1);

   id[0]=readw(item->data_io);
   id[1]=readw(item->data_io);
   id[2]=readw(item->data_io);
   id[3]=readw(item->data_io);
   id[4]=readw(item->data_io);

   dev_dbg(&dev->dev, "%s: signature=%02x %02x %02x %02x %02x\n", __func__, id[0],id[1],id[2],id[3],id[4]);

   if (!(id[0]==0x01 && id[1]==0x57 && id[2]==0x61 && id[3]==0x01)) {
      //ret = -ENODEV;
      dev_err(&dev->dev,
         "%s: unknown signature %02x %02x %02x %02x %02x\n", __func__, id[0],id[1],id[2],id[3],id[4]);
      //goto out_item;
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
   release_mem_region(data_res->start, data_res_size);
   release_mem_region(ctrl_res->start, ctrl_res_size);
out_no_release:
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
MODULE_AUTHOR("Miguel Angel Ajo Pelayo <miguelangel@nbee.es>");
MODULE_LICENSE("GPL");

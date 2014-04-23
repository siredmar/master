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
#include <linux/device.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/mm.h>
#include <linux/vmalloc.h>
#include <linux/fb.h>
#include <linux/delay.h>
#include <asm/io.h>

#define ssd1963_nop 0x00
#define soft_reset 0x01
#define get_power_mode 0x0A
#define get_address_mode 0x0B
#define get_pixel_format 0x0C
#define get_display_mode 0x0D
#define get_signal_mode 0x0E
#define enter_sleep_mode 0x10
#define exit_sleep_mode 0x11
#define enter_partial_mode 0x12
#define enter_normal_mode 0x13
#define exit_invert_mode 0x20
#define enter_invert_mode 0x21
#define set_gamma_curve 0x26
#define set_display_off 0x28
#define set_display_on 0x29
#define set_column_address 0x2A
#define ssd_set_page_address 0x2B
#define write_memory_start 0x2C
#define read_memory_start 0x2E
#define set_partial_area 0x30
#define set_scroll_area 0x33
#define set_tear_off 0x34
#define set_tear_on 0x35
#define set_address_mode 0x36
#define set_scroll_start 0x37
#define exit_idle_mode 0x38
#define enter_idle_mode 0x39
#define set_pixel_format 0x3A
#define write_memory_continue 0x3C
#define read_memory_continue 0x3E
#define set_tear_scanline 0x44
#define get_scanline 0x45
#define read_ddb 0xA1
#define set_lcd_mode 0xB0
#define get_lcd_mode 0xB1
#define set_hori_period 0xB4
#define get_hori_period 0xB5
#define set_vert_period 0xB6
#define get_vert_period 0xB7
#define set_gpio_conf 0xB8
#define get_gpio_conf 0xB9
#define set_gpio_value 0xBA
#define get_gpio_status 0xBB
#define set_post_proc 0xBC
#define get_post_proc 0xBD
#define set_pwm_conf 0xBE
#define get_pwm_conf 0xBF
#define set_lcd_gen0 0xC0
#define get_lcd_gen0 0xC1
#define set_lcd_gen1 0xC2
#define get_lcd_gen1 0xC3
#define set_lcd_gen2 0xC4
#define get_lcd_gen2 0xC5
#define set_lcd_gen3 0xC6
#define get_lcd_gen3 0xC7
#define set_gpio0_rop 0xC8
#define get_gpio0_rop 0xC9
#define set_gpio1_rop 0xCA
#define get_gpio1_rop 0xCB
#define set_gpio2_rop 0xCC
#define get_gpio2_rop 0xCD
#define set_gpio3_rop 0xCE
#define get_gpio3_rop 0xCF
#define set_dbc_conf 0xD0
#define get_dbc_conf 0xD1
#define set_dbc_th 0xD4
#define get_dbc_th 0xD5
#define set_pll 0xE0
#define set_pll_mn 0xE2
#define get_pll_mn 0xE3
#define get_pll_status 0xE4
#define set_deep_sleep 0xE5
#define set_lshift_freq 0xE6
#define get_lshift_freq 0xE7
#define set_pixel_data_interface 0xF0
#define get_pixel_data_interface 0xF1




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

//#define FHS_BOOT_IMAGE 0

#ifdef FHS_BOOT_IMAGE
#include "fhslogo.h"

void ssd1963_draw_boot_image(struct ssd1963 *item, unsigned short *buffer)
{

	unsigned char pixel[4];
	char *data;
	unsigned short rgb16,r,g,b;
	int x,y;

	memset(buffer,0xff,480*272*2);

	buffer+=(480*20)+180; // 180 pixels to the right

	data = (unsigned char*) header_data;

	for (y=0;y<height;y++)
	{
		for (x=0;x<width;x++)
		{
			HEADER_PIXEL(data,pixel);

			r = pixel[0]; r=(r<<8)&0xf800;
			g = pixel[1]; g=((g>>2)&0x3f)<<5;
			b = ((pixel[2]>>3)&0x1f);

			rgb16 = r|g|b;

			*buffer++=rgb16;
		/*
		dev_info(item->dev, "bl> p[0]=%d p[1]=%d p[2]=%d , rgb16=0x%04x\n",
		 pixel[0],pixel[1],pixel[2],rgb16);
			*/
		}
		buffer+=480-width; // jump to the next line
	}


}


#endif

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

static inline void ssd1963_send_data(struct ssd1963 *item, unsigned char value)
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

		if (x!=0)	/* The SSD1963 needs to start from X=0 to keep drawing in next lines */
		{
			diff = x*(item->info->var.bits_per_pixel / 16);
			buffer-=diff;
			len+=diff;
		}




		dev_dbg(item->dev,
			"%s: page[%u]: x=%3hu y=%3hu buffer=0x%p len=%3hu diff=%3hu\n",
			__func__, index, x, y, buffer, len,diff);

		ssd1963_send_cmd (item, set_column_address);
		ssd1963_send_data(item, 0);	  // current x position
		ssd1963_send_data(item, 0);
		ssd1963_send_data(item, 0x01);    // end position (479)
		ssd1963_send_data(item, 0xdf);


		ssd1963_send_cmd (item, ssd_set_page_address);
		ssd1963_send_data(item, y>>8);	  // current y position
		ssd1963_send_data(item, y&0xff);
		ssd1963_send_data(item, 0x01);    // end position (271)
		ssd1963_send_data(item, 0x0f);

		ssd1963_send_cmd (item, write_memory_start);
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

	dev_dbg(item->dev, "%s: item=0x%p\n", __func__, (void *)item);


	ssd1963_send_cmd_slow(item,set_pll);
	ssd1963_send_data_slow(item,0x01);
	mdelay(5);

	ssd1963_send_cmd_slow(item,set_pll_mn);
	ssd1963_send_data_slow(item,0x1D);
	ssd1963_send_data_slow(item,0x02);
	ssd1963_send_data_slow(item,0x54);

	ssd1963_send_cmd_slow(item,set_pll);
	ssd1963_send_data_slow(item,0x03);
	mdelay(5);

	/* Wait for PLL to lock */
	do
	{
		ssd1963_send_cmd_slow(item,get_pll_status);
	} while (ssd1963_read_data_slow(item)!=0x04);


/////////////////////////////////////////////////////////////////////
// Set LSHIFT frequency		 (Dot CLK 0xE6)
////////////////////////////////////////////////////////////////////
	ssd1963_send_cmd(item,set_lshift_freq);
	ssd1963_send_data(item,0x01);				 // 9Mhz=100Mhz(x+1)/2^20   , x=94371 = 0x0170A3
	ssd1963_send_data(item,0x70);
	ssd1963_send_data(item,0xA3);

/////////////////////////////////////////////////////////////////////
// Pannel Settings (0xB0 command)
////////////////////////////////////////////////////////////////////


	ssd1963_send_cmd(item,set_lcd_mode);
	udelay(100);
	ssd1963_send_data(item,0x38);		  //24bit
	ssd1963_send_data(item,0x00);       //TFT mode
	ssd1963_send_data(item,0x01);		  //1 11011111	= 480-1
	ssd1963_send_data(item,0xDF);
	ssd1963_send_data(item,0x01);       // 10F = 272-1
	ssd1963_send_data(item,0x0F);
	ssd1963_send_data(item,0x00);       // RGB/RGB  even/odd

/////////////////////////////////////////////////////////////////////
// This is a reserved command, but chinese doc sets it
////////////////////////////////////////////////////////////////////

	ssd1963_send_cmd(item,set_pixel_format);
	ssd1963_send_data(item,0x50);

/////////////////////////////////////////////////////////////////////
// Set Horizontal Period (0xb4)
////////////////////////////////////////////////////////////////////
	ssd1963_send_cmd(item,set_hori_period);

	ssd1963_send_data(item,0x02);			 /* Total Horiz size */
	ssd1963_send_data(item,0x0D);

	ssd1963_send_data(item,0x00);		     /* Non display period from LLINE */
	ssd1963_send_data(item,0x25+6);

	ssd1963_send_data(item,0x02);          /* Horizontal pulse LLINE width */

	ssd1963_send_data(item,0x00);          /* Start of LLINE in pixel clock */
	ssd1963_send_data(item,0x00);

	ssd1963_send_data(item,0x00);			/* Subpixel start position for serial TFT interface */

/////////////////////////////////////////////////////////////////////
// Set Vertical Period
////////////////////////////////////////////////////////////////////
	ssd1963_send_cmd(item,set_vert_period);

	ssd1963_send_data(item,0x01);	       /*Total Vert size */
	ssd1963_send_data(item,0x1E);

	ssd1963_send_data(item,0x00);		   /* non display period in lines from LFRAME */
	ssd1963_send_data(item,0x08+3);

	ssd1963_send_data(item,0x01);		   /* Vertical sync pulse width */

	ssd1963_send_data(item,0x00);        /* VSYNC pulse start */
	ssd1963_send_data(item,0x00);


/////////////////////////////////////////////////////////////////////
// Set Column Address	0x2A
////////////////////////////////////////////////////////////////////
	ssd1963_send_cmd(item,set_column_address);
	ssd1963_send_data(item,0x00);    // 0  (start)
	ssd1963_send_data(item,0x00);

	ssd1963_send_data(item,0x01);	   // 479  (end)
	ssd1963_send_data(item,0xdf);

/////////////////////////////////////////////////////////////////////
// Set Row Address
////////////////////////////////////////////////////////////////////
	ssd1963_send_cmd(item,ssd_set_page_address);
	ssd1963_send_data(item,0x00);    // 0  (start)
	ssd1963_send_data(item,0x00);

	ssd1963_send_data(item,0x01);	   // 271  (end)
	ssd1963_send_data(item,0x0f);

/////////////////////////////////////////////////////////////////////
// Set Address Mode
////////////////////////////////////////////////////////////////////

    ssd1963_send_cmd(item,set_address_mode);
	ssd1963_send_data(item,0x00);


/////////////////////////////////////////////////////////////////////
// Set pixel data interface
////////////////////////////////////////////////////////////////////
	ssd1963_send_cmd(item,set_pixel_data_interface);
	ssd1963_send_data(item,0x03);	   // 16bit ->  5x6x5

	ssd1963_send_cmd(item,set_display_on);

/////////////////////////////////////////////////////////////////////
// Set PWM
////////////////////////////////////////////////////////////////////
	ssd1963_send_cmd(item,set_pwm_conf);
	ssd1963_send_data(item,0x0E); // PWMF reg, clock = PLLClock/(256*PWMF)/256
	ssd1963_send_data(item,0xFF); // PWM reg (duty cycle)

	ssd1963_send_data(item,0x01);  // 0x8 (controlled by DBC, instead of host) 0x1 (enable PWM)
	ssd1963_send_data(item,0xFF);

	ssd1963_send_data(item,0x00);
	ssd1963_send_data(item,0x00);

/////////////////////////////////////////////////////////////////////
// Dynamic Backlight Threshold
////////////////////////////////////////////////////////////////////
	ssd1963_send_cmd(item,set_dbc_th);
	ssd1963_send_data(item,0x00);
	ssd1963_send_data(item,0x09);
	ssd1963_send_data(item,0x90);

	ssd1963_send_data(item,0x00);
	ssd1963_send_data(item,0x17);
	ssd1963_send_data(item,0xE8);

	ssd1963_send_data(item,0x00);
	ssd1963_send_data(item,0x39);
	ssd1963_send_data(item,0x60);

/////////////////////////////////////////////////////////////////////
// Dynamic Backlight Control
////////////////////////////////////////////////////////////////////

	ssd1963_send_cmd(item,set_dbc_conf);
	ssd1963_send_data(item,0x0D+(1<<6));

	ssd1963_send_cmd (item, write_memory_start);



/*
	// OSCEN=1
	ssd1963_reg_set(item, SSD1963_REG_OSCILLATION, 0x0001);
	// DCT=b1010=fosc/4 BT=b001=VGH:+6,VGL:-4
	// DC=b1010=fosc/4 AP=b010=small to medium
	ssd1963_reg_set(item, SSD1963_REG_POWER_CTRL_1, 0xa2a4);
	// VRC=b100:5.5V
	ssd1963_reg_set(item, SSD1963_REG_POWER_CTRL_2, 0x0004);
	// VRH=b1000:Vref*2.165
	ssd1963_reg_set(item, SSD1963_REG_POWER_CTRL_3, 0x0308);
	// VCOMG=1 VDV=b1000:VLCD63*1.05
	ssd1963_reg_set(item, SSD1963_REG_POWER_CTRL_4, 0x3000);
	// nOTP=1 VCM=0x2a:VLCD63*0.77
	ssd1963_reg_set(item, SSD1963_REG_POWER_CTRL_5, 0x00ea);
	// RL=0 REV=1 CAD=0 BGR=1 SM=0 TB=1 MUX=0x13f=319
	ssd1963_reg_set(item, SSD1963_REG_DRIVER_OUT_CTRL, 0x2b3f);
	// FLD=0 ENWS=0 D/C=1 EOR=1 WSMD=0 NW=0x00=0
	ssd1963_reg_set(item, SSD1963_REG_LCD_DRIVE_AC, 0x0600);
	// SLP=0
	ssd1963_reg_set(item, SSD1963_REG_SLEEP_MODE, 0x0000);
	// VSMode=0 DFM=3:65k TRAMS=0 OEDef=0 WMode=0 Dmode=0
	// TY=0 ID=3 AM=0 LG=0
	ssd1963_reg_set(item, SSD1963_REG_ENTRY_MODE, 0x6030);
	// PT=0 VLE=1 SPT=0 GON=1 DTE=1 CM=0 D=3
	ssd1963_reg_set(item, SSD1963_REG_DISPLAY_CTRL, 0x0233);
	// NO=0 SDT=0 EQ=0 DIV=0 SDIV=1 SRTN=1 RTN=9:25 clock
	ssd1963_reg_set(item, SSD1963_REG_FRAME_CYCLE, 0x0039);
	// SCN=0
	ssd1963_reg_set(item, SSD1963_REG_GATE_SCAN_START, 0x0000);

	// PKP1=7 PKP0=7
	ssd1963_reg_set(item, SSD1963_REG_GAMMA_CTRL_1, 0x0707);
	// PKP3=2 PKP2=4
	ssd1963_reg_set(item, SSD1963_REG_GAMME_CTRL_2, 0x0204);
	// PKP5=2 PKP4=2
	ssd1963_reg_set(item, SSD1963_REG_GAMMA_CTRL_3, 0x0204);
	// PRP1=5 PRP0=2
	ssd1963_reg_set(item, SSD1963_REG_GAMMA_CTRL_4, 0x0502);
	// PKN1=5 PKN0=7
	ssd1963_reg_set(item, SSD1963_REG_GAMMA_CTRL_5, 0x0507);
	// PKN3=2 PNN2=4
	ssd1963_reg_set(item, SSD1963_REG_GAMMA_CTRL_6, 0x0204);
	// PKN5=2 PKN4=4
	ssd1963_reg_set(item, SSD1963_REG_GAMMA_CTRL_7, 0x0204);
	// PRN1=5 PRN0=2
	ssd1963_reg_set(item, SSD1963_REG_GAMMA_CTRL_8, 0x0502);
	// VRP1=3 VRP0=2
	ssd1963_reg_set(item, SSD1963_REG_GAMMA_CTRL_9, 0x0302);
	// VRN1=3 VRN0=2
	ssd1963_reg_set(item, SSD1963_REG_GAMMA_CTRL_10, 0x0302);

	// WMR=0 WMG=0
	ssd1963_reg_set(item, SSD1963_REG_WR_DATA_MASK_1, 0x0000);
	// WMB=0
	ssd1963_reg_set(item, SSD1963_REG_WR_DATA_MASK_2, 0x0000);
	// OSC=b1010:548k
	ssd1963_reg_set(item, SSD1963_REG_FRAME_FREQUENCY, 0xa000);
	// SS1=0
	ssd1963_reg_set(item, SSD1963_REG_FIRST_WIN_START, 0x0000);
	// SE1=319
	ssd1963_reg_set(item, SSD1963_REG_FIRST_WIN_END,
			(item->info->var.yres - 1));
	// SS2=0
	ssd1963_reg_set(item, SSD1963_REG_SECND_WIN_START, 0x0000);
	// SE2=0
	ssd1963_reg_set(item, SSD1963_REG_SECND_WIN_END, 0x0000);
	// VL1=0
	ssd1963_reg_set(item, SSD1963_REG_V_SCROLL_CTRL_1, 0x0000);
	// VL2=0
	ssd1963_reg_set(item, SSD1963_REG_V_SCROLL_CTRL_2, 0x0000);
	// HEA=0xef=239 HSA=0
	ssd1963_reg_set(item, SSD1963_REG_H_RAM_ADR_POS,
			(item->info->var.xres - 1) << 8);
	// VSA=0
	ssd1963_reg_set(item, SSD1963_REG_V_RAM_ADR_START, 0x0000);
	// VEA=0x13f=319
	ssd1963_reg_set(item, SSD1963_REG_V_RAM_ADR_END,
			(item->info->var.yres - 1));

*/
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


#ifdef FHS_BOOT_IMAGE
	ssd1963_draw_boot_image(item,(unsigned short *)item->info->fix.smem_start);
#else
  memset((void *)item->info->fix.smem_start, 0, item->info->fix.smem_len);
#endif

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
	.xres		= 480,
	.yres		= 272,
	.xres_virtual	= 480,
	.yres_virtual	= 272,
	.bits_per_pixel	= 16,
	.red		= {6, 5, 0},
	.green		= {11, 5, 0},
	.blue		= {0, 6, 0},
	.activate	= FB_ACTIVATE_NOW,
	.height		= 272,
	.width		= 480,
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
//	MPMC_STCONFIG0 = 0x81;
//	MPMC_STWTWEN0  = 0;
//	MPMC_STWTOEN0  = 0;
//	MPMC_STWTRD0   = 31;
//	MPMC_STWTPG0   = 0;
//	MPMC_STWTWR0   = 3;
//	MPMC_STWTTURN0 = 0;

   MPMC_STCONFIG0 = 0x81;
   MPMC_STWTWEN0  = 31;
   MPMC_STWTOEN0  = 31;
   MPMC_STWTRD0   = 31;
   MPMC_STWTPG0   = 0;
   MPMC_STWTWR0   = 31;
   MPMC_STWTTURN0 = 10;

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


	ssd1963_send_cmd_slow (item, soft_reset);
	mdelay(200);
	ssd1963_send_cmd_slow (item, read_ddb);
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
	release_mem_region(data_res->start,	data_res_size);
	release_mem_region(ctrl_res->start,	ctrl_res_size);
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

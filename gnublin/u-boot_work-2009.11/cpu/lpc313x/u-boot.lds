/*
 * (C) Copyright 2010 NXP Semiconductors
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#include <config.h>

OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start)

SECTIONS
{
	. = TEXT_BASE;

	__IMAGE_START  = .;
	
	.text :
	{
		cpu/lpc313x/start.o		(.text)
		cpu/lpc313x/clock.o		(.text .rodata*)
		cpu/lpc313x/init.o		(.text .rodata*)
#ifdef CONFIG_EA3152
		cpu/lpc313x/i2c.o		(.text .rodata*)
#endif
		cpu/lpc313x/timer.o		(.text .rodata*)
		cpu/lpc313x/serial.o		(.text .rodata*)
		cpu/lpc313x/crc32.o		(.text .rodata*)
		board/ea31xx/ea31xx.o		(.text .rodata*)
#ifdef CONFIG_CPU_MMC_BOOT
		cpu/lpc313x/mci.o		(.text .rodata*)
		cpu/lpc313x/dos_partition.o	(.text .rodata*)
#endif
#ifdef CONFIG_CPU_NAND_BOOT
		drivers/mtd/nand/lpc313x_nand.o (.text .rodata*)
		drivers/mtd/nand/nand.o		(.text .rodata*)
		drivers/mtd/nand/nand_base.o	(.text .rodata*)
		drivers/mtd/nand/nand_bbt.o	(.text .rodata*)
		drivers/mtd/nand/nand_ecc.o	(.text .rodata*)
		drivers/mtd/nand/nand_util.o	(.text .rodata*)
		drivers/mtd/nand/nand_ids.o	(.text .rodata*)
#endif
#ifdef CONFIG_CPU_SPI_BOOT
		cpu/lpc313x/spi.o		(.text .rodata*)
		drivers/spi/lpc313x_spi.o	(.text .rodata*)
		drivers/mtd/spi/spi_flash.o	(.text .rodata*)
		drivers/mtd/spi/atmel.o		(.text .rodata*)
#endif
#ifdef CONFIG_CPU_USBDFU_BOOT
		drivers/serial/usbtty.o       	(.text .rodata*)
		drivers/usb/gadget/core.o     	(.text .rodata*)
		drivers/usb/gadget/lpc313x_udc.o (.text .rodata*)
		drivers/usb/gadget/dfu.o      	(.text .rodata*)
		drivers/usb/gadget/ep0.o      	(.text .rodata*)
		lib_generic/circbuf.o         	(.text .rodata*)
#endif
		common/console.o		(.text .rodata*)
		common/dlmalloc.o		(.text .rodata*)
		lib_generic/vsprintf.o		(.text .rodata*)
		lib_generic/div64.o		(.text .rodata*)
		lib_arm/libgcc.a              	(.text .rodata*)
		lib_generic/string.o 		(.text .rodata*)
	}

	. = ALIGN(4);
	.data : { *(.data) }

	. = ALIGN(512);
	__INITIAL_DATA_END  = .;
	.remtext : { *(.text) }

	. = ALIGN(4);
	.rodata : { *(.rodata) }


	. = ALIGN(4);
	.got : { *(.got) }

	. = .;
	__u_boot_cmd_start = .;
	.u_boot_cmd : { *(.u_boot_cmd) }
	__u_boot_cmd_end = .;
__DATA_END      = .;
__image_size    = ((__DATA_END - __IMAGE_START));
__initial_boot_image_size    = (((__INITIAL_DATA_END - __IMAGE_START) + 511) & ~ 511);


	. = ALIGN(4);
	__bss_start = .;
	.bss : { *(.bss) }
	_end = .;
}

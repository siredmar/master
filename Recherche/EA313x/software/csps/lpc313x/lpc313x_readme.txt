$Id:: lpc313x_readme.txt 1911 2009-06-16 00:31:46Z pdurgesh            $

NXP LPC313x Chip Support Package version 0.03

************************************************************************
************************************************************************
* Supported chip peripherals of the CSP
************************************************************************
************************************************************************
This CSP contains driver support for the following chip peripherals:
 * LPC313x USB device (Memory back Mass storage example)
 * LPC313x 10 bit ADC
 * LPC313x Color LCD controller
 * LPC313x Clock Generation Unit (CGU)
 * LPC313x CRC32 driver
 * LPC313x DMA controller (channel allocation driver only)
 * LPC313x Event Router
 * LPC313x I2C driver
 * LPC313x I2S audio
 * LPC313x Interrupt controller
 * LPC313x IPINT/PCM driver
 * LPC313x SD/MMC card controller/Memory card interface driver (MCI)
 * LPC313x PWM
 * LPC313x SPI
 * LPC313x Timer 0/1/2/3 driver
 * LPC313x UART driver
 * LPC313x Watchdog timer driver (WDT)
 * LPC313x GPIO controller (inline functions)
 * LPC313x SysCReg (register access header driver)

************************************************************************
************************************************************************
* Toolchain support
************************************************************************
************************************************************************
* ARM Realview support
************************************************************************
This package works under ARM Realview 3.x.

************************************************************************
* CodeSourcery GCC support
************************************************************************
Support for CodeSourcery GCC has not been fully tested and may
only be partially available.

************************************************************************
* ARM Developer's Suite support
************************************************************************
This package works under Arm Developer's Suite.

************************************************************************
* IAR Embedded Workbench support
************************************************************************
This package does not support IAR Embedded Workbench in this release. 

************************************************************************
* Keil support
************************************************************************
This package works under Keil uVision3.

************************************************************************
************************************************************************
* Known issues
************************************************************************
************************************************************************
- Some examples have issues with Codesourcery toolcahin.

************************************************************************
************************************************************************
* Package history
************************************************************************
************************************************************************
Version 0.03
 - Added USB mass storage example (software\csps\lpc313x\bsps\ea3131\examples\keil_usbmem)
	- This example only works with Keil tool chain
 - Added SD/MMC card read & write capability
	- Enhanced 'sdcard_poll' example to do FAT parsing and load images from card.
	
Version 0.02
 - Fixed CGU clock settings. 
 - Fixed SPI programmer to convert 528 byte page Atmel SPI data flash 
   device to 512 byte page device.
 - Added pre-built Spri_prog.bin and readme.txt for SPI programing.
Version 0.01
 Initial alpha release.

$Id:: lpc313x_readme.txt 8643 2011-11-17 04:52:22Z ing02124            $

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

Version 0.06
 - Following drivers are updated/bug fixed
       - Soft IRQ support included in DMA driver
       - Occcaional packet drop issue fixed in USB driver
 - Following example modified/updated
       - Multiple EPs added in USB bandwidth measurement tests
         (sram_timing)
       - Removed traces of iAP code in EA3152 & EA3131 USB Audio example
 
Version 0.05
 - Following drivers are updated
        - LPC3152 Analog CODEC driver
 - Following example applications are updated
	  - I2S Audio example using SD card example (Recording added)
        - USB Audio class device example (Recording added)
- Following example applications are added
        - SRAM timing example added      

Version 0.04
 - The Examples cane be built for the following targets
        - IRAM target
        - IRAM target with Cache enabled
        - SDRAM target
        - SDRAM target with Cache enabled 
 - Following drivers are updated/bug fixed
        - NAND driver
        - SD card driver
        - I2S driver
        - DMA driver
        - CGU driver
        - UART driver  
 - Following drivers added
	- I2S Clock driver
        - Master I2C driver
        - USB Device driver
        - LPC3152 Analog CODEC driver
        - LPC315X OTP driver
        - LPC315X PSU driver
        - LPC315X RTC driver
        - LPC313X Virtual Memory driver
 - Following example applications are added
	- I2S Audio playback from memory example
        - I2S Audio recording to memory example
        - I2S Audio example using SD card example (playback only)
        - LCD Interface configuration example
        - LCD Display example using SWIM library 
        - Lion Charger Example for LPC3152
        - NAND driver example
        - NAND performance measurement example
        - RTC example for LPC3152
        - SD card example
        - USB Audio class device example (Playback only)
        - USB CDC class Serial example
        - USB Display class example
        - USB HID class example
        - USB Mass Storage Class example using Memory as storage
        - USB Mass Storage Class example using Memory as SD card
        - Watchdog Timer Example      

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

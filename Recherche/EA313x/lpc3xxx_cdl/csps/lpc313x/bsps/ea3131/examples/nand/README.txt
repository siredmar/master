1. Information about nand Example:
======================================
The nand example application demonstrates the usage of NAND Flash controler 
driver functions. The example application demosnstrates the following 
functionalities of NAND Flash driver:
a. Reading Device Information (Device ID, Manufacturer ID, 
   page size, block size, etc)
b. Redaing NAND Flash chip status from chip
c. Reading bad blocks information
d. Marking blocks as bad
e. Erasing NAND blocks
f. Reading data from NAND Flash chips:
   a. In Non-DMA mode
   b. with DMA external flow control enabled
g. Writing data to NAND Flash chips:
   a. In Non-DMA mode
   b. with DMA external flow control enabled
    
The NAND Flash driver is tested on 256MB NAND Flash chip on EA31XX boards.

2. Supported Targets:
=====================
The following target builds are supported for this application.
a. ISRAM 
b. ISRAM with cache enabled
c. SDRAM
d. SDRAM with cache enabled
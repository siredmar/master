1. Information about USB MSC class using SDRAM memory example:
==============================================================
The usbmem example demonstrates the USB Mass storage class capabilities of
LPC315X USB device controller using SDRAM memory as storage. 

Instructions to run the usbmem application:
===========================================
1. Build  usbmem example in CDL 
2. After build \software\csps\lpc313x\bsps\<bsp>\examples\usbmem\usbmem_demo.bin 
   should be created.
3. Connect the EA board to PC using USB cable (connect to the port next to Ethernet 
   connector). This port is a USB-serial bridge and also supplies power to the board.
4. On PC launch terminal applications such as TeraTerm and download the 
   usbmem_demo.bin using binary download. 
5. If you have ULINK debugger you could download the firmware on to board directly 
   from the Keil IDE.
6. After you run the firmware on EA board, connect USB cable between board and PC.
7. The USB Mass storage device drive should be available in PC

2. Supported Targets:
=====================
The following target builds are supported for this application.
a. ISRAM 
b. ISRAM with cache enabled
c. SDRAM
d. SDRAM with cache enabled
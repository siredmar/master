1. Information about USB HID class example:
===========================================
The usbhid example demonstrates the USB HID class capabilities of
LPC313X USB device controller. 

Instructions to run the usbhid application:
===========================================
1. Build  usbhid example in CDL 
2. After build \software\csps\lpc313x\bsps\<bsp>\examples\usbhid\usbhid_demo.bin should
   be created.
3. Connect the EA board to PC using USB cable (connect to the port next to Ethernet 
   connector). This port is a USB-serial bridge and also supplies power to the board.
4. On PC launch terminal applications such as TeraTerm and download the usbhid_demo.bin 
   using binary download. 
5. If you have ULINK debugger you could download the firmware on to board directly 
   from the Keil IDE.
6. After you run the firmware on EA board, connect USB cable between board and PC.
7. The USB HID should be available as LPC HID device

2. Supported Targets:
=====================
The following target builds are supported for this application.
a. ISRAM 
b. ISRAM with cache enabled
c. SDRAM
d. SDRAM with cache enabled
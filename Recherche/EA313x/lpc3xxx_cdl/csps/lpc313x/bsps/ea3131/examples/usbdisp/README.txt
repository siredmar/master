1. WINUSB driver based USB Display example:
===========================================
The usbdisp example demonstrates the Display example using WinUSB driver.

Instructions to run the USBDISP application:
===========================================
1. Build  usbdisp example in CDL 
2. After build \software\csps\lpc313x\bsps\ea3131\examples\usbdisp\usbdisp_demo.bin should
   be created.
3. Connect the EA board to PC using USB cable (connect to the port next to Ethernet 
   connector). This port is a USB-serial bridge and also supplies power to the board.
4. On PC launch terminal applications such as TeraTerm and download the usbdisp_demo.bin 
   using binary download. 
5. If you have ULINK debugger you could download the firmware on to board directly 
   from the Keil IDE.
6. After you run the firmware on EA board, connect USB cable between board and PC.
7. On PC when prompted for the driver, select the driver in 
   \software\csps\lpc313x\bsps\ea3131\examples\usbdisp\install directory for 
   first time
8. Execute the \pcapp\LPCDisplay.exe .NET Framework 4 application and launch the app
9. In application window click "Find LPC device". On the bottom of the form you should 
   see the device status.
10. Once the device is detected, the application will capture part of desktop and 
    stream it to the board. 
11. On screen you will see 2 edit boxes. These are for low bandwidth EP1 bulk 
    loopback transfer. Type some text in topic edit box and click "send". 
    You should see that echo back in bottom edit box.

Notes:
=======
1. The LCDDisplay.exe application is not completely fool proof. So if you remove device 
   after connection it might crash. Also sometimes the display might be little out of 
   sync (wrap around effect), you need to restart both EA board and application
2. The LCDDisplay.exe is created using Visual C# 2010 express tools. This requires 
   .NET Framework 4 for execution. The source code for the application can be 
   downloaded from the website:
   ftp://www.lpclinux.com/lpc313x/cdl/04062010/LPCDisplay.zip

2. Supported Targets:
=====================
The following target builds are supported for this application.
a. ISRAM 
b. ISRAM with cache enabled
c. SDRAM
d. SDRAM with cache enabled
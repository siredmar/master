1. LPC315x Lion charger Demo application
=========================================
This demonstrates the Charging of Battery using Lion Charger 
module present in Analog die on LPC315X.

The Application can be used to charge Lithium Ion battery. 

The battery should be connected to EA3152 board as shown below:
==============================================================
a. The + of battery should be connected to VBAT pin on EA3152 board
b. The - of battery should be connected to GND pin on EA3152 board
c. The neutral of battery should be connected to NTC pin on EA3152 board

2. Instructions to run the li_charger application:
===================================================
a. Build  li_charger example in CDL 
b. After build \software\csps\lpc313x\bsps\<bsp>\examples\li_charger\li_charger_demo.bin 
   should be created
c. Connect the battery to EA3152 board as explained above
d. Connect the EA board to PC using USB cable (connect to the port next to Ethernet 
   connector). This port is a USB-serial bridge and also supplies power to the board.
e. On PC launch terminal applications such as TeraTerm and download the 
   li_charger_demo.bin using binary download. 
f. If you have ULINK debugger you could download the firmware on to board directly 
   from the Keil IDE.
g. Now the application will start charging the battery (It will print info on UART)
h. After the battery is fully charged, the application will come stop

3. Supported Targets:
=====================
The following target builds are supported for this application.
a. ISRAM 
b. ISRAM with cache enabled
c. SDRAM
d. SDRAM with cache enabled

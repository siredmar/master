1. Information about sdcard example:
==========================================
This example application demonstrates the data transfers to/from SD card 
using SD card driver. 

The application is user interactive & user can:
a. FATFS file system operations:
   i.    Mount the file system 
   ii.   Display the files in SD card
   iii.   Copy File - Will copy the file from SD card to SD card 
         (To execute this case, SD card should have file "filecopy.log")
   iv.   File write - The file of about 32KB (write.hex) will be created on SD card
   v.    File read - The file (write.hex) will be read from SD card & contents 
         will be displayed on UART prompt

b. Disk operations:
   i.    Initiliase Block device 
   ii.   Disk read - Read data from the disk 
   ii.   Disk write - Write to disk 

2. Supported Targets:
=====================
The following target builds are supported for this application.
a. ISRAM 
b. ISRAM with cache enabled
c. SDRAM
d. SDRAM with cache enabled
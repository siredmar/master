$Id:: readme.txt 1633 2009-02-06 02:21:00Z pdurgesh                    $

EA3131 board SPI programer version 1.00

Description:
------------
The SPI programmer example can be used to program the Atmel SPI-flash 
with bootable images. The current version of programmer operates only
in 512 byte page mode. The AT45DB321D chip used on EA board comes from
factory in 528 byte page mode. The SPI programmer will blow the OTP fuse
which controls the page mode on AT45DB321D the very first time. If your
application rquires 528 byte page flash devices then don't use this 
programer.

Usage:
------
Compile the program using "make" or the Kiel project project provided.
After project is compiled spi_prog.bin file is created in this 
directory (software\csps\lpc313x\bsps\ea3131\examples\spi_prog). 

1. Set the EA board in UART boot mode.
2. Connect EA board to PC. 
3. On PC configure terminal application (such as TeraTerm Pro) with
   115200-8-n settings. If you are using USB-to-serial bridge port on
   EA board the appropriate COMx port has to be selected. By the time
   USB-2-serial enumerates the bootROM of LPC313x would have transmitted
   the initial string. Hence reset the board using the "reset" button 
   after opening the terminal application. Note, the default installation
   of TeraTerm Pro allows only upto COM4 ports. To increse the number of 
   COM ports accessible by TeraTerm change the following line in TERATERM.INI 
   file present in the installation directory (C:\Program Files\TTERMPRO).

   MaxComPort=4
   to
   MaxComPort=10
   
4. After board is reset the following message should appear on the terminal.
   LPC31xx READY FOR PLAIN IMAGE>
5. Now download spi_prog.bin file to the board using 
	File -> send file...
6. Select spi_prog.bin file in the dialog box. Also select the "Binary" 
   checkbox in the file dailog box.
7. After download the following message should appear
	LPC31xx READY FOR PLAIN IMAGE>
	Download finished
	SPI programer is ready for download>   
8. Now download the actual image file (*.rom) using same "binary send file
   method" described above.
9. Use the lpc313xImgCreator.exe tool provided under tools directory of CDL
   (software\tools) to generate the .rom file for the corresponding demo.bin.
   
   lpc313xImgCreator.exe -pc -o ./ -i demo.bin
   
10. The above command generates the demo.rom file. Select the generated .rom 
   file for programing.


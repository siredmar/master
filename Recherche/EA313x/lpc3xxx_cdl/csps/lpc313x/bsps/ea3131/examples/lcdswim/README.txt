1. Information about lcdswim Example:
======================================
The lcdswim example application demonstrates the usage of LPC swim library 
to display various shapes, texts, image and colors on LCD display.
The example is implemented for EA3131 320 X 240 LCD.
 
The application is user interactive, user can select test options to 
draw on the LCD display: 

'S' : Get size (H & V) of LCD screen
'C' : Clear the LCD screen
'L' : Set Background & Foreground color on LCD screen
'T' : Display Text on LCD screen
'B' : Draw Box on LCD screen
'D' : Draw Diamond on LCD screen
'M' : Multiple SWIM windows on LCD screen

For SDRAM targets, the image will be present in the executable & image 
options are available

2. Supported Targets:
=====================
The following target builds are supported for this application.
a. ISRAM 
b. ISRAM with cache enabled
c. SDRAM
d. SDRAM with cache enabled
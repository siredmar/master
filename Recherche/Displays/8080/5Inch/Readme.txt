Coldtears electronics 5.0" TFT LCD Module 
LCD Resolution 800x480, SSD1963
http://www.coldtears.com/electronics
UTFT Library name: CTE50

UTFT library is written by Henning Karlsen, can be downloaded here:
http://electronics.henningkarlsen.com/

Usage with CTE Mega 2560 shield:
No adjustment in UTFT library needed, just change the LCD module to 
UTFT myGLCD(CTE50,38,39,40,41);

Usage with CTE DUE shield:
1.uncomment "#define CTE_DUE_SHIELD 1" in the HW_ARM_defines.h in the \hardware\arm folder of the UTFT library
2.Change the pinout to : UTFT myGLCD(CTE50,25,26,27,28);

----------------------------------------------------------
The default orientation is:
1. Connector on the right of the screen
2. (0,0) is at the top left corner

Using UTouch (Newbie approach)
1. Adjust "InitTouch(byte orientation = PORTRAIT);" in UTouch.h
2. Paste the following to UTouchCD.h:

#define CAL_X 0x001C4F7FUL
#define CAL_Y 0x00320EC4UL
#define CAL_S 0x0031f1dfUL
3. Insert "y=479-y;" right after y = myTouch.getY(); in the test sketch
4. Run the button test sketch, it will respond to touch correctly.

----------------------------------------------------------
5" TFT LCD Module Notes:

1. The power supply of the LCD is 3.3V. Never apply 5V to the power supply of the LCD. This will damage the LCD permanently.

2. As with other TFT LCD, the IO pins of the LCD is 3.3V. It is not recommend to use 5V IO as data input to the LCD. If it is used, use a level translator to change 5V TLL level to 3.3V TLL level IO, or use resistor to limit the current.

3. The LED-A pin is used to control the LCD backlight. Vcc will turn backlight to maximum level. Gnd will turn off the LCD backlight. PWM signal can be feed into this pin to control LCD brightness. This is not a power supply pin for the LCD backlight

------------------------------------------------------
More interesting products are avaliable in 
Coldtears electronics Ebay store and
http://www.coldtears.com/electronics
coldtearselectronics@gmail.com
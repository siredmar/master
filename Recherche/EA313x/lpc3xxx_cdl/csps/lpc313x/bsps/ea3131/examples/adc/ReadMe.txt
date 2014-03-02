1. Information about adc Example application:
=============================================
The adc example application tests the different ADC modes
      - single scan (Interrupt driven and polled)
      - continous scan (Interrupt driven and polled)

The out messages are displayed in Serial Terminal.

A. Single conversion examples (interrupt mode and polled):
==========================================================
Perform a conversion of the 4 ADC's (one scan) and display the results

B. Continuous scan examples:
===========================
a) Interrupt Mode - Channel 0 and Channel 1:
The ADC is connected to the accelerometer (X and Y axis only)
Continous scan of Channel 0 and 1 is performed until a movement in the 
X-axis or Y-axis is detected. When detected, the continuous scan stops, the
conversion values and the detected direction are displayed.
This is for demo purpose only and not guaranteed to be correctly calibrated.

Jumper configuration:
AIN0 = Accel
AIN1 = Accel

b) Polled Mode - Channel 2:
===========================
The ADC is connected to the potentiometer. Continous scan on Channel 2 is 
performed as long as the A to D result is different from 0x3FF. 
When 0x3FF is detected, conversions stop and the number of conversions 
performed is displayed. 0x3FF assumes that 10-bit resolution is selected.
Rotating the pot clockwise increases the input voltage.

Jumper configuration:
AIN2 = Trim pot

To select the mode, set one of the following to 1 in example code:
==================================================================
#define SINGLE_SCAN_INT			0
#define CONTINUOUS_SCAN_INT		1
#define SINGLE_SCAN_POLLED		0
#define CONTINUOUS_SCAN_POLLED	0

2. Supported Targets:
=====================
The following target builds are supported for this application.
a. ISRAM 
b. ISRAM with cache enabled
c. SDRAM
d. SDRAM with cache enabled 

1. LPC315x Audio Demo application using SD card
================================================
This demonstrates the following Audio capability.

Audio Playback functionality:
-----------------------------
The Audio WAVE files (Raw PCM format only) can be played from the 
SD card. The Audio will be played on CODEC present in the 
Analog Die of LPC315X.

Audio Recording functionality:
------------------------------
The Audio can be recorded by connecting the microphone to the microphone
socket on the EA3152 board. The recorded audio will be stored on SD card as
WAVE files (Raw PCM format)

NOTES:
------
When Audio record test is selected, it will record data from microphone & 
store on SD card. Stop the recording by pressing 's' on the COM port.

2. Supported Targets:
=====================
The following target builds are supported for this application.
a. ISRAM 
b. ISRAM with cache enabled
c. SDRAM
d. SDRAM with cache enabled

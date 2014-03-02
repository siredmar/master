1.USB AUDIO - DEMO
===================

This application will demonstrate, the following

a. Configuring LPC31xx USB Device controller to send/receive ISO endpoint data
b. Playing back and recording audio from the host on to the EA3152 MIC/Headphone


Known-BUG
=========
a. The Audio might go out of sync. at a constant interval of time

To achieve sync.the following clocks has to be tuned...
a. fs Clock     (Word Sampling clock)
b. Bit Clock    (32 * fs Clock)
c. Master Clock (256 * fs Clock)

Since the fractional dividers for Bit Clock and Master clock will not provide the fine resolution required, synchronization between ISO audio stream and I2S playback is still not completed.


NOTE:
=====
Windows 7 always use a fixed frequency to play back the sound file, this frequency can be modified using the following steps

a. In start menu search bars enter "sndvol.exe" and press enter
b. It will open the mixer, click on the speaker icon above the masker volume control slide bar
c. It will open speaker/headphone properties select the "Advanced" tab, and there will be a dropdown menu to select the frequency
d. For optimal playback set the frequency to 48000

2. Supported Targets:
=====================
The following target builds are supported for this application.
a. ISRAM 
b. ISRAM with cache enabled
c. SDRAM
d. SDRAM with cache enabled
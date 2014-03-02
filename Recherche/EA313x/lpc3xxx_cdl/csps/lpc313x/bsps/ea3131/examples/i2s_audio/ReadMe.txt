1. Information about i2s_audio PCM Audio example:
=================================================
This example demonstrates the Audio playback functionality using Audio CODEC. 
This example contains extracted PCM data (in form of array) within the executable. 
Hence the executable generated will not fit in internal ISRAM. 
The default target will generate execuable that will load from external SDRAM. 
Target "cache" will generate execuable that will run from external SDRAM, with cache and 
virtual memory enabled. 

2. Supported Targets:
=====================
The following target builds are supported for this application.
a. SDRAM
b. SDRAM with cache enabled


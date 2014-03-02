1. Information about nand_perf example:
======================================
The NAND performance tests implement the code to measure the performance 
of the NAND operations. 

This test measures the performance for the following operations & 
displayed on the UART prompt:
a. NAND Erase operation
b. NAND read operaton in Non-DMA mode
c. NAND read operation with DMA external enable flow control enabled
b. NAND write operaton in Non-DMA mode
c. NAND write operation with DMA external enable flow control enabled

The timer is used for measuring the time.

2. Supported Targets:
=====================
The following target builds are supported for this application.
a. ISRAM 
b. ISRAM with cache enabled
c. SDRAM
d. SDRAM with cache enabled


 
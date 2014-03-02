SRAM Timing example:
--------------------
The sram_timing example provides bandwidth data for the data transfers for the
specified memory access timings. The user can set the memory access timings 
by selecting the appropriate option in menu. The user can select the data transfer 
operation for which the bandwidth measurment needs to be done.

Memory Access timing settings:
------------------------------
The user set the memory access timings (For Static memory 0) by selecting the 
option '0' from the menu. When this option is selected, the user can set the 
following access parameters:
a. Static Memory Configuration
------------------------------
The user can select the following options:
Select 0 - To set Memory width = 16 bit,Write buffer = Disbaled,Page Mode = disabled
Select 1 - To set Memory width = 16 bit,Write buffer = Enaled,Page Mode = disabled
Select 2 - To set Memory width = 16 bit,Write buffer = Disbaled,Page Mode = Enabled
Select 3 - To set Memory width = 16 bit,Write buffer = Enaled,Page Mode = Enabled
Select 4 - To set Memory width = 8 bit,Write buffer = Disbaled,Page Mode = disabled
Select 5 - To set Memory width = 8 bit,Write buffer = Enaled,Page Mode = disabled
Select 6 - To set Memory width = 8 bit,Write buffer = Disbaled,Page Mode = Enabled
Select 7 - To set Memory width = 8 bit,Write buffer = Enabled,Page Mode = Enabled

b. Write Enable Delay
------------------------------------------
The user can program the delay from the chip select to write enable. The delay can be
programmed from 1 HCLK clock cycle to 16 HCLK clock cycles
Enter 0 - To set delay of 1 HCLK clock cycle
Enter 1 - To set delay of 2 HCLK clock cycles
:
:
Enter f - To set delay of 16 HCLK clock cycles

c. Output Enable Delay
------------------------------------------
The user can program the delay from the chip select/address change to output enable. 
The delay can be programmed from 1 HCLK clock cycle to 16 HCLK clock cycles
Enter 0 - To set delay of 1 HCLK clock cycle
Enter 1 - To set delay of 2 HCLK clock cycles
:
:
Enter f - To set delay of 16 HCLK clock cycles

d. Read access Delay
------------------------------------------
The user can program the delay from the chip select to read access. 
The delay can be programmed from 1 HCLK clock cycle to 16 HCLK clock 
cycles
Enter 0 - To set delay of 1 HCLK clock cycle
Enter 1 - To set delay of 2 HCLK clock cycles
:
:
Enter f - To set delay of 16 HCLK clock cycles

e. Async Page mode sequential access Delay
------------------------------------------
The user can program the delay for async page mode sequential access. 
The delay can be programmed from 1 HCLK clock cycle to 16 HCLK clock 
cycles
Enter 0 - To set delay of 1 HCLK clock cycle
Enter 1 - To set delay of 2 HCLK clock cycles
:
:
Enter f - To set delay of 16 HCLK clock cycles

f. Write access Delay
------------------------------------------
The user can program the delay from the chip select to write access. 
The delay can be programmed from 1 HCLK clock cycle to 16 HCLK clock 
cycles
Enter 0 - To set delay of 1 HCLK clock cycle
Enter 1 - To set delay of 2 HCLK clock cycles
:
:
Enter f - To set delay of 16 HCLK clock cycles
      
g. Number of turn around cycles 
--------------------------------
The user can program the number of turn around cycles. 
The delay can be programmed from 1 HCLK clock cycle to 16 HCLK clock 
cycles
Enter 0 - To set delay of 1 HCLK clock cycle
Enter 1 - To set delay of 2 HCLK clock cycles
:
:
Enter f - To set delay of 16 HCLK clock cycles

h. Wait read delay 
------------------
The user can split the output enable with an inactive OE cycle in 
the middle. The delay can be programmed from 1 HCLK clock cycle to 
15 HCLK clock cycles
Enter 0 - No delay
Enter 1 - To set delay of 1 HCLK clock cycle with inactive OE cycle
:
:
Enter f - To set delay of 15 HCLK clock cycles with inactive OE cycle

i. Select the verify option 
-----------------------------
Enter 0 to disable verify, 1 to enable verify option

j. Select the PLL frequency 
-----------------------------
The user can select from 0 to 34 to select the PLL frequency.
Select 0 - To set PLL to 180MHz
Select 1 - To set PLL to 178MHz
Select 2 - To set PLL to 176MHz
Select 3 - To set PLL to 174MHz
Select 4 - To set PLL to 172MHz
Select 5 - To set PLL to 170MHz
Select 6 - To set PLL to 168MHz
Select 7 - To set PLL to 166MHz
Select 8 - To set PLL to 164MHz
Select 9 - To set PLL to 162MHz
Select 10 - To set PLL to 160MHz
Select 11 - To set PLL to 158MHz
Select 12 - To set PLL to 156MHz
Select 13 - To set PLL to 154MHz
Select 14 - To set PLL to 152MHz
Select 15 - To set PLL to 150MHz
Select 16 - To set PLL to 148MHz
Select 17 - To set PLL to 146MHz
Select 18 - To set PLL to 144MHz
Select 19 - To set PLL to 142MHz
Select 20 - To set PLL to 140MHz
Select 21 - To set PLL to 138MHz
Select 22 - To set PLL to 136MHz
Select 23 - To set PLL to 134MHz
Select 24 - To set PLL to 132MHz
Select 25 - To set PLL to 130MHz
Select 26 - To set PLL to 128MHz
Select 27 - To set PLL to 126MHz
Select 28 - To set PLL to 124MHz
Select 29 - To set PLL to 122MHz
Select 30 - To set PLL to 120MHz
Select 31 - To set PLL to 114MHz
Select 32 - To set PLL to 108MHz
Select 33 - To set PLL to 102MHz
Select 34 - To set PLL to 96MHz

Data transfer test options:
---------------------------
The user can select the following options:
Select 1 - To measure the DMA Burst read transfer bandwidth
Select 2 - To measure the DMA Burst write transfer bandwidth 
Select 3 - To measure the DMA Half word (16-bit)read transfer bandwidth
Select 4 - To measure the DMA Half word (16-bit)write transfer bandwidth
Select 5 - To measure the ARM burst (16-bytes)read transfer bandwidth
Select 6 - To measure the ARM burst (16-bytes)write transfer bandwidth
Select 7 - To measure the ARM Half word (16-bit)read transfer bandwidth
Select 8 - To measure the ARM Half word (16-bit)write transfer bandwidth
Select 9 - To measure the ARM Half word (16-bit)Split read transfer bandwidth
Select a - To measure the ARM Half word (16-bit)Spilt write transfer bandwidth
Select b - To measure the USB transfers bandwidth

Instructions to run the USB transfers bandwidth test:
-----------------------------------------------------
1. Connect USB cable between board and PC
2. On PC when prompted for the driver, select the driver in 
   \BSP_DIR\examples\usbdisp\install directory for first time
3. Execute the \pcapp\LPCUsbTest.exe .NET Framework 4 application
4. In application window, click "Find LPC device" button. On the bottom of the form, 
   you should  see the device status.
5. Once the device is detected, select one of the the following options under 
   the "Test" menu.
    - USB Rx Bandwidth (KB/s)
    - USB Tx Bandwidth (KB/s)
6. Click on the Start Test Button, to start the measurement. The bandwidth data will
   be shown on the on the right side of the selected option. 
   
Notes:
------
1. The LCDUSBTest.exe is created using Visual C# 2010 express tools. This requires 
   .NET Framework 4 for execution. The source code for the application can be 
   downloaded from the website:
   ftp://www.lpclinux.com/lpc313x/cdl/04062010/LPCUSBTest.zip

2. Supported Targets:
=====================
The following target builds are supported for this application.
a. ISRAM 
b. ISRAM with cache enabled

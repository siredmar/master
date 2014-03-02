1. About USBCDC example application:
====================================
The USBCDC project is a demo program for using the NXP LPC31xx Microcontroller.
It demonstrates an USB Virtual COM port based on a Windows USB host 
driver (usbser.sys).

The PC will install a virtual COM port on the PC (see Driver Installation).
After installation an additional port "LPC31xx USB VCom Port(COMx)"
can be found under System/Hardware/Device Manager/Ports(COM&LPT).
Number "x" is not fixed as different PC configuration may have different 
"x" displayed on the device manager. The USB host driver assigns "x" 
dynamically based on the existing COM port configuration of the system.

2. Testing USBCDC example applications:
=======================================   
Testing the USB Virtual COM port with serial cable:
1. Open two Hyperterminal windows. 
   a. One with "LPC31xx USB VCom Port(COMx)" 
   b. One with "Communications Port (COMy)"
2. Connect PC port COMy to serial port on the board.
3. Connect PC port COMx to USB Virtual COM port
4. With this, Data from COMy can be echoed on "COMx" and visa versa. 
   So, this is bi-directional communication between the physical COM
   port y on the board and the virtual COM port COMx on host PC.    

3. Driver Installation:
========================
     "Welcome to the Found New Hardware Wizard" appears
     - select 'No, not this time'
     - press  'Next'

     - select 'Install from a list or specific location (Advanced)'
     - press  'Next'

     - select 'Search for the best driver in these locations'
     - check  'include this location in the serach'
     - set to </NXPMCU/SOFTWARE/CSPS/lpc313x/bsps/ea3131/examples/usbcdc> folder
     - press 'Next'


     "Hardware Installation" appears
     "has not passed Windows Logo testing..."
     - press 'Continue Anyway'

     "Completing the Found New Hardware Wizard" appears
     - press 'Finish'

4. Supported Targets:
=====================
The following target builds are supported for this application.
a. ISRAM 
b. ISRAM with cache enabled
c. SDRAM
d. SDRAM with cache enabled
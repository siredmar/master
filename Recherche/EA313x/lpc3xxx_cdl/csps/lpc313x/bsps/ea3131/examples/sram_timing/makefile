######################################################################## 
# $Id:: makefile 6421 2011-02-10 04:55:30Z ing02124                    $
# 
# Project: Example makefile
#
# Notes:
#     This makefile generates an image that will load in SDRAM at
#     address 0x80000000. The image can be loaded and run with the
#     stage 1 loader or with a debugger.
#
# Description: 
#  Makefile
# 
######################################################################## 
# Software that is described herein is for illustrative purposes only  
# which provides customers with programming information regarding the  
# products. This software is supplied "AS IS" without any warranties.  
# NXP Semiconductors assumes no responsibility or liability for the 
# use of the software, conveys no license or title under any patent, 
# copyright, or mask work right to the product. NXP Semiconductors 
# reserves the right to make changes in the software without 
# notification. NXP Semiconductors also make no representation or 
# warranty that such application will be suitable for the specified 
# use without further testing or modification. 
########################################################################

EXECNAME    = sram_test

# Path to sources, required to create additional objects
VPATH = $(NXPMCU_SOFTWARE)/ip/usbd/source

# Additional objects
ADDOBJS := usbcore.o  # USB Device stack

# Include the main makefile for compiling examples
include $(NXPMCU_SOFTWARE)/csps/$(CSP)/bsps/$(BSP)/examples/common/makefile.ex

# USB stack includes
CFLAGS  += -I$(NXPMCU_SOFTWARE)/ip/usbd/include -I$(BSP_DIR)/examples/sram_timing

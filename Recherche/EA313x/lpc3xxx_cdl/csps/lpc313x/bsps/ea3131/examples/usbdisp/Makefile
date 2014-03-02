######################################################################## 
# $Id:: Makefile 2382 2009-10-27 22:12:38Z pdurgesh                    $
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

EXECNAME    = usbdisp_demo

# Path to sources, required to create additional objects
VPATH = $(NXPMCU_SOFTWARE)/ip/usbd/source

# Additional objects
ADDOBJS := usbcore.o  # USB Device stack

# Include the main makefile for compiling examples
include $(NXPMCU_SOFTWARE)/csps/$(CSP)/bsps/$(BSP)/examples/common/makefile.ex

# IMPORTAT NOTE: Some settings like CFLAGS are assigned using := inside 
# general rule files present inside "makerules" directory, hence they 
# must be added after including makefile.ex, if not it will be igonred

# Additional include directories
CFLAGS  += -I$(NXPMCU_SOFTWARE)/ip/usbd/include -I$(BSP_DIR)/examples/usbdisp

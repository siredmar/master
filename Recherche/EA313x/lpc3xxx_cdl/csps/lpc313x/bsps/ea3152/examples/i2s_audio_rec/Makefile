######################################################################## 
# $Id:: Makefile 6422 2011-02-10 07:20:47Z ing02124                    $
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

EXECNAME    = i2s_audio_rec

# Include the main makefile for compiling examples
include $(NXPMCU_SOFTWARE)/csps/$(CSP)/bsps/$(BSP)/examples/common/makefile.ex

# IMPORTAT NOTE: Some settings like CFLAGS are assigned using := inside 
# general rule files present inside "makerules" directory, hence they 
# must be added after including makefile.ex, if not it will be igonred

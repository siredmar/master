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

EXECNAME    = usbmem_demo

########################################################################
#
# Pick up the default build rules 
#
########################################################################

include $(NXPMCU_SOFTWARE)/makerule/$(CSP)/make.$(CSP).$(TOOL)

########################################################################
#
# Pick up the assembler and C source files in the directory  
#
########################################################################
include $(NXPMCU_SOFTWARE)\makerule\common\make.rules.ftypes


VPATH = $(BSP_DIR)/startup:$(NXPMCU_SOFTWARE)/ip/usbd/source

# startup files
ADDOBJS     += ea3131_startup_entry.o ea3131_startup.o
# USB stack files
ADDOBJS += usbcore.o mscuser.o

# USB stack includes
CFLAGS  += -I$(NXPMCU_SOFTWARE)/ip/usbd/include -I$(BSP_DIR)/examples/usbmem

########################################################################
#
# GNU compiler/linker specific stuff
#
########################################################################

ifeq ($(TOOL), gnu)
MEXT        =.map
MAPFILE     =$(EXECNAME)
CFLAGS      +=-gdwarf-2
LDSCRIPT    =../common/ldscript_iram_$(TOOL).ld
ADDOBJS     += 
endif

########################################################################
#
# Arm compiler/linker specific stuff
#
# ARM examples enter via __main and are linked at address 0x80000000
#
########################################################################

ifeq ($(TOOL), ads)
MEXT        =.map
MAPFILE     =$(EXECNAME)
CFLAGS      +=-g
AFLAGS      +=-g
LDSCRIPT    =../common/ldscript_iram_ads.ld
ADDOBJS     += pad.o
endif
ifeq ($(TOOL), rvw)
MEXT        =.map
MAPFILE     =$(EXECNAME)
CFLAGS      +=-g
AFLAGS      +=-g
LDSCRIPT    =../common/ldscript_iram_ads.ld
ADDOBJS     += pad.o
endif
ifeq ($(TOOL), keil)
MEXT        =.map
MAPFILE     =$(EXECNAME)
CFLAGS      +=-g
AFLAGS      +=-g
LDSCRIPT    =../common/ldscript_iram_ads.ld
ADDOBJS     += pad.o
endif

########################################################################
#
# IAR compiler/linker specific stuff
#
########################################################################

ifeq ($(TOOL), iar)
MEXT        =.map
MAPFILE     =$(EXECNAME)
CFLAGS      +=
AFLAGS      +=
LDESC       = --config
LDSCRIPT    =../common/ldscript_iram_$(TOOL).ld
ADDOBJS     += 
endif

########################################################################
#
# Rules to build the executable 
#
########################################################################

default: $(OBJS) $(ADDOBJS) lpc_libs
	$(LD) $(OBJS) $(ADDOBJS) $(LDFLAGS) $(LK) $(SCAN) $(MAP) \
	$(MAPFILE)$(MEXT) $(LDESC) $(LDSCRIPT) -o $(EXECNAME)$(EXT)
	$(ELFTOBIN) $(EXECNAME)$(EXT) $(EFLTBINOPT) $(EXECNAME).bin

cleanall: realclean lpc_clean

########################################################################
#
# Pick up the compiler and assembler rules
#
########################################################################

include $(NXPMCU_SOFTWARE)/makerule/common/make.rules.build





######################################################################## 
# $Id:: makefile.ex 6055 2010-12-24 08:17:25Z ing03005                 $
# 
# Project: Example makefile
#
# Notes:
#     This makefile generates an image that will load in ISRAM at
#     address 0x11029000. The image can be loaded and run with the
#     boot ROM or with a debugger. It also can generate images that
#     can be loaded from SDRAM at address 0x31000000.
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
include $(NXPMCU_SOFTWARE)/makerule/common/make.rules.ftypes

VPATH += $(BSP_DIR)/startup

ADDOBJS     += ea3152_startup_entry.o ea3152_startup.o


########################################################################
#
# Default settings for Linker script and other options
#
########################################################################
LDSSUFFIX   = _$(TOOL)
LDSAREA = _iram
LDSCACHE =
REXECNAME = $(EXECNAME)$(CACHESUFFIX)
MAPFILE = $(REXECNAME)

########################################################################
#
# GNU compiler/linker specific stuff
#
########################################################################

ifeq ($(TOOL), gnu)
MEXT        =.map
CFLAGS      +=-gdwarf-2
ADDOBJS     += 
endif

########################################################################
#
# Arm compiler/linker specific stuff
#
# ARM Examples should start at c_entry and will be called from startup
#
########################################################################

ifeq ($(TOOL), ads)
MEXT        =.map
CFLAGS      +=-g
AFLAGS      +=-g
LDSSUFFIX    = _ads
ADDOBJS     += pad.o
endif
ifeq ($(TOOL), rvw)
MEXT        =.map
CFLAGS      +=-g
AFLAGS      +=-g
LDSSUFFIX    = _ads
ADDOBJS     += pad.o
endif
ifeq ($(TOOL), keil)
MEXT        =.map
CFLAGS      +=-g
AFLAGS      +=-g
LDSSUFFIX    = _ads
ADDOBJS     += pad.o
endif

########################################################################
#
# IAR compiler/linker specific stuff
#
########################################################################

ifeq ($(TOOL), iar)
MEXT        =.map
CFLAGS      +=
AFLAGS      +=
LDESC       = --config
ADDOBJS     += 
endif

LDSCRIPT    = $(BSP_DIR)/examples/common/ldscript$(LDSAREA)$(LDSSUFFIX)$(CACHESUFFIX).ld
########################################################################
#
# Rules to build the executable 
#
########################################################################

default: $(OBJS) $(ADDOBJS) lpc_libs
	$(LD) $(OBJS) $(ADDOBJS) $(LDFLAGS) $(LK) $(SCAN) $(MAP) \
	$(MAPFILE)$(MEXT) $(LDESC) $(LDSCRIPT) -o $(REXECNAME)$(EXT)
	$(ELFTOBIN) $(REXECNAME)$(EXT) $(EFLTBINOPT) $(REXECNAME).bin

cleanall: realclean lpc_clean

########################################################################
#
# Rules to build the cached executable 
#
########################################################################
cache: CACHESUFFIX = _cache
cache: default

########################################################################
#
# Rules to build the SDRAM executable 
#
########################################################################
sdram: LDSAREA = _sdram
sdram: CFLAGS += -DTGT_SDRAM
sdram: default

########################################################################
#
# Rules to build the SDRAM Cached executable 
#
########################################################################
sdram_cache: LDSAREA = _sdram
sdram_cache: CACHESUFFIX = _cache
sdram_cache: CFLAGS += -DTGT_SDRAM
sdram_cache: default

########################################################################
#
# Pick up the compiler and assembler rules
#
########################################################################

include $(NXPMCU_SOFTWARE)/makerule/common/make.rules.build

#
# Makefile for APEX loader
#
#   Copyright (C) 2004 Marc Singer
#
#   This program is free software; you can redistribute it and/or
#   modify it under the terms of the GNU General Public License
#   version 2 as published by the Free Software Foundation.
#   Please refer to the file debian/copyright for further details.
#

# npe-interface is in a world of it's own.  It has to build with
#   headers from the Intel Access Library which are incompatible with
#   header in the project.  So, it is isolated from the loader and
#   wrapper calls are defined to allow the header domain of the this
#   one source file to be separate from the header domain of the rest
#   of the application.

CFLAGS_npe-interface.o\
  += -I-\
     -Isrc/mach-ixp42x/_intel/ixp400_xscale_sw/src/include \
     -Isrc/mach-ixp42x/_intel/ixp_osal/include\
     -Isrc/mach-ixp42x/_intel/ixp_osal/os/linux/include/core\
     -Isrc/mach-ixp42x/_intel/ixp_osal/os/linux/include/modules/bufferMgt\
     -Isrc/mach-ixp42x/_intel/ixp_osal/os/linux/include/platforms/ixp400\
     -Isrc/mach-ixp42x/_intel/ixp_osal/include/modules/ioMem\
     -Isrc/mach-ixp42x/_intel/ixp_osal/include/modules/bufferMgt\
     -Isrc/mach-ixp42x/_intel/ixp_osal/include/platforms/ixp400\
     -Isrc/mach-ixp42x/_intel/ixp_osal/include/platforms/ixp400/bufferMgt\
     -Isrc/mach-ixp42x/_intel/linux/include\
     -Isrc/mach-ixp42x/_intel/linux/include/linux\
     -Isrc/mach-ixp42x/_intel/linux/include/asm/arch\
     -Isrc/mach-ixp42x/_intel/ixp400_xscale_sw/npeLib

obj-y := serial.o env.o cpuinfo.o
obj-y += initialize.o timer.o pci.o
obj-$(CONFIG_MMU) += initialize-sdram.o

obj-x$(CONFIG_MACH_NSLU2)$(CONFIG_SPINNER) += spinner-nslu2.o
obj-x$(CONFIG_MACH_NSLU2)$(CONFIG_VARIATIONS) += variation-nslu2.o
obj-x$(CONFIG_MACH_FSG3)$(CONFIG_VARIATIONS) += variation-fsg3.o
obj-$(CONFIG_INTERRUPTS)	+= exception_vectors.o

#obj-$(CONFIG_NPE)		+= npe.o npe-interface.o

#obj-$(CONFIG_DRIVER_IXP4XX_NPE_ETH) += drv-npe.o

obj-y				+= cmd-reset.o

obj-y += $(obj-xyy)

#libs-$(CONFIG_NPE) += _intel/lib/libnpeDl.a
#newlibs += $(obj)/_intel/lib/libnpeDl.a

#libs-$(CONFIG_NPE) := _intel/

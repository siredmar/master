######################################################################## 
# $Id:: makefile 4929 2010-09-15 22:17:33Z usb10131                    $
# 
# Project: Standard compile makefile
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

include $(NXPMCU_SOFTWARE)\makerule\$(CSP)\make.$(CSP).$(TOOL)

########################################################################
#
# Pick up the assembler and C source files in the directory  
#
########################################################################
include $(NXPMCU_SOFTWARE)\makerule\common\make.rules.ftypes
AFLAGS +=-I..\Include
CFLAGS +=-I..\Include

########################################################################
#
# Build the library
#
########################################################################

$(TARGET_GEN_LIB) : .vias $(OBJS) $(GEN_LIB_DIR)
	$(ECHO) "creating" $(BSP) "board support package library" 
	$(AR) $@ $(OBJS)

$(GEN_LIB_DIR):
	$(MKDIR) $(GEN_LIB_DIR)	

# delete all targets this Makefile can make
lib_clean:
	-@$(RM) $(TARGET_GEN_LIB)

# delete all targets this Makefile can make and all built libraries
# linked in
lib_realclean: 
	-@$(RMDIR) $(GEN_LIB_DIR)
	
clean: lib_clean
realclean: lib_realclean

########################################################################
#
# Compile the code base 
#
########################################################################

include $(NXPMCU_SOFTWARE)\makerule\common\make.rules.build

.PHONY: all lib_clean lib_realclean




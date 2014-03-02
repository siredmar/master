######################################################################## 
# $Id:: makefile 59 2007-11-02 23:21:55Z durgeshp                      $
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
# Create a list of codebases to be compiled 
#
########################################################################

TARGETS		=gen csp bsp
TARGETS_CLN	=gen_clean csp_clean bsp_clean

########################################################################
#
# Pick up the default build rules 
#
########################################################################

include $(NXPMCU_SOFTWARE)\makerule\$(CSP)\make.$(CSP).$(TOOL)

########################################################################
#
# Build the CSP and GEN libraries 
#
########################################################################

libs: $(TARGETS)

abl: gen

gen: 
	$(ECHO) "Building" $(GEN) "support package source ->" \
	$(GEN_SRC_DIR)
	@$(MAKE) TOOL=$(TOOL) -C $(GEN_SRC_DIR)
	$(ECHO) "done"

csp: 
	$(ECHO) "Building" $(CSP) "chip support package source ->" \
	$(CSP_SRC_DIR)
	@$(MAKE) TOOL=$(TOOL) -C $(CSP_SRC_DIR)
	$(ECHO) "done"

bsp: 
	$(ECHO) "Building" $(BSP) "board support package source ->" \
	$(BSP_SRC_DIR)
	@$(MAKE) TOOL=$(TOOL) -C $(BSP_SRC_DIR)
	$(ECHO) "done"

########################################################################
#
# Clean the CSP and GEN libraries 
#
########################################################################

clean: $(TARGETS_CLN)

abl_clean: gen_clean

gen_clean: 
	$(ECHO) "Cleaning" $(GEN) "support package ->" $(GEN_SRC_DIR)
	@$(MAKE) TOOL=$(TOOL) -C $(GEN_SRC_DIR) clean -s

csp_clean: 
	$(ECHO) "Cleaning" $(CSP) "chip support package ->" $(CSP_SRC_DIR)
	@$(MAKE) TOOL=$(TOOL) -C $(CSP_SRC_DIR) clean -s

bsp_clean: 
	$(ECHO) "Cleaning" $(BSP) "board support package ->" $(BSP_SRC_DIR)
	@$(MAKE) TOOL=$(TOOL) -C $(BSP_SRC_DIR) clean -s

# delete all targets this Makefile can make and all built libraries
# linked in
realclean: clean
	@$(MAKE) TOOL=$(TOOL) -C $(GEN_SRC_DIR) realclean
	@$(MAKE) TOOL=$(TOOL) -C $(CSP_SRC_DIR) realclean 
	@$(MAKE) TOOL=$(TOOL) -C $(BSP_SRC_DIR) realclean 

distclean: realclean
	@$(RMREC) "*.o"
	@$(RMREC) "*.elf"
	@$(RMREC) "*.exe"
	@$(RMREC) "*~"
	@$(RMREC) "*.map"
	@$(RMREC) "*.lst"
	@$(RMREC) "*.axf"
	@$(RMREC) "*.bin"
	@$(RMREC) "*.dbo"
	@$(RMREC) "*.dla"
	@$(RMREC) "*.dnm"
	@$(RMREC) "*.srec"
	@$(RMREC) ".depend"
	@$(RMREC) "*.wrn"

.PHONY: libs abl gen csp bsp clean abl_clean gen_clean csp_clean bsp_clean realclean distclean

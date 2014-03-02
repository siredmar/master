######################################################################## 
# $Id:: make.lpc313x.ads 1179 2008-09-11 18:01:10Z pdurgesh            $
# 
# Project: LPC315X toolset rules for ARM ADS
# 
# Description: 
#     Make rules for the arm compiler suite
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

include $(NXPMCU_SOFTWARE)/makerule/common/make.rules.environment

CC        =armcc
CCP       =armcpp
AS        =armasm
LD        =armlink
AR        =armar -r -s
CFLAGS    =-O1 -Otime -g -cpu ARM9EJ-S
CFLAGS    +=-I$(CSP_INC_DIR) -I$(BSP_INC_DIR) -I$(GEN_INC_DIR) 
AFLAGS    =-cpu ARM9EJ-S 
AFLAGS    +=-I$(CSP_INC_DIR) -I$(BSP_INC_DIR) -I$(GEN_INC_DIR)
LDFLAGS   =-remove -info totals -armlib  
LK        =-scan $(TARGET_CSP_LIB)
LK        +=-scan $(TARGET_BSP_LIB)
LK        +=-scan $(TARGET_GEN_LIB)
MAP       =-map -list
LDESC     =-scatter
ENTRY     =-entry     
BIN       =-bin
EXT       =.axf
LEXT      =
ELFTOREC  =fromelf -m32
ELFTOBIN  =fromelf -bin
EFLTBINOPT=-output
REC       =.srec

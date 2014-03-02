 ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 
 ; $Id:: ldscript_iram_ads_cache.ld 5973 2010-12-22 04:17:57Z ing03005 $
 ; 
 ; Project: EA3152 startup code linker file for ARM ADS
 ; 
 ; Description: 
 ;     This file implements linker script for the EA3152 board when
 ;     used with the EA3152 startup code. The image will be linked to
 ;     execute out of address 0x11029000.
 ;
 ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;  
 ; Software that is described herein is for illustrative purposes only  
 ; which provides customers with programming information regarding the  
 ; products. This software is supplied "AS IS" without any warranties.  
 ; NXP Semiconductors assumes no responsibility or liability for the 
 ; use of the software, conveys no license or title under any patent, 
 ; copyright, or mask work right to the product. NXP Semiconductors 
 ; reserves the right to make changes in the software without 
 ; notification. NXP Semiconductors also make no representation or 
 ; warranty that such application will be suitable for the specified 
 ; use without further testing or modification. 
 ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 

ISRAM 0x11129000
{
	; The executable region starts at the load region
    	ER_RO +0
        {
                ea3152_startup_entry.o (STARTUP, +FIRST) 
                * (+RO) 
        }
        ER_RW +0
        {
                * (+RW)
        }
        ER_ZI +0 OVERLAY       
        {
 			pad.o (+RW)
            * (+ZI)
        }

}

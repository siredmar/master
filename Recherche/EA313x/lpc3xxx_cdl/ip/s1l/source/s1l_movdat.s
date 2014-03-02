;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; $Id:: s1l_movdat.s 871 2008-07-07 22:36:49Z wellsk                   $
; 
; Project: Memmory test data move operation
;
; Description:
;     64 byte aligned data move operation used for benchmarking
;
; Notes:
;     This version of the file is for the Realview 3.x toolset.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
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

    export mmove

	PRESERVE8
	AREA MTEST, CODE

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; Function: mmove
;
; Purpose: Perform aligned memory test
;
; Description:
;     A simple LDM/STM copy loop.
;
; Parameters:
;     r0 : source, 32-bit aligned
;     r1 : destination, 32-bit aligned
;     r2 : bytes (/16 granularity)
;
; Outputs; NA
;
; Returns: NA
;
; Notes: NA
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
mmove
    STMFD sp!, {r0 - r6, lr}
	cmp   r2, #0
	BEQ   lstdone ; Test should be skipped, no size

nloop
	LDMIA r0!, {r3-r6}
	STMIA r1!, {r3-r6}
	LDMIA r0!, {r3-r6}
	STMIA r1!, {r3-r6}
	LDMIA r0!, {r3-r6}
	STMIA r1!, {r3-r6}
	LDMIA r0!, {r3-r6}
	STMIA r1!, {r3-r6}
	SUBS  r2, r2, #64
	BNE   nloop

lstdone
	LDMFD sp!, {r0 - r6, pc}

    END

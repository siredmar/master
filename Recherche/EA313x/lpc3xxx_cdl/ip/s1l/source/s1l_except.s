;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; $Id:: s1l_except.s 871 2008-07-07 22:36:49Z wellsk                   $
; 
; Project: Exception handler
;
; Description:
;     Saves exception data, switches to SVC mode, and jumps to;
;     exception handler
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

    export arm9_exchand

    extern excdata
    extern trap_exception

MODE_SVC    EQU 0x013
I_MASK      EQU 0x080
F_MASK      EQU 0x040

	PRESERVE8
	AREA EXCEPT, CODE

; Exception handler simply saves some items in a global variable
arm9_exchand
    ; Get base address of storage area
    STMFD sp!, {r11-r12}
    LDR   r11, =excdata

	; Save current and previous status and
    MRS   r12, cpsr
    STR   r12, [r11], #4
    MRS   r12, spsr
    STR   r12, [r11], #4

	; Saved address of exception
    STR   lr, [r11], #4

	; Save current registers
    STR   r0, [r11], #4
    STR   r1, [r11], #4
    STR   r2, [r11], #4
    STR   r3, [r11], #4
    STR   r4, [r11], #4
    STR   r5, [r11], #4
    STR   r6, [r11], #4
    STR   r7, [r11], #4
    STR   r8, [r11], #4
    STR   r9, [r11], #4
    STR   r10, [r11], #4
    LDMFD sp!, {r0-r1}
    STR   r0, [r11], #4
    STR   r1, [r11], #4

	; Switch back to SVC mode and jump to handler

    MOV   r1, #F_MASK ; Enable IRQ
    ORR   r0, r1, #MODE_SVC
    MSR   cpsr_cxsf, r0
	B     trap_exception

    END

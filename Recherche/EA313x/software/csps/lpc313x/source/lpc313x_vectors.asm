/*;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; $Id:: lpc313x_vectors.asm 1180 2008-09-11 19:32:40Z pdurgesh         $
; 
; Project: LPC313x interrupt and exception vectors
;
; Description:
;     Interrupt and exception handler vector layout used by the
;     interrupt driver, exception functions, and startup code. This
;     block is placed at the start of the ARM memory region at address
;     0x00000000 or 0xFFFF0000 (if high vectors are enabled).
;
; Notes:
;     This version of the file is used with the GNU Pro toolset.
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
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;*/

    .text
    .code 32
    .align 2

    .global lpc313x_reset_vector
    .global vec_reset_handler
    .global vec_undefined_handler
    .global vec_swi_handler
    .global vec_prefetch_handler
    .global vec_abort_handler
    .global vec_irq_handler
    .global vec_fiq_handler

    .global lpc313x_irq_handler
    .global g_irq_func_ptrs

.EQU INTC_BASE_ADDR, 0x60000000 /* Base address of interrupt controller */
.EQU IRQ_VEC_OFF, 0x100         /* Offset to IRQ vectored status  */
.EQU FIQ_VEC_OFF, 0x104         /* Offset to FIQ vectored status  */

/*;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; Function: Basic interrupt and exception jump block
;
; Purpose: Block that defines the interrupt and exception jump points
;
; Description:
;     On a bootable image that is being built, this area should be
;     linked to address 0x00000000. This area may be used with the
;     interrupt and exception drivers when installing handlers and
;     routers. For each interrupt and exception that gets routed
;     here, the indirect PC value of the handler function is loaded
;     to jump to that function.
;
; Parameters: NA
;
; Outputs; NA
;
; Returns: NA
;
; Notes: NA
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;*/
lpc313x_reset_vector:
    LDR  pc, [pc, #24]          /* Get address of Reset handler */
    LDR  pc, [pc, #24]          /* Get address of Undefined handler */
    LDR  pc, [pc, #24]          /* Get address of SWI handler */
    LDR  pc, [pc, #24]          /* Get address of Prefetch handler */
    LDR  pc, [pc, #24]          /* Get address of Abort handler */
    NOP                         /* Reserved */
    LDR  pc, [pc, #20]          /* Get address of IRQ handler */
    LDR  pc, [pc, #20]          /* Get address of FIQ handler */

vec_reset_handler:
    .word lpc313x_reset_handler

vec_undefined_handler:
    .word lpc313x_undefined_handler

vec_swi_handler:
    .word lpc313x_swi_handler

vec_prefetch_handler:
    .word lpc313x_prefetch_handler

vec_abort_handler:
    .word lpc313x_abort_handler

vec_irq_handler:
    .word lpc313x_irq_handler

vec_fiq_handler:
    .word lpc313x_fiq_handler

/*;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;
; Function: lpc313x_reset_handler
;
; Purpose:
;   Default reset handler
; 
; Processing:
;   Loop forever
;
; Parameters: None
;
; Outputs:  None
;
; Returns: Nothing
;
; Notes:
;     The board startup code does not use this handler! However, when
;     the interrupt and exception table is installed, the reset handler
;     will be updated to this handler.
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;*/
lpc313x_reset_handler:
    B    lpc313x_reset_handler

/*;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;
; Function: lpc313x_undefined_handler
;
; Purpose:
;   Handle the undefined exception
; 
; Processing:
;   Loop forever
;
; Parameters: None
;
; Outputs:  None
;
; Returns: Nothing
;
; Notes: None
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;*/
lpc313x_undefined_handler:
    B    lpc313x_undefined_handler

/*;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;
; Function: lpc313x_swi_handler
;
; Purpose:
;   Handle the software interrupt
; 
; Processing:
;   Loop forever
;
; Parameters: None
;
; Outputs:  None
;
; Returns: Nothing
;
; Notes: None
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;*/
lpc313x_swi_handler:
    B    lpc313x_swi_handler

/*;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;
; Function: lpc313x_prefetch_handler
;
; Purpose:
;   Handle the prefetch abort exception
; 
; Processing:
;   Loop forever
;
; Parameters: None
;
; Outputs:  None
;
; Returns: Nothing
;
; Notes: None
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;*/
lpc313x_prefetch_handler:
    B    lpc313x_prefetch_handler

/*;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;
; Function: lpc313x_abort_handler
;
; Purpose:
;   Handle the abort exception
; 
; Processing:
;   Loop forever
;
; Parameters: None
;
; Outputs:  None
;
; Returns: Nothing
;
; Notes: None
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;*/
lpc313x_abort_handler:
    B    lpc313x_abort_handler

/*;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;
; Function: lpc313x_irq_handler
;
; Purpose:
;   Handle the IRQ interrupt
; 
; Processing:
;   Loop forever
;
; Parameters: None
;
; Outputs:  None
;
; Returns: Nothing
;
; Notes: None
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;*/
lpc313x_irq_handler:
    SUB    lr, lr, #4                 /* Get return address */
    STMFD  sp!, {r0-r12, lr}          /* Save registers */

    /* Read the IRQ vector status registers */
    LDR    r2, =INTC_BASE_ADDR
    LDR    r3, [r2, #IRQ_VEC_OFF]
    
    /* Assuming TABLE_ADDR in Interrupt vector_0 is set 0 during init. */
    /* If not then add masking instruction below. uncomment the following.
	; AND r3, r3, #0x7FC
	*/
	
    LDR    r0, =g_irq_func_ptrs    /* Get address of jump table */
    LDR    r0, [r0, r3, LSR #1]  /* Get handler address. Add by interrupt offset */
    CMP    r0, #0                /* Is handler address NULL? */
    BEQ    int_exit              /* If null, the exit */
    MOV    lr, pc                /* Will return to int_exit */
    BX     r0                    /* Jump to handler */

int_exit:
    LDMFD  sp!, {r0-r12, pc}^    /* Restore registers and exit */

/*;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;
; Function: lpc313x_fiq_handler
;
; Purpose:
;   Handle the FIQ interrupt
; 
; Processing:
;   Loop forever
;
; Parameters: None
;
; Outputs:  None
;
; Returns: Nothing
;
; Notes: None
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;*/
lpc313x_fiq_handler:
    SUB    lr, lr, #4                 /* Get return address */
    STMFD  sp!, {r0-r12, lr}          /* Save registers */

    /* Read the IRQ vector status registers */
    LDR    r2, =INTC_BASE_ADDR
    LDR    r3, [r2, #FIQ_VEC_OFF]
    
    /* Assuming TABLE_ADDR in Interrupt vector_0 is set 0 during init. */
    /* If not then add masking instruction below. uncomment the following.
	; AND r3, r3, #0x7FC
	*/
	
    LDR    r0, =g_irq_func_ptrs    /* Get address of jump table */
    LDR    r0, [r0, r3, LSR #1]  /* Get handler address. Add by interrupt offset */
    CMP    r0, #0                /* Is handler address NULL? */
    BEQ    fiq_exit              /* If null, the exit */
    MOV    lr, pc                /* Will return to int_exit */
    BX     r0                    /* Jump to handler */

fiq_exit:
    LDMFD  sp!, {r0-r12, pc}^    /* Restore registers and exit */

    .end

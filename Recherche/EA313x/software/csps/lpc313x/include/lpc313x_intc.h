/***********************************************************************
* $Id:: lpc313x_intc.h 1180 2008-09-11 19:32:40Z pdurgesh             $
*
* Project: LPC313X interrupt controller definitions
*
* Description:
*     This file contains the structure definitions and manifest
*     constants for the LPC313X chip family component:
*         Interrupt Controller
*
***********************************************************************
* Software that is described herein is for illustrative purposes only
* which provides customers with programming information regarding the
* products. This software is supplied "AS IS" without any warranties.
* NXP Semiconductors assumes no responsibility or liability for the
* use of the software, conveys no license or title under any patent,
* copyright, or mask work right to the product. NXP Semiconductors
* reserves the right to make changes in the software without
* notification. NXP Semiconductors also make no representation or
* warranty that such application will be suitable for the specified
* use without further testing or modification.
**********************************************************************/

#ifndef LPC313X_INTC_H
#define LPC313X_INTC_H

#include "lpc_types.h"
#include "lpc313x_chip.h"

#ifdef __cplusplus
extern "C"
{
#endif

/***********************************************************************
* Interrupt controller Register Structures
**********************************************************************/

/* Interrupt Controller Module Register Structure */
typedef volatile struct
{
  volatile UNS_32 irq_prio_mask; // IRQ priority threshold
  volatile UNS_32 fiq_prio_mask; // FIQ priority threshold
  volatile UNS_32 dummy0[62];
  volatile UNS_32 irq_vec;    // IRQ vector
  volatile UNS_32 fiq_vec;    // FIQ vector
  volatile UNS_32 dummy1[62];
  volatile UNS_32 pending;    // Status of interrupt request   1.. 31
  volatile UNS_32 dummy2[63];
  volatile UNS_32 features;   // Interrupt Controller config features
  volatile UNS_32 dummy3[63];
  volatile UNS_32 request[30];
  volatile UNS_32 dummy4[737];
  volatile UNS_32 modId;
} INTC_REGS_T;

/***********************************************************************
* Interrupt source defines & enumerations
**********************************************************************/
/* Interrupt source enumerations */
typedef enum
{
  /* main interrupts */
  IRQ_EVT_ROUTER0 = 1,      /*interrupts from Event router 0*/
  IRQ_EVT_ROUTER1,          /*interrupts from Event router 1*/
  IRQ_EVT_ROUTER2,          /*interrupts from Event router 2*/
  IRQ_EVT_ROUTER3,          /*interrupts from Event router 3*/
  IRQ_TIMER0,               /*Timer 0 IRQ */
  IRQ_TIMER1,               /*Timer 1 IRQ */
  IRQ_TIMER2,               /*Timer 2 IRQ */
  IRQ_TIMER3,               /*Timer 3 IRQ */
  IRQ_ADC,                  /*10bit ADC irq*/
  IRQ_UART,                 /*UART irq */
  IRQ_I2C0,                 /*I2C 0 IRQ */
  IRQ_I2C1,                 /*I2C 1 IRQ */
  IRQ_I2S0_OUT,             /*I2S 0 out IRQ */
  IRQ_I2S1_OUT,             /*I2S 1 out IRQ */
  IRQ_I2S0_IN,              /*I2S 0 IN IRQ */
  IRQ_I2S1_IN,              /*I2S 1 IN IRQ */
  IRQ_RSVD1,
  IRQ_LCD = 18,             /*LCD irq */
  IRQ_SPI_SMS,              /*SPI SMS IRQ */
  IRQ_SPI_TX,               /*SPI Transmit IRQ */
  IRQ_SPI_RX,               /*SPI Receive IRQ */
  IRQ_SPI_OVR,              /*SPI overrun IRQ */
  IRQ_SPI,                  /*SPI interrupt IRQ */
  IRQ_DMA,                  /*DMA irq */
  IRQ_NAND_FLASH,           /*NAND flash irq */
  IRQ_MCI,                  /*MCI irq */
  IRQ_USB,                  /*USB irq */
  IRQ_ISRAM0,               /*ISRAM0 irq */
  IRQ_ISRAM1,               /*ISRAM1 irq */
  IRQ_END_OF_INTERRUPTS = IRQ_ISRAM1
} INTERRUPT_SOURCE_T;

/***********************************************************************
 * Priority mask register definitions
 **********************************************************************/
#define INTC_PRIO_LIMIT_MSK   0xFF

/***********************************************************************
 * Vector register definitions
 **********************************************************************/
#define INTC_VEC_TBL_ADR(n)   ((n) & ~(0x3FF))
#define INTC_VEC_GET_INDEX(n) (((n) >> 3) & 0xFF)
#define INTC_VEC_INDEX_MSK    _SBF(3, 0xFF)

/***********************************************************************
 * pending register definitions
 **********************************************************************/
#define INTC_PEND_GET(n)      _BIT(((n) & (0x1F)))

/***********************************************************************
 * Request register definitions
 **********************************************************************/
#define INTC_REQ_PEND         _BIT(31)
#define INTC_REQ_SET_SWINT    _BIT(30)
#define INTC_REQ_CLR_SWINT    _BIT(29)
#define INTC_REQ_WE_PRIO_LVL  _BIT(28)
#define INTC_REQ_WE_TARGET    _BIT(27)
#define INTC_REQ_WE_ENABLE    _BIT(26)
#define INTC_REQ_WE_ACT_LOW   _BIT(25)

#define INTC_REQ_ACT_LOW      _BIT(17)
#define INTC_REQ_ENABLE       _BIT(16)
#define INTC_REQ_TARGET(n)    _SBF(8, ((n) & 0x3F))
#define INTC_REQ_PRIO_LVL(n)  ((n) & 0xFF)
#define INTC_REQ_TARGET_IRQ   (INTC_REQ_WE_TARGET)
#define INTC_REQ_TARGET_FIQ   (INTC_REQ_WE_TARGET | _BIT(8))


/**********************************************************************
* Macro to access WDT registers
**********************************************************************/
#define INTC        ((INTC_REGS_T*)INTC_BASE)

#ifdef __cplusplus
}
#endif

#endif /* LPC313X_INTC_H */

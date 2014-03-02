/***********************************************************************
 * $Id:: lpc313x_evt_driver.h 1388 2008-12-01 19:20:06Z pdurgesh       $
 *
 * Project: LPC313X Event router driver
 *
 * Description:
 *     This file contains driver support for the LPC313X Event router
 *  block.
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
 *********************************************************************/

#ifndef LPC313X_EVT_DRIVER_H
#define LPC313X_EVT_DRIVER_H

#include "lpc313x_evt_router.h"

#ifdef __cplusplus
extern "C"
{
#endif
/***********************************************************************
 * Event router driver enums & structures
 **********************************************************************/
/* External interrupt type enumerations */
typedef enum
{
  ACTIVE_LOW,
  ACTIVE_HIGH,
  FALLING_EDGE,
  RISING_EDGE
} EVENT_TYPE_T;

typedef struct
{
  EVENT_T pin;          /* which pin or internal signal */
  EVENT_TYPE_T sense;   /* Event type */
  EVT_VEC_OUT_T vector; /* on which vector the event is routed. */
  PFV func_ptr;         /* interrupt routine. */
} EVENT_SETUP_T;


/***********************************************************************
 * Event router driver functions
 **********************************************************************/
/* Initialize the Event router */
void evtr_int_initialize(void);

/* Install an event */
BOOL_32 evtr_int_install_handler(EVENT_SETUP_T* evt_setup);

/* Change senstivity of the event */
BOOL_32 evtr_int_set_sense(EVENT_T source, EVENT_TYPE_T sense);

/* Enable an interrupt */
void evtr_int_enable(EVENT_T source);

/* Disable an interrupt */
void evtr_int_disable(EVENT_T source);

/* Check to see if a unmasked interrupt is pending */
BOOL_32 evtr_int_pending(EVENT_T source);

/* Check to see if a raw interrupt is pending */
BOOL_32 evtr_int_raw_pending(EVENT_T source);

/* Check to see if an interrupt is enabled */
BOOL_32 evtr_int_enabled(EVENT_T source);

/* Clear a pending (latched) interrupt */
BOOL_32 evtr_int_clear(EVENT_T source);





#ifdef __cplusplus
}
#endif

#endif /* LPC313X_EVT_DRIVER_H */

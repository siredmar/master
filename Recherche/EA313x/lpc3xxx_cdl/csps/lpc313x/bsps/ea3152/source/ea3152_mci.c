/***********************************************************************
 * $Id:: ea3152_mci.c 6164 2011-01-12 06:40:58Z ing02124               $
 *
 * Project: EA3131 board functions
 *
 * Description:
 *     This file contains driver support for various EA3131 board
 *     functions.
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

#include "lpc313x_sysreg.h"
#include "lpc313x_evt_driver.h"
#include "lpc313x_mci_driver.h"
#include "lpc313x_ioconf_driver.h"
#include "lpc313x_timer_driver.h"

/***********************************************************************
 * Private functions
 **********************************************************************/

/***********************************************************************
 *
 * Function: card_detect_event_handler
 *
 * Purpose: SD/MMC card detect handler function
 *
 * Processing:
 *     See function
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 **********************************************************************/
static void card_detect_event_handler(void)
{
  volatile SYSCREG_REGS_T *pSysRegBase = SYS_REGS;

  if (evtr_int_pending(EVT_mDAO_BCK0))
  {
    /* clear event */
    evtr_int_clear(EVT_mDAO_BCK0);

    // clear bit1 in SYSCREG_SD_MMC_CFG to trigger card detect interrupt of the MMC driver
    pSysRegBase->sd_mmc_cfg = 0; //presence of card
  }
  return;
}

/***********************************************************************
 * Public functions
 **********************************************************************/

/***********************************************************************
 *
 * Function: mci_init
 *
 * Purpose: Initializes SD/MMC controller for card detection
 *
 * Processing:
 *     See function
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: None
 *
 **********************************************************************/
void mci_init(void)
{
  EVENT_SETUP_T evt_setup;

  /* enable power to SD card slot by setting I2STX_DATA0 low. */
  gpio_set_outpin_low(IOCONF_MUX1, 5);

  /* wait atleast 10msec for card to stablize */
  timer_wait_ms(NULL, 10);

  /* Set the I2STX_BCK0 as input for card detect */
  gpio_set_as_input((IOCONF_BLOCK_T)0, 7);

  /* Prepare to register event router interrupt handler for card detect */
  evtr_int_initialize();
  evt_setup.pin = EVT_mDAO_BCK0;
  evt_setup.sense = FALLING_EDGE;

  /* route the interrupt on intout 2 */
  evt_setup.vector = EVT_VEC_ROUTER2;

  /* Handler */
  evt_setup.func_ptr = (PFV)card_detect_event_handler;
  evtr_int_install_handler((EVENT_SETUP_T *)&evt_setup);
  evtr_int_enable(EVT_mDAO_BCK0);
}


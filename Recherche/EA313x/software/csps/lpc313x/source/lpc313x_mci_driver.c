/***********************************************************************
* $Id:: lpc313x_mci_driver.c 3557 2010-05-20 00:31:30Z usb10131       $
*
* Project: LPC313x SD/MMC/SDIO controller driver
*
* Description:
*     This file contains driver support for the LPC313x SD/MMC/SDIO
*     controller.
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
#include <stdio.h>
#include <string.h>

#include "lpc_sdmmc.h"
#include "lpc313x_sysreg.h"
#include "lpc313x_ioconf_driver.h"
#include "lpc313x_cgu_driver.h"
#include "lpc313x_timer_driver.h"
#include "lpc313x_mci_driver.h"

#define USE_WIDE    /* Allow WIDE mode */
#define US_TIMEOUT 1000000 /* give 1 atleast 1 sec for the card to respond */
#define MS_ACQUIRE_DELAY	(10)/* inter-command acquire oper condition delay in msec*/
#define INIT_OP_RETRIES   10  /* initial OP_COND retries */
#define SET_OP_RETRIES    200 /* set OP_COND retries */


/* global instance of the current card*/
static MCI_CARD_INFO_T g_card_info;

/***********************************************************************
 * MCI driver private functions
 **********************************************************************/

/***********************************************************************
 *
 * Function: prv_card_acquired
 *
 * Purpose: Checks whether card is acquired properly or not.
 *
 * Processing:
 *     Check if a valid CID data is present for the card.
 *
 * Parameters:
 *     pdev: Pointer to card info structure
 *
 * Outputs: None
 *
 * Returns: TRUE is card is enumerated else return FALSE.
 *
 * Notes: None
 *
 **********************************************************************/
static BOOL_32 prv_card_acquired(MCI_CARD_INFO_T* pdev)
{
  return (pdev->cid[0] != 0);
}

/***********************************************************************
 *
 * Function: prv_get_bits
 *
 * Purpose: Helper function to get a bit field withing multi-word
 *  buffer. Used to get fields with-in CSD & EXT-CSD structures.
 *
 * Processing:
 *     Based bit position computes the 32 word index with buffer and
 *  extracts the bit field value. The function takes care of bit fieds
 *  which cross the word boundary.
 *
 * Parameters:
 *     start: Start position of the bit field
 *     end  : Start position of the bit field
 *     data : Pointer to buffer
 *
 * Outputs: None
 *
 * Returns: The bit field value.
 *
 * Notes:
 *  The function assumes max. of 32 bi per bit field. For bit fields
 *  above 32-bit shouldn't use this function.
 *
 **********************************************************************/
static UNS_32 prv_get_bits(int start, int end, UNS_32* data)
{
  UNS_32 v;
  UNS_32 i = end >> 5;
  UNS_32 j = start & 0x1f;

  if (i == (start >> 5))
    v = (data[i] >> j);
  else
    v = ((data[i] << (32 - j)) | (data[start >> 5] >> j));

  return (v & ((1 << (end - start + 1)) - 1));
}

/***********************************************************************
 *
 * Function: prv_clear_all
 *
 * Purpose: Clears the FIFOs, response and data, and the interrupt status.
 *
 * Processing:
 *     clears the FIFOs, response and data, and the interrupt status.
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
static void prv_clear_all(void)
{
  /* reset all blocks */
  MCI->ctrl |= MCI_CTRL_FIFO_RESET;
  /* wait till resets clear */
  while (MCI->ctrl & MCI_CTRL_FIFO_RESET);

  /* Clear interrupt status */
  MCI->rintsts = 0xFFFFFFFF;
}

/***********************************************************************
 *
 * Function: prv_send_cmd
 *
 * Purpose: Function to send command to Card interface unit (CIU).
 *
 * Processing:
 *     Send command to Card interface unit (CIU).
 *
 * Parameters:
 *     cmd  : Command with all flags set.
 *     arg  : Argument for the command
 *
 * Outputs: None
 *
 * Returns: TRUE if times-out.
 *
 * Notes: None
 *
 **********************************************************************/
static int prv_send_cmd(UNS_32 cmd, UNS_32 arg)
{
  volatile int tmo = 50;
  volatile int delay;
  /* set command arg reg*/
  MCI->cmdarg = arg;
  MCI->cmd = MCI_CMD_START | cmd;

  /* poll untill command is accepted by the CIU */
  while (--tmo && (MCI->cmd & MCI_CMD_START))
  {
    if (tmo & 1)
      delay = 50;
    else
      delay = 18000;

    while (--delay > 1);
  }

  return (tmo < 1) ? 1 : 0;
}

/***********************************************************************
 *
 * Function: prv_set_clock
 *
 * Purpose: Function to set speed of the clock going to card.
 *
 * Processing:
 *     Sets speed of the clock going to external card.
 *
 * Parameters:
 *     speed  : Clock speed.
 *
 * Outputs: None
 *
 * Returns: None.
 *
 * Notes: None
 *
 **********************************************************************/
static void prv_set_clock(UNS_32 speed)
{
  /* compute SD/MMC clock dividers */
  UNS_32 mmc_clk = cgu_get_clk_freq(CGU_SB_SD_MMC_CCLK_IN_ID);
  UNS_32 div = ((mmc_clk / speed) + 2) >> 1;

  if ((div == MCI->clkdiv) && MCI->clkena)
    return; /* requested speed is already set */

  /* disable clock */
  MCI->clkena = 0;
  MCI->clksrc = 0;

  /* inform CIU */
  prv_send_cmd(MCI_CMD_UPD_CLK | MCI_CMD_PRV_DAT_WAIT, 0);

  /* set clock to desired speed */
  MCI->clkdiv = div;
  /* inform CIU */
  prv_send_cmd(MCI_CMD_UPD_CLK | MCI_CMD_PRV_DAT_WAIT, 0);

  /* enable clock */
  MCI->clkena = MCI_CLKEN_ENABLE;
  /* inform CIU */
  prv_send_cmd(MCI_CMD_UPD_CLK | MCI_CMD_PRV_DAT_WAIT, 0);
}

/***********************************************************************
 *
 * Function: prv_pull_response
 *
 * Purpose: Function to retrieve command response.
 *
 * Processing:
 *     Retrieves a command response.
 *
 * Parameters:
 *     pdev: Pointer to card info structure
 *     length  : the length of the expected response, in bits.
 *
 * Outputs: None
 *
 * Returns: None.
 *
 * Notes: None
 *
 **********************************************************************/
static void prv_pull_response(MCI_CARD_INFO_T* pdev, int length)
{
  /* on this chip response is not a fifo so read all 4 regs */
  pdev->response[0] = MCI->resp0;
  pdev->response[1] = MCI->resp1;
  pdev->response[2] = MCI->resp2;
  pdev->response[3] = MCI->resp3;
}

/***********************************************************************
 *
 * Function: prv_pull_data
 *
 * Purpose: Function to read data from the FIFO.
 *
 * Processing:
 *     Reads the data from fifo.
 *
 * Parameters:
 *     pv: Pointer to buffer
 *     cnt  : the length of the expected data, in bytes.
 *
 * Outputs: None
 *
 * Returns: None.
 *
 * Notes: None
 *
 **********************************************************************/
static int prv_pull_data(MCI_CARD_INFO_T* pdev, UNS_8* pv, int cnt)
{
  int i = 0;
  int fcnt;

  while (i < cnt)
  {
    fcnt = MCI_STS_GET_FCNT(MCI->status) << 2;
    /* LPC313x maps address above 0x18000100 to FIFO to support
    burst reading. So let's use memcpy to speedup read. Since we
    always pass 4byte aligned address and also multiple of 4 count
    let's hope memcpy of C runtime does LDMs.*/
    if (fcnt)
      memcpy(pv + i, (void*)&MCI->data, fcnt);

    i += fcnt;

  }
  return i;
}

/***********************************************************************
 *
 * Function: prv_push_data
 *
 * Purpose: Function to write data to the FIFO.
 *
 * Processing:
 *     Writes the data to fifo.
 *
 * Parameters:
 *     pv: Pointer to buffer
 *     cnt  : the length of the expected data, in bytes.
 *
 * Outputs: None
 *
 * Returns: None.
 *
 * Notes: None
 *
 **********************************************************************/
static void prv_push_data(const UNS_8* pv, UNS_32 cb)
{
  int i = 0;
  int fcnt;

  while (i < cb)
  {
    fcnt = MCI_FIFO_SZ - (MCI_STS_GET_FCNT(MCI->status) << 2);
    /* LPC313x maps address above 0x18000100 to FIFO to support
    burst writing. So let's use memcpy to speedup write. Since we
    always pass 4byte aligned address and also multiple of 4 count
    let's hope memcpy of C runtime does STMs (store multiples).*/
    memcpy((void*)&MCI->data, pv + i, fcnt);
    i += fcnt;
  }
}

/***********************************************************************
 *
 * Function: prv_wait_for_completion
 *
 * Purpose: Polls for command completion.
 *
 * Processing:
 *     Appends additional error flags to the status bits requested by
 *  the caller. And invokes the busy poll routine or wait_func set by
 *  the application.
 *
 * Parameters:
 *     pdev : Pointer to card info structure
 *     bit  : Status bits to poll for command completion.
 *
 * Outputs: None
 *
 * Returns: Status of the command.
 *
 * Notes: None
 *
 **********************************************************************/
static UNS_32 prv_wait_for_completion(MCI_CARD_INFO_T* pdev, UNS_32 bits)
{
  UNS_32 status = 0;
  int tmo_count = US_TIMEOUT;

  /* also check error conditions */
  bits |= MCI_INT_EBE | MCI_INT_SBE | MCI_INT_HLE
          | MCI_INT_RTO | MCI_INT_RCRC | MCI_INT_RESP_ERR;

  if (bits & MCI_INT_DATA_OVER)
    bits |= MCI_INT_FRUN | MCI_INT_HTO | MCI_INT_DTO
            | MCI_INT_DCRC;

  if (pdev->wait_func == NULL)
  {
    /* do busy polling when wait_func is not set*/
    do
    {
      timer_wait_us(NULL, 1);
      status = MCI->rintsts;

      if (--tmo_count < 1)
      {
        break;
      }
    }
    while ((status  & bits) == 0);
    /* set time out flag for driver timeout also */
    status |= ((tmo_count < 1) ? MCI_INT_RTO : 0);
  }
  else
  {
    /* call wait function set by application */
    status = pdev->wait_func(pdev, bits);
  }

  return status;
}

/***********************************************************************
 *
 * Function: prv_process_csd
 *
 * Purpose: Function to process the CSD & EXT-CSD of the card.
 *
 * Processing:
 *     Computes the card paramaters such as size, read block length,
 * 	 no. of blocks etc. based on cards CSD & EXT-CSD response.
 *
 * Parameters:
 *     pdev: Pointer to card info structure
 *
 * Outputs: None
 *
 * Returns: None.
 *
 * Notes: None
 *
 **********************************************************************/
static void prv_process_csd(MCI_CARD_INFO_T* pdev)
{
  INT_32 status = 0;
  INT_32 c_size = 0;
  INT_32 c_size_mult = 0;
  INT_32 mult = 0;

  /* compute block length based on CSD response */
  pdev->block_len = 1 << prv_get_bits(80, 83, pdev->csd);

  if ((pdev->card_type & CARD_TYPE_HC) &&
      (pdev->card_type & CARD_TYPE_SD))
  {
    /* See section 5.3.3 CSD Register (CSD Version 2.0) of SD2.0 spec
    an explanation for the calculation of these values
    */
    c_size = prv_get_bits(48, 63, (UNS_32*)pdev->csd) + 1;
    pdev->blocknr = c_size << 10; /* 512 byte blocks */
  }
  else
  {
    /* See section 5.3 of the 4.1 revision of the MMC specs for
    an explanation for the calculation of these values
    */
    c_size = prv_get_bits(62, 73, (UNS_32*)pdev->csd);
    c_size_mult = prv_get_bits(47, 49, (UNS_32*)pdev->csd); //csd_c_size_mult ();
    mult = 1 << (c_size_mult + 2);
    pdev->blocknr = (c_size + 1) * mult;
    /* adjust blocknr to 512/block */
    if (pdev->block_len > MMC_SECTOR_SIZE)
      pdev->blocknr = pdev->blocknr * (pdev->block_len >> 9);

    /* get extended CSD for newer MMC cards CSD spec >= 4.0*/
    if (((pdev->card_type & CARD_TYPE_SD) == 0) &&
        (prv_get_bits(122, 125, (UNS_32*)pdev->csd) >= 4))
    {
      /* put card in trans state */
      status = mci_execute_command(pdev, CMD_SELECT_CARD, pdev->rca << 16, 0);
      /* set block size and byte count */
      MCI->blksiz = MMC_SECTOR_SIZE;
      MCI->bytcnt = MMC_SECTOR_SIZE;
      /* send EXT_CSD command */
      status = mci_execute_command(pdev, CMD_SEND_EXT_CSD, 0, MCI_INT_CMD_DONE
                                   | MCI_INT_DATA_OVER | MCI_INT_RXDR);

      if ((status & MCI_INT_ERROR) == 0)
      {
        /* read 512bytes EXT-CSD data */
        prv_pull_data(pdev, (UNS_8*)pdev->ext_csd, MMC_SECTOR_SIZE);

        /* check EXT_CSD_VER is greater than 1.1 */
        if ((pdev->ext_csd[48] & 0xFF) > 1)
          pdev->blocknr = pdev->ext_csd[53]; /* bytes 212:215 represent sec count */

        /* switch to 52MHz clock if card type is set to 1 or else set to 26MHz */
        if ((pdev->ext_csd[49] & 0xFF) == 1)
        {
          /* for type 1 MMC cards high speed is 52MHz */
          pdev->speed = MMC_HIGH_BUS_MAX_CLOCK;
        }
        else
        {
          /* for type 0 MMC cards high speed is 26MHz */
          pdev->speed = MMC_LOW_BUS_MAX_CLOCK;
        }
      }
    }
  }

  pdev->device_size = pdev->blocknr << 9; /* blocknr * 512 */
}

/***********************************************************************
 *
 * Function: prv_set_trans_state
 *
 * Purpose: Puts current selected card in trans state
 *
 * Processing:
 *     Puts current selected card in trans state
 *
 * Parameters:
 *     pdev: Pointer to card info structure
 *
 * Outputs: None
 *
 * Returns: Error if unable to put card in trans state.
 *
 * Notes: None
 *
 **********************************************************************/
static int prv_set_trans_state(MCI_CARD_INFO_T* pdev)
{
  UNS_32 status;

  /* get current state of the card */
  status = mci_execute_command(pdev, CMD_SEND_STATUS, pdev->rca << 16, 0);
  if (status & MCI_INT_RTO)
  {
    /* unable to get the card state. So return immediatly. */
    return _ERROR;
  }
  /* check card state in response */
  status = R1_CURRENT_STATE(pdev->response[0]);
  switch (status)
  {
    case SDMMC_STBY_ST:
      /* put card in 'Trans' state */
      status = mci_execute_command(pdev, CMD_SELECT_CARD, pdev->rca << 16, 0);
      if (status != 0)
      {
        /* unable to put the card in Trans state. So return immediatly. */
        return _ERROR;
      }
      break;
    case SDMMC_TRAN_ST:
      /*do nothing */
      break;
    default:
      /* card shouldn't be in other states so return */
      return _ERROR;
  }

#if defined (USE_WIDE)
  if (pdev->card_type & CARD_TYPE_SD)
  {
    mci_execute_command(pdev, CMD_SD_SET_WIDTH, 2, 0);  /* SD, 4 bit width */
    /* if positive response */
    MCI->ctype = MCI_CTYPE_4BIT;
  }
#endif

  /* set block length */
  MCI->blksiz = MMC_SECTOR_SIZE;
  status = mci_execute_command(pdev, CMD_SET_BLOCKLEN, MMC_SECTOR_SIZE, 0);

  return _NO_ERROR;
}

/***********************************************************************
 * MCI driver public functions
 **********************************************************************/

/***********************************************************************
 *
 * Function: mci_execute_command
 *
 * Purpose: Function to execute a command.
 *
 * Processing:
 *     Send the requested command to the card and retrieve the response.
 *
 * Parameters:
 *     pdev: Pointer to card info structure
 *     cmd  : Command with all flags set.
 *     arg  : Argument for the command
 *     wait_status  : Status bits to poll for command completion.
 *
 * Outputs: None
 *
 * Returns: Zero on success. Else returns the status register on error.
 *
 * Notes: None
 *
 **********************************************************************/
INT_32 mci_execute_command(MCI_CARD_INFO_T* pdev,
                           UNS_32 cmd,
                           UNS_32 arg,
                           UNS_32 wait_status)
{
  /* if APP command there are 2 stages */
  int step = (cmd & CMD_BIT_APP) ? 2 : 1;
  INT_32 status = 0;
  UNS_32 cmd_reg = 0;

  if (!wait_status)
    wait_status = (cmd & CMD_MASK_RESP) ? MCI_INT_CMD_DONE : MCI_INT_DATA_OVER;

  /* Clear the interrupts & FIFOs*/
  if (cmd & CMD_BIT_DATA)
    prv_clear_all();

  while (step)
  {
    prv_set_clock((cmd & CMD_BIT_LS) ? SD_MMC_ENUM_CLOCK : pdev->speed);

    /* Clear the interrupts */
    MCI->rintsts = 0xFFFFFFFF;

    switch (step)
    {
      case 1:     /* Execute command */
        cmd_reg = ((cmd & CMD_MASK_CMD) >> CMD_SHIFT_CMD)
                  | ((cmd & CMD_BIT_INIT)  ? MCI_CMD_INIT : 0)
                  | ((cmd & CMD_BIT_DATA)  ? (MCI_CMD_DAT_EXP | MCI_CMD_PRV_DAT_WAIT) : 0)
                  | (((cmd & CMD_MASK_RESP) == CMD_RESP_R2) ? MCI_CMD_RESP_LONG : 0)
                  | ((cmd & CMD_MASK_RESP) ? MCI_CMD_RESP_EXP : 0)
                  | ((cmd & CMD_BIT_WRITE)  ? MCI_CMD_DAT_WR : 0)
                  | ((cmd & CMD_BIT_STREAM) ? MCI_CMD_STRM_MODE : 0)
                  | ((cmd & CMD_BIT_BUSY) ? MCI_CMD_STOP : 0)
                  | ((cmd & CMD_BIT_AUTO_STOP)  ? MCI_CMD_SEND_STOP : 0)
                  | MCI_CMD_START
                  ;
        /* wait for previos data finsh for select/deselect commands */
        if (((cmd & CMD_MASK_CMD) >> CMD_SHIFT_CMD) == MMC_SELECT_CARD)
        {
          cmd_reg |= MCI_CMD_PRV_DAT_WAIT;
        }

        /* wait for command to be accepted by CIU */
        if (prv_send_cmd(cmd_reg, arg) == 0)
          --step;
        break;

      case 0:
        return 0;

      case 2:      /* APP prefix */
        cmd_reg = MMC_APP_CMD
                  | MCI_CMD_RESP_EXP /* Response is status */
                  | ((cmd & CMD_BIT_INIT)  ? MCI_CMD_INIT : 0)
                  | MCI_CMD_START
                  ;
        if (prv_send_cmd(cmd_reg, pdev->rca << 16) == 0)
          --step;
        break;
    }

    /* wait for command response*/
    status = prv_wait_for_completion(pdev, wait_status);

    /* We return an error if there is a timeout, even if we've fetched
    a response */
    if (status & MCI_INT_ERROR)
      return status;

    if (status & MCI_INT_CMD_DONE)
    {
      switch (cmd & CMD_MASK_RESP)
      {
        case 0:
          break;
        case CMD_RESP_R1:
        case CMD_RESP_R3:
          prv_pull_response(pdev, 48);
          break;
        case CMD_RESP_R2:
          prv_pull_response(pdev, 136);
          break;
      }
    }
  }
  return 0;
}


/***********************************************************************
 *
 * Function: mci_acquire
 *
 * Purpose: Function to enumerate the SD/MMC/SDHC/MMC+ cards.
 *
 * Processing:
 *     This function perform card identification process described in
 *  MMC 4.2 and SD 2.0 specification. The function uses "state" variable
 *  to track the identification process.
 *
 *  For SD/SDHC cards the "state" transitions from:
 *  0 -> 1 -> 2 -> 3 -> 4 -> 5 -> 6 -> 7
 *
 *  For MMC/ MMC+ cards the "state" transitions from:
 *  0 -> 1 -> 10 -> 11 -> 12 -> 13 -> 14 -> 15 -> 16 -> 17
 *
 * Parameters:
 *     devid: Pointer to card info structure
 *
 * Outputs: None
 *
 * Returns: None.
 *
 * Notes: None
 *
 **********************************************************************/
void mci_acquire(MCI_CARD_INFO_T* pdev)
{
  int status;
  int tries = 0;
  UNS_32 ocr = OCR_VOLTAGE_RANGE_MSK;
  UNS_32 r;
  int state = 0;
  UNS_32 command = 0;
  /* preserve wait_func */
  MCI_CMD_WAIT_FUNC_T temp = pdev->wait_func;

  /* clear card struct */
  memset(pdev, 0, sizeof(MCI_CARD_INFO_T));
  /* restore wait_func */
  pdev->wait_func = temp;

  /* clear card type */
  MCI->ctype = 0;

  /* we could call board specific card detect routine here */

  /* set high speed for the card as 20MHz */
  pdev->speed = MMC_MAX_CLOCK;

  status = mci_execute_command(pdev, CMD_IDLE, 0, MCI_INT_CMD_DONE);

  while (state < 100)
  {
    switch (state)
    {
      case 0:     /* Setup for SD */
        /* check if it is SDHC card */
        status = mci_execute_command(pdev, CMD_SD_SEND_IF_COND, SD_SEND_IF_ARG, 0);
        if (!(status & MCI_INT_RTO))
        {

          /* check response has same echo pattern */
          if ((pdev->response[0] & SD_SEND_IF_ECHO_MSK) == SD_SEND_IF_RESP)
            /* it is SD 2.0 card so indicate we are SDHC capable*/
            ocr |= OCR_HC_CCS;
        }

        ++state;
        command = CMD_SD_OP_COND;
        tries = INIT_OP_RETRIES;
        /* assume SD card */
        pdev->card_type |= CARD_TYPE_SD;
        /* for SD cards high speed is 25MHz */
        pdev->speed = SD_MAX_CLOCK;

        break;

      case 10:      /* Setup for MMC */
        /* start fresh for MMC crds */
        pdev->card_type &= ~CARD_TYPE_SD;
        status = mci_execute_command(pdev, CMD_IDLE, 0, MCI_INT_CMD_DONE);
        command = CMD_MMC_OP_COND;
        tries = INIT_OP_RETRIES;
        ocr |= OCR_HC_CCS;
        ++state;
        /* for MMC cards high speed is 20MHz */
        pdev->speed = MMC_MAX_CLOCK;
        break;

      case 1:
      case 11:
        status = mci_execute_command(pdev, command, 0, 0);
        if (status & MCI_INT_RTO)
          state += 9;		/* Mode unavailable */
        else
          ++state;
        break;

      case 2:			/* Initial OCR check  */
      case 12:
        ocr = pdev->response[0] | (ocr & OCR_HC_CCS);
        if (ocr & OCR_ALL_READY)
          ++state;
        else
          state += 2;
        break;

      case 3:			/* Initial wait for OCR clear */
      case 13:
        while ((ocr & OCR_ALL_READY) && --tries > 0)
        {
          timer_wait_ms(NULL, MS_ACQUIRE_DELAY);
          status = mci_execute_command(pdev, command, 0, 0);
          ocr = pdev->response[0] | (ocr & OCR_HC_CCS);
        }
        if (ocr & OCR_ALL_READY)
          state += 7;
        else
          ++state;
        break;

      case 14:
        /* for MMC cards set high capacity bit */
        ocr |= OCR_HC_CCS;
      case 4:     /* Assign OCR */
        tries = SET_OP_RETRIES;
        ocr &= OCR_VOLTAGE_RANGE_MSK | OCR_HC_CCS;	/* Mask for the bits we care about */
        do
        {
          timer_wait_ms(NULL, MS_ACQUIRE_DELAY);
          status = mci_execute_command(pdev, command, ocr, 0);
          r = pdev->response[0];
        }
        while (!(r & OCR_ALL_READY) && --tries > 0);
        if (r & OCR_ALL_READY)
        {
          /* is it high capacity card */
          pdev->card_type |= (r & OCR_HC_CCS);
          ++state;
        }
        else
          state += 6;

        break;

      case 5:     /* CID polling */
      case 15:
        status = mci_execute_command(pdev, CMD_ALL_SEND_CID, 0, 0);
        memcpy(pdev->cid, pdev->response, 16);
        ++state;
        break;

      case 6:     /* RCA send, for SD get RCA */
        status = mci_execute_command(pdev, CMD_SD_SEND_RCA, 0, 0);
        pdev->rca = (pdev->response[0]) >> 16;
        ++state;
        break;
      case 16:      /* RCA assignment for MMC set to 1 */
        pdev->rca = 1;
        status = mci_execute_command(pdev, CMD_MMC_SET_RCA, pdev->rca << 16, 0);
        ++state;
        break;

      case 7:
      case 17:
        status = mci_execute_command(pdev, CMD_SEND_CSD, pdev->rca << 16, 0);
        memcpy(pdev->csd, pdev->response, 16);
        state = 100;
        break;

      default:
        state += 100; /* break from while loop */
        break;
    }
  }

  if (prv_card_acquired(pdev))
  {
    /* change delay gates per card type */
    if (pdev->card_type & CARD_TYPE_SD)
      SYS_REGS->mci_delaymodes = SYS_REG_SD_CARD_DELAY;
    else
      SYS_REGS->mci_delaymodes = SYS_REG_MMC_CARD_DELAY;

    /* now compute card size, block size and no. of blocks
    based on CSD response recived. */
    prv_process_csd(pdev);
  }
}

/***********************************************************************
 *
 * Function: mci_open
 *
 * Purpose: Opens the MCI card controller
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     ipbase: Pointer to a MCI peripheral block
 *     arg   : Pointer to wait function to be used during for poll
 *             command status. Application with OS capabilities can use
 *             this option to avoid busy wait implemented in the driver's
 *             default wait routine.
 *
 * Outputs: None
 *
 * Returns: The pointer to a card info structure or NULL
 *
 * Notes: None
 *
 **********************************************************************/
INT_32 mci_open(void *ipbase, INT_32 arg)
{
  volatile UNS_32 i;
  /* enable SD/MMC clock */
  cgu_clk_en_dis(CGU_SB_SD_MMC_HCLK_ID, 1);
  cgu_clk_en_dis(CGU_SB_SD_MMC_CCLK_IN_ID, 1);

  /* reset SD/MMC/MCI modules through CGU */
  /* reset SD/MMC/MCI modules through CGU */
  /* clear and set the register */
  CGU_CFG->sd_mmc_pnres_soft = 0;
  CGU_CFG->sd_mmc_nres_cclk_in_soft = 0;
  /* introduce some delay */
  for (i = 0;i < 1000;i++);
  CGU_CFG->sd_mmc_nres_cclk_in_soft = CGU_CONFIG_SOFT_RESET;
  CGU_CFG->sd_mmc_pnres_soft = CGU_CONFIG_SOFT_RESET;

  /* Set IOCONF to MCI pins */
  SYS_REGS->mci_delaymodes = 0;
  SYS_REGS->mux_gpio_mci_sel = 1;
  /* set the pins as driven by IP in IOCONF */
  IOCONF->block[IOCONF_MUX0].mode1_clear = 0xF0000003;
  IOCONF->block[IOCONF_MUX0].mode0_set = 0xF0000003;

  /* set delay gates */
  SYS_REGS->mci_delaymodes = SYS_REG_SD_CARD_DELAY;

  /* reset all blocks */
  MCI->ctrl = MCI_CTRL_RESET | MCI_CTRL_FIFO_RESET
              | MCI_CTRL_DMA_RESET;
  /* wait till resets clear */
  while (MCI->ctrl &
         (MCI_CTRL_RESET | MCI_CTRL_FIFO_RESET | MCI_CTRL_DMA_RESET));

  /* Clear the interrupts for the host controller */
  MCI->rintsts = 0xFFFFFFFF;

  /* Put in max timeout */
  MCI->tmout = 0xFFFFFFFF;

  /* FIFO threshold settings  */
  MCI->fifoth = (0x2 << 28) | (0xF << 16) | (0x10 << 0);

  /* disable clock to CIU */
  MCI->clkena = 0;
  MCI->clksrc = 0;

  /* clear mmc structure*/
  memset(&g_card_info, 0, sizeof(MCI_CARD_INFO_T));
  /* set the wait_func if passed */
  g_card_info.wait_func = (MCI_CMD_WAIT_FUNC_T)arg;

  /* start card enumeration */
  mci_acquire(&g_card_info);

  return ((INT_32)&g_card_info);
}

/***********************************************************************
 *
 * Function: mci_close
 *
 * Purpose: Close the MCI
 *
 * Processing:
 *     IResets MCI controller and disables clocks going to the MCI.
 *
 * Parameters:
 *     devid: Pointer to card info structure
 *
 * Outputs: None
 *
 * Returns: The status of the close operation
 *
 * Notes: None
 *
 **********************************************************************/
STATUS mci_close(INT_32 devid)
{
  /* clear mmc structure*/
  memset(&g_card_info, 0, sizeof(MCI_CARD_INFO_T));
  /* reset SD/MMC/MCI modules through CGU */
  cgu_soft_reset_module(SD_MMC_PNRES_SOFT);
  cgu_soft_reset_module(SD_MMC_NRES_CCLK_IN_SOFT);
  /* Turn off SD/MMC clocks */
  cgu_clk_en_dis(CGU_SB_SD_MMC_HCLK_ID, FALSE);
  cgu_clk_en_dis(CGU_SB_SD_MMC_CCLK_IN_ID, FALSE);

  return _NO_ERROR;
}


/***********************************************************************
 *
 * Function: mci_read_blocks
 *
 * Purpose: Performs the read of data from the SD/MMC card
 *
 * Processing:
 *     Reads the requested blocks in sequence.
 *
 * Parameters:
 *     devid:     Pointer to card info structure
 *     buffer:    Pointer to data buffer to copy to
 *     start_block: Start block number
 *     end_block: End block number
 *
 * Outputs: None
 *
 * Returns: Number of bytes actually read
 *
 * Notes:
 *  To read single block set start_block = end_block.
 *
 **********************************************************************/
INT_32 mci_read_blocks(INT_32 devid,
                       void* buffer,
                       INT_32 start_block,
                       INT_32 end_block)
{
  MCI_CARD_INFO_T* pdev = (MCI_CARD_INFO_T*) devid;
  INT_32 cbRead = (end_block - start_block + 1) << 9; /*(end_block - start_block) * 512 */
  INT_32 status = 0;
  INT_32 index;

  /* if card is not acquired return immediately */
  if ((prv_card_acquired(pdev) == FALSE)
      || (end_block < start_block) /* check block index in range */
      || (start_block < 0)
      || (end_block > pdev->blocknr)
     )
  {
    return 0;
  }
  /* put card in trans state */
  if (prv_set_trans_state(pdev) != _NO_ERROR)
    return 0;

  /* set number of bytes to read */
  MCI->bytcnt = cbRead;

  /* if high capacity card use block indexing */
  if (pdev->card_type & CARD_TYPE_HC)
    index = start_block;
  else
    index = start_block << 9;

  /* check how many blocks to read */
  if (end_block ==  start_block)
  {
    status = mci_execute_command(pdev, CMD_READ_SINGLE, index,
                                 MCI_INT_CMD_DONE | MCI_INT_DATA_OVER | MCI_INT_RXDR);
  }
  else
  {
    /* do read multiple */
    status = mci_execute_command(pdev, CMD_READ_MULTIPLE, index,
                                 MCI_INT_CMD_DONE | MCI_INT_DATA_OVER | MCI_INT_RXDR);
  }
  /* read data from the FIFO */
  if (status == 0)
    cbRead = prv_pull_data(pdev, (UNS_8*)buffer, cbRead);
  else
    cbRead = 0; /* return error if command fails */

  return cbRead;
}

/***********************************************************************
 *
 * Function: mci_write_blocks
 *
 * Purpose: Performs write of data to the SD/MMC card
 *
 * Processing:
 *     Write the requested blocks in sequence with data passed.
 *
 * Parameters:
 *     devid:     Pointer to card info structure
 *     buffer:    Pointer to data buffer to copy to
 *     start_block: Start block number
 *     end_block: End block number
 *
 * Outputs: None
 *
 * Returns: Number of bytes actually written
 *
 * Notes:
 *  To write single block set start_block = end_block.
 *
 **********************************************************************/
INT_32 mci_write_blocks(INT_32 devid,
                        void* buffer,
                        INT_32 start_block,
                        INT_32 end_block)
{
  MCI_CARD_INFO_T* pdev = (MCI_CARD_INFO_T*) devid;
  /*(end_block - start_block) * 512 */
  INT_32 cbWrote = (end_block - start_block + 1) << 9;
  int status;
  INT_32 index;

  /* if card is not acquired return immediately */
  if ((prv_card_acquired(pdev) == FALSE)
      || (end_block < start_block) /* check block index in range */
      || (start_block < 0)
      || (end_block > pdev->blocknr)
     )
  {
    return 0;
  }

  /* put card in trans state */
  if (prv_set_trans_state(pdev) != _NO_ERROR)
    return 0;

  /* set number of bytes to write */
  MCI->bytcnt = cbWrote;

  /* if high capacity card use block indexing */
  if (pdev->card_type & CARD_TYPE_HC)
    index = start_block;
  else
    index = start_block << 9;

  /* check how many blocks to write */
  if (end_block == start_block)
  {
    status = mci_execute_command(pdev, CMD_WRITE_SINGLE, index,
                                 MCI_INT_CMD_DONE | MCI_INT_TXDR);
  }
  else
  {
    /* do write multiple */
    status = mci_execute_command(pdev, CMD_WRITE_MULTIPLE, index,
                                 MCI_INT_CMD_DONE | MCI_INT_TXDR);
  }
  /* write data to the FIFO */
  if (status == 0)
  {
    prv_push_data((UNS_8*)buffer, cbWrote);
    /* wait for transfer done */
    status = prv_wait_for_completion(pdev, MCI_INT_DATA_OVER);
  }
  else
  {
    cbWrote = 0; /* return error if command fails */
  }

  return cbWrote;
}


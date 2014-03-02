/***********************************************************************
 * $Id:: main.c 6636 2011-03-01 04:26:56Z ing02124                     $
 *
 * Project: USB Audio device example
 *
 * Description:
 *     A USB Audio device example.
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
#include <stdio.h>
#include <string.h>

#include "lpc_types.h"
#include "lpc_string.h"
#include "lpc_irq_fiq.h"
#include "lpc_arm922t_cp15_driver.h"
#include "lpc313x_timer_driver.h"
#include "lpc313x_intc_driver.h"
#include "lpc313x_ioconf_driver.h"
#include "lpc313x_uart_driver.h"
#include "lpc313x_usbotg.h"
#include "lpc313x_usbd_driver.h"
#include "lpc313x_i2s_clock_driver.h"
#include "lpc313x_dma_driver.h"
#include "lpc313x_cgu_driver.h"
#include "adcuser.h"
#include "usbcore.h"
#include "usbhw.h"
#include "lpc3152_codec_driver.h"
#include "isohandler.h"
#include "pcmhandler.h"
#include "dbg.h"

extern DQH_T* const ep_QH;
extern DTD_T* const ep_TD;
extern UNS_32 DevStatusFS2HS;

extern void USB_EndPoint0(UNS_32 event);
#define USB_AUDIO_VOL_SILENCE  0x8000

enum usbops
{
  USB_IDLE,
  USB_SET_MUTE,
  USB_SET_VOL,
  USB_SET_SAMPLE_RATE,
  USB_START_STOP_PLAYBACK,
};

struct usbtsk
{
  volatile enum usbops op;
  volatile int val[3];
}objusbtsk, *usbtsk = &objusbtsk;

#if !USB_INTERFACE_EVENT
#error USB_INTERFACE_EVENT is mandatory for ISO handling.
#endif
/***********************************************************************
 *
 * Function: USB_Interface_Event
 *
 * Purpose: USB Interface handler event
 *
 * Processing:
 *     See function.
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
void USB_Interface_Event(void)
{
  /* write code to enable/disable audo capture when interface
  ALT setting is changed */
  if (SetupPacket.wIndex.W == USB_ADC_SIF2_NUM)
  {
    if (SetupPacket.wValue.W == 0x0001)
    {
      if (recording->active)
        return;
      usbtsk->val[1] = 1;
      /* Enable IN Endpoints */
      iso_ep_setup(1, 0); /* Enable all input end points */
    }
    else
    {
      if (!recording->active)
        return;
      usbtsk->val[1] = 0;
    }
    usbtsk->op = USB_START_STOP_PLAYBACK;
    usbtsk->val[0] = PCM_RECORDING;
  }

  /* write code to enable/disable audo playback when interface
  ALT setting is changed */
  if (SetupPacket.wIndex.W == USB_ADC_SIF1_NUM)
  {
    if (SetupPacket.wValue.W == 0x0001)
    {
      if (playback->active)
        return;
      usbtsk->val[1] = 1;
      iso_ep_setup(0, 0); /* Enable all output end points */
    }
    else
    {
      if (!playback->active)
        return;
      usbtsk->val[1] = 0;
    }
    usbtsk->op = USB_START_STOP_PLAYBACK;
    usbtsk->val[0] = PCM_PLAYBACK;
  }
}

/***********************************************************************
 *
 * Function: USB_Reset_Event
 *
 * Purpose: Handler for USB Reset event
 *
 * Processing:
 *     See function.
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
void USB_Reset_Event(void)
{
  USB_ResetCore();
  usb_iso_reset();
}


/***********************************************************************
 *
 * Function: USB_SOF_Event
 *
 * Purpose: SOF event handler that will be called every 1ms
 *
 * Processing:
 *     See function.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: None
 *
 * Notes: This function is used to sync the playback/recording
 *
 **********************************************************************/
void USB_SOF_Event(void)
{
  if (recording->active)
  {
    recording->sof_cnt++;
  }
}

/***********************************************************************
 *
 * Function: mute
 *
 * Purpose: Mute or unmute the PCM device
 *
 * Processing:
 *     See function.
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
static void mute(int x)
{
  usbtsk->op = USB_SET_MUTE;
  usbtsk->val[0] = x;
}

/***********************************************************************
 *
 * Function: set_sample_rate
 *
 * Purpose: Sets the sample rate of playback or recording
 *
 * Processing:
 *     See function.
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
uint32_t set_sample_rate(uint32_t dir, uint32_t rate)
{
  if (dir)
  {
    recording->freq = rate;
  }
  else
  {
    pcm_set_playback_rate(rate);
  }

  return rate;
}

/***********************************************************************
 *
 * Function: set_volume
 *
 * Purpose: sets the volume of the PCM device
 *
 * Processing:
 *     See function.
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
void set_volume(uint16_t vol)
{
  usbtsk->val[0] = vol;
  usbtsk->op = USB_SET_VOL;
}

/***********************************************************************
 *
 * Function: usb_task
 *
 * Purpose: handler for USB events, that cannot be handled within ISR.
 *
 * Processing:
 *     See function.
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
void usb_task(void)
{
  int tmp;
  int op = usbtsk->op;
  usbtsk->op = USB_IDLE;
  switch (op)
  {
    case USB_SET_VOL:
      tmp = usbtsk->val[0];
      if (tmp == USB_AUDIO_VOL_SILENCE)
        tmp = 0x00FC;
      else
      {
        tmp = -(int16_t)tmp / 0x40;
      }
      playback->vol = recording->vol = tmp;
      lpc3152_codec_set_volume(tmp, tmp);
      sprintf(buftmp, "Volume set to 0x%x\r\n", tmp);
      uart_puts(buftmp);
      break;

    case USB_SET_MUTE:
      tmp = usbtsk->val[0];
      sprintf(buftmp, "Mute set to %d\r\n", tmp);
      uart_puts(buftmp);
      recording->mute = playback->mute = tmp;
      lpc3152_codec_mute(tmp);
      break;

    case USB_SET_SAMPLE_RATE:
      break;
    case USB_START_STOP_PLAYBACK:
      if (usbtsk->val[0] == PCM_PLAYBACK)
      {
        if (usbtsk->val[1])
        {
          pcm_setup_playback();
          uart_puts("setup pb\r\n");
        }
        else
        {
          iso_ep_setup(0, 1); /* Disable all output end points */
          pcm_cleanup_playback();
          uart_puts("Clean pb\r\n");
          playback->sof_cnt = 0;
        }
      }
      else if (usbtsk->val[0] == PCM_RECORDING)
      {
        if (usbtsk->val[1])
        {
          pcm_setup_recording();
          uart_puts("setup rec\r\n");
        }
        else
        {
          iso_ep_setup(1, 1); /* Disable all input end points */
          pcm_cleanup_recording();
          uart_puts("clean rec\r\n");
        }
      }
      break;
  }
}

/***********************************************************************
 *
 * Function: usb_adc_init
 *
 * Purpose: USB ADC Interface Init function
 *
 * Processing:
 *     See function.
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
void usb_adc_init(void)
{
  struct ac_ops ops;
  struct ac_volume vol;

  ops.mute = mute;
  ops.set_sample_rate = set_sample_rate;
  ops.set_volume = set_volume;
  vol.max = 0x00;     /* Max Vol = 0 db */
  vol.min = 0xE600;   /* Min Vol = -26 db */
  vol.res = 0x40;     /* Steps 0.25 db */

  setup_usb_audio_codec(&ops, &vol);
  USB_OTG->portsc1 |= _BIT(24);
  playback->done = recording->done = 1;

}

/***********************************************************************
 *
 * Function: c_entry
 *
 * Purpose: Application entry point from the startup code
 *
 * Processing:
 *     See function.
 *
 * Parameters: None
 *
 * Outputs: None
 *
 * Returns: Always returns 1
 *
 * Notes: None
 *
 **********************************************************************/
int c_entry(void)
{
  int ch;
  UNS_32* start_adr, *end_adr;
  LPC_USBDRV_INIT_T usb_cb;
#if (defined(TGT_SDRAM) && defined(DEBUG))
  extern unsigned char *audbuf;
  extern volatile int acnt;
#endif

  /* Disable interrupts in ARM core */
  disable_irq_fiq();

  /* Setup miscellaneous board functions */
  dbgbuf_init();

  /* Set virtual address of MMU table */
  cp15_set_vmmu_addr((void *)ISROM_MMU_TTB_BASE);

  /* Initialize interrupt system */
  int_initialize(0xFFFFFFFF);

  /* Install timer interrupts handlers as a IRQ interrupts */
  int_install_irq_handler(IRQ_USB, (PFV) USB_ISR);

  /* initilize call back structures */
  memset((void*)&usb_cb, 0, sizeof(LPC_USBDRV_INIT_T));
  usb_cb.USB_Reset_Event = USB_Reset_Event;
  usb_cb.USB_SOF_Event = USB_SOF_Event;
  usb_cb.USB_P_EP[0] = USB_EndPoint0;
  usb_cb.USB_P_EP[USB_ADC_OUT_EP] =
    usb_cb.USB_P_EP[USB_ADC_IN_EP & 0xF] = iso_ep_event;
  usb_cb.USB_virt_to_phys = lpc31xx_va_to_pa;
  usb_cb.USB_cache_flush = lpc31xx_flush_dcache_range;
  usb_cb.USB_cache_invalidate = lpc31xx_invalidate_dcache_range;
  usb_cb.ep0_maxp = USB_MAX_PACKET0;

  /* USB Initialization */
  USB_Init(&usb_cb);

  /* Enable USB interrupts in the interrupt controller */
  int_enable(IRQ_USB);
  enable_irq();
  usb_adc_init();
  pcm_device_init();

  /* Initialize the DMA */
  dma_init();

  /* USB Connect */
  USB_Connect(TRUE);

  /* Display the prompt */
  uart_puts("\r\nLPC315x USB Audio example>");
  ch = 0;
  while (ch != 27)
  {

    pcm_sync_playback();
    if (usbtsk->op != USB_IDLE)
      usb_task();
    start_adr = end_adr = 0;
    if ((ch = uart_getchar()) > 0 || dbgbuf_needs_flush())
    {
      if (dbgbuf_needs_flush())
      {
        ch = 'd';
      }
      else
      {
        uart_puts("\r\n");
      }
      switch (ch)
      {
        case 'd':
        case 'D':
          /* Dump debug buffer */
          dbgbuf_flush();
          break;

        case 'Q':
        case 'q':
          dbgbuf_puts("Dumping out endpoint queue\r\n");
          iso_ep_dump_queue(EP_ADR_LOG_PHY(USB_ADC_OUT_EP));
          dbgbuf_flush();
          dbgbuf_puts("Dumping in endpoint queue\r\n");
          iso_ep_dump_queue(EP_ADR_LOG_PHY(USB_ADC_IN_EP));
          dbgbuf_flush();
          break;
        case 'f':
        case 'F':
          /* force full-speed mode */
          USB_OTG->portsc1 |= _BIT(24);
          DevStatusFS2HS = FALSE;
          break;
        case 'h':
        case 'H':
          /* remove force full-speed mode */
          USB_OTG->portsc1 &= ~_BIT(24);
          break;

        case 'r':
        case 'R':
          /* dump usb regs */
          start_adr = (UNS_32*)(USBOTG_BASE + 0x100);
          end_adr = (UNS_32*)(USBOTG_BASE + 0x1D0);
          dump_addr_range(start_adr, end_adr);
          dbgbuf_flush();
          break;
#if (defined(TGT_SDRAM) && defined(DEBUG))
        /* Playback from buffer */
        case 'p':
        case 'P':
          if (acnt)
          {
            pcm_setup_playback();
            pcm_set_playback_rate(playback->freq);
            play_buffer(audbuf, acnt);
            pcm_cleanup_playback();
          }
          /* Check for Data tag */
          else if (audbuf[0x24] == 'd' && audbuf[0x27] == 'a')
          {
            /* Check Format tag and get sampling rate from it */
            if (audbuf[0xc] == 'f' && audbuf[0xe] == 't')
            {
              playback->freq = *(unsigned long *) & audbuf[0x18];
              pcm_set_playback_rate(playback->freq);
            }
            else
            {
              break;
            }
            pcm_setup_playback();
            play_buffer(audbuf + 0x2c, *(unsigned long *)&audbuf[0x28]);
            pcm_cleanup_playback();
          }
          else
          {
            uart_puts("Nothing to play!\r\n");
          }
          break;
#endif
        default:
          break;
      }
      /* Display the prompt */
      uart_puts("\r\nLPC315x USB Audio example>");
    }
  }

  /* Disable timer interrupts in the interrupt controller */
  int_disable(IRQ_USB);

  /* Disable interrupts in ARM core */
  disable_irq_fiq();

  return 1;
}


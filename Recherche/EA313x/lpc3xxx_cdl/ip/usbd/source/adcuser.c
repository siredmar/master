/*----------------------------------------------------------------------------
 *      U S B  -  K e r n e l
 *----------------------------------------------------------------------------
 *      Name:    ADCUSER.C
 *      Purpose: Audio Device Class Custom User Module
 *      Version: V1.10
 *----------------------------------------------------------------------------
 *      This software is supplied "AS IS" without any warranties, express,
 *      implied or statutory, including but not limited to the implied
 *      warranties of fitness for purpose, satisfactory quality and
 *      noninfringement. Keil extends you a royalty-free right to reproduce
 *      and distribute executable files created using this software for use
 *      on NXP ARM microcontroller devices only. Nothing else gives
 *      you the right to use this software.
 *
 *      Copyright (c) 2005-2009 Keil Software.
 *      Adaption to LPC23xx, Copyright (c) 2009 NXP.
 *---------------------------------------------------------------------------*/

//#include "board.h"

/* USB header files */
#include "lpc_usb.h"
#include "usbaudio.h"
#include "usbcore.h"
#include "adcuser.h"

/* Application header files */
//#include "audio/codec.h"


#if 0
volatile uint16_t VolCur = 0x003D;     /* Volume Current Value */
uint16_t VolMin = 0x0000;     /* Volume Minimum Value */
uint16_t VolMax = 0x003F;     /* Volume Maximum Value */
uint16_t VolRes = 0x0001;     /* Volume Resolution */
uint32_t g_sample_rate[2] = { 48000, 48000};
#endif

static struct acodec objcodec, *acodec = &objcodec;

void setup_usb_audio_codec(const struct ac_ops *ops, const struct ac_volume *vol)
{
  acodec->ops.mute = ops->mute;
  acodec->ops.set_sample_rate = ops->set_sample_rate;
  acodec->ops.set_volume = ops->set_volume;
  acodec->volume.cur = vol->cur;
  acodec->volume.min = vol->min;
  acodec->volume.max = vol->max;
  acodec->volume.res = vol->res;
}

/*
 *  Audio Device Class Interface Get Request Callback
 *   Called automatically on ADC Interface Get Request
 *    Parameters:      None (global SetupPacket and EP0Buf)
 *    Return Value:    TRUE - Success, FALSE - Error
 */

BOOL_32 ADC_IF_GetRequest(void)
{

  /*
    Interface = SetupPacket.wIndex.WB.L;
    EntityID  = SetupPacket.wIndex.WB.H;
    Request   = SetupPacket.bRequest;
    Value     = SetupPacket.wValue.W;
    ...
  */

  if (SetupPacket.wIndex.W == 0x0200)
  {
    /* Feature Unit: Interface = 0, ID = 2 */
    if (SetupPacket.wValue.WB.L == 0)
    {
      /* Master Channel */
      switch (SetupPacket.wValue.WB.H)
      {
        case AUDIO_MUTE_CONTROL:
          switch (SetupPacket.bRequest)
          {
            case AUDIO_REQUEST_GET_CUR:
              EP0Buf[0] = acodec->mute;
              return (TRUE);
          }
          break;
        case AUDIO_VOLUME_CONTROL:
          switch (SetupPacket.bRequest)
          {
            case AUDIO_REQUEST_GET_CUR:
              //*((uint16_t *)EP0Buf) = acodec->volume.cur;
              EP0Buf[0] = acodec->volume.cur & 0xFF;
              EP0Buf[1] = (acodec->volume.cur >> 8) & 0xFF;
              return (TRUE);
            case AUDIO_REQUEST_GET_MIN:
              //*((uint16_t *)EP0Buf) = acodec->volume.min;
              EP0Buf[0] = acodec->volume.min & 0xFF;
              EP0Buf[1] = (acodec->volume.min >> 8) & 0xFF;
              return (TRUE);
            case AUDIO_REQUEST_GET_MAX:
              //*((uint16_t *)EP0Buf) = acodec->volume.max;
              EP0Buf[0] = acodec->volume.max & 0xFF;
              EP0Buf[1] = (acodec->volume.max >> 8) & 0xFF;
              return (TRUE);
            case AUDIO_REQUEST_GET_RES:
              //*((uint16_t *)EP0Buf) = acodec->volume.res;
              EP0Buf[0] = acodec->volume.res & 0xFF;
              EP0Buf[1] = (acodec->volume.res >> 8) & 0xFF;
              return (TRUE);
          }
          break;
      }
    }
  }

  return (FALSE);  /* Not Supported */
}


/*
 *  Audio Device Class Interface Set Request Callback
 *   Called automatically on ADC Interface Set Request
 *    Parameters:      None (global SetupPacket and EP0Buf)
 *    Return Value:    TRUE - Success, FALSE - Error
 */

BOOL_32 ADC_IF_SetRequest(void)
{

  /*
    Interface = SetupPacket.wIndex.WB.L;
    EntityID  = SetupPacket.wIndex.WB.H;
    Request   = SetupPacket.bRequest;
    Value     = SetupPacket.wValue.W;
    ...
  */
  if (SetupPacket.wIndex.W == 0x0200)
  {
    /* Feature Unit: Interface = 0, ID = 2 */
    if (SetupPacket.wValue.WB.L == 0)
    {
      /* Master Channel */
      switch (SetupPacket.wValue.WB.H)
      {
        case AUDIO_MUTE_CONTROL:
          switch (SetupPacket.bRequest)
          {
            case AUDIO_REQUEST_SET_CUR:
              acodec->mute = EP0Buf[0];

              /* Invoke codec mute function */
              if (acodec->ops.mute)
              {
                acodec->ops.mute(acodec->mute);
              }
              return (TRUE);
          }
          break;
        case AUDIO_VOLUME_CONTROL:
          switch (SetupPacket.bRequest)
          {
            case AUDIO_REQUEST_SET_CUR:
              //acodec->volume.cur = *((uint16_t *)EP0Buf);
              acodec->volume.cur = (uint16_t)((EP0Buf[1] << 8) | EP0Buf[0]);
              acodec->ops.set_volume(acodec->volume.cur);
              return (TRUE);
          }
          break;
      }
    }
  }

  return (FALSE);  /* Not Supported */
}


/*
 *  Audio Device Class EndPoint Get Request Callback
 *   Called automatically on ADC EndPoint Get Request
 *    Parameters:      None (global SetupPacket and EP0Buf)
 *    Return Value:    TRUE - Success, FALSE - Error
 */

BOOL_32 ADC_EP_GetRequest(void)
{

  /*
    EndPoint = SetupPacket.wIndex.WB.L;
    Request  = SetupPacket.bRequest;
    Value    = SetupPacket.wValue.W;
    ...
  */
  int dir = CODEC_DIR_SPEAKER;

  switch (SetupPacket.wIndex.W)
  {
    case USB_ADC_IN_EP:
      dir = CODEC_DIR_MIC;
    case USB_ADC_OUT_EP:
      /* Feature Unit: Interface = 0, ID = 2 */
      if (SetupPacket.wValue.WB.L == 0)
      {
        /* Master Channel */
        if (SetupPacket.wValue.WB.H == AUDIO_CONTROL_SAMPLING_FREQ)
        {
          if (SetupPacket.bRequest == AUDIO_REQUEST_GET_CUR)
          {
            EP0Buf[0] = (uint8_t)(acodec->sample_rate[dir] & 0xFF);
            EP0Buf[1] = (uint8_t)((acodec->sample_rate[dir] >> 8) & 0xFF);
            EP0Buf[2] = (uint8_t)((acodec->sample_rate[dir] >> 16) & 0xFF);
            return (TRUE);
          }
        }
      }
      break;
    default:
      break;
  }
  return (FALSE);  /* Not Supported */
}


/*
 *  Audio Device Class EndPoint Set Request Callback
 *   Called automatically on ADC EndPoint Set Request
 *    Parameters:      None (global SetupPacket and EP0Buf)
 *    Return Value:    TRUE - Success, FALSE - Error
 */

BOOL_32 ADC_EP_SetRequest(void)
{

  /*
    EndPoint = SetupPacket.wIndex.WB.L;
    Request  = SetupPacket.bRequest;
    Value    = SetupPacket.wValue.W;
    ...
  */
  int dir = CODEC_DIR_SPEAKER;
  uint32_t rate;

  switch (SetupPacket.wIndex.W)
  {
    case USB_ADC_IN_EP:
      dir = CODEC_DIR_MIC;
    case USB_ADC_OUT_EP:
      /* Feature Unit: Interface = 0, ID = 2 */
      if (SetupPacket.wValue.WB.L == 0)
      {
        /* Master Channel */
        if (SetupPacket.wValue.WB.H == AUDIO_CONTROL_SAMPLING_FREQ)
        {
          rate = EP0Buf[0] | (EP0Buf[1] << 8) | (EP0Buf[2] << 16);
          if (SetupPacket.bRequest == AUDIO_REQUEST_SET_CUR)
          {
            rate = acodec->ops.set_sample_rate(dir, rate);
            if (rate != 0)
            {
              acodec->sample_rate[dir] = rate;
              return (TRUE);
            }
          }
        }
      }
      break;
    default:
      break;
  }
  return (FALSE);  /* Not Supported */
}

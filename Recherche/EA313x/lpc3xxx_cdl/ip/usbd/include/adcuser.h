/*----------------------------------------------------------------------------
 *      U S B  -  K e r n e l
 *----------------------------------------------------------------------------
 *      Name:    ADCUSER.H
 *      Purpose: Audio Device Class Custom User Definitions
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

#ifndef __ADCUSER_H__
#define __ADCUSER_H__

#define CODEC_DIR_SPEAKER          0
#define CODEC_DIR_MIC              1
struct ac_ops
{
  void (*mute)(int);
  uint32_t (*set_sample_rate)(uint32_t dir, uint32_t rate);
  void (*set_volume)(uint16_t);
};


struct ac_volume
{
  volatile uint16_t cur; /* Current volume */
  uint16_t min;          /* Minimum volume */
  uint16_t max;          /* Maximum volume */
  uint16_t res;          /* Volume Resolution */
};

struct acodec
{
  int mute;
  uint32_t sample_rate[2];
  struct ac_volume volume;
  struct ac_ops ops;
};
extern void setup_usb_audio_codec(const struct ac_ops *ac, const struct ac_volume *vol);

/* Audio Device Class Requests Callback Functions */
extern BOOL_32 ADC_IF_GetRequest(void);
extern BOOL_32 ADC_IF_SetRequest(void);
extern BOOL_32 ADC_EP_GetRequest(void);
extern BOOL_32 ADC_EP_SetRequest(void);

#endif  /* __ADCUSER_H__ */

/***********************************************************************
 * $Id:: lpc_ac97_codec.h 745 2008-05-13 19:59:29Z pdurgesh            $
 *
 * Project: AC97 CODEC definitions
 *
 * Description:
 *     This file contains the structure definitions and manifest
 *     constants for the following component:
 *         Standard AC97 CODEC
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

#ifndef LPC_AC97_CODEC_H
#define LPC_AC97_CODEC_H

/***********************************************************************
 * AC97 CODEC register defines
 **********************************************************************/

#define AC97_RESET        0x00    /* AC97 CODEC reset index */
#define AC97_MASTER_VOL   0x02    /* AC97 CODEC master volume index */
#define AC97_AUX_OUT_VOL  0x04    /* AC97 CODEC aux out volume index */
#define AC97_MONO_VOL     0x06    /* AC97 CODEC mono volume index */
#define AC97_MASTER_TONE  0x08    /* AC97 CODEC master tone index */
#define AC97_BEEP_VOL     0x0A    /* AC97 CODEC PC beep volume index */
#define AC97_PHONE_VOL    0x0C    /* AC97 CODEC phone volume index */
#define AC97_MIC_VOL      0x0E    /* AC97 CODEC MIC volume index */
#define AC97_LINE_IN_VOL  0x10    /* AC97 CODEC line in volume index */
#define AC97_CD_VOL       0x12    /* AC97 CODEC CD volume index */
#define AC97_VIDEO_VOL    0x14    /* AC97 CODEC video volume index */
#define AC97_AUX_IN_VOL   0x16    /* AC97 CODEC aux in volume index */
#define AC97_PCM_OUT_VOL  0x18    /* AC97 CODEC PCM out volume index */
#define AC97_RECORD_SEL   0x1A    /* AC97 record channel select index */
#define AC97_RECORD_GAIN  0x1C    /* AC97 recoed gain index */
#define AC97_MIC_GAIN     0x1E    /* Not used, use AC97_MIC_VOL instead */
#define AC97_GP           0x20    /* AC97 GP index */
#define AC97_CONTROL_3D   0x22    /* AC97 3D control index */
#define AC97_POWERDOWN    0x26    /* AC97 powerdown control index */
#define AC97_EXT_AUDIO_ID 0x28    /* AC97 extended audio ID index */
#define AC97_EXTD_STS_CTL 0x2A    /* AC97 Extended audio index */
#define AC97_FR_DAC_RATE  0x2C    /* AC97 PCM front channels DAC rate */
#define AC97_PCM_ADC_RATE 0x32    /* AC97 PCM left/right ADC rate */
#define AC97_MIC_ADC_RATE 0x34    /* AC97 MIC ADC rate */
#define AC97_EXT_MODEM    0x3C    /* AC97 external modem ctl index */
#define AC97_VENDOR_ID1   0x7C    /* AC97 vendor ID1 index */
#define AC97_VENDOR_ID2   0x7E    /* AC97 vendor ID2 index */

/***********************************************************************
 * AC97 CODEC volume and record level,tone level, MIC specific defines
 **********************************************************************/

#define AC97_MUTE         0x80    /* AC97 CODEC mute bit */
#define AC97_MAX_VOL      0x00    /* AC97 CODEC min attenuation */
#define AC97_MIN_VOL      0x3F    /* AC97 CODEC max attenuation */

#define AC97_TONE_BOOST   0x00    /* AC97 CODEC tone max boost value */
#define AC97_TONE_FLAT    0x07    /* AC97 CODEC tone flat value */
#define AC97_TONE_ATTEN   0x0E    /* AC97 CODEC tone min boost value */
#define AC97_TONE_BYPASS  0x0F    /* AC97 CODEC tone bypass value */

#define AC97_MIX_GN_MIN   0x1F    /* AC97 CODEC mixer min gain */
#define AC97_MIX_GN_MAX   0x00    /* AC97 CODEC mixer max gain */
#define AC97_MIX_GN_MUTE  0x80    /* AC97 CODEC mixer mute value */
#define AC97_MIC_20DB_GN  0x40    /* AC97 CODEC mixer 20db MIC gain */

#define AC97_REC_GN_MIN   0x00    /* AC97 CODEC record min gain */
#define AC97_REC_GN_MAX   0x0F    /* AC97 CODEC record max gain */
#define AC97_REC_GN_MUTE  0x80    /* AC97 CODEC record mute value */

/***********************************************************************
 * AC97 CODEC powerdown register defines
 **********************************************************************/

#define AC97_REFS_NORMAL  0x0010  /* AC97 CODEC VREFs to normal level */
#define AC97_MIXERS_READY 0x0008  /* AC97 CODEC Analog mixers ready */
#define AC97_DAC_READY    0x0004  /* AC97 CODEC DAC(s) ready */
#define AC97_ADC_READY    0x0002  /* AC97 CODEC ADC(s) ready */
#define AC97_PR0          0x0100  /* AC97 CODEC Powerdown ADCs */
#define AC97_PR1          0x0200  /* AC97 CODEC Powerdown DACs */
#define AC97_PR2          0x0400  /* AC97 CODEC Powerdown VREF */
#define AC97_PR3          0x0800  /* AC97 CODEC Powerdown mixer */
#define AC97_PR4          0x1000  /* AC97 CODEC Powerdown AC-Link */
#define AC97_PR5          0x2000  /* AC97 CODEC Powerdown int clock */
#define AC97_PR6          0x4000  /* AC97 CODEC Powerdown AUX out */
#define AC97_EAPD         0x8000  /* AC97 CODEC powerdown ext AMP */

/***********************************************************************
 * AC97 CODEC sample rate defines
 **********************************************************************/

#define AC97_SR_8000      8000    /* AC97 CODEC 8KHz sample rate */
#define AC97_SR_11025     11025   /* AC97 CODEC 11KHz sample rate */
#define AC97_SR_16000     16000   /* AC97 CODEC 16KHz sample rate */
#define AC97_SR_22050     22050   /* AC97 CODEC 22KHz sample rate */
#define AC97_SR_24000     24000   /* AC97 CODEC 24KHz sample rate */
#define AC97_SR_32000     32000   /* AC97 CODEC 32KHz sample rate */
#define AC97_SR_44100     44100   /* AC97 CODEC 44.1KHz sample rate */
#define AC97_SR_48000     48000   /* AC97 CODEC 48KHz sample rate */

/***********************************************************************
 * AC97 CODEC bits per sample defines
 **********************************************************************/

#define AC97_BPS12        12      /* AC97 CODEC 12 bits per sample */
#define AC97_BPS16        16      /* AC97 CODEC 16 bits per sample */
#define AC97_BPS18        18      /* AC97 CODEC 18 bits per sample */
#define AC97_BPS20        20      /* AC97 CODEC 20 bits per sample */

/***********************************************************************
 * AC97 CODEC control bit defines defines
 **********************************************************************/

#define AC97_BPS12_REG    0x6000  // bit value for AAC control
#define AC97_BPS16_REG    0x0000  // bit value for AAC control
#define AC97_BPS18_REG    0x2000  // bit value for AAC control
#define AC97_BPS20_REG    0x4000  // bit value for AAC control

/***********************************************************************
 * AC97 CODEC extended control bit defines defines
 **********************************************************************/

#define AC97_VRA          0x0001  /* AC97 CODEC variable rate audio */
#define AC97_VRM          0x0008  /* AC97 CODEC variable rate MIC */

#endif /* LPC_AC97_CODEC_H */

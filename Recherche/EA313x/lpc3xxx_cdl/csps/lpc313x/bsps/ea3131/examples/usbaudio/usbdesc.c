/*----------------------------------------------------------------------------
 *      U S B  -  K e r n e l
 *----------------------------------------------------------------------------
 *      Name:    USBDESC.C
 *      Purpose: USB Descriptors
 *      Version: V1.10
 *----------------------------------------------------------------------------
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

#include "lpc_usb.h"
#include "hid.h"
#include "usbaudio.h"
#include "usbdesc.h"
#include "usbcfg.h"

/* USB Standard Device Descriptor */
const uint8_t USB_DeviceDescriptor[] =
{
  USB_DEVICE_DESC_SIZE,                 /* bLength */
  USB_DEVICE_DESCRIPTOR_TYPE,           /* bDescriptorType */
  WBVAL(0x0200), /* 2.0 */              /* bcdUSB */
  0x00,                                 /* bDeviceClass */
  0x00,                                 /* bDeviceSubClass */
  0x00,                                 /* bDeviceProtocol */
  USB_MAX_PACKET0,                      /* bMaxPacketSize0 */
  WBVAL(0x1FC9),                        /* idVendor */
  WBVAL(0x0001),                        /* idProduct */
  WBVAL(0x0100), /* 1.00 */             /* bcdDevice */
  0x01,                                 /* iManufacturer */
  0x02,                                 /* iProduct */
  0x03,                                 /* iSerialNumber */
  0x01                                  /* bNumConfigurations */
};

uint8_t USB_FsConfigDescriptorx[] =
{
  0x09, 0x02, 0x58, 0x01, 0x03, 0x01, 0x00, 0xC0, 0x04,
  0x09, 0x04, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00,
  0x0A, 0x24, 0x01, 0x00, 0x01, 0x4E, 0x00, 0x02, 0x01, 0x02,
  0x0C, 0x24, 0x02, 0x0C, 0x01, 0x01, 0x0B, 0x02, 0x03, 0x00, 0x00, 0x00,
  0x0D, 0x24, 0x06, 0x01, 0x0C, 0x02, 0x55, 0x01, 0x02, 0x00, 0x02, 0x00, 0x00,
  0x09, 0x24, 0x03, 0x0E, 0x01, 0x03, 0x00, 0x01, 0x00,
  0x0C, 0x24, 0x02, 0x0B, 0x01, 0x02, 0x0C, 0x02, 0x03, 0x00, 0x00, 0x00,
  0x0D, 0x24, 0x06, 0x06, 0x0B, 0x02, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x09, 0x24, 0x03, 0x0D, 0x01, 0x01, 0x00, 0x06, 0x00,
  0x09, 0x04, 0x01, 0x00, 0x00, 0x01, 0x02, 0x00, 0x00,
  0x09, 0x04, 0x01, 0x01, 0x01, 0x01, 0x02, 0x00, 0x00,
  0x07, 0x24, 0x01, 0x0C, 0x00, 0x01, 0x00,
  0x0E, 0x24, 0x02, 0x01, 0x01, 0x02, 0x10, 0x00, 0x00, 0x19, 0x00, 0x80, 0xBB, 0x00,
  0x09, 0x05, 0x03, 0x09, 0x64, 0x00, 0x01, 0x00, 0x00,
  0x07, 0x25, 0x01, 0x01, 0x02, 0x01, 0x00,
  0x09, 0x04, 0x01, 0x02, 0x01, 0x01, 0x02, 0x00, 0x00,
  0x07, 0x24, 0x01, 0x0C, 0x00, 0x01, 0x00,
  0x0E, 0x24, 0x02, 0x01, 0x02, 0x02, 0x10, 0x00, 0x00, 0x19, 0x00, 0x80, 0xBB, 0x00,
  0x09, 0x05, 0x03, 0x09, 0xC8, 0x00, 0x01, 0x00, 0x00,
  0x07, 0x25, 0x01, 0x01, 0x02, 0x01, 0x00,
  0x09, 0x04, 0x02, 0x00, 0x00, 0x01, 0x02, 0x00, 0x00,
  0x09, 0x04, 0x02, 0x01, 0x01, 0x01, 0x02, 0x00, 0x00,
  0x07, 0x24, 0x01, 0x0D, 0x00, 0x01, 0x00,
  0x0E, 0x24, 0x02, 0x01, 0x01, 0x01, 0x08, 0x00, 0x00, 0x19, 0x00, 0x80, 0xBB, 0x00,
  0x09, 0x05, 0x82, 0x09, 0x32, 0x00, 0x01, 0x00, 0x00,
  0x07, 0x25, 0x01, 0x01, 0x02, 0x01, 0x00,
  0x09, 0x04, 0x02, 0x02, 0x01, 0x01, 0x02, 0x00, 0x00,
  0x07, 0x24, 0x01, 0x0D, 0x00, 0x01, 0x00,
  0x0E, 0x24, 0x02, 0x01, 0x01, 0x02, 0x10, 0x00, 0x00, 0x19, 0x00, 0x80, 0xBB, 0x00,
  0x09, 0x05, 0x82, 0x09, 0x64, 0x00, 0x01, 0x00, 0x00,
  0x07, 0x25, 0x01, 0x01, 0x02, 0x01, 0x00,
  0x09, 0x04, 0x02, 0x03, 0x01, 0x01, 0x02, 0x00, 0x00,
  0x07, 0x24, 0x01, 0x0D, 0x00, 0x01, 0x00,
  0x0E, 0x24, 0x02, 0x01, 0x02, 0x02, 0x10, 0x00, 0x00, 0x19, 0x00, 0x80, 0xBB, 0x00,
  0x09, 0x05, 0x82, 0x09, 0xC8, 0x00, 0x01, 0x00, 0x00,
  0x07, 0x25, 0x01, 0x01, 0x02, 0x01, 0x00
};

/* USB Configuration Descriptor */
/*   All Descriptors (Configuration, Interface, Endpoint, Class, Vendor */
uint8_t USB_FsConfigDescriptor[] =
{
  /* Configuration 1 */
  USB_CONFIGUARTION_DESC_SIZE,          /* bLength */
  USB_CONFIGURATION_DESCRIPTOR_TYPE,    /* bDescriptorType */
  WBVAL(                                /* wTotalLength ##pd##*/
    USB_CONFIGUARTION_DESC_SIZE         +
    USB_INTERFACE_DESC_SIZE             +
    AUDIO_CONTROL_INTERFACE_DESC_SZ(2)  +
    AUDIO_INPUT_TERMINAL_DESC_SIZE      +
    AUDIO_FEATURE_UNIT_DESC_SZ(2, 1)     +
    AUDIO_OUTPUT_TERMINAL_DESC_SIZE     +

    AUDIO_INPUT_TERMINAL_DESC_SIZE      +
    AUDIO_OUTPUT_TERMINAL_DESC_SIZE     +

    USB_INTERFACE_DESC_SIZE             +
    USB_INTERFACE_DESC_SIZE             +
    AUDIO_STREAMING_INTERFACE_DESC_SIZE +
    AUDIO_FORMAT_TYPE_I_DESC_SZ(3)      +
    AUDIO_STANDARD_ENDPOINT_DESC_SIZE   +
    AUDIO_STREAMING_ENDPOINT_DESC_SIZE  +

    USB_INTERFACE_DESC_SIZE             +
    USB_INTERFACE_DESC_SIZE             +
    AUDIO_STREAMING_INTERFACE_DESC_SIZE +
    AUDIO_FORMAT_TYPE_I_DESC_SZ(3)      +
    AUDIO_STANDARD_ENDPOINT_DESC_SIZE   +
    AUDIO_STREAMING_ENDPOINT_DESC_SIZE  +
    0

  ),
  0x03,
  0x01,                                 /* bConfigurationValue */
  0x00,                                 /* iConfiguration */
  USB_CONFIG_SELF_POWERED,              /* bmAttributes */
  USB_CONFIG_POWER_MA(8),               /* bMaxPower */
  /* Interface 0, Alternate Setting 0, Audio Control */
  USB_INTERFACE_DESC_SIZE,              /* bLength */
  USB_INTERFACE_DESCRIPTOR_TYPE,        /* bDescriptorType */
  0x00,                                 /* bInterfaceNumber */
  0x00,                                 /* bAlternateSetting */
  0x00,                                 /* bNumEndpoints */
  USB_DEVICE_CLASS_AUDIO,               /* bInterfaceClass */
  AUDIO_SUBCLASS_AUDIOCONTROL,          /* bInterfaceSubClass */
  AUDIO_PROTOCOL_UNDEFINED,             /* bInterfaceProtocol */
  0x00,                                 /* iInterface */
  /* Audio Control Interface */
  AUDIO_CONTROL_INTERFACE_DESC_SZ(2),   /* bLength */
  AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
  AUDIO_CONTROL_HEADER,                 /* bDescriptorSubtype */
  WBVAL(0x0100), /* 1.00 */             /* bcdADC */
  WBVAL(                                /* wTotalLength ##pd##*/
    AUDIO_CONTROL_INTERFACE_DESC_SZ(2) +
    AUDIO_INPUT_TERMINAL_DESC_SIZE     +
    AUDIO_FEATURE_UNIT_DESC_SZ(2, 1)    +
    AUDIO_OUTPUT_TERMINAL_DESC_SIZE    +
    AUDIO_INPUT_TERMINAL_DESC_SIZE     +
    AUDIO_OUTPUT_TERMINAL_DESC_SIZE
  ),
  0x02,                                 /* bInCollection */
  0x01,                                 /* baInterfaceNr(1) */
  0x02,                                 /* baInterfaceNr(2) */
  /* Audio Input Terminal */
  AUDIO_INPUT_TERMINAL_DESC_SIZE,       /* bLength */
  AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
  AUDIO_CONTROL_INPUT_TERMINAL,         /* bDescriptorSubtype */
  0x01,                                 /* bTerminalID */
  WBVAL(AUDIO_TERMINAL_USB_STREAMING),  /* wTerminalType */
  0x00,                                 /* bAssocTerminal */
  0x02,                                 /* bNrChannels */
  WBVAL(AUDIO_CHANNEL_L |
  AUDIO_CHANNEL_R),                     /* wChannelConfig */
  0x00,                                 /* iChannelNames */
  0x00,                                 /* iTerminal */
  /* Audio Feature Unit */
  AUDIO_FEATURE_UNIT_DESC_SZ(2, 1),     /* bLength */
  AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
  AUDIO_CONTROL_FEATURE_UNIT,           /* bDescriptorSubtype */
  0x02,                                 /* bUnitID */
  0x01,                                 /* bSourceID */
  0x01,                                 /* bControlSize */
  AUDIO_CONTROL_MUTE |
  AUDIO_CONTROL_VOLUME,                 /* bmaControls(0) */
  0x00,                                 /* bmaControls(1) */
  0x00,                                 /* bmaControls(2) */
  0x00,                                 /* iTerminal */
  /* Audio Output Terminal */
  AUDIO_OUTPUT_TERMINAL_DESC_SIZE,      /* bLength */
  AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
  AUDIO_CONTROL_OUTPUT_TERMINAL,        /* bDescriptorSubtype */
  0x03,                                 /* bTerminalID */
  WBVAL(AUDIO_TERMINAL_SPEAKER),        /* wTerminalType */
  0x00,                                 /* bAssocTerminal */
  0x02,                                 /* bSourceID */
  0x00,                                 /* iTerminal */

  /* Audio Input Terminal -microphone */
  AUDIO_INPUT_TERMINAL_DESC_SIZE,       /* bLength */
  AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
  AUDIO_CONTROL_INPUT_TERMINAL,         /* bDescriptorSubtype */
  0x04,                                 /* bTerminalID */
  WBVAL(AUDIO_TERMINAL_MICROPHONE),     /* wTerminalType */
  0x00,                                 /* bAssocTerminal */
  0x02,                                 /* bNrChannels */
  WBVAL(AUDIO_CHANNEL_L |
  AUDIO_CHANNEL_R),                     /* wChannelConfig */
  0x00,                                 /* iChannelNames */
  0x00,                                 /* iTerminal */
  /* Audio Output Terminal -microphone*/
  AUDIO_OUTPUT_TERMINAL_DESC_SIZE,      /* bLength */
  AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
  AUDIO_CONTROL_OUTPUT_TERMINAL,        /* bDescriptorSubtype */
  0x05,                                 /* bTerminalID */
  WBVAL(AUDIO_TERMINAL_USB_STREAMING),  /* wTerminalType */
  0x00,                                 /* bAssocTerminal */
  0x04,                                 /* bSourceID */
  0x00,                                 /* iTerminal */

  /* Interface 1, Alternate Setting 0, Audio Streaming - Zero Bandwith */
  USB_INTERFACE_DESC_SIZE,              /* bLength */
  USB_INTERFACE_DESCRIPTOR_TYPE,        /* bDescriptorType */
  0x01,                                 /* bInterfaceNumber */
  0x00,                                 /* bAlternateSetting */
  0x00,                                 /* bNumEndpoints */
  USB_DEVICE_CLASS_AUDIO,               /* bInterfaceClass */
  AUDIO_SUBCLASS_AUDIOSTREAMING,        /* bInterfaceSubClass */
  AUDIO_PROTOCOL_UNDEFINED,             /* bInterfaceProtocol */
  0x00,                                 /* iInterface */
  /* Interface 1, Alternate Setting 1, Audio Streaming - Operational */
  USB_INTERFACE_DESC_SIZE,              /* bLength */
  USB_INTERFACE_DESCRIPTOR_TYPE,        /* bDescriptorType */
  0x01,                                 /* bInterfaceNumber */
  0x01,                                 /* bAlternateSetting */
  0x01,                                 /* bNumEndpoints */
  USB_DEVICE_CLASS_AUDIO,               /* bInterfaceClass */
  AUDIO_SUBCLASS_AUDIOSTREAMING,        /* bInterfaceSubClass */
  AUDIO_PROTOCOL_UNDEFINED,             /* bInterfaceProtocol */
  0x00,                                 /* iInterface */
  /* Audio Streaming Interface */
  AUDIO_STREAMING_INTERFACE_DESC_SIZE,  /* bLength */
  AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
  AUDIO_STREAMING_GENERAL,              /* bDescriptorSubtype */
  0x01,                                 /* bTerminalLink */
  0x01,                                 /* bDelay */
  WBVAL(AUDIO_FORMAT_PCM),              /* wFormatTag */
  /* Audio Type I Format */
  AUDIO_FORMAT_TYPE_I_DESC_SZ(3),       /* bLength */
  AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
  AUDIO_STREAMING_FORMAT_TYPE,          /* bDescriptorSubtype */
  AUDIO_FORMAT_TYPE_I,                  /* bFormatType */
  0x02,                                 /* bNrChannels */
  0x02,                                 /* bSubFrameSize */
  16,                                   /* bBitResolution */
  0x03,                                 /* bSamFreqType */
  B3VAL(32000),                         /* tSamFreq */
  B3VAL(44100),                         /* tSamFreq */
  B3VAL(48000),                         /* tSamFreq */
  /* Endpoint - Standard Descriptor */
  AUDIO_STANDARD_ENDPOINT_DESC_SIZE,    /* bLength */
  USB_ENDPOINT_DESCRIPTOR_TYPE,         /* bDescriptorType */
  USB_ADC_OUT_EP,                       /* bEndpointAddress */
  USB_ENDPOINT_SYNC_ASYNCHRONOUS | USB_ENDPOINT_TYPE_ISOCHRONOUS,        /* bmAttributes */
  WBVAL(USB_ISO_EP_PKT_SZ),             /* wMaxPacketSize */
  0x01,                                 /* bInterval */
  0x00,                                 /* bRefresh */
  0x00,                                 /* bSynchAddress */
  /* Endpoint - Audio Streaming */
  AUDIO_STREAMING_ENDPOINT_DESC_SIZE,   /* bLength */
  AUDIO_ENDPOINT_DESCRIPTOR_TYPE,       /* bDescriptorType */
  AUDIO_ENDPOINT_GENERAL,               /* bDescriptor */
  AUDIO_CONTROL_SAMPLING_FREQ,          /* bmAttributes */
  0x00,                                 /* bLockDelayUnits */
  WBVAL(0x0000),                        /* wLockDelay */

  /* Interface 2, Alternate Setting 0, Audio Streaming - Zero Bandwith */
  USB_INTERFACE_DESC_SIZE,              /* bLength */
  USB_INTERFACE_DESCRIPTOR_TYPE,        /* bDescriptorType */
  0x02,                                 /* bInterfaceNumber */
  0x00,                                 /* bAlternateSetting */
  0x00,                                 /* bNumEndpoints */
  USB_DEVICE_CLASS_AUDIO,               /* bInterfaceClass */
  AUDIO_SUBCLASS_AUDIOSTREAMING,        /* bInterfaceSubClass */
  AUDIO_PROTOCOL_UNDEFINED,             /* bInterfaceProtocol */
  0x00,                                 /* iInterface */
  /* Interface 2, Alternate Setting 1, Audio Streaming - Operational */
  USB_INTERFACE_DESC_SIZE,              /* bLength */
  USB_INTERFACE_DESCRIPTOR_TYPE,        /* bDescriptorType */
  0x02,                                 /* bInterfaceNumber */
  0x01,                                 /* bAlternateSetting */
  0x01,                                 /* bNumEndpoints */
  USB_DEVICE_CLASS_AUDIO,               /* bInterfaceClass */
  AUDIO_SUBCLASS_AUDIOSTREAMING,        /* bInterfaceSubClass */
  AUDIO_PROTOCOL_UNDEFINED,             /* bInterfaceProtocol */
  0x00,                                 /* iInterface */
  /* Audio Streaming Interface */
  AUDIO_STREAMING_INTERFACE_DESC_SIZE,  /* bLength */
  AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
  AUDIO_STREAMING_GENERAL,              /* bDescriptorSubtype */
  0x05,                                 /* bTerminalLink */
  0x01,                                 /* bDelay */
  WBVAL(AUDIO_FORMAT_PCM),              /* wFormatTag */
  /* Audio Type I Format */
  AUDIO_FORMAT_TYPE_I_DESC_SZ(3),       /* bLength */
  AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */
  AUDIO_STREAMING_FORMAT_TYPE,          /* bDescriptorSubtype */
  AUDIO_FORMAT_TYPE_I,                  /* bFormatType */
  0x02,                                 /* bNrChannels */
  0x02,                                 /* bSubFrameSize */
  16,                                   /* bBitResolution */
  0x03,                                 /* bSamFreqType */
  B3VAL(32000),                         /* tSamFreq */
  B3VAL(44100),                         /* tSamFreq */
  B3VAL(48000),                         /* tSamFreq */
  /* Endpoint - Standard Descriptor */
  AUDIO_STANDARD_ENDPOINT_DESC_SIZE,    /* bLength */
  USB_ENDPOINT_DESCRIPTOR_TYPE,         /* bDescriptorType */
  USB_ADC_IN_EP,                        /* bEndpointAddress */
  USB_ENDPOINT_SYNC_ASYNCHRONOUS | USB_ENDPOINT_TYPE_ISOCHRONOUS,        /* bmAttributes */
  WBVAL(USB_ISO_EP_PKT_SZ),             /* wMaxPacketSize */
  0x01,                                 /* bInterval */
  0x00,                                 /* bRefresh */
  0x00,                                 /* bSynchAddress */
  /* Endpoint - Audio Streaming */
  AUDIO_STREAMING_ENDPOINT_DESC_SIZE,   /* bLength */
  AUDIO_ENDPOINT_DESCRIPTOR_TYPE,       /* bDescriptorType */
  AUDIO_ENDPOINT_GENERAL,               /* bDescriptor */
  AUDIO_CONTROL_SAMPLING_FREQ,          /* bmAttributes */
  0x00,                                 /* bLockDelayUnits */
  WBVAL(0x0000),                        /* wLockDelay */
  /* Terminator */
  0                                     /* bLength */
};

const uint8_t* USB_ConfigDescriptors[2] =
{
  (uint8_t*)&USB_FsConfigDescriptor[0],
  (uint8_t*)&USB_FsConfigDescriptor[0]
};

/* USB String Descriptor (optional) */
const uint8_t USB_StringDescriptor[] =
{
  /* Index 0x00: LANGID Codes */
  0x04,                              /* bLength */
  USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
  WBVAL(0x0409), /* US English */    /* wLANGID */
  /* Index 0x01: Manufacturer */
  (18*2 + 2),                              /* bLength */
  USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
  'N', 0,
  'X', 0,
  'P', 0,
  ' ', 0,
  'S', 0,
  'e', 0,
  'm', 0,
  'i', 0,
  'c', 0,
  'o', 0,
  'n', 0,
  'd', 0,
  'u', 0,
  'c', 0,
  't', 0,
  'o', 0,
  'r', 0,
  's', 0,
  /* Index 0x02: Product */
  (20*2 + 2),                              /* bLength */
  USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
  'N', 0,
  'X', 0,
  'P', 0,
  ' ', 0,
  'R', 0,
  'e', 0,
  'f', 0,
  'e', 0,
  'r', 0,
  'e', 0,
  'n', 0,
  'c', 0,
  'e', 0,
  'S', 0,
  'p', 0,
  'e', 0,
  'a', 0,
  'k', 0,
  'e', 0,
  'r', 0,
  /* Index 0x03: Serial Number */
  (10*2 + 2),                              /* bLength */
  USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
  'D', 0,
  'E', 0,
  'M', 0,
  'O', 0,
  '4', 0,
  '4', 0,
  '.', 0,
  '1', 0,
  'K', 0,
  'S', 0,
};

/* USB Device Qualifier */
const UNS_8 USB_DeviceQualifier[] =
{
  USB_DEVICE_QUALI_SIZE,             	/* bLength */
  USB_DEVICE_QUALIFIER_DESCRIPTOR_TYPE,	/* bDescriptorType */
  WBVAL(0x0200), /* 2.00 */          /* bcdUSB */
  0x00,                              /* bDeviceClass */
  0x00,                              /* bDeviceSubClass */
  0x00,                              /* bDeviceProtocol */
  USB_MAX_PACKET0,                   /* bMaxPacketSize0 */
  0x01,                              /* bNumOtherSpeedConfigurations */
  0x00                               /* bReserved */
};


/*----------------------------------------------------------------------------
 * U S B  -  K e r n e l
 *----------------------------------------------------------------------------
 * Name:    usbdesc.c
 * Purpose: USB Descriptors
 * Version: V1.20
 *----------------------------------------------------------------------------
 * This software is supplied "AS IS" without any warranties, express,
 * implied or statutory, including but not limited to the implied
 * warranties of fitness for purpose, satisfactory quality and
 * noninfringement. Keil extends you a royalty-free right to reproduce
 * and distribute executable files created using this software for use
 * on NXP ARM microcontroller devices only. Nothing else gives
 * you the right to use this software.
 *
 * Copyright (c) 2005-2009 Keil Software.
 * Adaption to LPCxxxx, Copyright (c) 2010 NXP.
 *----------------------------------------------------------------------------*/

#include "lpc_usb.h"
#include "usbcfg.h"
#include "usbdesc.h"


/* USB Standard Device Descriptor */
const UNS_8 USB_DeviceDescriptor[] =
{
  USB_DEVICE_DESC_SIZE,              /* bLength */
  USB_DEVICE_DESCRIPTOR_TYPE,        /* bDescriptorType */
  WBVAL(0x0200), /* 2.00 */          /* bcdUSB */
  0x00,                              /* bDeviceClass */
  0x00,                              /* bDeviceSubClass */
  0x00,                              /* bDeviceProtocol */
  USB_MAX_PACKET0,                   /* bMaxPacketSize0 */
  WBVAL(0x1FC9),                     /* idVendor */
  WBVAL(0x0004),                     /* idProduct */
  WBVAL(0x0100), /* 1.00 */          /* bcdDevice */
  0x01,                              /* iManufacturer */
  0x02,                              /* iProduct */
  0x03,                              /* iSerialNumber */
  0x01                               /* bNumConfigurations */
};

/* USB FSConfiguration Descriptor */
/*   All Descriptors (Configuration, Interface, Endpoint, Class, Vendor */
const UNS_8 USB_FsConfigDescriptor[] =
{
  /* Configuration 1 */
  USB_CONFIGUARTION_DESC_SIZE,       /* bLength */
  USB_CONFIGURATION_DESCRIPTOR_TYPE, /* bDescriptorType */
  WBVAL(                             /* wTotalLength */
    1*USB_CONFIGUARTION_DESC_SIZE +
    1*USB_INTERFACE_DESC_SIZE     +
    4*USB_ENDPOINT_DESC_SIZE
  ),
  0x01,                              /* bNumInterfaces */
  0x01,                              /* bConfigurationValue */
  0x00,                              /* iConfiguration */
  USB_CONFIG_BUS_POWERED,           /* bmAttributes */
  USB_CONFIG_POWER_MA(100),          /* bMaxPower */
  /* Interface 0, Alternate Setting 0, Custom Class */
  USB_INTERFACE_DESC_SIZE,           /* bLength */
  USB_INTERFACE_DESCRIPTOR_TYPE,     /* bDescriptorType */
  0x00,                              /* bInterfaceNumber */
  0x00,                              /* bAlternateSetting */
  0x04,                              /* bNumEndpoints */
  0xFF,                 /* bInterfaceClass */
  0xF0,                 /* bInterfaceSubClass */
  0x00,                 /* bInterfaceProtocol */
  0x04,                              /* iInterface */
  /* EP1 Bulk Out custom protocol Endpoint */
  USB_ENDPOINT_DESC_SIZE,            /* bLength */
  USB_ENDPOINT_DESCRIPTOR_TYPE,      /* bDescriptorType */
  USB_ENDPOINT_OUT(1),               /* bEndpointAddress */
  USB_ENDPOINT_TYPE_BULK,            /* bmAttributes */
  WBVAL(64),                         /* wMaxPacketSize */
  0,                                 /* bInterval */
  /* EP1 Bulk In custom protocol Endpoint */
  USB_ENDPOINT_DESC_SIZE,            /* bLength */
  USB_ENDPOINT_DESCRIPTOR_TYPE,      /* bDescriptorType */
  USB_ENDPOINT_IN(1),                /* bEndpointAddress */
  USB_ENDPOINT_TYPE_BULK,            /* bmAttributes */
  WBVAL(64),                         /* wMaxPacketSize */
  0,                                 /* bInterval */
  /* Video frame buffer Bulk OUT Endpoint */
  USB_ENDPOINT_DESC_SIZE,            /* bLength */
  USB_ENDPOINT_DESCRIPTOR_TYPE,      /* bDescriptorType */
  USB_ENDPOINT_OUT(2),               /* bEndpointAddress */
  USB_ENDPOINT_TYPE_BULK,            /* bmAttributes */
  WBVAL(64),                         /* wMaxPacketSize */
  0,                                 /* bInterval */
  /* Video frame buffer Bulk IN Endpoint */
  USB_ENDPOINT_DESC_SIZE,            /* bLength */
  USB_ENDPOINT_DESCRIPTOR_TYPE,      /* bDescriptorType */
  USB_ENDPOINT_IN(2),               /* bEndpointAddress */
  USB_ENDPOINT_TYPE_BULK,            /* bmAttributes */
  WBVAL(64),                         /* wMaxPacketSize */
  0,                                 /* bInterval */
  /* Terminator */
  0                                  /* bLength */
};

/* USB HSConfiguration Descriptor */
/*   All Descriptors (Configuration, Interface, Endpoint, Class, Vendor */
const UNS_8 USB_HsConfigDescriptor[] =
{
  /* Configuration 1 */
  USB_CONFIGUARTION_DESC_SIZE,       /* bLength */
  USB_CONFIGURATION_DESCRIPTOR_TYPE, /* bDescriptorType */
  WBVAL(                             /* wTotalLength */
    1*USB_CONFIGUARTION_DESC_SIZE +
    1*USB_INTERFACE_DESC_SIZE     +
    4*USB_ENDPOINT_DESC_SIZE
  ),
  0x01,                              /* bNumInterfaces */
  0x01,                              /* bConfigurationValue */
  0x00,                              /* iConfiguration */
  USB_CONFIG_BUS_POWERED /*|*/       /* bmAttributes */
  /*USB_CONFIG_REMOTE_WAKEUP*/,
  USB_CONFIG_POWER_MA(100),          /* bMaxPower */
  /* Interface 0, Alternate Setting 0, Custom Class */
  USB_INTERFACE_DESC_SIZE,           /* bLength */
  USB_INTERFACE_DESCRIPTOR_TYPE,     /* bDescriptorType */
  0x00,                              /* bInterfaceNumber */
  0x00,                              /* bAlternateSetting */
  0x04,                              /* bNumEndpoints */
  0xFF,                 /* bInterfaceClass */
  0xF0,                 /* bInterfaceSubClass */
  0x00,                 /* bInterfaceProtocol */
  0x04,                              /* iInterface */
  /* EP1 Bulk Out custom protocol Endpoint */
  USB_ENDPOINT_DESC_SIZE,            /* bLength */
  USB_ENDPOINT_DESCRIPTOR_TYPE,      /* bDescriptorType */
  USB_ENDPOINT_OUT(1),               /* bEndpointAddress */
  USB_ENDPOINT_TYPE_BULK,            /* bmAttributes */
  WBVAL(512),                         /* wMaxPacketSize */
  0,                                 /* bInterval */
  /* EP1 Bulk In custom protocol Endpoint */
  USB_ENDPOINT_DESC_SIZE,            /* bLength */
  USB_ENDPOINT_DESCRIPTOR_TYPE,      /* bDescriptorType */
  USB_ENDPOINT_IN(1),                /* bEndpointAddress */
  USB_ENDPOINT_TYPE_BULK,            /* bmAttributes */
  WBVAL(512),                         /* wMaxPacketSize */
  0,                                 /* bInterval */
  /* Video frame buffer Bulk OUT Endpoint */
  USB_ENDPOINT_DESC_SIZE,            /* bLength */
  USB_ENDPOINT_DESCRIPTOR_TYPE,      /* bDescriptorType */
  USB_ENDPOINT_OUT(2),               /* bEndpointAddress */
  USB_ENDPOINT_TYPE_BULK,            /* bmAttributes */
  WBVAL(512),                         /* wMaxPacketSize */
  0,                                 /* bInterval */
  /* Video frame buffer Bulk IN Endpoint */
  USB_ENDPOINT_DESC_SIZE,            /* bLength */
  USB_ENDPOINT_DESCRIPTOR_TYPE,      /* bDescriptorType */
  USB_ENDPOINT_IN(2),               /* bEndpointAddress */
  USB_ENDPOINT_TYPE_BULK,            /* bmAttributes */
  WBVAL(512),                         /* wMaxPacketSize */
  0,                                 /* bInterval */
  /* Terminator */
  0                                  /* bLength */
};

const uint8_t* USB_ConfigDescriptors[2] =
{
  (uint8_t*)USB_FsConfigDescriptor,
  (uint8_t*)USB_HsConfigDescriptor
};

/* USB String Descriptor (optional) */
const UNS_8 USB_StringDescriptor[] =
{
  /* Index 0x00: LANGID Codes */
  0x04,                              /* bLength */
  USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
  WBVAL(0x0409), /* US English */    /* wLANGID */
  /* Index 0x01: Manufacturer */
  (18*2 + 2),                        /* bLength (18 Char + Type + lenght) */
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
  (11*2 + 2),                        /* bLength (11 Char + Type + lenght) */
  USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
  'U', 0,
  'S', 0,
  'B', 0,
  ' ', 0,
  'D', 0,
  'i', 0,
  's', 0,
  'p', 0,
  'l', 0,
  'a', 0,
  'y', 0,
  /* Index 0x03: Serial Number */
  (13*2 + 2),                        /* bLength (13 Char + Type + lenght) */
  USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
  'A', 0,
  'B', 0,
  'C', 0,
  'D', 0,
  '1', 0,
  '2', 0,
  '3', 0,
  '4', 0,
  '5', 0,
  '6', 0,
  '7', 0,
  '8', 0,
  '9', 0,
  /* Index 0x04: Interface 0, Alternate Setting 0 */
  (20*2 + 2),                        /* bLength (15 Char + Type + lenght) */
  USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
  'L', 0,
  'P', 0,
  'C', 0,
  'D', 0,
  'i', 0,
  's', 0,
  'p', 0,
  'l', 0,
  'a', 0,
  'y', 0,
  ' ', 0,
  'I', 0,
  'n', 0,
  't', 0,
  'e', 0,
  'r', 0,
  'f', 0,
  'a', 0,
  'c', 0,
  'e', 0,
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



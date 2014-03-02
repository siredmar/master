#ifndef PLATFORM_TYPES_H
#define PLATFORM_TYPES_H


#define FALSE 			0
#define TRUE 			1

#define MASK_8BIT_UI8		0xFF
#define MASK_8BIT_HI_UI8	0xF0
#define MASK_8BIT_LO_UI8	0x0F
#define MASK_16BIT_LO_UI16	0x00FF
#define MASK_16BIT_HI_UI16	0xFF00
#define MASK_32BIT_1BYTE_UI32	0x000000FF
#define MASK_32BIT_2BYTE_UI32	0x0000FF00
#define MASK_32BIT_3BYTE_UI32	0x00FF0000
#define MASK_32BIT_4BYTE_UI32	0xFF000000

#define RGB565_RED_UI16		0b1111100000000000
#define RGB565_BLUE_UI16	0b0000011111100000
#define RGB565_GREEN_UI16	0b0000000000011111

#define HARDWARE_BOARD_REV 1 /* 1 new, 0 old */

typedef unsigned char      boolean;
typedef signed char        sint8;
typedef unsigned char      uint8;
typedef signed short       sint16;
typedef unsigned short     uint16;
typedef signed long        sint32;
typedef unsigned long int  uint32;
typedef signed long long   sint64;
typedef unsigned long long uint64;
typedef float              float32;
typedef double             float64;






#endif /* PLATFORM_TYPES_H */


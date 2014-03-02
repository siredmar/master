/*
 * filter_types.h
 *
 *  Created on: Jan 26, 2014
 *      Author: armin
 */

#include "../inc/std_types.h"

#ifndef FILTER_TYPES_H_
#define FILTER_TYPES_H_


typedef struct
{
   float R;
   float G;
   float B;
} image_rgb888float_type;

typedef struct
{
   uint8 R;
   uint8 G;
   uint8 B;
} image_rgb888int_type;

typedef struct
{
   uint16 xres;
   uint16 yres;
   uint16 *content;
   uint16 x_offset;
   uint16 y_offset;

} framebuffer_type;

typedef struct
{
   uint16 x_dimension;
   uint16 y_dimension;
   float *coeff;
   float inv_normalization;
   uint8 *name;
}filter_type;

#endif /* FILTER_TYPES_H_ */

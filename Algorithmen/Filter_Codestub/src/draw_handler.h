/*
 * draw_handler.h
 *
 *  Created on: Jan 26, 2014
 *      Author: armin
 */

#ifndef DRAW_HANDLER_H_
#define DRAW_HANDLER_H_

#include "../inc/std_types.h"
#include "../inc/filter_types.h"

void
drawPixel (SDL_Surface * screen, int x, int y, image_rgb888int_type rgb);

#endif /* DRAW_HANDLER_H_ */

/*
 * draw_handler.c
 *
 *  Created on: Jan 26, 2014
 *      Author: armin
 */
#include "draw_handler.h"


void
drawPixel (SDL_Surface * screen, int x, int y, image_rgb888int_type rgb)
{
   uint8 R = rgb.R;
   uint8 G = rgb.G;
   uint8 B = rgb.B;

   uint32 color = SDL_MapRGB (screen->format, R, G, B);
   if (SDL_MUSTLOCK (screen))
   {
      if (SDL_LockSurface (screen) < 0)
      {
         return;
      }
   }
   switch (screen->format->BytesPerPixel)
   {
   case 1:
   {           /* vermutlich 8 Bit */
      uint8 *bufp;
      bufp = (uint8 *) screen->pixels + y * screen->pitch + x;
      *bufp = color;
   }
   break;
   case 2:
   {           /* vermutlich 15 Bit oder 16 Bit */
      uint16 *bufp;
      bufp = (uint16 *) screen->pixels + y * screen->pitch / 2 + x;
      *bufp = color;
   }
   break;
   case 3:
   {           /* langsamer 24-Bit-Modus, selten verwendet */
      uint8 *bufp;
      bufp = (uint8 *) screen->pixels + y * screen->pitch + x * 3;
      if (SDL_BYTEORDER == SDL_LIL_ENDIAN)
      {
         bufp[0] = color;
         bufp[1] = color >> 8;
         bufp[2] = color >> 16;
      }
      else
      {
         bufp[2] = color;
         bufp[1] = color >> 8;
         bufp[0] = color >> 16;
      }
   }
   break;
   case 4:
   {           /* vermutlich 32 Bit */
      uint32 *bufp;
      bufp = (uint32 *) screen->pixels + y * screen->pitch / 4 + x;
      *bufp = color;
   }
   break;
   }
   if (SDL_MUSTLOCK (screen))
   {
      SDL_UnlockSurface (screen);
   }
}


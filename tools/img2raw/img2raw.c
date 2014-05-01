/* img2raw - convert image to framebuffer format   */
/* Author: Armin Schlegel (c), 05/01/2014          */
/* This program is licensed under the GNU GPL V2   */

#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#define SCREEN_DEPTH 16
typedef unsigned short framebuffer_type;

int
main (int argc, char *argv[])
{
   SDL_Surface *screen, *infile;
   int x, y, i;
   FILE *outfile;

   if (argc < 3 || argc > 3)
   {
      printf ("Usage: img2raw infile outfile [RGB_FORMAT]\n");
      printf ("Convert an image to RGB565 framebuffer format.\n");
      printf ("Armin Schlegel (C), 2014\n");
      return -1;
   }

   infile = IMG_Load (argv[1]);
   if (!infile)
   {
      printf ("Error: Cannot open %s\n", argv[1]);
      return -1;
   }

   printf ("Image width: %d, Image height: %d\n", infile->w, infile->h);

   outfile = fopen (argv[2], "w+b");
   if (!outfile)
   {
      printf ("Error: Cannot open %s\n", argv[2]);
      return -1;
   }

   if (SDL_Init (SDL_INIT_VIDEO) == -1)
   {
      printf ("Cannot initialize SDL\n");
      return -1;
   }

   screen = SDL_SetVideoMode (infile->w, infile->h, SCREEN_DEPTH, SDL_SWSURFACE);
   if (!screen)
   {
      printf ("Cannot create SDL window\n");

      return -1;
   }

   SDL_WM_SetCaption ("img2raw", "img2raw");

   SDL_BlitSurface(infile, 0, screen, 0);
   SDL_Flip(screen);

   if(SDL_MUSTLOCK(screen))
      SDL_LockSurface(screen);

   for(i = 0; i < (screen->w * screen->h); i++)
   {
      framebuffer_type *pixels = (framebuffer_type*)screen->pixels;
      x = i % screen->w;
      y =  i / screen->w;
      fwrite(&pixels[(y * screen->w) + x], 2, 1, outfile);
   }

   if(SDL_MUSTLOCK(screen))
      SDL_UnlockSurface(screen);


   fclose(outfile);


   return 0;
}


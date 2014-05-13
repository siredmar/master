/* img2raw - convert image to framebuffer format   */
/* Author: Armin Schlegel (c), 05/01/2014          */
/* This program is licensed under the GNU GPL V2   */

#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#define SCREEN_DEPTH 16
typedef unsigned short framebuffer_type;

typedef enum
{
   RAW_FORMAT = 0,
   C_FORMAT = 1
}outputFormatType;

int hexToAsciiHex(unsigned short hex, char *buffer)
{
   int nibble;
   char byte;
   int nibblepos = 0;

   buffer[0] = '0';
   buffer[1] = 'x';

   for(nibble = 3; nibble >= 0; nibble--)
   {
      byte = hex >> (4 * nibble) & 0x0F;
      if(byte >= 0 && byte <= 9)
         buffer[2 + nibblepos] = byte + 48;
      if(byte >= 10 && byte <= 15)
         buffer[2 + nibblepos] = byte + 55;
      nibblepos++;
   }
   buffer[6] = '\0';
   return 1;
}



int
main (int argc, char *argv[])
{
   SDL_Surface *screen, *infile;
   int x, y, i;
   FILE *outfile;
   char buffer[255];
   outputFormatType format = RAW_FORMAT;

   if (argc < 3 || argc > 4)
   {
      printf ("Usage: img2raw infile outfile [OUTPUT_FORMAT]\n");
      printf ("Convert an image to RGB565 framebuffer format.\n");
      printf ("OUTPUT_FORMAT: raw - creates raw file - default\n");
      printf ("               c   - creates C header file\n");
      printf ("Armin Schlegel (C), 2014\n");
      return -1;
   }

   if(argc == 3)
   {
      format = RAW_FORMAT;
   }
   else
   {
      if(!strcmp(argv[3], "c"))
      {
         format = C_FORMAT;
      }
      else
      {
         format = RAW_FORMAT;
      }
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

   if(format == RAW_FORMAT)
   {


      for(i = 0; i < (screen->w * screen->h); i++)
      {
         framebuffer_type *pixels = (framebuffer_type*)screen->pixels;
         x = i % screen->w;
         y =  i / screen->w;
         fwrite(&pixels[(y * screen->w) + x], 2, 1, outfile);
      }

      if(SDL_MUSTLOCK(screen))
         SDL_UnlockSurface(screen);

   }
   else if (format == C_FORMAT)
   {
      sprintf(buffer, "/* Image in RGB565 format, width: %d, height: %d */\n\n", screen->w, screen->h);
      fwrite(buffer, strlen(buffer), 1, outfile);

      sprintf(buffer, "#ifndef IMAGE_H_\n#define IMAGE_H_\n\n");
      fwrite(buffer, strlen(buffer), 1, outfile);

      sprintf(buffer, "u16 image[] = {%d, %d, \n", screen->w, screen->h);
      fwrite(buffer, strlen(buffer), 1, outfile);

      for(i = 0; i < (screen->w * screen->h); i++)
      {

         framebuffer_type *pixels = (framebuffer_type*)screen->pixels;
         x = i % screen->w;
         y =  i / screen->w;

         if(i % 8 == 0 && i > 7)
         {
            sprintf(buffer, "\n");
            fwrite(buffer, strlen(buffer), 1, outfile);
         }
         hexToAsciiHex(pixels[(y * screen->w)] + x, buffer);
         fwrite(buffer, strlen(buffer), 1, outfile);
         if(i == (screen->w * screen->h) -1)
            break;
         sprintf(buffer, ", ");
         fwrite(buffer, strlen(buffer), 1, outfile);
      }

      sprintf(buffer, "};\n\n");
      fwrite(buffer, strlen(buffer), 1, outfile);
      sprintf(buffer, "#endif /* IMAGE_H_ */");
      fwrite(buffer, strlen(buffer), 1, outfile);

   }
   fclose(outfile);
   return 0;
}


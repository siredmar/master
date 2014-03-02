/*
 * event_handler.c
 *
 *  Created on: Jan 26, 2014
 *      Author: armin
 */
#include "../inc/std_types.h"
#include "event_handler.h"


boolean main_loop_handle()//SDL_Event event)
{
  boolean quit = FALSE;
  int key_pressed = 0;
  SDL_Event event;
   while (SDL_PollEvent (&event))
   {
      switch (event.type)
      {
      case SDL_KEYDOWN:
         key_pressed = event.key.keysym.sym;
         printf("Key pressed\n");
         //printf(" Name: %s\n", key_pressed);
         if (key_pressed == SDLK_ESCAPE)
            printf("ESC pressed\n");
            quit = TRUE;
         break;
      case SDL_QUIT: // SDL_QUIT int ein schliessen des windows
         quit = TRUE;
         break;
      default:
         break;
      }
   }
return quit;
}

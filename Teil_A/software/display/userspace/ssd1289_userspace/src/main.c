//#include "../inc/std_types.h"
//
//#include "dio/dio.h"
//#include "tft/tft.h"
//
//#include <time.h>
//#include <stdlib.h>
//#include <string.h>
//
//
///* LPC3131 UM chapter 15 */
//#define IOCONFIG    0x13003000
//#define CLK_REG		0x1300442C
//#define wh  160560
//
//#include <time.h>
//
//
//uint8 debug_output;
//
//uint16 getRGB565Rainbow()
//{
//   sint8 states[6][3] = { { 0, +1,  0}, // rot auf gelb: init R=255,G=0,B=0
//         {-1,  0,  0},
//         { 0,  0,  1},
//         { 0, -1,  0},
//         { 1,  0,  0},
//         { 0,  0, -1}};
//
//   static uint8 R = 255;
//   static uint8 G = 0;
//   static uint8 B = 0;
//   static int state = 0;
//   static uint8 cnt = 0;
//
//   R += states[state][0];
//   G += states[state][1];
//   B += states[state][2];
//
//   if(cnt >= 254)
//   {
//      cnt = 0;
//      if(state < 5)
//         state++;
//      else
//         state = 0;
//   }
//   else
//   {
//      cnt++;
//   }
//  return  (uint16)((((((uint16)R) << 8) & 0xF800)) + ((((uint16)G) << 3) & 0x07E0) + ((((uint16)B) >> 3) & 0x001F));
//}
//
//int main(int argc, const char *argv[])
//{
//  unsigned short xs = 0, xe = 320, ys = 0, ye = 240;
//
//
//
//   if(argc < 2){
//      printf("usage: \ngnublin_ssd1963 xs ys width height init|noinit 0|1|2 color\n");
//      printf("xs: start of rect x\n");
//      printf("ys: start of rect y\n");
//      printf("width: width of rect\n");
//      printf("height: height of rect\n");
//      printf("init|noinit: if initialization is run\n");
//      printf("0|1|2: normal, single step, timeout\n");
//      printf("color: background color\n");
//      exit(1);
//   }
//
//   xs = atoi(argv[1]);
//   ys = atoi(argv[2]);
//   xe = atoi(argv[3]);
//   ye = atoi(argv[4]);
//   debug_output = atoi(argv[6]);
//
//   printf("xs: %d\n", xs);
//   printf("ys: %d\n", ys);
//   printf("xe: %d\n", xe);
//   printf("ye: %d\n", ye);
//
//   dio_init();
//   if(!strcmp(argv[5], "init")){
//
//      tft_init(1);
//   }
//   else
//   {
//      tft_init(0);
//   }
//
//   tft_drawRectangle2(0, 0, 240, 320, atoi(argv[7]));
//
//   while(1)
//   {
//      tft_drawRectangle(xs, ys, xe, ye, getRGB565Rainbow());
//   }
//
//  return 0;
//}



#include "../inc/std_types.h"
#include "dio/dio.h"
#include "tft/tft.h"

#define BENCHMARK FALSE
#if BENCHMARK == TRUE
   #include <time.h>
   #define CYCLES 250
#endif

#define MAIN   5

sint32 main (sint32 * argc, uint8 * argv[])
{
   dio_init ();
   tft_init (1);
//   tft_selectBacklight ();
   tft_clearScreen (BLACK);

#if BENCHMARK == TRUE

   struct timeval start, stop;
   double start_mil;
   double stop_mil;
   double time_diff;

   uint16 cmp = 0;

   gettimeofday (&start, NULL);
   while (cmp < CYCLES)
   {
      cmp++;
      tft_clearScreen (cmp);
   }
   gettimeofday (&stop, NULL);

   start_mil = (start.tv_sec) * 1000 + (start.tv_usec) / 1000; // convert tv_sec & tv_usec to millisecond
   stop_mil = (stop.tv_sec) * 1000 + (stop.tv_usec) / 1000; // convert tv_sec & tv_usec to millisecond
   time_diff = stop_mil - start_mil;

   printf ("Finished in about %f seconds. \n", time_diff);
   printf ("%d Frames sent in %f seconds. -> FPS: %f\n", cmp, time_diff,
         (cmp / (time_diff / 1000)));
#endif

#if BENCHMARK == FALSE
   /* Use MAIN 1 for fullscreen updates! */
#if MAIN == 1
   uint16 buf_display[TFT_WIDTH_UI16 * TFT_HEIGHT_UI16 * 2];   // the current display content
   uint16 buf_source[TFT_WIDTH_UI16 * TFT_HEIGHT_UI16 * 2]; // the source framebuffer content
   FILE *fb;
   int i, j;

   fb = fopen (argv[1], "rb");
   // run forever...
   while (1)
   {
      fseek (fb, 0, SEEK_SET);   // rewind source framebuffer
      fread (buf_source, 1, TFT_WIDTH_UI16 * TFT_HEIGHT_UI16 * 2, fb);  // read complete source framebuffer


      tft_setWindow (0, 0, TFT_WIDTH_UI16 - 1, TFT_HEIGHT_UI16 - 1);
      tft_drawStart ();
      // re-transmit the unchanged pixels of current row up to here
      for (j = 0; j < TFT_WIDTH_UI16 * TFT_HEIGHT_UI16; j++)
      {
         tft_sendPixelData (buf_source[j]);
      }




      usleep (5000L);      // sleep for 5ms
   }

   // actually we never get here
   fclose (fb);
#endif

   /* Use MAIN 2 for line based update */
#if MAIN == 2
   int y, x, j, i;

   // open the source framebuffer deviarm-linux-gnueabihf-gcc srce
   FILE *fb = fopen (argv[1], "rb");

   // allocate two buffers for tracking the source framebuffer changes
   uint16 buf_display[TFT_WIDTH_UI16 * TFT_HEIGHT_UI16 * 2];   // the current display content
   uint16 buf_source[TFT_WIDTH_UI16 * TFT_HEIGHT_UI16 * 2]; // the source framebuffer content

   // run forever...
   while (TRUE)
   {
      fseek (fb, 0, SEEK_SET);   // rewind source framebuffer
      fread (buf_source, 1, TFT_WIDTH_UI16 * TFT_HEIGHT_UI16 * 2, fb);  // read complete source framebuffer

      // statistics
      uint32 changed = 0;  // how many pixels have changed since last refresh
      uint32 drawed = 0;   // how many pixel actualy where transmitted since last refresh

      // state for merging adjacent changes into one transmission
      uint32 i = 0;     // current pixel index
      boolean drawing = FALSE;   // if pixels are currently transmitted
      uint32 lastChange = 0;  // index of first pixel of this transmission

      // for all pixels on screen...
      for (y = 0; y < TFT_HEIGHT_UI16; y++)
      {
         for (x = 0; x < TFT_WIDTH_UI16; x++)
         {
            uint16 color = buf_source[i]; // get this pixels source color
            if (color != buf_display[i])
            {
               // the pixel has changed
               changed++;
               lastChange = i;   // store last change index
               if (!drawing)
               {
                  // whe are not drawing, so start a run here.
                  // we only draw full rows:
                  // set display area to the rest of the screen, including this row
                  tft_setWindow (0, y, TFT_WIDTH_UI16 - 1,
                        TFT_HEIGHT_UI16 - 1);
                  tft_drawStart ();
                  // re-transmit the unchanged pixels of current row up to here
                  for (j = 0; j < x; j++)
                     tft_sendPixelData (buf_display
                           [y * TFT_WIDTH_UI16 + j]);
                  drawing = TRUE;
               }
            }

            if (drawing)
            {
               // where in a transmission run.
               tft_sendPixelData (color); // send pixel
               buf_display[i] = color; // store change
               drawed++;
               // end transmission, if the was no change for some time
               if (i > lastChange + 400)
               {
                  // stop drawing, lcd actually gets updated.
                  tft_drawStop ();
                  drawing = FALSE;
               }
            }

            i++;
         }
      }
      // if(changed>0) printf("Pixels changed: %i, drawed: %i\n",changed,drawed);
      usleep (10000L);     // sleep for 10ms
   }

   // actually we never get here
   fclose (fb);
#endif

   /* this routine sends every pixel by itself */
#if MAIN == 3
   int y, x, j, i;

   // open the source framebuffer device
   FILE *fb = fopen (argv[1], "rb");

   // allocate two buffers for tracking the source framebuffer changes
   uint16 buf_display[TFT_WIDTH_UI16 * TFT_HEIGHT_UI16 * 2];   // the current display content
   uint16 buf_source[TFT_WIDTH_UI16 * TFT_HEIGHT_UI16 * 2]; // the source framebuffer content

   // run forever...
   while (TRUE)
   {
      fseek (fb, 0, SEEK_SET);   // rewind source framebuffer
      fread (buf_source, 1, TFT_WIDTH_UI16 * TFT_HEIGHT_UI16 * 2, fb);  // read complete source framebuffer

      // statistics
      uint32 changed = 0;  // how many pixels have changed since last refresh
      uint32 drawed = 0;   //for(n = 0; n < f_size; n++)
      //  {
      //     printf("old_buffer[%d]: %c \t new_buffer[%d]: %c\n", n, old_buffer[n], n, new_buffer[n]);
      //  }/ how many pixel actualy where transmitted since last refresh

      // state for merging adjacent changes into one transmission
      uint32 i = 0;     // current pixel index
      boolean drawing = FALSE;   // if pixels are currently transmitted
      uint32 lastChange = 0;  // index of first pixel of this transmission
      uint16 color = 0;

      // for all pixels on screen...
      for (y = 0; y < TFT_HEIGHT_UI16; y++)
      {
         for (x = 0; x < TFT_WIDTH_UI16; x++)
         {
            color = buf_source[i];
            if (color != buf_display[i])
            {
               buf_display[i] = color;
               changed++;
               tft_drawPixel (x, y, color);
            }
            i++;
         }
      }
      //printf("pixel changed: %d\n", changed);
      // if(changed>0) printf("Pixels changed: %i, drawed: %i\n",changed,drawed);
      usleep (10000L);     // sleep for 10ms
   }

   // actually we never get here
   fclose (fb);
#endif

   /* This Routine sends a group of pixels after one has altered */
#if MAIN == 5
#define PIXELGROUPLEN 40
   int y, x, j, i, xend;

   // open the source framebuffer device
   FILE *fb = fopen (argv[1], "rb");

   // allocate two buffers for tracking the source framebuffer changes
   uint16 buf_display[TFT_HEIGHT_UI16][TFT_WIDTH_UI16];  // the current display content
   uint16 buf_source[TFT_HEIGHT_UI16][TFT_WIDTH_UI16];   // the source framebuffer content

   for (i = 0; i < TFT_HEIGHT_UI16; i++)
   {
      for (j = 0; j < TFT_WIDTH_UI16; j++)
      {
         buf_display[i][j] = 0;
      }
   }
   // run forever...
   while (TRUE)
   {

      /* This can be speed up a little by using DMA or memcopy */
      fseek (fb, 0, SEEK_SET);   // rewind source framebuffer
      //for (i = 0; i < TFT_HEIGHT_UI16; i++)
         //fread(buf_source[i], 2, TFT_WIDTH_UI16, fb);  // read complete source framebuffer


      for (y = 0; y < TFT_HEIGHT_UI16; y++)
      {
         uint32 changed = 0;  // how many pixels have changed since last refresh
         uint32 drawed = 0;   // how many pixel actualy where transmitted since last refresh
         xend = 0;
         fread (buf_source[y], TFT_WIDTH_UI16 * 2, 1,fb); // read complete source framebuffer
         for (x = 0; x < TFT_WIDTH_UI16; x++)
         {
            //x = i ;//% TFT_WIDTH_UI16;
            //y = lines;

            if (buf_source[y][x] != buf_display[y][x])
            {
               //printf("buf_source[%d] = %d \t | \t buf_display[%d]: %d\n",i, buf_source[i], i, buf_display[i]);
               changed++;
               if ((x + PIXELGROUPLEN) > TFT_WIDTH_UI16)
               {
                  xend = TFT_WIDTH_UI16 - 1;
                  //printf("x: %d\n", x);
               }
               else
               {
                  xend = x + PIXELGROUPLEN;
               }
               tft_setWindow (x, y, xend, y);
               tft_drawStart ();

               //for(j = tmpx; j < tmpx+xend; j++)
               for (j = x; j <= xend; j++)
               {
                  tft_sendPixelData (buf_source[y][j]);
                  buf_display[y][j] = buf_source[y][j];
                  drawed++;
                  //   x++;
               }
               x = xend;
            }
         }
      }
      usleep (30000L);     // sleep for 15ms
   }
   fclose (fb);

#endif

   /* This Routine analyzes the screen alteration and switches to the best method for the screen update */
#if MAIN == 6
   int y, x, j, i, cnt;

   // open the source framebuffer device
   FILE *fb = fopen (argv[1], "rb");

   // allocate two buffers for tracking the source framebuffer changes
   uint16 buf_display[TFT_WIDTH_UI16 * TFT_HEIGHT_UI16 * 2];   // the current display content
   uint16 buf_source[TFT_WIDTH_UI16 * TFT_HEIGHT_UI16 * 2]; // the source framebuffer content

   // run forever...
   while (TRUE)
   {
      fseek (fb, 0, SEEK_SET);   // rewind source framebuffer
      fread (buf_source, 1, TFT_WIDTH_UI16 * TFT_HEIGHT_UI16 * 2, fb);  // read complete source framebuffer

      // statistics
      uint32 changed = 0;  // how many pixels have changed since last refresh
      uint32 drawed = 0;   // how many pixel actualy where transmitted since last refresh

      // state for merging adjacent changes into one transmission
      uint32 i = 0;     // current pixel index
      boolean drawing = FALSE;   // if pixels are currently transmitted
      uint32 lastChange = 0;  // index of first pixel of this transmission
      cnt = 0;
      // for all pixels on screen...
      for (y = 0; y < TFT_HEIGHT_UI16; y++)
      {
         for (x = 0; x < TFT_WIDTH_UI16; x++)
         {
            cnt++;
            if (buf_display[cnt] != buf_source[cnt])
               changed++;
         }
      }

      if (changed < 1000)
      {
         // open the source framebuffer device
         //FILE* fb=fopen(argv[1],"rb");

         // allocate two buffers for tracking the source framebuffer changes
         uint16 buf_display[TFT_WIDTH_UI16 * TFT_HEIGHT_UI16 * 2];   // the current display content
         uint16 buf_source[TFT_WIDTH_UI16 * TFT_HEIGHT_UI16 * 2]; // the source framebuffer content

         fseek (fb, 0, SEEK_SET);   // rewind source framebuffer
         fread (buf_source, 1, TFT_WIDTH_UI16 * TFT_HEIGHT_UI16 * 2, fb);  // read complete source framebuffer

         // statistics
         // uint32 changed=0; // how many pixels have changed since last refresh
         //for(n = 0; n < f_size; n++)
         //  {
         //     printf("old_buffer[%d]: %c \t new_buffer[%d]: %c\n", n, old_buffer[n], n, new_buffer[n]);
         //  }/ how many pixel actualy where transmitted since last refresh

         // state for merging adjacent changes into one transmission
         uint32 i = 0;     // current pixel index
         boolean drawing = FALSE;   // if pixels are currently transmitted
         uint32 lastChange = 0;  // index of first pixel of this transmission
         uint16 color = 0;

         // for all pixels on screen...
         for (y = 0; y < TFT_HEIGHT_UI16; y++)
         {
            for (x = 0; x < TFT_WIDTH_UI16; x++)
            {
               color = buf_source[i];
               if (color != buf_display[i])
               {
                  buf_display[i] = color;
                  drawed++;
                  tft_drawPixel (x, y, color);
               }
               i++;
            }
         }
         //printf("pixel changed: %d\n", changed);
         // if(changed>0) printf("Pixels changed: %i, drawed: %i\n",changed,drawed);
         usleep (15000L);  // sleep for 15ms
      }
      else
      {


#define PIXELGROUPLEN 40
         int y, x, j, i, xend, lines, tmpx;

         // open the source framebuffer device

         // allocate two buffers for tracking the source framebuffer changes
         uint16 buf_display[TFT_HEIGHT_UI16][TFT_WIDTH_UI16];  // the current display content
         uint16 buf_source[TFT_HEIGHT_UI16][TFT_WIDTH_UI16];   // the source framebuffer content

         for (i = 0; i < TFT_HEIGHT_UI16; i++)
            for (j = 0; j < TFT_WIDTH_UI16; j++)
               buf_display[i][j] = 0;
         // run forever...


         fseek (fb, 0, SEEK_SET);   // rewind source framebuffer
         for (i = 0; i < TFT_HEIGHT_UI16 * 2; i++)
            //fread(buf_source[i], 2, TFT_WIDTH_UI16, fb);  // read complete source framebuffer
            fread (buf_source[i], 1, TFT_WIDTH_UI16 * 2, fb);  // read complete source framebuffer

         i = 0;
         tmpx = 0;

         for (y = 0; y < TFT_HEIGHT_UI16; y++)
         {
            //uint32 changed=0; // how many pixels have changed since last refresh
            //uint32  drawed=0; // how many pixel actualy where transmitted since last refresh
            xend = 0;

            for (x = 0; x < TFT_WIDTH_UI16; x++)
            {
               //x = i ;//% TFT_WIDTH_UI16;
               //y = lines;
               tmpx = x;
               if (buf_source[y][x] != buf_display[y][x])
               {
                  //printf("buf_source[%d] = %d \t | \t buf_display[%d]: %d\n",i, buf_source[i], i, buf_display[i]);
                  //changed++;
                  if ((x + PIXELGROUPLEN) > TFT_WIDTH_UI16)
                  {
                     xend = TFT_WIDTH_UI16 - 1;
                     //printf("x: %d\n", x);
                     //system("sleep 2");
                  }
                  else
                     xend = x + PIXELGROUPLEN;

                  tft_setWindow (x, y, xend, y);
                  tft_drawStart ();

                  //for(j = tmpx; j < tmpx+xend; j++)
                  for (j = x; j <= xend; j++)
                  {
                     tft_sendPixelData (buf_source[y][j]);
                     buf_display[y][j] = buf_source[y][j];
                     drawed++;
                     //   x++;
                  }
                  x = xend;
               }
            }
         }
         usleep (15000L);  // sleep for 15ms
      }
      //printf("Pixels changed: %i, drawed: %i\n",changed,drawed);
   }
   // actually we never get here
   fclose (fb);
#endif

#endif


   return 0;


}

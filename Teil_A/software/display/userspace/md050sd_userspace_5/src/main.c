#include "../inc/std_types.h"

#include "dio/dio.h"
#include "tft/tft.h"

#include <time.h>
#include <stdlib.h>
#include <string.h>


/* LPC3131 UM chapter 15 */
#define IOCONFIG    0x13003000
#define CLK_REG      0x1300442C
//#define wh  160560

#include <time.h>


uint8 debug_output;

uint16 getRGB565Rainbow()
{
   sint8 states[6][3] = { { 0, +1,  0}, // rot auf gelb: init R=255,G=0,B=0
         {-1,  0,  0},
         { 0,  0,  1},
         { 0, -1,  0},
         { 1,  0,  0},
         { 0,  0, -1}};

   static uint8 R = 255;
   static uint8 G = 0;
   static uint8 B = 0;
   static int state = 0;
   static uint8 cnt = 0;

   R += states[state][0];
   G += states[state][1];
   B += states[state][2];

   if(cnt >= 254)
   {
      cnt = 0;
      if(state < 5)
         state++;
      else
         state = 0;
   }
   else
   {
      cnt++;
   }
  return  (uint16)((((((uint16)R) << 8) & 0xF800)) + ((((uint16)G) << 3) & 0x07E0) + ((((uint16)B) >> 3) & 0x001F));
}

int main(int argc, const char *argv[])
{
  unsigned short xs = 0, xe = 800, ys = 0, ye = 480;
  unsigned short color;


   if(argc < 2){
      printf("usage: \ngnublin_md050sd xs ys width height init|noinit 0|1|2 color\n");
      printf("xs: start of rect x\n");
      printf("ys: start of rect y\n");
      printf("width: width of rect\n");
      printf("height: height of rect\n");
      printf("init|noinit: if initialization is run\n");
      printf("0|1|2: normal, single step, timeout\n");
      printf("color: background color\n");
      exit(1);
   }

   xs = atoi(argv[1]);
   ys = atoi(argv[2]);
   xe = atoi(argv[3]);
   ye = atoi(argv[4]);
   color = atoi(argv[7]);
   debug_output = atoi(argv[6]);

   printf("xs: %d\n", xs);
   printf("ys: %d\n", ys);
   printf("xe: %d\n", xe);
   printf("ye: %d\n", ye);

   dio_init();
   if(!strcmp(argv[5], "init")){

      tft_init(1);
   }
   else
   {
      tft_init(0);
   }

//   tft_drawRectangle(0, 0, 10, 10, color);

   while(1)
   {
      tft_drawRectangle(xs, ys, xe, ye, getRGB565Rainbow());
   }

  return 0;
}

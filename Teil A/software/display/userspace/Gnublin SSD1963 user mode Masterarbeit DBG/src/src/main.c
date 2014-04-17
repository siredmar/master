#include "../inc/std_types.h"

#include "dio/dio.h"
#include "tft/tft.h"

#include <time.h>
#include <stdlib.h>
#include <string.h>


/* LPC3131 UM chapter 15 */
#define IOCONFIG    0x13003000
#define CLK_REG      0x1300442C
#define wh  160560


#define BENCHMARK 0

//#if BENCHMARK == TRUE
#include <time.h>
#define CYCLES 250
//#endif
//
//#define w TFT_WIDTH_UI16
//#define h TFT_HEIGHT_UI16
//#define FBLO_GPIO   0x1C0   /* GPIO functional block */
//
//int memory_fd;
//char *memory;

uint8 debug_output;

int main(int argc, const char *argv[])
{
   unsigned short xs = 0, xe = 480, ys = 0, ye = 272;

   //printf("MPMC_STCONF0:\t0x%.2X\n", (*((uint32 *)0xF0000F00)))



   if(argc < 2){
      printf("usage: \ngnublin_ssd1963 xs ys width height init|noinit 0|1|2\n");
      printf("xs: start of rect x\n");
      printf("ys: start of rect y\n");
      printf("width: width of rect\n");
      printf("height: height of rect\n");
      printf("init|noinit: if initialization is run\n");
      printf("0|1|2: normal, single step, timeout\n");
      exit(1);
   }

   // uint16 buf_display[TFT_HEIGHT_UI16][TFT_WIDTH_UI16];  // the current display content
   // uint16 buf_source[TFT_HEIGHT_UI16][TFT_WIDTH_UI16];   // the source framebuffer content


   // srand(time(NULL));
   // uint16 color = rand() % 0xFFFF;
   //
   // {
   xs = atoi(argv[1]);
   ys = atoi(argv[2]);
   xe = atoi(argv[3]);
   ye = atoi(argv[4]);
   debug_output = atoi(argv[6]);

   printf("xs: %d\n", xs);
   printf("ys: %d\n", ys);
   printf("xe: %d\n", xe);
   printf("ye: %d\n", ye);

   dio_init();
   dio_writeChannel(DIO_CHANNEL_20, DIO_HIGH);

   if(!strcmp(argv[5], "init")){

      tft_init(1);
   }
   else
   {
      tft_init(0);
   }

   //tft_clearScreen(YELLOW);

   while(1)
   {
      tft_drawRectangle(xs, ys, xe, ye, getRGB565Rainbow());
   }


   return 0;



}

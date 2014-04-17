/**
 *  COPYRIGHT: Armin Schlegel
 *  \file tft.h
 *  \brief Display Graphic Driver
 *
 *  This header file contains all tft specific datatypes and defines.
 *
 *******************************************************************************/

/* ***************************** includes < > ******************************* */

/* ***************************** includes " " ******************************* */
#include "tft.h"
#include "tft_lcfg.h"
#include "../dio/dio.h"
#include "tft_cfg.h"
#include "../../inc/platform_types.h"
#include <stdlib.h>

#define TFT_COLOR_WORKAROUND  6000
#define  waitForUserInput() getchar()
/* ***************************** # defines ********************************** */

/* ***************************** modul global data ************************** */

/* ***************************** modul global functions ********************* */

#define WITH_MPMC 0


static int  mem_fd;

void *sram0_ctrl_map;
void *sram0_data_map;
volatile unsigned short *sram0_ctrl;
volatile unsigned short *sram0_data;

#if WITH_MPMC == 1
volatile unsigned int *mpmc_memory;
void *mpmc_memory_map;
#endif

static int send_counter = 1;
extern uint8 debug_output;

static Std_ReturnType tft_openSRAM0Memory()
{
   Std_ReturnType returnValue = E_NOT_OK;
   /* open /dev/mem */
   if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) {
      printf("can't open /dev/mem \n");
      exit(-1);
   }

   /* mmap SRAM0 */
   sram0_ctrl_map = mmap(
         NULL,             //Any adddress in our space will do
         getpagesize(),       //Map length
         PROT_READ|PROT_WRITE,// Enable reading & writting to mapped memory
         MAP_SHARED,       //Shared with other processes
         mem_fd,           //File to map
         SRAM0_BASE + SRAM0_CTRL        //Offset to SRAM0_CTRL peripheral
   );


   if (sram0_ctrl_map == MAP_FAILED) {
      printf("mmap error %d\n", (int)sram0_ctrl_map);//errno also set!
      exit(-1);
   }

   // Always use volatile pointer!
   sram0_ctrl = (volatile unsigned short *)sram0_ctrl_map;


   /* mmap SRAM0 */
   sram0_data_map = mmap(
         NULL,             //Any adddress in our space will do
         getpagesize(),      //Map length
         PROT_READ|PROT_WRITE,// Enable reading & writting to mapped memory
         MAP_SHARED,       //Shared with other processes
         mem_fd,           //File to map
         SRAM0_BASE + SRAM0_DATA         //Offset to SRAM0_DATA peripheral
   );


   if (sram0_data_map == MAP_FAILED) {
      printf("mmap error %d\n", (int)sram0_data_map);//errno also set!
      exit(-1);
   }

   // Always use volatile pointer!
   sram0_data = (volatile unsigned short *)sram0_data_map;

#if WITH_MPMC == 1
   /* mmap MPMC */
   mpmc_memory_map = mmap(
         NULL,             //Any adddress in our space will do
         getpagesize(),       //Map length
         PROT_READ|PROT_WRITE,// Enable reading & writting to mapped memory
         MAP_SHARED,       //Shared with other processes
         mem_fd,           //File to map
         MPMC_BASE         //Offset to GPIO peripheral
   );

   if (mpmc_memory_map == MAP_FAILED) {
      printf("mmap error %d\n", (int)mpmc_memory_map);//errno also set!
      exit(-1);
   }

   // Always use volatile pointer!
   mpmc_memory = (volatile unsigned int *)mpmc_memory_map;
   printf("mpmc_memory = 0x%x\n", (volatile unsigned int)mpmc_memory);
#endif
   close(mem_fd); //No need to keep mem_fd open after mmap

   returnValue = E_OK;
   return returnValue;
}

void tft_cleanupSRAM0()
{
   //
   //   free(sram0_ctrl);
   //   free(sram0_data);
   //   free(sram0_ctrl_map);
   //   free(sram0_data_map);
   //
   //#if WITH_MPMC == 1
   //   free(mpmc_memory);
   //   free(mpmc_memory_map);
   //#endif

}


#if WITH_MPMC == 1
static void tft_readSRAM0Timings()
{
   unsigned int val;

   val = *(unsigned int*)(mpmc_memory + MPMC_STCONFIG0);
   printf("MPMC_STCONFIG0:\t 0x%.2X\n", val);

   val = *(unsigned int*)(mpmc_memory + MPMC_STWTWEN0);
   printf("MPMC_STWTWEN0:\t %d\n", val);

   val = *(unsigned int*)(mpmc_memory + MPMC_STWTOEN0);
   printf("MPMC_STWTOEN0:\t %d\n", val);

   val = *(unsigned int*)(mpmc_memory + MPMC_STWTRD0);
   printf("MPMC_STWTRD0:\t %d\n",  val);

   val = *(unsigned int*)(mpmc_memory + MPMC_STWTPG0);
   printf("MPMC_STWTPG0:\t %d\n",  val);

   val = *(unsigned int*)(mpmc_memory + MPMC_STWTWR0);
   printf("MPMC_STWTWR0:\t %d\n",  val);

   val = *(unsigned int*)(mpmc_memory + MPMC_STWTTURN0);
   printf("MPMC_STWTTURN0:\t %d\n",val);

}

static void tft_initSRAM0Timings()
{
   unsigned int val;
   while(1)
   {
      val = *(unsigned int*)(mpmc_memory + MPMC_STATUS);
      if(!(val & (1 << 0)))
      {
         *(mpmc_memory + MPMC_CONTROL) = 0x05;
         // usleep(10);
         break;
      }

   }
   printf("MPMC_STATUS:\t 0x%.8X\n", *(unsigned int*)(mpmc_memory + MPMC_STATUS));
   *(mpmc_memory + MPMC_STCONFIG0) = 0x81;
   *(mpmc_memory + MPMC_STWTWEN0 ) = 1;
   *(mpmc_memory + MPMC_STWTOEN0 ) = 1;
   *(mpmc_memory + MPMC_STWTRD0  ) = 30;
   *(mpmc_memory + MPMC_STWTPG0  ) = 0;
   *(mpmc_memory + MPMC_STWTWR0  ) = 30;
   *(mpmc_memory + MPMC_STWTTURN0) = 0;
   tft_waitXms(1);
   while(1)
   {
      val = *(unsigned int*)(mpmc_memory + MPMC_STATUS);
      if(!(val & (1 << 0)))
      {
         *(mpmc_memory + MPMC_CONTROL) = 0x01;
         //usleep(10);
         break;
      }
   }
}
#endif



/*  \brief Waits x ms
 *
 *  \param [in]  waitTime_ui16 = Wait time in ms
 *  \param [out] ---
 *  \return      ---
 */
static void tft_waitXms
(
      uint16 waitTime_ui16
)
{
   uint16 tmp_ui16 = 0;
   for(tmp_ui16 = 0; tmp_ui16 < waitTime_ui16; tmp_ui16++)
   {
      usleep(1000);
   }
}

void tft_sendData
(
      uint16 data_ui16
)
{
   if(debug_output == 1)
   {
      printf("%d\ttft_sendData:\t\t 0x%.4X", send_counter++, data_ui16);
      waitForUserInput();
      *(sram0_data) = data_ui16;
   }else {
      if(debug_output == 2)
      {
         printf("%d\ttft_sendData:\t\t 0x%.4X", send_counter++, data_ui16);
         *(sram0_data) = data_ui16;
         tft_waitXms(10);
      }else
      {
         *(sram0_data) = data_ui16;
      }


   }
}

void tft_sendPixelData
(
      uint16 data_ui16
)
{
   if(debug_output == 1)
   {
      printf("%d\ttft_sendPixelData:\t 0x%.4X",  send_counter++, data_ui16);
      waitForUserInput();
      *(sram0_data) = data_ui16;
   } else {
      if(debug_output == 2)
      {
         printf("%d\ttft_sendPixelData:\t 0x%.4X",  send_counter++, data_ui16);
         *(sram0_data) = data_ui16;
         tft_waitXms(10);
      }else
      {
         *(sram0_data) = data_ui16;
      }
   }
}

//void tft_sendPixelData_8Bit
//(
//      uint16 data_ui16
//)
//{
//   *(sram0_data) = ((uint16)((data_ui16 >> 9) & 0xF8));
//
//   *(sram0_data) = ((uint16)((data_ui16 >> 3) & 0xFC));
//
//   *(sram0_data) = ((uint16)((data_ui16 << 3) & 0xF8));
//}

/**  \brief Sends data
 *
 *  \param [in]  data_ui16 = Data to send
 *  \param [out] ---
 *  \return      ---
 */
void tft_sendCommand
(
      uint16 data_ui16
)
{

   if(debug_output == 1)
   {
      printf("\n%d\ttft_sendCommand:\t 0x%.2X", send_counter++, data_ui16);
      waitForUserInput();
      *(sram0_ctrl) = (data_ui16 & 0xFF);
   } else {
      if(debug_output == 2)
      {
         printf("\n%d\ttft_sendCommand:\t 0x%.2X", send_counter++, data_ui16);
         *(sram0_ctrl) = (data_ui16 & 0xFF);
         tft_waitXms(10);
      } else
      {
         *(sram0_ctrl) = (data_ui16 & 0xFF);
      }
   }


}

void tft_sendCommand_slow
(
      uint16 ctrl_ui16
)
{
   *(sram0_ctrl) = (uint8)(ctrl_ui16 & 0xFF);
   tft_waitXms(10);
}

void tft_sendCommand_slowPLL
(
      uint16 ctrl_ui16
)
{
   *(sram0_ctrl) = (ctrl_ui16 & 0xFF);
   //usleep(1000);
}

void tft_sendData_slowPLL
(
      uint16 ctrl_ui16
)
{
   *(sram0_data) = (ctrl_ui16 & 0xFF);
   //usleep(1000);
}

uint16 tft_readData_slowPLL(void)
{
   //usleep(1000);
   return *(sram0_data);
}


void tft_sendData_slow
(
      uint16 data_ui16
)
{
   *(sram0_data) = data_ui16;
   tft_waitXms(10);
}

uint16 tft_readData_slow(void)
{
   usleep(1000);
   return *(sram0_data);
}

static void tft_setDisplayRegister
(
      uint16 tft_register_ui16,
      uint16 tft_registerValue_ui16
)
{
   tft_sendCommand(tft_register_ui16);
   //usleep(10);
   tft_sendData(tft_registerValue_ui16);
   //usleep(10);
}


/*  \brief Set window with defined height and width window
 *          will be drawn form top to bottom
 *
 *  \param [in]  startPositionX_ui16 = Top start X position window
 *  \param [in]  startPositionY_ui16 = Top start Y position window
 *  \param [in]  windowHeight_ui16 = Window height
 *  \param [in]  windowWidth_ui16 = Window width
 *  \param [out] ---
 *  \return      ---
 */
void tft_setWindow
(
      uint16 startPositionX_ui16,
      uint16 startPositionY_ui16,
      uint16 endPositionX_ui16,
      uint16 endPositionY_ui16
)
{
   tft_setDisplayRegister(SSD1289_REG_H_RAM_ADR_POS, (uint16)((endPositionX_ui16 << 8) | startPositionX_ui16));
   tft_setDisplayRegister(SSD1289_REG_V_RAM_ADR_START, startPositionY_ui16);
   tft_setDisplayRegister(SSD1289_REG_V_RAM_ADR_END, endPositionY_ui16);
   tft_setDisplayRegister(SSD1289_REG_GDDRAM_X_ADDR, startPositionX_ui16);
   tft_setDisplayRegister(SSD1289_REG_GDDRAM_Y_ADDR, startPositionY_ui16);
   tft_sendCommand(SSD1289_REG_GDDRAM_DATA);
}

/* ***************************** global functions *************************** */
/*
 *  \brief Function initialize the Display Graphic Driver and the tft Display
 *
 *  \param [in]  initSequence = Pointer to init Sequence
 *  \param [out] ---
 *  \return      ---
 */
void tft_readSignature()
{
   uint16 id = 0;
   id = tft_readData_slowPLL();

   printf("Signature: 0x%.4X\n", id);
}

void tft_init
(
      uint8 init
)
{
   printf("tft_openSRAM0Memory() called\n");
   tft_openSRAM0Memory();
   printf("tft_openSRAM0Memory() returned\n\n");

#if WITH_MPMC == 1
   printf("tft_readSRAM0Timings() called\n");
   tft_readSRAM0Timings();
   printf("tft_readSRAM0Timings() returned\n\n");

   printf("tft_initSRAM0Timings() called\n");
   tft_initSRAM0Timings();
   printf("tft_initSRAM0Timings() returned\n\n");

   printf("tft_readSRAM0Timings() called\n");
   tft_readSRAM0Timings();
   printf("tft_readSRAM0Timings() returned\n\n");
#endif

   if(init == 1)
   {
      tft_deSelectReset();
      tft_waitXms(100);   /* Wait 100ms */
      tft_selectReset(); /* Reset Display done */
      usleep(1000);



      tft_setDisplayRegister(0x0000, 0x0001);  tft_waitXms(150);
      tft_setDisplayRegister(0x0003, 0xA8A4);  tft_waitXms(10);
      tft_setDisplayRegister(0x000C, 0x0000);  tft_waitXms(10);
      tft_setDisplayRegister(0x000D, 0x080C);  tft_waitXms(10);
      tft_setDisplayRegister(0x000E, 0x2B00);  tft_waitXms(10);
      tft_setDisplayRegister(0x001E, 0x00B0);  tft_waitXms(10);
      tft_setDisplayRegister(0x0001, 0x2B3F);  tft_waitXms(150);
      tft_setDisplayRegister(0x0002, 0x0600);  tft_waitXms(10);
      tft_setDisplayRegister(0x0010, 0x0000);  tft_waitXms(10);
      tft_setDisplayRegister(0x0011, 0x6070);  tft_waitXms(200);
      tft_setDisplayRegister(0x0005, 0x0000);  tft_waitXms(10);
      tft_setDisplayRegister(0x0006, 0x0000);  tft_waitXms(10);
      tft_setDisplayRegister(0x0016, 0xEF1C);  tft_waitXms(10);
      tft_setDisplayRegister(0x0017, 0x0003);  tft_waitXms(10);
      tft_setDisplayRegister(0x0007, 0x0233);  tft_waitXms(10);
      tft_setDisplayRegister(0x000B, 0x0000);  tft_waitXms(10);
      tft_setDisplayRegister(0x000F, 0x0000);  tft_waitXms(200);
      tft_setDisplayRegister(0x0041, 0x0000);  tft_waitXms(10);
      tft_setDisplayRegister(0x0042, 0x0000);  tft_waitXms(10);
      tft_setDisplayRegister(0x0048, 0x0000);  tft_waitXms(10);
      tft_setDisplayRegister(0x0049, 0x013F);  tft_waitXms(10);
      tft_setDisplayRegister(0x004A, 0x0000);  tft_waitXms(10);
      tft_setDisplayRegister(0x004B, 0x0000);  tft_waitXms(10);
      tft_setDisplayRegister(0x0044, 0xEF00);  tft_waitXms(10);
      tft_setDisplayRegister(0x0045, 0x0000);  tft_waitXms(10);
      tft_setDisplayRegister(0x0046, 0x013F);  tft_waitXms(200);
      tft_setDisplayRegister(0x0030, 0x0707);  tft_waitXms(10);
      tft_setDisplayRegister(0x0031, 0x0204);  tft_waitXms(10);
      tft_setDisplayRegister(0x0032, 0x0204);  tft_waitXms(10);
      tft_setDisplayRegister(0x0033, 0x0502);  tft_waitXms(10);
      tft_setDisplayRegister(0x0034, 0x0507);  tft_waitXms(10);
      tft_setDisplayRegister(0x0035, 0x0204);  tft_waitXms(10);
      tft_setDisplayRegister(0x0036, 0x0204);  tft_waitXms(10);
      tft_setDisplayRegister(0x0037, 0x0502);  tft_waitXms(10);
      tft_setDisplayRegister(0x003A, 0x0302);  tft_waitXms(10);
      tft_setDisplayRegister(0x003B, 0x0302);  tft_waitXms(20);
      //      tft_setDisplayRegister(0x0023, 0x0000);  tft_waitXms(1000);
      //      tft_setDisplayRegister(0x0024, 0x0000);  tft_waitXms(1000);
      //      tft_setDisplayRegister(0x0025, 0x8000);  tft_waitXms(1000);
      //      tft_setDisplayRegister(0x004f, 0x0000);  tft_waitXms(1000);
      //      tft_setDisplayRegister(0x004e, 0x0000);  tft_waitXms(1000);

   }
}



/*
 *  \brief Function clears whole screen with one color
 *
 *  \param [in]  color_ui16 =  Color Value
 *  \param [out] ---
 *  \return      ---
 */
void tft_clearScreen
(
      tft_ColorType color_ui16
)
{

   tft_drawRectangle(0, 0, 320-1, 240-1, color_ui16);
}

static void tft_setPixelCoordinates
(
      uint16 positionX_ui16,
      uint16 positionY_ui16
)
{
   tft_setDisplayRegister(SSD1289_REG_GDDRAM_X_ADDR, positionX_ui16);
   tft_setDisplayRegister(SSD1289_REG_GDDRAM_Y_ADDR, positionY_ui16);
   tft_sendCommand(SSD1289_REG_GDDRAM_DATA);
}

/*
 *  \brief Function draw a filled Rectangle at x/y position
 *
 *  \param [in]  xStartPosition_ui16 = X Start position of rectangle
 *  \param [in]  yStartPosition_ui16 = Y Start position of rectangle
 *  \param [in]  rectangleWidth_ui16 = Width of rectangle in pixel
 *  \param [in]  rectangleHeight_ui16 = Height of rectangle in pixel
 *  \param [in]  rectangleColor_ui16 = Color of rectangle

 *  \param [out] ---
 *  \return      E_OK = Rectangle draw successfull
 *               E_NOT_OK = Rectangle draw not successfull
 */
Std_ReturnType tft_drawPixel
(
      uint16 xPosition_ui16,
      uint16 yPosition_ui16,
      tft_ColorType rectangleColor_ui16
)
{
   Std_ReturnType returnValue;
   //   uint32 forCounter_ui32;
   //   uint32 pixelSizeOfRectangle_ui32;

   returnValue = E_NOT_OK;

   /* calculate pixel size */


   /* Set window size and position - modified for portrait view */
   tft_setPixelCoordinates(xPosition_ui16, yPosition_ui16);


   tft_sendData(rectangleColor_ui16);


   returnValue = E_OK;

   return returnValue;
}

/*
 *  \brief Function draw a filled Rectangle at x/y position
 *
 *  \param [in]  xStartPosition_ui16 = X Start position of rectangle
 *  \param [in]  yStartPosition_ui16 = Y Start position of rectangle
 *  \param [in]  rectangleWidth_ui16 = Width of rectangle in pixel
 *  \param [in]  rectangleHeight_ui16 = Height of rectangle in pixel
 *  \param [in]  rectangleColor_ui16 = Color of rectangle

 *  \param [out] ---
 *  \return      E_OK = Rectangle draw successfull
 *               E_NOT_OK = Rectangle draw not successfull
 */
Std_ReturnType tft_drawRectangle
(
      uint16 xPosition_ui16,
      uint16 yPosition_ui16,
      uint16 rectangleWidth_ui16,
      uint16 rectangleHeight_ui16,
      tft_ColorType rectangleColor_ui16
)
{
   Std_ReturnType returnValue;
   uint32 forCounter_ui32;
   uint16 endPositionX_ui16;
   uint16 endPositionY_ui16;
   uint32 pixelSizeOfRectangle_ui32;

   returnValue = E_NOT_OK;
   endPositionX_ui16 = xPosition_ui16 + rectangleWidth_ui16;
   endPositionY_ui16 = yPosition_ui16 + rectangleHeight_ui16;

   /* secure border of display */
   if(endPositionX_ui16 > (TFT_WIDTH_UI16))
   {
      endPositionX_ui16 = TFT_WIDTH_UI16;
   }
   else if(endPositionY_ui16 > (TFT_HEIGHT_UI16))
   {
      endPositionY_ui16 = TFT_HEIGHT_UI16;
   }
   else
   {
      // do nothing
   }
   /* calculate pixel size */
   pixelSizeOfRectangle_ui32 = rectangleWidth_ui16 * rectangleHeight_ui16;

   tft_setWindow(xPosition_ui16, yPosition_ui16, endPositionX_ui16, endPositionY_ui16);

   for(forCounter_ui32 = 0; forCounter_ui32 < pixelSizeOfRectangle_ui32; forCounter_ui32++)
   {
      tft_sendData(rectangleColor_ui16);
     // usleep(100);
   }


   returnValue = E_OK;

   return returnValue;
}

Std_ReturnType tft_drawRectangle2
(
      uint16 xPosition_ui16,
      uint16 yPosition_ui16,
      uint16 rectangleWidth_ui16,
      uint16 rectangleHeight_ui16,
      tft_ColorType rectangleColor_ui16
)
{
   Std_ReturnType returnValue;
   uint32 forCounter_ui32;
   uint32 forCounter2_ui32;
   uint16 endPositionX_ui16;
   uint16 endPositionY_ui16;
   uint32 pixelSizeOfRectangle_ui32;

   returnValue = E_NOT_OK;
   endPositionX_ui16 = xPosition_ui16 + rectangleWidth_ui16;
   endPositionY_ui16 = yPosition_ui16 + rectangleHeight_ui16;

   /* secure border of display */
   if(endPositionX_ui16 > (TFT_WIDTH_UI16))
   {
      endPositionX_ui16 = TFT_WIDTH_UI16;
   }
   else if(endPositionY_ui16 > (TFT_HEIGHT_UI16))
   {
      endPositionY_ui16 = TFT_HEIGHT_UI16;
   }
   else
   {
      // do nothing
   }
   /* calculate pixel size */
   pixelSizeOfRectangle_ui32 = rectangleWidth_ui16 * rectangleHeight_ui16;

   tft_setWindow(xPosition_ui16, yPosition_ui16, endPositionX_ui16, endPositionY_ui16);

   for(forCounter_ui32 = xPosition_ui16; forCounter_ui32 < endPositionX_ui16; forCounter_ui32++)
   {
      for(forCounter2_ui32 = yPosition_ui16; forCounter2_ui32 < endPositionY_ui16; forCounter2_ui32++)
      {
         tft_drawPixel(forCounter_ui32, forCounter2_ui32, rectangleColor_ui16);
      }
   }


   returnValue = E_OK;

   return returnValue;
}

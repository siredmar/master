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

#define TFT_COLOR_WORKAROUND  6000

/* ***************************** # defines ********************************** */

/* ***************************** modul global data ************************** */

/* ***************************** modul global functions ********************* */


// I/O access
//unsigned int *sram0_memory;
//unsigned int *mpmc_memory;
static int  mem_fd;

void *sram0_ctrl_map;
void *sram0_data_map;
void *mpmc_memory_map;

// I/O access
volatile unsigned short *sram0_ctrl;
volatile unsigned short *sram0_data;
volatile unsigned int *mpmc_memory;

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
         0x10000,       //Map length
         PROT_READ|PROT_WRITE,// Enable reading & writting to mapped memory
         MAP_SHARED,       //Shared with other processes
         mem_fd,           //File to map
         SRAM0_BASE + SRAM0_CTRL        //Offset to GPIO peripheral
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
         0x10000,       //Map length
         PROT_READ|PROT_WRITE,// Enable reading & writting to mapped memory
         MAP_SHARED,       //Shared with other processes
         mem_fd,           //File to map
         SRAM0_BASE + SRAM0_DATA         //Offset to GPIO peripheral
   );


   if (sram0_data_map == MAP_FAILED) {
      printf("mmap error %d\n", (int)sram0_data_map);//errno also set!
      exit(-1);
   }

   // Always use volatile pointer!
   sram0_data = (volatile unsigned short *)sram0_data_map;


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

   close(mem_fd); //No need to keep mem_fd open after mmap

   returnValue = E_OK;
   return returnValue;
}


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
      if(val & (1 << 0))
      {
         *(mpmc_memory) = 0x05;
         usleep(10);
      }
      break;

   }
   printf("MPMC_STATUS:\t 0x%.8X\n", *(unsigned int*)(mpmc_memory + MPMC_STATUS));
   *(mpmc_memory + MPMC_STCONFIG0) = 0x81;
   *(mpmc_memory + MPMC_STWTWEN0 ) = 0;
   *(mpmc_memory + MPMC_STWTOEN0 ) = 0;
   *(mpmc_memory + MPMC_STWTRD0  ) = 31;
   *(mpmc_memory + MPMC_STWTPG0  ) = 0;
   *(mpmc_memory + MPMC_STWTWR0  ) = 3;
   *(mpmc_memory + MPMC_STWTTURN0) = 0;
   usleep(100);
   *(mpmc_memory) = 0x01;
   usleep(100);
}


static void waitForUserInput()
{
   //char a[255];
   getchar();
   // scanf("%s", &a);
}

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
      printf("%d\ttft_sendData:\t\t 0x%.2X", send_counter++, data_ui16);
      waitForUserInput();
   }
   if(debug_output == 2)
   {
      printf("%d\ttft_sendData:\t\t 0x%.2X", send_counter++, data_ui16);
      tft_waitXms(1000);
   }
   *(sram0_data) = (uint16)data_ui16;


}

void tft_sendPixelData
(
      uint16 data_ui16
)
{
   if(debug_output == 1)
   {
      printf("%d\ttft_sendPixelData:\t 0x%.2X",  send_counter++, data_ui16);
      waitForUserInput();
   }
   if(debug_output == 2)
   {
      printf("%d\ttft_sendPixelData:\t 0x%.2X",  send_counter++, data_ui16);
      tft_waitXms(1000);
   }
   //data_ui16 = data_ui16 * TFT_COLOR_WORKAROUND;
   *(sram0_data) = (uint16)data_ui16;

   //   tft_waitXms(1000);
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
      printf("%d\ttft_sendCommand:\t 0x%.2X", send_counter++, data_ui16);
      waitForUserInput();
   }
   if(debug_output == 2)
   {
      printf("%d\ttft_sendCommand:\t 0x%.2X", send_counter++, data_ui16);
      tft_waitXms(1000);
   }
   data_ui16 = data_ui16 & 0x00FF;
   *(sram0_ctrl) = (uint16)data_ui16;

   //   tft_waitXms(1000);
}

void tft_sendCommand_slow
(
      uint16 ctrl_ui16
)
{
   uint16 cnt;
   ctrl_ui16 = ctrl_ui16 & 0x00FF;
   *(sram0_ctrl) = (uint16)ctrl_ui16;
   tft_waitXms(10);
}

void tft_sendCommand_slowPLL
(
      uint16 ctrl_ui16
)
{
   uint16 cnt;
   ctrl_ui16 = ctrl_ui16 & 0x00FF;
   *(sram0_ctrl) = (uint16)ctrl_ui16;
}

void tft_sendData_slow
(
      uint16 data_ui16
)
{
   uint16 cnt;
   *(sram0_data) = (uint16)data_ui16;
   tft_waitXms(10);
}

uint16 tft_readData_slow(void)
{
   return *(sram0_data);
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
      uint16 xs,
      uint16 ys,
      uint16 xe,
      uint16 ye
)
{
   tft_sendCommand(SSD1963_SET_COLUMN_ADDRESS);
   tft_sendData((xs >> 8) & 0x00FF);
   tft_sendData(xs & 0x00FF);
   tft_sendData((xe  >> 8 ) & 0x00FF);
   tft_sendData(xe & 0x00FF);

   tft_sendCommand(SSD1963_SET_PAGE_ADDRESS);
   tft_sendData((ys >> 8) & 0x00FF);
   tft_sendData(ys & 0x00FF);
   tft_sendData((ye  >> 8 ) & 0x00FF);
   tft_sendData(ye & 0x00FF);

}

/* ***************************** global functions *************************** */
/*
 *  \brief Function initialize the Display Graphic Driver and the tft Display
 *
 *  \param [in]  initSequence = Pointer to init Sequence
 *  \param [out] ---
 *  \return      ---
 */

void tft_init
(
      uint8 init
)
{

   uint8 get_pll_mn[3] = {0};
   uint16 id[5] = {0};

   //   uint16 forCounter_ui16;
   //   tft_initSequenceType const *tmpInitSequence_ps;
   //   tmpInitSequence_ps = ConfigPtr->tft_initSequence_ps;


   printf("tft_openSRAM0Memory() called\n");
   tft_openSRAM0Memory();
   printf("tft_openSRAM0Memory() returned\n\n");

   printf("tft_readSRAM0Timings() called\n");
   tft_readSRAM0Timings();
   printf("tft_readSRAM0Timings() returned\n\n");

   printf("tft_initSRAM0Timings() called\n");
   tft_initSRAM0Timings();
   printf("tft_initSRAM0Timings() returned\n\n");

   printf("tft_readSRAM0Timings() called\n");
   tft_readSRAM0Timings();
   printf("tft_readSRAM0Timings() returned\n\n");


   if(init == 1)
   {

      tft_deSelectReset();
      tft_waitXms(100);   /* Wait 100ms */
      tft_selectReset(); /* Reset Display done */
      tft_waitXms(100);   /* Wait 100ms */

      tft_sendCommand_slow(SSD1963_SOFT_RESET);
      usleep(200000);

      tft_sendCommand_slowPLL(SSD1963_GET_PLL_MN);
      usleep(1000);
      get_pll_mn[0] = tft_readData_slow();
      get_pll_mn[1] = tft_readData_slow();
      get_pll_mn[2] = tft_readData_slow();

      printf("GetPLLMN[0]: 0x%.2X\n", get_pll_mn[0]);
      printf("GetPLLMN[1]: 0x%.2X\n", get_pll_mn[1]);
      printf("GetPLLMN[2]: 0x%.2X\n", get_pll_mn[2]);


      tft_sendCommand_slowPLL(SSD1963_READ_DDB);
      usleep(1000);
      id[0] = tft_readData_slow();
      id[1] = tft_readData_slow();
      id[2] = tft_readData_slow();
      id[3] = tft_readData_slow();
      id[4] = tft_readData_slow();

      printf("Signature: 0x%.2X 0x%.2X 0x%.2X 0x%.2X 0x%.2X\n", id[0], id[1], id[2], id[3], id[4]);





      tft_sendCommand_slow(SSD1963_SET_PLL); // PLL config - continued
      usleep(1000);
      tft_sendData_slow(0x0001);
      usleep(10000);

      tft_sendCommand_slow(SSD1963_SET_PLL_MN); // PLL config - continued
      usleep(1000);
      tft_sendData_slow(0x001D);
      usleep(1000);
      tft_sendData_slow(0x0002);
      usleep(1000);
      tft_sendData_slow(0x0054);
      usleep(10000);

      tft_sendCommand_slow(SSD1963_SET_PLL); // PLL config - continued
      usleep(1000);
      tft_sendData_slow(0x0003);
      usleep(1000);
      usleep(100000);





      /* Wait for PLL to lock */
//      do
//      {
//         tft_sendCommand_slowPLL(SSD1963_GET_PLL_STATUS);
//      } while (tft_readData_slow() != 0x04);


      tft_sendCommand(SSD1963_SET_LSHIFT_FREQ);
      tft_sendData(0x0001); // LSHIFT freq
      tft_sendData(0x0070);
      tft_sendData(0x00A2);

      tft_sendCommand(SSD1963_SET_LCD_MODE);
      tft_sendData(0x0038);  // set lcd mode
      tft_sendData(0x0000);
      tft_sendData(0x0001);
      tft_sendData(0x00DF);
      tft_sendData(0x0001);
      tft_sendData(0x000F);
      tft_sendData(0x0000);
      usleep(5000);

      tft_sendCommand(SSD1963_SET_HORI_PERIOD);
      tft_sendData(((HT >> 8U) & 0x00FF));  // horizontal period
      tft_sendData((HT & 0x00FF));
      tft_sendData(((HPS >> 8U) & 0x00FF));
      tft_sendData((HPS & 0x00FF));
      tft_sendData(HPW);
      tft_sendData(((LPS >> 8U) & 0x00FF));
      tft_sendData((LPS & 0x00FF));
      tft_sendData(0x0000);

      tft_sendCommand(SSD1963_SET_VERT_PERIOD);
      tft_sendData(((VT >> 8U) & 0x00FF)); // vertical period
      tft_sendData((VT & 0x00FF));
      tft_sendData((VPS >> 8U) & 0x00FF);
      tft_sendData((VPS & 0x00FF));
      tft_sendData(VPW);
      tft_sendData(((FPS >> 8U) & 0x00FF));
      tft_sendData((FPS & 0x00FF));

      tft_sendCommand(SSD1963_SET_ADDRESS_MODE);
      tft_sendData(0x0000);  // rotation - landscape

      tft_sendCommand(SSD1963_SET_PIXEL_DATA_INTERFACE);
      tft_sendData(0x0003);  // pixel data interface 16 bit    0b011 is 16 Bit
      usleep(5000);

      tft_sendCommand(SSD1963_SET_DISPLAY_ON); // set display on

      tft_sendCommand(SSD1963_SET_PWM_CONF);
      tft_sendData(0x0006);  // pwm config
      tft_sendData(0x00F0);
      tft_sendData(0x0001);
      tft_sendData(0x00F0);
      tft_sendData(0x0000);
      tft_sendData(0x0000);

      tft_sendCommand(SSD1963_SET_DBC_CONF);
      tft_sendData(0x000D); // pwm coffig continued

      tft_sendCommand(SSD1963_SET_GPIO_CONF);
      tft_sendData(0x0000); // gpio config
      tft_sendData(0x0000);

      tft_sendCommand(SSD1963_SET_GPIO_VALUE);
      tft_sendData(0x0000); // gpio to 0x00
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

   tft_drawRectangle(0, 0, 480-1, 272-1, color_ui16);
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

   returnValue = E_NOT_OK;

   /* Set window size and position - modified for portrait view */
   tft_setWindow(xPosition_ui16, yPosition_ui16, xPosition_ui16+1, yPosition_ui16);

   tft_sendCommand(SSD1963_WRITE_MEMORY_START);

   tft_sendPixelData(rectangleColor_ui16);


   returnValue = E_OK;

   return returnValue;
}

void tft_drawStart()
{

   tft_sendCommand(SSD1963_WRITE_MEMORY_START);
}

void tft_drawStop()
{
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
   if(endPositionX_ui16 > (TFT_WIDTH_UI16 - 1))
   {
      endPositionX_ui16 = TFT_WIDTH_UI16 - 1;
   }
   else if(endPositionY_ui16 > (TFT_HEIGHT_UI16 - 1))
   {
      endPositionY_ui16 = TFT_HEIGHT_UI16 - 1;
   }
   else
   {
      // do nothing
   }
   /* calculate pixel size */
   pixelSizeOfRectangle_ui32 = rectangleWidth_ui16 * rectangleHeight_ui16;

   tft_setWindow(xPosition_ui16, yPosition_ui16, endPositionX_ui16, endPositionY_ui16);


   tft_sendCommand(SSD1963_WRITE_MEMORY_START);


   for(forCounter_ui32 = 0; forCounter_ui32 < pixelSizeOfRectangle_ui32; forCounter_ui32++)
   {
      tft_sendPixelData(rectangleColor_ui16);
      //tft_sendData(rectangleColor_ui16);
   }


   returnValue = E_OK;

   return returnValue;
}

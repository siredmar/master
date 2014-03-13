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
static int send_counter = 1;
extern uint8 debug_output;

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


/* ***************************** modul global functions ********************* */
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
   tft_deSelectCS();

   tft_selectWR();

   tft_selectRS();
   dio_writePortGroup((uint8)data_ui16);

   tft_deSelectWR();
   usleep(1);
   tft_selectWR();
   tft_selectCS();
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
   tft_deSelectCS();

   tft_selectWR();

   tft_selectRS();
   dio_writePortGroup((uint8)((data_ui16 >> 8) & 0xF8));
   tft_deSelectWR();
   usleep(1);
   tft_selectWR();
   tft_selectCS();

   tft_deSelectCS();

   tft_selectWR();

   tft_selectRS();
   dio_writePortGroup((uint8)((data_ui16 >> 3) & 0xFC));
   tft_deSelectWR();
   usleep(1);
   tft_selectWR();
   tft_selectCS();

   tft_deSelectCS();

   tft_selectWR();

   tft_selectRS();
   dio_writePortGroup((uint8)((data_ui16 << 3) & 0xF8));
   tft_deSelectWR();
   usleep(1);
   tft_selectWR();
   tft_selectCS();

}

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
   tft_deSelectCS();

   tft_selectWR();

   tft_deSelectRS();
   dio_writePortGroup((uint8)data_ui16);

   tft_deSelectWR();
   usleep(1);
   tft_selectWR();
   tft_selectCS();
}

void tft_sendCommand_slow
(
      uint16 ctrl_ui16
)
{
   tft_deSelectCS();

   tft_selectWR();

   tft_deSelectRS();
   dio_writePortGroup((uint8)(ctrl_ui16 & 0xFF));

   tft_deSelectWR();
   usleep(1);
   tft_selectWR();
   tft_selectCS();

   tft_waitXms(10);
}

void tft_sendCommand_slowPLL
(
      uint16 ctrl_ui16
)
{
   tft_deSelectCS();

   tft_selectWR();

   tft_deSelectRS();
   dio_writePortGroup((uint8)(ctrl_ui16 & 0xFF));

   tft_deSelectWR();
   usleep(1);
   tft_selectWR();
   tft_selectCS();
}

void tft_sendData_slowPLL
(
      uint16 ctrl_ui16
)
{
   tft_deSelectCS();

   tft_selectWR();

   tft_deSelectRS();
   dio_writePortGroup((uint8)(ctrl_ui16 & 0xFF));

   tft_deSelectWR();
   usleep(1);
   tft_selectWR();
   tft_selectCS();
}

uint16 tft_readData_slowPLL(void)
{
   //usleep(1000);
   return dio_readPortGroup();
}


void tft_sendData_slow
(
      uint16 data_ui16
)
{
   tft_deSelectCS();

   tft_selectWR();

   tft_deSelectRS();
   dio_writePortGroup((uint8)(data_ui16 & 0xFF));

   tft_deSelectWR();
   usleep(1);
   tft_selectWR();
   tft_selectCS();
   tft_waitXms(10);
}

uint16 tft_readData_slow(void)
{
   usleep(1000);
   return dio_readPortGroup();
}

/*  \brief Sets Register on tft Display Controller
 *
 *  \param [in]  tft_register_ui16 = tft Controler register address
 *  \param [in]  tft_registerValue_ui16 = Register value to write
 *  \param [in]  tft_registerParameters_ui16 = Parameters count to write
 *  \param [out] ---
 *  \return      ---
 */
static void tft_setDisplayRegister
(
      uint16 tft_register_ui16,
      uint16 tft_registerValue_ui16,
      uint16 tft_registerParameters_ui16
)
{
   /* First send Command if it is != NOCMD */
   if(tft_register_ui16 != NOCMD)
   {
      tft_sendCommand(tft_register_ui16);
   }
   /* Then send or continue sending Parameter to tft controler if needed */
   if(tft_registerParameters_ui16 != NOPARAM)
   {
      tft_sendData(tft_registerValue_ui16);
   }
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

   uint16 get_pll_mn[3] = {0};
   uint16 id[5] = {0};
   uint16 get_pll_status = 0;



   if(init == 1)
   {


      tft_deSelectReset();
      tft_waitXms(100);   /* Wait 100ms */
      tft_selectReset(); /* Reset Display done */
      usleep(100);
            tft_sendCommand_slow(SSD1963_SOFT_RESET);
      //      usleep(200000);



      //  while(1){
//      tft_sendCommand_slowPLL(SSD1963_READ_DDB);
//      //tft_waitXms(1);
//      id[0] = tft_readData_slowPLL();
//      id[1] = tft_readData_slowPLL();
//      id[2] = tft_readData_slowPLL();
//      id[3] = tft_readData_slowPLL();
//      id[4] = tft_readData_slowPLL();

//      printf("Signature: 0x%.2X 0x%.2X 0x%.2X 0x%.2X 0x%.2X\n", id[0], id[1], id[2], id[3], id[4]);
//      tft_waitXms(1000);


//      tft_sendCommand_slowPLL(SSD1963_GET_PLL_MN);
//      get_pll_mn[0] = tft_readData_slowPLL();
//      get_pll_mn[1] = tft_readData_slowPLL();
//      get_pll_mn[2] = tft_readData_slowPLL();
//
//      printf("GetPLLMN[0]: 0x%.2X\n", get_pll_mn[0]);
//      printf("GetPLLMN[1]: 0x%.2X\n", get_pll_mn[1]);
//      printf("GetPLLMN[2]: 0x%.2X\n", get_pll_mn[2]);
//      tft_waitXms(1000);
      //}
//
//      get_pll_mn[0] = 0xFF;
//      get_pll_mn[0] = 0xFF;
//      get_pll_mn[0] = 0xFF;

      tft_sendCommand(SSD1963_SET_PLL_MN); // PLL config - continued
      tft_sendData(0x1D);
      tft_sendData(0x02);
      tft_sendData(0x04);
      tft_waitXms(100);

//      tft_sendCommand_slowPLL(SSD1963_GET_PLL_MN);
//      get_pll_mn[0] = tft_readData_slowPLL();
//      get_pll_mn[1] = tft_readData_slowPLL();
//      get_pll_mn[2] = tft_readData_slowPLL();
//
//      printf("GetPLLMN[0]: 0x%.2X\n", get_pll_mn[0]);
//      printf("GetPLLMN[1]: 0x%.2X\n", get_pll_mn[1]);
//      printf("GetPLLMN[2]: 0x%.2X\n", get_pll_mn[2]);
//      tft_waitXms(1000);

      tft_sendCommand(SSD1963_SET_PLL); // PLL config - continued
      tft_sendData(0x01);
      tft_waitXms(1000);



      tft_sendCommand(SSD1963_SET_PLL); // PLL config - continued
      tft_sendData(0x0003);
      tft_waitXms(100);





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
      tft_waitXms(5);

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
      tft_sendData(0x0000);  // pixel data interface 16 bit    0b011 is 16 Bit
      tft_waitXms(5);

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
   uint32 forCounter_ui32;
   tft_drawRectangle(0, 0, 480, 272, color_ui16);
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

   /* calculate pixel size */

   /*
    *
    tft_deSelectRS();
	dio_writePortGroup((uint8)data_ui16);
	tft_deSelectWR();
	tft_selectWR();
    *
    */

   /* Set window size and position - modified for portrait view */
   tft_setWindow(xPosition_ui16, yPosition_ui16, ++xPosition_ui16, ++yPosition_ui16);

   tft_deSelectCS();

   tft_sendCommand(SSD1963_WRITE_MEMORY_START);

   // tft_selectRS();

   tft_sendPixelData(rectangleColor_ui16);
   //tft_sendData16(rectangleColor_ui16);
   tft_selectCS();

   returnValue = E_OK;

   return returnValue;
}

void tft_drawStart()
{

   tft_deSelectCS();

   tft_sendCommand(SSD1963_WRITE_MEMORY_START);
}

void tft_drawStop()
{

   tft_selectCS();

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

//   /* activate CS */
//   tft_deSelectCS();
//
//   tft_deSelectRS();

   tft_sendCommand(SSD1963_WRITE_MEMORY_START);

//   tft_selectRS();
   // tft_setWindow(TFT_ZERO_X, TFT_ZERO_Y, TFT_WIDTH_UI16 - 1, TFT_HEIGHT_UI16 - 1);


   for(forCounter_ui32 = 0; forCounter_ui32 < pixelSizeOfRectangle_ui32; ++forCounter_ui32)
   {
      tft_sendPixelData(rectangleColor_ui16);
      //tft_sendData(rectangleColor_ui16);
   }
//   tft_selectCS();


   returnValue = E_OK;

   return returnValue;
}

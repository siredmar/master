/***********************************************************************
* $Id:: lpc313x_spi.h 1181 2008-09-11 20:59:59Z pdurgesh              $
*
* Project: LPC313X Serial Peripheral Interface (SPI) definitions
*
* Description:
*     This file contains the structure definitions and manifest
*     constants for the LPC313X chip family component:
*         Serial Peripheral Interface (SPI)
*
***********************************************************************
* Software that is described herein is for illustrative purposes only
* which provides customers with programming information regarding the
* products. This software is supplied "AS IS" without any warranties.
* NXP Semiconductors assumes no responsibility or liability for the
* use of the software, conveys no license or title under any patent,
* copyright, or mask work right to the product. NXP Semiconductors
* reserves the right to make changes in the software without
* notification. NXP Semiconductors also make no representation or
* warranty that such application will be suitable for the specified
* use without further testing or modification.
**********************************************************************/

#ifndef LPC313X_SPI_H
#define LPC313X_SPI_H

#include "lpc_types.h"
#include "lpc313x_chip.h"

#ifdef __cplusplus
extern "C"
{
#endif

/***********************************************************************
* Serial Peripheral Interface (SPI) Register Structures
**********************************************************************/
/* SPI slave setting Register Structure */
typedef volatile struct
{
  volatile UNS_32 setting1;
  volatile UNS_32 setting2;
} SPI_SLV_SETTING_T;

/* Serial Peripheral Interface (SPI) Module Register Structure */
typedef volatile struct
{
  volatile UNS_32       spi_config ;     /*  0x000            */
  volatile UNS_32       slave_enable ;   /*  0x004            */
  volatile UNS_32       tx_fifo_flush ;  /*  0x008            */
  volatile UNS_32       fifo_data ;      /*  0x00C            */
  volatile UNS_32       nhp_pop ;        /*  0x010            */
  volatile UNS_32       nhp_mode ;       /*  0x014            */
  volatile UNS_32       dma_settings ;   /*  0x018            */
  volatile const UNS_32 status ;         /*  0x01C            */
  volatile const UNS_32 hw_info ;        /*  0x020            */
  SPI_SLV_SETTING_T     slv_setting[3] ; /*  0x024 ... 0x038  */
  volatile const UNS_32 _reserved1[998] ;
  volatile UNS_32       int_threshold ; /*  0xFD4            */
  volatile UNS_32       int_clr_enable; /*  0xFD8            */
  volatile UNS_32       int_set_enable; /*  0xFDC            */
  volatile const UNS_32 int_status ;    /*  0xFE0            */
  volatile const UNS_32 int_enable ;    /*  0xFE4            */
  volatile UNS_32       int_clr_status ;/*  0xFE8            */
  volatile UNS_32       int_set_status ;/*  0xFEC            */
  volatile const UNS_32 _reserved2[3] ;
  volatile const UNS_32 module_id ;     /*  0xFFC            */
} SPI_REGS_T;
/***********************************************************************
 * SPI device contants
 **********************************************************************/
#define SPI_FIFO_DEPTH  64 /* 64 words (16bit) deep */
#define SPI_NUM_SLAVES  3  /* number of slaves supported */
#define SPI_MAX_DIV2    254
#define SPI_MAX_DIVIDER 65024 /* = 254 * (255 + 1) */
#define SPI_MIN_DIVIDER 2

/***********************************************************************
 * SPI Configuration register definitions
 **********************************************************************/
#define SPI_CFG_INTER_DLY(n)      _SBF(16, ((n) & 0xFFFF))
#define SPI_CFG_INTER_DLY_GET(n)  (((n) >> 16) & 0xFFFF)
#define SPI_CFG_UPDATE_EN         _BIT(7)
#define SPI_CFG_SW_RESET          _BIT(6)
#define SPI_CFG_SLAVE_DISABLE     _BIT(4)
#define SPI_CFG_MULTI_SLAVE       _BIT(3)
#define SPI_CFG_LOOPBACK          _BIT(2)
#define SPI_CFG_SLAVE_MODE        _BIT(1)
#define SPI_CFG_ENABLE            _BIT(0)
/***********************************************************************
 * SPI slave_enable register definitions
 **********************************************************************/
#define SPI_SLV_EN(n)             _SBF(((n) << 1), 0x1)
#define SPI_SLV_SUSPEND(n)        _SBF(((n) << 1), 0x3)

/***********************************************************************
 * SPI tx_fifo_flush register definitions
 **********************************************************************/
#define SPI_TXFF_FLUSH            _BIT(1)

/***********************************************************************
 * SPI dma_settings register definitions
 **********************************************************************/
#define SPI_DMA_TX_EN             _BIT(1)
#define SPI_DMA_RX_EN             _BIT(0)

/***********************************************************************
 * SPI status register definitions
 **********************************************************************/
#define SPI_ST_SMS_BUSY           _BIT(5)
#define SPI_ST_BUSY               _BIT(4)
#define SPI_ST_RX_FF              _BIT(3)
#define SPI_ST_RX_EMPTY           _BIT(2)
#define SPI_ST_TX_FF              _BIT(1)
#define SPI_ST_TX_EMPTY           _BIT(0)

/***********************************************************************
 * SPI slv_setting registers definitions
 **********************************************************************/
#define SPI_SLV1_INTER_TX_DLY(n)  _SBF(24, ((n) & 0xFF))
#define SPI_SLV1_NUM_WORDS(n)     _SBF(16, ((n) & 0xFF))
#define SPI_SLV1_CLK_PS(n)        _SBF(8, ((n) & 0xFF))
#define SPI_SLV1_CLK_PS_GET(n)    (((n) >> 8) & 0xFF)
#define SPI_SLV1_CLK_DIV1(n)      ((n) & 0xFF)
#define SPI_SLV1_CLK_DIV1_GET(n)  ((n) & 0xFF)

#define SPI_SLV2_PPCS_DLY(n)      _SBF(9, ((n) & 0xFF))
#define SPI_SLV2_CS_HIGH          _BIT(8)
#define SPI_SLV2_SSI_MODE         _BIT(7)
#define SPI_SLV2_SPO              _BIT(6)
#define SPI_SLV2_SPH              _BIT(5)
#define SPI_SLV2_WD_SZ(n)         ((n) & 0x1F)

/***********************************************************************
 * SPI int_threshold registers definitions
 **********************************************************************/
#define SPI_INT_TSHLD_TX          _SBF(8, ((n) & 0xFF))
#define SPI_INT_TSHLD_RX          ((n) & 0xFF)

/***********************************************************************
 * SPI intterrupt registers definitions
 **********************************************************************/
#define SPI_SMS_INT               _BIT(4)
#define SPI_TX_INT                _BIT(3)
#define SPI_RX_INT                _BIT(2)
#define SPI_OVR_INT               _BIT(0)
#define SPI_ALL_INTS              (SPI_SMS_INT | SPI_TX_INT | SPI_RX_INT | SPI_OVR_INT)

#ifdef __cplusplus
}
#endif

#endif /* LPC313X_SPI_H */

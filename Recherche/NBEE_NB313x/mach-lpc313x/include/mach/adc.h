#ifndef LPC31XX_ADC_H
#define LPC31XX_ADC_H

#define ADC_BASE                  0x13002000
#define ADC_LENGTH                0x00000400

/***********************************************************************
* 10-bit Analog-to-Digital Converter (ADC 10B) Register Structures
**********************************************************************/

/* 10-bit Analog-to-Digital Converter Module Register Structure */
typedef volatile struct
{
  volatile const u32 adc_r0_reg;				/* Digital conversion Channel 0*/
  volatile const u32 adc_r1_reg;				/* Digital conversion Channel 1*/
  volatile const u32 adc_r2_reg;				/* Digital conversion Channel 2*/
  volatile const u32 adc_r3_reg;				/* Digital conversion Channel 3*/
  volatile const u32 unused1[4];				/* Unused registers */
  volatile u32 adc_con_reg;     				/* ADC operation modes and status */
  volatile u32 adc_csel_reg;					/* ADC input channel selection and resolution */
  volatile u32 adc_int_enable_reg;			/* ADC interrupt request generation */
  volatile const u32 adc_int_status_reg;		/* ADC interrupt status */
  volatile u32 adc_int_clear_reg;			/* ADC interrupt clear */
  volatile const u32 unused2[1];				/* Unused register */

} ADC_REGS_T;

#define ADC    ((ADC_REGS_T*) io_p2v(ADC_BASE))


#define ADCDAT_VALUE_MASK           0x000003FF

/**********************************************************************
* ADC_CON_REG register definitions
**********************************************************************/
/* Bit for monitoring conversion status */
#define ADC_STATUS					(1<<4)
/* Bit for starting the ADC conversion */
#define ADC_START					(1<<3)
/* Bit for stoping the ADC conversion */
#define ADC_STOP					0xFFFFFFF7
/* Bit for enabling continuous conversion scan */
#define ADC_CSCAN					(1<<2)
/* Bit for enabling single conversion */
#define ADC_SINGLE_CONV				0xFFFFFFB
/* Bit for enabling the ADC */

#define ADC_ENABLE					(1<<1)
/* Bit for selecting the reference voltage input */
#define ADC_SELREF					(1<<0)

#define ADC_CON_DEFAULT_STATE		0

/**********************************************************************
* ADC_CSEL_RES_REG register definitions
**********************************************************************/
#define ADC_CHSEL_RES(n,m)			(m<<(4*n))		 /* n = channel (0,1,2,3) m = resolution (2 to 10) */
#define ADC_CSEL_DEFAULT_STATE		0

/**********************************************************************
* ADC_INT_ENABLE_REG register definitions
**********************************************************************/
/* Bit for contolling Interrupt Enable / Disable */
#define ADC_INT_ENABLE        		(1<<0)
#define ADC_INT_EN_DEFAULT_STATE	0

/**********************************************************************
* ADC_INT_STATUS_REG register definitions
**********************************************************************/
/* Bit containing Interrupt Status */
#define ADC_INT_STATUS        		(1<<0)

/**********************************************************************
* ADC_INT_CLEAR_REG register definitions
**********************************************************************/
/* Bit clearing interrupt status */
#define ADC_INT_CLEAR        		(1<<0)


void lpc31xx_adc_init(void);
void lpc31xx_adc_close(void);
u16 lpc31xx_adc_read(int channel);

#endif



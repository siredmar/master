/* lpc315x_ad.c

   written by Durgesh Pattamatta
   1 Nov 2004

   Copyright (C) 2008 NXP Semiconductors

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------
   This implementation assumes the I2C1 clock is sourced by 12MHz FFAST_IN
   clock in CGU. 
   

*/

#include <linux/types.h>
#include <linux/string.h>
#include <apex.h>
#include <command.h>
#include <error.h>
#include <linux/kernel.h>
#include <config.h>
#include <environment.h>
#include <service.h>
#include <mach/lpc313x.h>
#include <mach/hardware.h>
#include "lpc313x_cgu.h"



#define I2C_CLK_DIV_12MHZ           (60)
#define PSU_VOLT_US_DELAY           (10000)

void __section (.bootstrap) analog_reg_init(void)
{
	cgu_clk_en_dis(CGU_SB_SYSCLK_O_ID, 1);
	cgu_clk_en_dis(CGU_SB_I2C1_PCLK_ID, 1);
	I2C_REG_CKH(I2C1_PHYS) = I2C_CLK_DIV_12MHZ;
	I2C_REG_CKL(I2C1_PHYS) = I2C_CLK_DIV_12MHZ;

	I2C_REG_CTL(I2C1_PHYS) = I2C_CTL_SOFT_RESET;  //Issue soft reset of the block
}

void __section (.bootstrap) analog_reg_read(u32 reg_addr, u32 * pReg_value)
{
	u32   status = 0;

	// Write dummy value 
	*pReg_value = 0;

	I2C_REG_TX(I2C1_PHYS) = AD_I2C_WRITE_ADR;           // Send write address of the AD I2C slave with the start condition
	I2C_REG_TX(I2C1_PHYS) = ((reg_addr >> 8) & 0xFF);   // Send the high byte of the register address
	I2C_REG_TX(I2C1_PHYS) = (reg_addr & 0xFF);          // Send the low byte of the register address
	I2C_REG_TX(I2C1_PHYS) = AD_I2C_READ_ADR;            // Send read address of the AD I2C slave with the (re)start condition
	I2C_REG_TX(I2C1_PHYS) = 0xff;                       // Dummy 1
	I2C_REG_TX(I2C1_PHYS) = 0xff;                       // Dummy 2
	I2C_REG_TX(I2C1_PHYS) = 0xff;                       // Dummy 3
	I2C_REG_TX(I2C1_PHYS) = 0xff | I2C_TXFF_STOP_CND;   // Dummy 4 with the stop condition

	while ((status & (I2C_STS_NAI | I2C_STS_TDI)) == 0) {
		status = I2C_REG_STS(I2C1_PHYS);
	}

	if (status & I2C_STS_TDI) {
		// Clear the TDI status
		I2C_REG_STS(I2C1_PHYS) = I2C_STS_TDI;

		// Read out the register value
		*pReg_value = (((I2C_REG_RX(I2C1_PHYS)) & 0xFF) << 24);
		*pReg_value |= (((I2C_REG_RX(I2C1_PHYS)) & 0xFF) << 16);
		*pReg_value |= (((I2C_REG_RX(I2C1_PHYS)) & 0xFF) << 8);
		*pReg_value |= ((I2C_REG_RX(I2C1_PHYS)) & 0xFF);
	}
}

void __section (.bootstrap) analog_reg_write(u32 reg_addr, u32 reg_value)
{
	u32   status = 0;

	I2C_REG_TX(I2C1_PHYS) = AD_I2C_WRITE_ADR;           // Send write address of the AD I2C slave with the start condition
	I2C_REG_TX(I2C1_PHYS) = ((reg_addr >> 8) & 0xFF);   // Send the high byte of the register address
	I2C_REG_TX(I2C1_PHYS) = (reg_addr & 0xFF);          // Send the low byte of the register address
	I2C_REG_TX(I2C1_PHYS) = (reg_value >> 24);          // Send MSB of the register value
	I2C_REG_TX(I2C1_PHYS) = ((reg_value >> 16) & 0xFF); // Send Byte 2
	I2C_REG_TX(I2C1_PHYS) = ((reg_value >> 8) & 0xFF);  // Sent Byte1
	I2C_REG_TX(I2C1_PHYS) = ((reg_value & 0xFF)         // Send LSB of the register value
		     | I2C_TXFF_STOP_CND);   // ... with the stop condition

	while ((status & (I2C_STS_NAI | I2C_STS_TDI)) == 0) {
		status = I2C_REG_STS(I2C1_PHYS);
	}

	if (status & I2C_STS_TDI) 	{
		// Clear the TDI status
		I2C_REG_STS(I2C1_PHYS) = I2C_STS_TDI;
	}
}

void __section (.bootstrap) psu_set_voltage(u32 rail, u32 volt)
{
	u32 reg_val;
	int bit_pos;

	/* read PSU register */
	analog_reg_read (AD_REG_OTGDCLIC_RW, &reg_val);
	/* check if this is to set VOUT3 rail */
	if (ANALOG_1V8_RAIL == rail) {
		reg_val |= (volt)?_BIT(16):0;
	} else {
		/* for 1v2 bitpos is 17 and for 3v3 bit pos 20 */
		bit_pos = (rail)? 17 : 20;

		if (volt > 0x7)
			volt = 0x7;

		/* zero the dcdc1 adjust bits */
		reg_val &= ~(0x7 << bit_pos);
		/* write the new adjust value */
		reg_val |= (volt << bit_pos);
	}


	analog_reg_write (AD_REG_OTGDCLIC_RW, reg_val);
	return;

}

static int cmd_adset (int argc, const char** argv)
{
	u32 rail = 0;
	u32 volt = 0;

	if (argc != 3)
		return ERROR_PARAM;

	if (*argv[1] != '0') {
		rail = 1;
	}
	volt = simple_strtoul (argv[2], NULL, 0);

	if (volt > 7)
		return ERROR_PARAM;

	psu_set_voltage(rail, volt);

	return 0;
}

static __command struct command_d c_pause = {
	.command = "adset",
	.func = cmd_adset,
	COMMAND_DESCRIPTION ("Analog die set voltage")
	COMMAND_HELP(
	"adset rail voltage\n"
	"  Set voltage for selected rail.\n"
	"  rail =  \n"
	"	- 0 ANALOG_3V3_RAIL \n"
	"	- 1 ANALOG_1V2_RAIL \n"
	"  voltage param range is 0 to 7 \n"
	"    3V3  - 3.2(0), 3.09(1), 2.97, 2.86, 2.74, 2.63, 2.51, 2.40(7)\n"
	"    1V2  - 1.4(0), 1.33(1), 1.26, 1.19, 1.11, 1.04, 0.97, 0.90(7)\n"
	)
};


#ifndef LCD_CONFIG_H
#define LCD_CONFIG_H

//*** <<< Use Configuration Wizard in Context Menu >>> ***

/*
// <h> LCD Interface Configuration
// <e> Enable Serial Mode
//   <o1> First bit to transmit  <0=> bit 0 <1=> bit 7
//   <o2> Chip Select active <0=> High  <1=> Low
//   <o3> Serial CLK Shift Mode <0=> Mode 0 <1=> Mode 1 <2=> Mode 2 <3=> Mode 3 
// </e>
*/
#define Enable_Serial_Mode  	1
#define MSB_First_serial		0
#define Invert_CS_Serial		0
#define Serial_CLK_Shift		3

/*
// <e> Enable Parallel Mode
//   <o1> Compatibility Mode  <0=> Intel 8080 <1=> Motorola 6800
//   <o2> Data bus width  <4=> 4-bit mode  <8=> 8-bit mode <16=> 16-bit mode
//   <o3> First bits to transmit (only 4-bit mode) <0=> bits 3-0 <1=> bits 7-4
//   <o4> Chip Select active <0=> High  <1=> Low
//   <o5> E_RD signal active <0=> Low  <1=> High
// </e> 
// </h>
*/
#define Enable_Parallel_Mode  	0
#define MI_Motorola_6800		0
#define Data_bus_width			16
#define MSB_First_4_bits		1
#define Invert_CS_Parallel		1
#define E_RD_Active				1

#endif /*LCD_CONFIG_H*/

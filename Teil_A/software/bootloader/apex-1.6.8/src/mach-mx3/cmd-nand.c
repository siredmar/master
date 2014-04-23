/* cmd-nand.c

   written by Marc Singer
   2 Mar 2007

   Copyright (C) 2007 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

*/

#include <config.h>
#include <apex.h>
#include <command.h>
#include <service.h>

#include "hardware.h"

#define NFC_BUFSIZ		__REG16 (PHYS_NANDFC + 0x00)
#define NFC_BUFFER_ADDR		__REG16 (PHYS_NANDFC + 0x04)
#define NFC_FLASH_ADD		__REG16 (PHYS_NANDFC + 0x06)
#define NFC_FLASH_CMD		__REG16 (PHYS_NANDFC + 0x08)
#define NFC_CONFIG		__REG16 (PHYS_NANDFC + 0x0a)
#define NFC_ECC_STATUS		__REG16 (PHYS_NANDFC + 0x0c)
#define NFC_ECC_MAIN_AREA	__REG16 (PHYS_NANDFC + 0x0e)
#define NFC_ECC_SPARE_AREA	__REG16 (PHYS_NANDFC + 0x10)
#define NFC_WP			__REG16 (PHYS_NANDFC + 0x12)
#define NFC_UNLOCK_START	__REG16 (PHYS_NANDFC + 0x14)
#define NFC_UNLOCK_END		__REG16 (PHYS_NANDFC + 0x16)
#define NFC_WP_STATUS		__REG16 (PHYS_NANDFC + 0x18)
#define NFC_CONFIG1		__REG16 (PHYS_NANDFC + 0x1a)
#define NFC_CONFIG2		__REG16 (PHYS_NANDFC + 0x1c)

#define NFC_MAIN0_PHYS		0xb8000000
#define NFC_MAIN1_PHYS		0xb8000200
#define NFC_SPARE0_PHYS		0xb8000800

/*!
 * Set INT to 0, FCMD to 1, rest to 0 in NFC_CONFIG2 Register for Command
 * operation
 */
#define NFC_CMD            0x1

/*!
 * Set INT to 0, FADD to 1, rest to 0 in NFC_CONFIG2 Register for Address
 * operation
 */
#define NFC_ADDR           0x2

/*!
 * Set INT to 0, FDI to 1, rest to 0 in NFC_CONFIG2 Register for Input
 * operation
 */
#define NFC_INPUT          0x4

/*!
 * Set INT to 0, FDO to 001, rest to 0 in NFC_CONFIG2 Register for Data Output
 * operation
 */
#define NFC_OUTPUT         0x8

/*!
 * Set INT to 0, FD0 to 010, rest to 0 in NFC_CONFIG2 Register for Read ID
 * operation
 */
#define NFC_ID             0x10

/*!
 * Set INT to 0, FDO to 100, rest to 0 in NFC_CONFIG2 Register for Read Status
 * operation
 */
#define NFC_STATUS         0x20

/*!
 * Set INT to 1, rest to 0 in NFC_CONFIG2 Register for Read Status
 * operation
 */
#define NFC_INT            0x8000

#define NFC_SP_EN           (1 << 2)
#define NFC_ECC_EN          (1 << 3)
#define NFC_INT_MSK         (1 << 4)
#define NFC_BIG             (1 << 5)
#define NFC_RST             (1 << 6)
#define NFC_CE              (1 << 7)

/*
 * Standard NAND flash commands
 */
#define NAND_CMD_READ0          0
#define NAND_CMD_READ1          1
#define NAND_CMD_PAGEPROG       0x10
#define NAND_CMD_READOOB        0x50
#define NAND_CMD_ERASE1         0x60
#define NAND_CMD_STATUS         0x70
#define NAND_CMD_STATUS_MULTI   0x71
#define NAND_CMD_SEQIN          0x80
#define NAND_CMD_READID         0x90
#define NAND_CMD_ERASE2         0xd0
#define NAND_CMD_RESET          0xff

/* Extended commands for large page devices */
#define NAND_CMD_READSTART      0x30
#define NAND_CMD_CACHEDPROG     0x15




static void cmd_nand (int argc, const char** argv)
{
  printf ("cmd_nand\n");

  NFC_CONFIG1   = NFC_INT_MSK;
  NFC_CONFIG1  |= NFC_ECC_EN;

  NFC_FLASH_CMD = NAND_CMD_RESET;
  NFC_CONFIG2   = NFC_CMD;

  usleep (1000);

  /* Reset NAND */
  //  this->cmdfunc(mtd, NFC_CMD_RESET, -1, -1);

  NFC_CONFIG = 0x2;		/* Unlock first two (all) internal pages */

		/* Unlock all of the flash blocks */
  NFC_UNLOCK_START = 0x0;
  //  NFC_UNLOCK_END = 0x4000;
  NFC_UNLOCK_END = 0x800;
  NFC_WP = 0x4;

  usleep (1000);

  /* From the kernel */
#if 0

        /* NANDFC buffer 0 is used for device ID output */
        NFC_BUF_ADDR = 0x0;

        /* Read ID into main buffer */
        NFC_CONFIG1 &= (~(NFC_SP_EN));
        NFC_CONFIG2 = NFC_ID;
#endif

  /* From RedBoot */
#if 0
    NFC_PRESET(flash_dev_info->block_count - 1);
    NFC_CMD_INPUT(FLASH_Read_ID); // 0x90
     // 0x90 ==> CMD_REG
     //  1   ==> CONFIG2_REG
    start_nfc_addr_ops(ADDRESS_INPUT_READ_ID, 0, 0); // NFC_ADDR_INPUT(0)
    // writes FLASH_ADD_REG and then FLASH_CONFIG2_REG with FADD_EN
    NFC_DATA_OUTPUT(SCRATCH_BUF == RAM_BUF_3 == 3, FDO_FLASH_ID == 0x10);

    *id = readw(nand_main_buf[SCRATCH_BUF]);
#endif

  NFC_BUFFER_ADDR = 0;
  NFC_CONFIG1 = 0x10;
  NFC_CONFIG2 = NFC_ID;
}

static __command struct command_d c_reset = {
  .command = "nand",
  .func = (command_func_t) cmd_nand,
  COMMAND_DESCRIPTION ("nand test")
  COMMAND_HELP(
"nand\n"
"  NAND test.\n"
  )
};

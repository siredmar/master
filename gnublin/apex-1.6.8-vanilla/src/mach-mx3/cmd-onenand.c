/* cmd-onenand.c

   written by Marc Singer
   30 Mar 2007

   Copyright (C) 2007 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   OneNAND driver and test routines.

   o Partial Writes.  We must infer from the OneNAND documentation
     that rewriting portions of a page require a read-modify-write
     cycle.  We don't really assume that we can rewrite a page from
     APEX, so the HW implementation details are somewhat irrelevent.

*/

#include <config.h>
#include <apex.h>
#include <command.h>
#include <service.h>
#include <linux/string.h>
#include <error.h>
#include <linux/kernel.h>
#include <driver.h>
#include <service.h>
#include <spinner.h>
#include <drv-onenand-base.h>

#include "hardware.h"

static int cmd_onenand (int argc, const char** argv)
{
  printf ("cmd_onenand\n");

  if (argc == 1) {
    printf ("  manu 0x%x  dev 0x%x  ver 0x%x  data 0x%x"
	    "  boot 0x%x  cnt 0x%x\n",
	    ONENAND_MAN_ID, ONENAND_DEV_ID, ONENAND_VER_ID,
	    ONENAND_DATA_SIZE, ONENAND_BOOT_SIZE, ONENAND_BUFF_CNT);
    printf ("  sa 1 0x%04x  2 0x%04x  3 0x%04x  4 0x%04x\n"
	    "     5 0x%04x  6 0x%04x  7 0x%04x  8 0x%04x\n",
	    ONENAND_SA_1, ONENAND_SA_2, ONENAND_SA_3, ONENAND_SA_4,
	    ONENAND_SA_5, ONENAND_SA_6, ONENAND_SA_7, ONENAND_SA_8);
    printf ("  sb 0x%04x\n", ONENAND_SB);
    printf ("  status 0x%04x  int 0x%04x  cfg1 0x%04x\n",
	    ONENAND_STATUS, ONENAND_INTR, ONENAND_CONFIG_1);
    printf ("  wp 0x%04x\n", ONENAND_WP_STATUS);
    return 0;
  }

  if (PARTIAL_MATCH (argv[1], "se", "lect") == 0) {
    int chip;
    if (argc < 3)
      ERROR_RETURN (ERROR_PARAM, "select requires chip number");
    chip = simple_strtoul (argv[2], NULL, 0);
    ONENAND_SA_2 = DBS (chip);
  }

  if (PARTIAL_MATCH (argv[1], "r", "eset") == 0) {
    ONENAND_OP = ONENAND_CMD_RESET_CORE;
  }

  if (PARTIAL_MATCH (argv[1], "l", "oad") == 0) {
    int page;
    if (argc < 3)
      ERROR_RETURN (ERROR_PARAM, "load requires page number");
    page = simple_strtoul (argv[2], NULL, 0);
    ONENAND_SA_1 = DFS_FBA (page/PAGES_PER_BLOCK);
    ONENAND_SA_2 = DBS (0);
    ONENAND_SA_8 = FPA_FSA ((page%PAGES_PER_BLOCK)*4);
    ONENAND_SB = BSA_BSC (1,0,4);
    ONENAND_INTR = 0;
    ONENAND_CMD = ONENAND_CMD_LOAD;
    printf ("NAND_INTR 0x%x  NAND_STATUS 0x%x\n",
	    ONENAND_INTR, ONENAND_STATUS);
    while (ONENAND_IS_BUSY)
      ;
    if (ONENAND_IS_ERROR)
      printf ("error\n");
  }

  if (PARTIAL_MATCH (argv[1], "st", "ore") == 0) {
    int page;
    if (argc < 3)
      ERROR_RETURN (ERROR_PARAM, "load requires page number");
    page = simple_strtoul (argv[2], NULL, 0);
    ONENAND_SA_1 = DFS_FBA (page/PAGES_PER_BLOCK);
    ONENAND_SA_2 = DBS (0);
    ONENAND_SA_8 = FPA_FSA ((page%PAGES_PER_BLOCK)*4);
    ONENAND_SB = BSA_BSC (1,0,4);
    ONENAND_INTR = 0;
    ONENAND_CMD = ONENAND_CMD_PROGRAM;
    while (ONENAND_IS_BUSY)
      ;
    if (ONENAND_IS_ERROR)
      printf ("error\n");
  }

  if (PARTIAL_MATCH (argv[1], "u", "nlock") == 0) {
    int page;
    if (argc < 3)
      ERROR_RETURN (ERROR_PARAM, "unlock requires page number");
    page = simple_strtoul (argv[2], NULL, 0);
    ONENAND_SA_1 = DFS_FBA (page/PAGES_PER_BLOCK);
    ONENAND_SA_2 = DBS (0);
    ONENAND_SBA  = SBA (page);
    ONENAND_INTR = 0;
    ONENAND_CMD = ONENAND_CMD_UNLOCK;
    while (ONENAND_IS_BUSY)
      ;
    if (ONENAND_IS_ERROR)
      printf ("error\n");
  }

  if (PARTIAL_MATCH (argv[1], "bu", "rst") == 0) {
    ONENAND_CONFIG_1 |= 0x8000;
  }

  if (PARTIAL_MATCH (argv[1], "sy", "nc") == 0) {
    __REG (0xb8002000)
      |= (1<<20)		/* SYNC */
      |  (2<<28)		/* BCD, /3 */
      |  (1<<7)			/* EW */
      ;
  }

  if (PARTIAL_MATCH (argv[1], "bo", "ot") == 0) {
//    memset ((void*) ONENAND_DATARAM0, 0xa5, 2048);
    ONENAND_SA_1 = DFS_FBA (0);
    ONENAND_SA_2 = DBS (0);
    ONENAND_SA_8 = FPA_FSA (2);
    ONENAND_SB = BSA_BSC (1,0,2);
    printf ("sa1 0x%x sa2 0x%x sa8 0x%x sb 0x%x\n",
	    ONENAND_SA_1, ONENAND_SA_2, ONENAND_SA_8, ONENAND_SB);
    printf ("NAND_INTR 0x%x  NAND_STATUS 0x%x\n",
	    ONENAND_INTR, ONENAND_STATUS);
    ONENAND_INTR = 0;
    ONENAND_CMD = ONENAND_CMD_LOAD;
    while (ONENAND_IS_BUSY)
      ;
    if (ONENAND_IS_ERROR)
      printf ("error\n");

    ONENAND_SA_1 = DFS_FBA (0);
    ONENAND_SA_2 = DBS (0);
    ONENAND_SA_8 = FPA_FSA (4);
    ONENAND_SB = BSA_BSC (1,2,2);
    printf ("sa1 0x%x sa2 0x%x sa8 0x%x sb 0x%x\n",
	    ONENAND_SA_1, ONENAND_SA_2, ONENAND_SA_8, ONENAND_SB);
    printf ("NAND_INTR 0x%x  NAND_STATUS 0x%x\n",
	    ONENAND_INTR, ONENAND_STATUS);
    ONENAND_INTR = 0;
    ONENAND_CMD = ONENAND_CMD_LOAD;
    while (ONENAND_IS_BUSY)
      ;
    if (ONENAND_IS_ERROR)
      printf ("error\n");
  }

  return 0;
}

static __command struct command_d c_onenand = {
  .command = "onenand",
  .func = (command_func_t) cmd_onenand,
  COMMAND_DESCRIPTION ("OneNAND test")
  COMMAND_HELP(
"onenand\n"
"  OneNAND test.\n"
"  select CHIP     - Select chip 0 or 1 within the OneNAND part\n"
"  reset           - Resets the OneNAND chip controller\n"
"  load PAGE       - Loads a page from OneNAND to Data0\n"
"  store PAGE      - Writes page to OneNAND from Data0\n"
"  unlock PAGE     - Unlock a page\n"
"  burst           - Enable burst read mode\n"
"  boot            - Reset to boot-time status [?]\n"
  )
};

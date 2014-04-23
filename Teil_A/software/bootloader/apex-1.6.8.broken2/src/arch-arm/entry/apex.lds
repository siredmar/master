/*
 * Automatically generated C config: don't edit
 * Linux kernel version: KERNELVERSION
 * Mon Mar  3 12:11:36 2014
 */
/* Linking loader script for APEX Boot Loader */
OUTPUT_ARCH(arm)
EXTERN(entry)
ENTRY(entry)
SECTIONS
{
 . = 0x11029000;
 APEX_VMA_ENTRY = .;
 APEX_VMA_START = .;
 APEX_VMA_COPY_START = .;
 .entry : { /* First code to execute */
  *(.entry)
 }
  /* The placement of exception vectors here, in the
		   boot loader image, isn't necessarily helpful.  It
		   is only worthwhile if these vectors jump to code
		   within the loader that we can see.  It may be the
		   case that the only way to define useful exception
		   vectors is to enable the MMU. */
 .rom.exceptions : { /* Exception vector code for ROM vectors */
  *(.rom.exceptions)
 }
 .envlink : { /* Link to $env for user-land */
  *(.envlink)
 }
 . = ALIGN (4); /* should be redundant */
 .reset : { /* Reset handler, first real code */
  *(.reset)
  *(.reset.exit)
 }
 .bootstrap.early : { /* Early bootstrap setups */
  *(.bootstrap.early)
  *(.bootstrap.early.func)
  *(.bootstrap.early.exit)
 }
 .rlocate.early : { /* Early relocation for constrained boot */
  *(.rlocate.early)
  *(.rlocate.early.func)
  *(.rlocate.early.exit)
 }
 .bootstrap.pre : { /* Mandatory bootstrap initializations  */
  *(.bootstrap.pre)
  *(.bootstrap.pre.func)
  *(.bootstrap.pre.exit)
 }
 .bootstrap.sdram.pre : { /* Pre-SDRAM init code, check PC in SDRAM */
  *(.bootstrap.sdram.pre)
  *(.bootstrap.sdram.pre.func)
  *(.bootstrap.sdram.pre.exit)
 }
 .bootstrap.sdram : { /* Platform SDRAM initialization */
  *(.bootstrap.sdram)
  *(.bootstrap.sdram.func)
  *(.bootstrap.sdram.exit)
 }
 .bootstrap.sdram.post : { /* Post-SDRAM init code.*/
  *(.bootstrap.sdram.post)
  *(.bootstrap.sdram.post.cont)
  *(.bootstrap.sdram.post.func)
  *(.bootstrap.sdram.post.exit)
 }
 .rlocate : { /* Final/only relocation of APEX to SDRAM */
  *(.rlocate)
  *(.rlocate.func)
  *(.rlocate.exit)
 }
 .reset.finish : {
  *(.reset.finish)
  *(.reset.finish.func)
  *(.reset.finish.exit)
 }
 .bootstrap : { /* Bootstrap initialization */
  *(.bootstrap) /* Target initialization, out-of-line */
 }
  /* Everything up to this point is used to get the
		   target off it's back and onto it's feet.  In the
		   case of a NOR boot, it doesn't really matter how
		   much code this is.  In the case of NAND, I2C, and
		   MMC booting, there may be significant constrants on
		   the amount of the boot loader that is available for
		   execution.  The early sections are meant to be very
		   small and critical for getting enough of the loader
		   into some sort of memory so that the SDRAM can be
		   initialized.  We may get as little as 512 bytes to
		   begin with, so the apexrelocate.early section is
		   the first opportunity to load more of the loader
		   into memory should we be highly constrained.  After
		   that, we must have enough in memory to include
		   everything in .bootstrap (or whatever preceeds
		   .text).  The relocate_apex() function that makes
		   sure *all* of APEX is in memory must be able to
		   execute to bring all of the loader into 


		/* Everything up to this point must fit in the first
		   4K (determined by the CPU boot rom) of NAND flash
		   so that we can bootstrap copy the loader from NAND
		   to SDRAM. */
 .vector : { /* Exception vector support, if there is any */
  APEX_VMA_VECTOR_START = .;
  *(.vector.0)
  *(.vector.1)
  APEX_VMA_VECTOR_END = .;
  *(.vector)
 }
 .text : {
  *(.text)
  *(.fixup)
  *(.rodata)
  *(.rodata.*)
  *(.glue_7)
  *(.glue_7t)
 }
 . = ALIGN (4);
 APEX_DATA_START = .;
 /* Discard data because there *must* not be mutable,
	   initialized data.  Use __rodata attribute for initialized
	   data to move it into the text section. */
 /DISCARD/ . : {
  *(.data)
 }
 . = ALIGN (4);
 .service . : {
  APEX_SERVICE_START = .;
  *(.service.0)
  *(.service.1)
  *(.service.2)
  *(.service.3)
  *(.service.4)
  *(.service.5)
  *(.service.6)
  *(.service.7)
  *(.service.8)
  *(.service.9)
  APEX_SERVICE_END = .;
 }
 .driver . : {
  APEX_DRIVER_START = .;
  *(.driver.0)
  *(.driver.1)
  *(.driver.2)
  *(.driver.3)
  *(.driver.4)
  *(.driver.5)
  *(.driver.6)
  *(.driver.7)
  APEX_DRIVER_END = .;
 }
 .atag . : {
  APEX_ATAG_START = .;
  *(.atag.0)
  *(.atag.1)
  *(.atag.2)
  *(.atag.3)
  *(.atag.4)
  *(.atag.5)
  *(.atag.6)
  *(.atag.7)
  APEX_ATAG_END = .;
 }
 .command . : {
  APEX_COMMAND_START = .;
  *(.command)
  APEX_COMMAND_END = .;
 }
 .env . : {
  APEX_ENV_START = .;
  *(.env)
  APEX_ENV_END = .;
 }
 .pcm.data . : {
  *(.pcm.data)
 }
 APEX_DATA_END = .;
 APEX_VMA_COPY_END = .;
 . = ALIGN (4);
 APEX_VMA_PROBE_END = .;
 APEX_IMAGE_SIZE = APEX_VMA_COPY_END - APEX_VMA_COPY_START;
 APEX_IMAGE_512_SIZE = (APEX_IMAGE_SIZE + 511) & ~ 511;
 /* The next word is the location we'll be writing to when
	   probing.  See note in drv-mem.c. */
 APEX_VMA_STACKS_START = .;
 . += 0x1000;
 . = ALIGN (4);
 APEX_VMA_STACK_START = .;


 APEX_VMA_STACKS_END = .;
 . = ALIGN (4);
 /* *** Here is where we'd insert the checksum avoidance block.
	       However, it should be aligned to a cache line, at the
	       very least. */
 .bss . : { /* Normal, zeroed BSS */
  APEX_VMA_BSS_START = .;
  *(.bss)
  . = ALIGN (4);
  *(.*.bss) /* All other bss sections */
  APEX_VMA_BSS_END = .;
 }
 .xbss (NOLOAD) : { /* Special BSS, non-zeroed */




  . = ALIGN (4);
  *(.*.xbss) /* Misc xbss sections */
  *(.pcm.xbss) /* Audio xbss data */
 }
 APEX_VMA_END = .;
}

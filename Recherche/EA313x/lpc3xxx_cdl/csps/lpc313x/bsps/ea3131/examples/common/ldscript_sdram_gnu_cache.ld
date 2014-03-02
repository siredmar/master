 /*;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 ; $Id:: ldscript_sdram_gnu_cache.ld 5978 2010-12-22 05:59:12Z ing0300#$
 ; 
 ; Project: VAL3153 example code linker file for GNU toolset
 ; 
 ; Description: 
 ;     This file implements a linker script for the NXP VAL3153 board
 ;     when used with the VAL3153 example code.
 ;
 ; Notes: None
 ;
 ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;  
 ; Software that is described herein is for illustrative purposes only  
 ; which provides customers with programming information regarding the  
 ; products. This software is supplied "AS IS" without any warranties.  
 ; NXP Semiconductors assumes no responsibility or liability for the 
 ; use of the software, conveys no license or title under any patent, 
 ; copyright, or mask work right to the product. NXP Semiconductors 
 ; reserves the right to make changes in the software without 
 ; notification. NXP Semiconductors also make no representation or 
 ; warranty that such application will be suitable for the specified 
 ; use without further testing or modification. 
 ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;*/

/* Define the supported output formats - elf32-littlearm is the
   default */
OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")

/* Define the target architecture */
OUTPUT_ARCH(arm)

/* Define the system entry point */
ENTRY(__start)

/* Define the memory layout for the board */
SECTIONS
{
    /* Set up the location counter */
    . = 0x41000000;

    /* Define a execution region that resides at ram base */
    .text :
    {
        /* Start of .text segment */
        __exidx_start = .;
        PROVIDE (__gnu_textstart = .);
		ea3131_startup_entry.o(.text .text.*)
        *(.text .text.*)               
    }

    /* Define a readonly data section in flash that resides at the
       end of the .rom execution region */
    .rodata : 
    { 
        /* Place all .rodata regions in this section. */
        *(.rodata .rodata.*); 
    }

    /* (Real) End of .text segment */
    __exidx_end = .;
    PROVIDE (__gnu_textend = .);

    /* Define a read/write section at the end of the .text section */
    .data :
    {
        /* Start of .data segment */
        PROVIDE (__gnu_datastart = .);

        *(.data .data.*); 

        /* End of .data segment */
        PROVIDE (__gnu_dataend = .);
    }

    /* Define a ZI section at the end of the .data section */
    .bss :
    {
        /* Start of .bss segment */
        PROVIDE (__gnu_bssstart = .);

        *(.bss .bss.*) *(COMMON); 

        /* End of .bss segment */
        PROVIDE (__gnu_bssend = .);
    }

    /* Define linker variable to reference end address of this
       section - required by gcc */
    PROVIDE (end = .);
    PROVIDE (__end = .);

	/* pad the image size to 512bytes boundary */
	__image_size = ((__gnu_bssstart - __gnu_textstart) + 511) & ~ 511;


      /* The __EH_FRAME_BEGIN__ symbol is not used, but the linker
         thinks it needs it for some libraries */
      __EH_FRAME_BEGIN__ = 0;

}

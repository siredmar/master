/* env.c

   written by Durgesh Pattamatta
   10 Oct 2008

   Copyright (C) 2008 NXP Semiconductors

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.


   -----------
   DESCRIPTION
   -----------

   Environment for the LPC313x.

*/

#include <config.h>
#include <environment.h>
#include <driver.h>
#include <service.h>

#if ! defined (CONFIG_ENV_DEFAULT_CMDLINE)

__env struct env_d e_cmdline = {
  .key = "cmdline",
#if defined(CONFIG_MACH_VAL3153) || defined(CONFIG_MACH_EA313x_V2) || defined(CONFIG_MACH_EA3152)
  .default_value = "console=ttyS0,115200n8 root=/dev/nfs rw nfsroot=/tftpboot/arm ip=rarp loglevel=7",
  //.default_value = "console=ttyS0,115200n8 root=/dev/ram0 rw ip=rarp loglevel=7 ramdisk_size=12000K",
#else
  .default_value = "console=ttyS0,115200n8 root=/dev/ram0 rw loglevel=7 ramdisk_size=12000K",
#endif
  .description = "Linux kernel command line",
};

#endif

#if defined (CONFIG_ENV_LINK)
#error "For LPC313x CONFIG_ENV_LINK can't be used."
#endif

extern char APEX_ENV_START;
extern char APEX_ENV_END;
extern char APEX_VMA_COPY_START;
extern char APEX_VMA_COPY_END;
extern char APEX_IMAGE_512_SIZE;

typedef struct LPC313x_BOOT_HDR 
{
    u32	 magic;
  	u32  image_crc32;
  	u32  rsrvd_img[4];
  	u32  imageType;
  	u32  imageLength;
  	u32  releaseID;
  	u32  buildTime;
  	u32  sbzBootParameter;
    
    u32  apex_env_magic;
    const char apexversion[16];
    void* apex_env_link;		/* Align loader within flash */
    void* apex_start;
    void* apex_end;
    void* apex_env_start;
    void* apex_env_end;
    u32 apex_env_d_size;
    const char apex_region[16];
  	
  	u32  header_crc32;
  	u32  rsrvd_hdr[4];
} LPC313x_BOOT_HDR;

static __used __section (.envlink) LPC313x_BOOT_HDR env_link = {
  .magic	= 0x41676d69,
  .imageType	= 10,
  .imageLength = (u32)&APEX_IMAGE_512_SIZE,

  .apex_env_magic	= ENV_LINK_MAGIC,
  .apexversion	= APEXVERSION,
  .apex_start	= &APEX_VMA_COPY_START, /* Immutable portion of APEX */
  .apex_end	= &APEX_VMA_COPY_END,
  .apex_env_start	= &APEX_ENV_START,	/* Environment variable def's */
  .apex_env_end	= &APEX_ENV_END,
  .apex_env_link	= &(env_link.apex_env_magic),
  .apex_env_d_size	= sizeof (struct env_d),
#if defined (CONFIG_ENV_REGION)
  .apex_region	= CONFIG_ENV_REGION,
#endif
 };


/* env.c

   written by Marc Singer
   10 Nov 2004

   Copyright (C) 2004 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   Default environment variables.

*/

#include <config.h>
#include <apex.h>
#include <environment.h>
#include <driver.h>
#include <service.h>
#include <linux/string.h>

#define _s(v) #v
#define _t(v) _s(v)

#if defined (CONFIG_ARCH_NUMBER)
static __env struct env_d e_arch_number = {
  .key = "arch-number",
  .default_value = _t(CONFIG_ARCH_NUMBER),
  .description = "Linux Kernel Architecture Number",
};
#endif

#if defined (CONFIG_KERNEL_LMA)
static __env struct env_d e_bootaddr = {
  .key = "bootaddr",
  .default_value = _t(CONFIG_KERNEL_LMA),
  .description = "Linux start address",
};
#endif

#if defined (CONFIG_RAMDISK_LMA)
static __env struct env_d e_ramdiskaddr = {
  .key = "ramdiskaddr",
  .default_value = _t(CONFIG_RAMDISK_LMA),
  .description = "Ramdisk start address for kernel",
};
#endif

#if defined (CONFIG_RAMDISK_SIZE)
static __env struct env_d e_ramdisksize = {
  .key = "ramdisksize",
  .default_value = _t(CONFIG_RAMDISK_SIZE),
  .description = "Ramdisk size for kernel",
};
#endif

#if defined (CONFIG_ENV_REGION_KERNEL)
static __env struct env_d e_region_kernel = {
  .key = "kernelsrc",
  .default_value = CONFIG_ENV_REGION_KERNEL,
  .description = "Kernel source region",
};
#endif

#if defined (CONFIG_ENV_REGION_KERNEL_ALT)
static __env struct env_d e_region_kernel_alt = {
  .key = "kernelsrc" CONFIG_VARIATION_SUFFIX,
  .default_value = CONFIG_ENV_REGION_KERNEL_ALT,
  .description = "Alternative kernel source region",
};
#endif

#if defined (CONFIG_ENV_REGION_RAMDISK)
static __env struct env_d e_region_ramdisk = {
  .key = "ramdisksrc",
  .default_value = CONFIG_ENV_REGION_RAMDISK,
  .description = "Ramdisk image source region",
};
#endif

#if defined (CONFIG_ENV_REGION_RAMDISK_ALT)
static __env struct env_d e_region_ramdisk_alt = {
  .key = "ramdisksrc" CONFIG_VARIATION_SUFFIX,
  .default_value = CONFIG_ENV_REGION_RAMDISK_ALT,
  .description = "Alternative ramdisk source region",
};
#endif

#if defined (CONFIG_ENV_DEFAULT_STARTUP_ALT)
static __env struct env_d e_startup_alt = {
  .key = "startup" CONFIG_VARIATION_SUFFIX,
  .default_value = CONFIG_ENV_DEFAULT_STARTUP_ALT,
  .description = "Alternative startup commands",
};
#endif

#if defined (CONFIG_ENV_DEFAULT_STARTUP_OVERRIDE)
static __env struct env_d e_startup = {
  .key = "startup",
  .default_value = CONFIG_ENV_DEFAULT_STARTUP,
  .description = "Startup commands",
};
#elif (defined (CONFIG_ENV_REGION_KERNEL) && defined (CONFIG_KERNEL_LMA))\
    || defined (CONFIG_ENV_STARTUP_PREFIX)\
    || defined (CONFIG_ENV_STARTUP_KERNEL_COPY)\
    || defined (CONFIG_ENV_STARTUP_RAMDISK_COPY)\
    || defined (CONFIG_AUTOBOOT)
static __env struct env_d e_startup = {
  .key = "startup",
  .default_value =
#if defined (CONFIG_ENV_STARTUP_PREFIX)
  CONFIG_ENV_STARTUP_PREFIX
  "; "
#endif
#if defined (CONFIG_ENV_STARTUP_KERNEL_COPY)
    "copy "
# if defined (CONFIG_ENV_REGION_KERNEL_SWAP)
    "-s "
# endif
     "$kernelsrc $bootaddr; "
#endif
#if defined (CONFIG_ENV_STARTUP_RAMDISK_COPY)
    "copy "
# if defined (CONFIG_ENV_REGION_RAMDISK_SWAP)
    "-s "
# endif
    "$ramdisksrc $ramdiskaddr; "
#endif
#if defined (CONFIG_AUTOBOOT)
# if CONFIG_AUTOBOOT_DELAY != 0
    "wait " _t(CONFIG_AUTOBOOT_DELAY) " Type ^C key to cancel autoboot.; "
# endif
    "boot"
#endif
  "",
  .description = "Startup commands",
};
#endif

#if defined (CONFIG_ENV_LINK)

extern char APEX_ENV_START;
extern char APEX_ENV_END;
extern char APEX_VMA_COPY_START;
extern char APEX_VMA_COPY_END;

static __used __section (.envlink) struct env_link env_link = {
  .magic	= ENV_LINK_MAGIC,
  .apexversion	= APEXVERSION,
  .apex_start	= &APEX_VMA_COPY_START, /* Immutable portion of APEX */
  .apex_end	= &APEX_VMA_COPY_END,
  .env_start	= &APEX_ENV_START,	/* Environment variable def's */
  .env_end	= &APEX_ENV_END,
  .env_link	= &env_link,
  .env_d_size	= sizeof (struct env_d),
#if defined (CONFIG_ENV_REGION)
  .region	= CONFIG_ENV_REGION,
#endif
 };

#endif

#if defined (CONFIG_ENV_REGION)
# if defined (CONFIG_ENV_LINK)
#  define ENV_REGION_STRING env_link.region
# else
#  define ENV_REGION_STRING CONFIG_ENV_REGION
# endif

static void env_init (void)
{
  if (parse_descriptor (ENV_REGION_STRING, &d_env))
    return;
  if (d_env.driver->open (&d_env)) {
    d_env.driver->close (&d_env);
    return;
  }

  pd_env = &d_env;

# if defined (CONFIG_ENV_SAVEATONCE) && defined (CONFIG_ENV_SIZE)
  d_env.driver->seek (&d_env, 0, SEEK_SET);
  if (d_env.driver->read (&d_env, g_rgbEnv, CONFIG_ENV_SIZE)
      != CONFIG_ENV_SIZE)
    memset (g_rgbEnv, 0, CONFIG_ENV_SIZE); /* Failed read?  Very
    	   	      	 		      bad. Set the array to
					      zero so we don't write
					      to it. */
  parse_descriptor_simple ("memory", (unsigned long) g_rgbEnv, CONFIG_ENV_SIZE,
			   &d_envmem);
  pd_env = &d_envmem;
#endif

}

static __service_7 struct service_d env_service = {
  .init = env_init,
};
#endif

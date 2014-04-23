/* npe.c

   written by Marc Singer
   14 Feb 2005

   Copyright (C) 2005 Marc Singer

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   version 2 as published by the Free Software Foundation.
   Please refer to the file debian/copyright for further details.

   -----------
   DESCRIPTION
   -----------

   This service performs initialization of the NPEs.  It also has to
   provide sufficient glue to the Intel Access Library to let the
   library do it's work.

*/

#include <config.h>
#include <apex.h>
#include <service.h>
#include <linux/types.h>
#include <linux/linkage.h>
#include <linux/time.h>
#include <linux/utsname.h>
#include <linux/string.h>

unsigned long jiffies;
struct new_utsname system_utsname;

int printk (const char* fmt, ...)
{
  return 0;
}

volatile void __bug (const char* file, int line, void* data)
{
}

int request_irq (int irq, void* func, long flags, char* driver, void* data)
{
  return 0;
}

void free_irq (unsigned int irq, void* data)
{
}

asmlinkage void schedule (void)
{
}

void* __ioremap (unsigned long port, unsigned long size)
{
  return NULL;
}

void __iounmap (void* pv)
{
}

void* __kmalloc (size_t cb, int flags)
{
  return NULL;
}

void kfree (void* pv)
{
}

int in_irq (void)
{
  return 0;
}

void do_gettimeofday (struct timeval* tv)
{
}

signed long schedule_timeout (signed long timeout)
{
  return 0;
}

asmlinkage void __down_failed(void) {}
asmlinkage int  __down_interruptible_failed(void) { return 0; }
asmlinkage int  __down_trylock_failed(void) { return 0; }
asmlinkage void __up_wakeup(void) {}

extern int _npe_init (void);

static void npe_init (void)
{
  _npe_init ();
}

void __udelay (unsigned long us)
{
  usleep (us);
}

#undef memset

void* memset(void* pv, int v, __kernel_size_t cb)
{
  return pv;
}


static __service_7 struct service_d npe_service = {
  .init = npe_init,
#if !defined (CONFIG_SMALL)
  //  .report = npe_report,
#endif
};

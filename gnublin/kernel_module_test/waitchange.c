#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <mach/hardware.h>
#include <mach/gpio.h>
#include <linux/timer.h>
#include <linux/platform_device.h>
#include <mach/hardware.h>
#include <mach/gpio.h>
#include <asm/io.h>


// Defining __KERNEL__ and MODULE allows us to access kernel-level code not usually available to userspace programs.
#undef __KERNEL__
#define __KERNEL__

#undef MODULE
#define MODULE

// Linux Kernel/LKM headers: module.h is needed by all modules and kernel.h is needed for KERN_INFO.
#include <linux/module.h>    // included for all kernel modules
#include <linux/kernel.h>    // included for KERN_INFO
#include <linux/init.h>        // included for __init and __exit macros

static int __init hello_init(void)
{
   printk("Changing MPMCST0 wait cycles\n");
    MPMC_STCONFIG0 = 0x81;
    MPMC_STWTWEN0  = 0;
    MPMC_STWTOEN0  = 0;
    MPMC_STWTRD0   = 31;
    MPMC_STWTPG0   = 0;
    MPMC_STWTWR0   = 0;
    MPMC_STWTTURN0 = 15;
    SYS_MPMC_WTD_DEL0 = 0;
    return 0;    // Non-zero return means that the module couldn't be loaded.
}

static void __exit hello_cleanup(void)
{
   printk(KERN_ALERT "MPMPST0 wait cycles changed - goodbye...\n");
}

module_init(hello_init);
module_exit(hello_cleanup);

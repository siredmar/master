#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = KBUILD_MODNAME,
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
 .arch = MODULE_ARCH_INIT,
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=gspca_main";

MODULE_ALIAS("usb:v093Ap2600d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v093Ap2601d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v093Ap2603d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v093Ap2608d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v093Ap260Ed*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v093Ap260Fd*dc*dsc*dp*ic*isc*ip*");

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
"depends=usbnet";

MODULE_ALIAS("usb:v04DDp8004d*dc*dsc*dp*ic02isc06ip00*");
MODULE_ALIAS("usb:v04DDp8005d*dc*dsc*dp*ic02isc06ip00*");
MODULE_ALIAS("usb:v04DDp8006d*dc*dsc*dp*ic02isc06ip00*");
MODULE_ALIAS("usb:v04DDp8007d*dc*dsc*dp*ic02isc06ip00*");
MODULE_ALIAS("usb:v04DDp9031d*dc*dsc*dp*ic02isc06ip00*");
MODULE_ALIAS("usb:v04DDp9032d*dc*dsc*dp*ic02isc06ip00*");
MODULE_ALIAS("usb:v04DDp9050d*dc*dsc*dp*ic02isc06ip00*");
MODULE_ALIAS("usb:v07B4p0F02d*dc*dsc*dp*ic02isc06ip00*");
MODULE_ALIAS("usb:v*p*d*dc*dsc*dp*ic02isc06ip00*");
MODULE_ALIAS("usb:v0BDBp1900d*dc*dsc*dp*ic02isc0Aip00*");
MODULE_ALIAS("usb:v0BDBp1902d*dc*dsc*dp*ic02isc0Aip00*");
MODULE_ALIAS("usb:v0BDBp1904d*dc*dsc*dp*ic02isc0Aip00*");
MODULE_ALIAS("usb:v0BDBp1905d*dc*dsc*dp*ic02isc0Aip00*");
MODULE_ALIAS("usb:v0BDBp1906d*dc*dsc*dp*ic02isc0Aip00*");
MODULE_ALIAS("usb:v0BDBp190Ad*dc*dsc*dp*ic02isc0Aip00*");
MODULE_ALIAS("usb:v0BDBp1909d*dc*dsc*dp*ic02isc0Aip00*");
MODULE_ALIAS("usb:v0BDBp1049d*dc*dsc*dp*ic02isc0Aip00*");
MODULE_ALIAS("usb:v0BDBp190Bd*dc*dsc*dp*ic02isc0Aip00*");
MODULE_ALIAS("usb:v0930p130Bd*dc*dsc*dp*ic02isc0Aip00*");
MODULE_ALIAS("usb:v0930p130Cd*dc*dsc*dp*ic02isc0Aip00*");
MODULE_ALIAS("usb:v0930p1311d*dc*dsc*dp*ic02isc0Aip00*");
MODULE_ALIAS("usb:v413Cp8147d*dc*dsc*dp*ic02isc0Aip00*");
MODULE_ALIAS("usb:v413Cp8183d*dc*dsc*dp*ic02isc0Aip00*");
MODULE_ALIAS("usb:v413Cp8184d*dc*dsc*dp*ic02isc0Aip00*");

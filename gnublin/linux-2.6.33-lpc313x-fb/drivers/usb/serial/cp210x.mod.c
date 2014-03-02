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
"depends=usbserial";

MODULE_ALIAS("usb:v0471p066Ad*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0489pE000d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0745p1000d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v08E6p5501d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v08FDp000Ad*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0FCFp1003d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0FCFp1004d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0FCFp1006d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v10A6pAA26d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v10ABp10C5d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v10B5pAC70d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v10C4p0F91d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v10C4p1101d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v10C4p1601d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v10C4p800Ad*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v10C4p803Bd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v10C4p8053d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v10C4p8054d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v10C4p8066d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v10C4p807Ad*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v10C4p80CAd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v10C4p80DDd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v10C4p80F6d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v10C4p8115d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v10C4p813Dd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v10C4p813Fd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v10C4p814Ad*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v10C4p814Bd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v10C4p815Ed*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v10C4p819Fd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v10C4p81A6d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v10C4p81ACd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v10C4p81C8d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v10C4p81E2d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v10C4p81E7d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v10C4p81F2d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v10C4p8218d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v10C4p822Bd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v10C4p826Bd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v10C4p8293d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v10C4p82F9d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v10C4p8341d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v10C4p8382d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v10C4p83A8d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v10C4p8411d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v10C4p846Ed*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v10C4pEA60d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v10C4pEA61d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v10C4pF001d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v10C4pF002d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v10C4pF003d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v10C4pF004d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v10C5pEA61d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v10CEpEA6Ad*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v13ADp9999d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v1555p0004d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v166Ap0303d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v16D6p0001d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v18EFpE00Fd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v413Cp9500d*dc*dsc*dp*ic*isc*ip*");

MODULE_INFO(srcversion, "8EF71F03D512AF963A2B094");

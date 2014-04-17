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
"depends=";

MODULE_ALIAS("usb:v0870p0001d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0E8Dp0003d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0E8Dp3329d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0482p0203d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v079Bp000Fd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0ACEp1602d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0ACEp1608d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0ACEp1611d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v22B8p7000d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0803p3095d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0572p1321d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0572p1324d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0572p1328d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v22B8p6425d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0572p1329d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v1BBBp0003d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0421p042Dd*dc*dsc*dp*ic02isc02ipFF*");
MODULE_ALIAS("usb:v0421p04D8d*dc*dsc*dp*ic02isc02ipFF*");
MODULE_ALIAS("usb:v0421p04C9d*dc*dsc*dp*ic02isc02ipFF*");
MODULE_ALIAS("usb:v0421p0419d*dc*dsc*dp*ic02isc02ipFF*");
MODULE_ALIAS("usb:v0421p044Dd*dc*dsc*dp*ic02isc02ipFF*");
MODULE_ALIAS("usb:v0421p0001d*dc*dsc*dp*ic02isc02ipFF*");
MODULE_ALIAS("usb:v0421p0475d*dc*dsc*dp*ic02isc02ipFF*");
MODULE_ALIAS("usb:v0421p0508d*dc*dsc*dp*ic02isc02ipFF*");
MODULE_ALIAS("usb:v0421p0418d*dc*dsc*dp*ic02isc02ipFF*");
MODULE_ALIAS("usb:v0421p0425d*dc*dsc*dp*ic02isc02ipFF*");
MODULE_ALIAS("usb:v0421p0486d*dc*dsc*dp*ic02isc02ipFF*");
MODULE_ALIAS("usb:v0421p04DFd*dc*dsc*dp*ic02isc02ipFF*");
MODULE_ALIAS("usb:v0421p000Ed*dc*dsc*dp*ic02isc02ipFF*");
MODULE_ALIAS("usb:v0421p0445d*dc*dsc*dp*ic02isc02ipFF*");
MODULE_ALIAS("usb:v0421p042Fd*dc*dsc*dp*ic02isc02ipFF*");
MODULE_ALIAS("usb:v0421p048Ed*dc*dsc*dp*ic02isc02ipFF*");
MODULE_ALIAS("usb:v0421p0420d*dc*dsc*dp*ic02isc02ipFF*");
MODULE_ALIAS("usb:v0421p04E6d*dc*dsc*dp*ic02isc02ipFF*");
MODULE_ALIAS("usb:v0421p04B2d*dc*dsc*dp*ic02isc02ipFF*");
MODULE_ALIAS("usb:v0421p0134d*dc*dsc*dp*ic02isc02ipFF*");
MODULE_ALIAS("usb:v0421p046Ed*dc*dsc*dp*ic02isc02ipFF*");
MODULE_ALIAS("usb:v0421p002Fd*dc*dsc*dp*ic02isc02ipFF*");
MODULE_ALIAS("usb:v0421p0088d*dc*dsc*dp*ic02isc02ipFF*");
MODULE_ALIAS("usb:v0421p00FCd*dc*dsc*dp*ic02isc02ipFF*");
MODULE_ALIAS("usb:v0421p0042d*dc*dsc*dp*ic02isc02ipFF*");
MODULE_ALIAS("usb:v0421p00B0d*dc*dsc*dp*ic02isc02ipFF*");
MODULE_ALIAS("usb:v0421p00ABd*dc*dsc*dp*ic02isc02ipFF*");
MODULE_ALIAS("usb:v0421p0481d*dc*dsc*dp*ic02isc02ipFF*");
MODULE_ALIAS("usb:v0421p0007d*dc*dsc*dp*ic02isc02ipFF*");
MODULE_ALIAS("usb:v0421p0071d*dc*dsc*dp*ic02isc02ipFF*");
MODULE_ALIAS("usb:v0421p04F0d*dc*dsc*dp*ic02isc02ipFF*");
MODULE_ALIAS("usb:v0421p0070d*dc*dsc*dp*ic02isc02ipFF*");
MODULE_ALIAS("usb:v0421p00E9d*dc*dsc*dp*ic02isc02ipFF*");
MODULE_ALIAS("usb:v0421p0099d*dc*dsc*dp*ic02isc02ipFF*");
MODULE_ALIAS("usb:v0421p0128d*dc*dsc*dp*ic02isc02ipFF*");
MODULE_ALIAS("usb:v0421p008Fd*dc*dsc*dp*ic02isc02ipFF*");
MODULE_ALIAS("usb:v0421p00A0d*dc*dsc*dp*ic02isc02ipFF*");
MODULE_ALIAS("usb:v0421p007Bd*dc*dsc*dp*ic02isc02ipFF*");
MODULE_ALIAS("usb:v0421p0094d*dc*dsc*dp*ic02isc02ipFF*");
MODULE_ALIAS("usb:v0421p003Ad*dc*dsc*dp*ic02isc02ipFF*");
MODULE_ALIAS("usb:v0421p00E9d*dc*dsc*dp*ic02isc02ipFF*");
MODULE_ALIAS("usb:v0421p0108d*dc*dsc*dp*ic02isc02ipFF*");
MODULE_ALIAS("usb:v0421p01F5d*dc*dsc*dp*ic02isc02ipFF*");
MODULE_ALIAS("usb:v*p*d*dc*dsc*dp*ic02isc02ip01*");
MODULE_ALIAS("usb:v*p*d*dc*dsc*dp*ic02isc02ip02*");
MODULE_ALIAS("usb:v*p*d*dc*dsc*dp*ic02isc02ip03*");
MODULE_ALIAS("usb:v*p*d*dc*dsc*dp*ic02isc02ip04*");
MODULE_ALIAS("usb:v*p*d*dc*dsc*dp*ic02isc02ip05*");
MODULE_ALIAS("usb:v*p*d*dc*dsc*dp*ic02isc02ip06*");

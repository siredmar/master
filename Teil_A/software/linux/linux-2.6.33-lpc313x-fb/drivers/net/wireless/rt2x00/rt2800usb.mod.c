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
"depends=rt2x00usb,rt2800lib,crc-ccitt";

MODULE_ALIAS("usb:v07B8p2870d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v07B8p2770d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v07B8p3070d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v07B8p3071d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v07B8p3072d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v1482p3C09d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v1EDAp2310d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0E0Bp9031d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0E0Bp9041d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v15C5p0008d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v1690p0740d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v1690p0744d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0930p0A07d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0B05p1731d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0B05p1732d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0B05p1742d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0B05p1760d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0B05p1761d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0B05p1784d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v13D3p3247d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v13D3p3262d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v13D3p3273d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v13D3p3284d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v13D3p3305d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v050Dp8053d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v050Dp805Cd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v050Dp815Cd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v050Dp825Ad*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0411p00E8d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0411p012Ed*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v167Bp4001d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v14B2p3C06d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v14B2p3C07d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v14B2p3C08d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v14B2p3C09d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v14B2p3C11d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v14B2p3C12d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v14B2p3C23d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v14B2p3C25d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v14B2p3C27d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v14B2p3C28d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v07AAp002Fd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v07AAp003Cd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v07AAp003Fd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v07AAp0041d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v07AAp0042d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v18C5p0008d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v18C5p0012d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v07D1p3C09d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v07D1p3C0Ad*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v07D1p3C0Bd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v07D1p3C0Dd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v07D1p3C0Ed*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v07D1p3C0Fd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v07D1p3C11d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v07D1p3C13d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v07D1p3C15d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v7392p7711d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v7392p7717d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v7392p7718d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v203Dp1480d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v203Dp14A1d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v203Dp14A9d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v1740p9701d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v1740p9702d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v1740p9703d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v1740p9705d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v1740p9706d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v1740p9707d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v1740p9708d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v1740p9709d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v1740p9801d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v15A9p0010d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v1044p800Bd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v1044p800Cd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v1044p800Dd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0E66p0001d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0E66p0003d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0E66p0009d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0E66p000Bd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v04BBp0944d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v04BBp0945d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v04BBp0947d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v04BBp0948d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v1740p0605d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v1740p0615d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v1737p0070d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v1737p0071d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v1737p0077d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v1737p0079d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0789p0162d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0789p0163d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0789p0164d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v100Dp9031d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v100Dp9032d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0DB0p3820d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0DB0p3821d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0DB0p3870d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0DB0p6899d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0DB0p821Ad*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0DB0p870Ad*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0DB0p899Ad*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v1B75p3072d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v20B8p8888d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v1D4Dp0002d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v1D4Dp000Cd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v1D4Dp000Ed*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0471p200Fd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v2019pED06d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v2019pAB24d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v2019pAB25d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v18E8p6259d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v1A32p0304d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v148Fp2070d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v148Fp2770d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v148Fp2870d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v148Fp3070d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v148Fp3071d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v148Fp3072d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v148Fp3572d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v04E8p2018d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v129Bp1828d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0DF6p0017d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0DF6p002Bd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0DF6p002Cd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0DF6p002Dd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0DF6p0039d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0DF6p003Bd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0DF6p003Cd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0DF6p003Dd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0DF6p003Ed*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0DF6p003Fd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0DF6p0040d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0DF6p0041d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0DF6p0042d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0DF6p0047d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0DF6p0048d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0DF6p004Ad*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0DF6p004Dd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v083Ap6618d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v083Ap7511d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v083Ap7512d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v083Ap7522d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v083Ap8522d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v083ApA512d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v083ApA618d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v083ApA701d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v083ApA702d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v083ApB522d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v083ApC522d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v15A9p0006d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v177Fp0153d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v177Fp0302d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v177Fp0313d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v157Ep300Ed*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0CDEp0022d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0CDEp0025d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v5A57p0280d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v5A57p0282d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v5A57p0283d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v5A57p0284d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v5A57p5257d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0586p3416d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0586p341Ad*dc*dsc*dp*ic*isc*ip*");

MODULE_INFO(srcversion, "40D6826CF636D30263C15D9");

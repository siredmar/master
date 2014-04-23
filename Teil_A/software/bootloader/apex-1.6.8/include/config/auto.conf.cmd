deps_config := \
	src/net/Kconfig \
	src/drivers-lh/Kconfig \
	src/drivers/Kconfig \
	src/apex/Kconfig \
	src/mach-lpc313x/Kconfig \
	src/mach-orion5x/Kconfig \
	src/mach-mx3/Kconfig \
	src/mach-s3c2410/Kconfig \
	src/mach-ixp42x/Kconfig \
	src/mach-lh7a40x/Kconfig \
	src/mach-lh7952x/Kconfig \
	src/arch-arm/Kconfig

include/config/auto.conf: \
	$(deps_config)


$(deps_config): ;

cmd_src/arch-arm/entry/apex.lds := arm-linux-gnueabi-gcc -E -Wp,-MD,src/arch-arm/entry/.apex.lds.d  -nostdinc -isystem /opt/eldk-5.2.1/armv5te/sysroots/i686-eldk-linux/usr/lib/armv5te-linux-gnueabi/gcc/arm-linux-gnueabi/4.6.4/include -D__KERNEL__ -Iinclude  -include include/linux/autoconf.h   -P -C -Uarm -DAPEX_VMA=0x11029000 -DSTACK_SIZE=0x1000 -D__ASSEMBLY__ -o src/arch-arm/entry/apex.lds src/arch-arm/entry/apex.lds.S

deps_src/arch-arm/entry/apex.lds := \
  src/arch-arm/entry/apex.lds.S \
    $(wildcard include/config/interrupts.h) \
    $(wildcard include/config/mmu.h) \
    $(wildcard include/config/relocate/companion/evt1.h) \

src/arch-arm/entry/apex.lds: $(deps_src/arch-arm/entry/apex.lds)

$(deps_src/arch-arm/entry/apex.lds):

cmd_fs/nfsd/nfsxdr.o := arm-linux-gnueabi-gcc -Wp,-MD,fs/nfsd/.nfsxdr.o.d  -nostdinc -isystem /opt/eldk-5.2.1/armv5te/sysroots/i686-eldk-linux/usr/lib/armv5te-linux-gnueabi/gcc/arm-linux-gnueabi/4.6.4/include -I/home/armin/dev/gnublin/gnublin-lpc3131-2.6.33/linux-2.6.33-lpc313x/arch/arm/include -Iinclude  -include include/generated/autoconf.h -D__KERNEL__ -mlittle-endian -Iarch/arm/mach-lpc313x/include -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -Wno-format-security -fno-delete-null-pointer-checks -Os -marm -mabi=aapcs-linux -mno-thumb-interwork -funwind-tables -D__LINUX_ARM_ARCH__=5 -march=armv5te -mtune=arm9tdmi -msoft-float -Uarm -Wframe-larger-than=1024 -fno-stack-protector -fomit-frame-pointer -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -fno-dwarf2-cfi-asm -fconserve-stack  -DMODULE -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(nfsxdr)"  -D"KBUILD_MODNAME=KBUILD_STR(nfsd)"  -c -o fs/nfsd/nfsxdr.o fs/nfsd/nfsxdr.c

deps_fs/nfsd/nfsxdr.o := \
  fs/nfsd/nfsxdr.c \
  fs/nfsd/xdr.h \
  include/linux/vfs.h \
  include/linux/statfs.h \
  include/linux/types.h \
    $(wildcard include/config/uid16.h) \
    $(wildcard include/config/lbdaf.h) \
    $(wildcard include/config/phys/addr/t/64bit.h) \
    $(wildcard include/config/64bit.h) \
  /home/armin/dev/gnublin/gnublin-lpc3131-2.6.33/linux-2.6.33-lpc313x/arch/arm/include/asm/types.h \
  include/asm-generic/int-ll64.h \
  /home/armin/dev/gnublin/gnublin-lpc3131-2.6.33/linux-2.6.33-lpc313x/arch/arm/include/asm/bitsperlong.h \
  include/asm-generic/bitsperlong.h \
  include/linux/posix_types.h \
  include/linux/stddef.h \
  include/linux/compiler.h \
    $(wildcard include/config/trace/branch/profiling.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
  include/linux/compiler-gcc.h \
    $(wildcard include/config/arch/supports/optimized/inlining.h) \
    $(wildcard include/config/optimize/inlining.h) \
  include/linux/compiler-gcc4.h \
  /home/armin/dev/gnublin/gnublin-lpc3131-2.6.33/linux-2.6.33-lpc313x/arch/arm/include/asm/posix_types.h \
  /home/armin/dev/gnublin/gnublin-lpc3131-2.6.33/linux-2.6.33-lpc313x/arch/arm/include/asm/statfs.h \
  include/asm-generic/statfs.h \
  fs/nfsd/nfsd.h \
    $(wildcard include/config/nfsd/v2/acl.h) \
    $(wildcard include/config/nfsd/v3/acl.h) \
    $(wildcard include/config/nfsd/v4.h) \
  include/linux/mount.h \
    $(wildcard include/config/smp.h) \
  include/linux/list.h \
    $(wildcard include/config/debug/list.h) \
  include/linux/poison.h \
    $(wildcard include/config/illegal/pointer/value.h) \
  include/linux/prefetch.h \
  /home/armin/dev/gnublin/gnublin-lpc3131-2.6.33/linux-2.6.33-lpc313x/arch/arm/include/asm/processor.h \
    $(wildcard include/config/mmu.h) \
  /home/armin/dev/gnublin/gnublin-lpc3131-2.6.33/linux-2.6.33-lpc313x/arch/arm/include/asm/ptrace.h \
    $(wildcard include/config/cpu/endian/be8.h) \
    $(wildcard include/config/arm/thumb.h) \
  /home/armin/dev/gnublin/gnublin-lpc3131-2.6.33/linux-2.6.33-lpc313x/arch/arm/include/asm/hwcap.h \
  /home/armin/dev/gnublin/gnublin-lpc3131-2.6.33/linux-2.6.33-lpc313x/arch/arm/include/asm/cache.h \
    $(wildcard include/config/arm/l1/cache/shift.h) \
    $(wildcard include/config/aeabi.h) \
  /home/armin/dev/gnublin/gnublin-lpc3131-2.6.33/linux-2.6.33-lpc313x/arch/arm/include/asm/system.h \
    $(wildcard include/config/cpu/xsc3.h) \
    $(wildcard include/config/cpu/fa526.h) \
    $(wildcard include/config/cpu/sa1100.h) \
    $(wildcard include/config/cpu/sa110.h) \
    $(wildcard include/config/cpu/32v6k.h) \
  include/linux/linkage.h \
  /home/armin/dev/gnublin/gnublin-lpc3131-2.6.33/linux-2.6.33-lpc313x/arch/arm/include/asm/linkage.h \
  include/linux/irqflags.h \
    $(wildcard include/config/trace/irqflags.h) \
    $(wildcard include/config/irqsoff/tracer.h) \
    $(wildcard include/config/preempt/tracer.h) \
    $(wildcard include/config/trace/irqflags/support.h) \
  include/linux/typecheck.h \
  /home/armin/dev/gnublin/gnublin-lpc3131-2.6.33/linux-2.6.33-lpc313x/arch/arm/include/asm/irqflags.h \
  include/asm-generic/cmpxchg-local.h \
  include/asm-generic/cmpxchg.h \
  include/linux/nodemask.h \
    $(wildcard include/config/highmem.h) \
  include/linux/kernel.h \
    $(wildcard include/config/preempt/voluntary.h) \
    $(wildcard include/config/debug/spinlock/sleep.h) \
    $(wildcard include/config/prove/locking.h) \
    $(wildcard include/config/printk.h) \
    $(wildcard include/config/dynamic/debug.h) \
    $(wildcard include/config/ring/buffer.h) \
    $(wildcard include/config/tracing.h) \
    $(wildcard include/config/numa.h) \
    $(wildcard include/config/ftrace/mcount/record.h) \
  /opt/eldk-5.2.1/armv5te/sysroots/i686-eldk-linux/usr/lib/armv5te-linux-gnueabi/gcc/arm-linux-gnueabi/4.6.4/include/stdarg.h \
  include/linux/bitops.h \
    $(wildcard include/config/generic/find/first/bit.h) \
    $(wildcard include/config/generic/find/last/bit.h) \
    $(wildcard include/config/generic/find/next/bit.h) \
  /home/armin/dev/gnublin/gnublin-lpc3131-2.6.33/linux-2.6.33-lpc313x/arch/arm/include/asm/bitops.h \
  include/asm-generic/bitops/non-atomic.h \
  include/asm-generic/bitops/fls64.h \
  include/asm-generic/bitops/sched.h \
  include/asm-generic/bitops/hweight.h \
  include/asm-generic/bitops/lock.h \
  include/linux/log2.h \
    $(wildcard include/config/arch/has/ilog2/u32.h) \
    $(wildcard include/config/arch/has/ilog2/u64.h) \
  include/linux/dynamic_debug.h \
  /home/armin/dev/gnublin/gnublin-lpc3131-2.6.33/linux-2.6.33-lpc313x/arch/arm/include/asm/byteorder.h \
  include/linux/byteorder/little_endian.h \
  include/linux/swab.h \
  /home/armin/dev/gnublin/gnublin-lpc3131-2.6.33/linux-2.6.33-lpc313x/arch/arm/include/asm/swab.h \
  include/linux/byteorder/generic.h \
  /home/armin/dev/gnublin/gnublin-lpc3131-2.6.33/linux-2.6.33-lpc313x/arch/arm/include/asm/bug.h \
    $(wildcard include/config/bug.h) \
    $(wildcard include/config/debug/bugverbose.h) \
  include/asm-generic/bug.h \
    $(wildcard include/config/generic/bug.h) \
    $(wildcard include/config/generic/bug/relative/pointers.h) \
  /home/armin/dev/gnublin/gnublin-lpc3131-2.6.33/linux-2.6.33-lpc313x/arch/arm/include/asm/div64.h \
  include/linux/threads.h \
    $(wildcard include/config/nr/cpus.h) \
    $(wildcard include/config/base/small.h) \
  include/linux/bitmap.h \
  include/linux/string.h \
    $(wildcard include/config/binary/printf.h) \
  /home/armin/dev/gnublin/gnublin-lpc3131-2.6.33/linux-2.6.33-lpc313x/arch/arm/include/asm/string.h \
  include/linux/numa.h \
    $(wildcard include/config/nodes/shift.h) \
  include/linux/spinlock.h \
    $(wildcard include/config/debug/spinlock.h) \
    $(wildcard include/config/generic/lockbreak.h) \
    $(wildcard include/config/preempt.h) \
    $(wildcard include/config/debug/lock/alloc.h) \
  include/linux/preempt.h \
    $(wildcard include/config/debug/preempt.h) \
    $(wildcard include/config/preempt/notifiers.h) \
  include/linux/thread_info.h \
    $(wildcard include/config/compat.h) \
  /home/armin/dev/gnublin/gnublin-lpc3131-2.6.33/linux-2.6.33-lpc313x/arch/arm/include/asm/thread_info.h \
    $(wildcard include/config/arm/thumbee.h) \
  /home/armin/dev/gnublin/gnublin-lpc3131-2.6.33/linux-2.6.33-lpc313x/arch/arm/include/asm/fpstate.h \
    $(wildcard include/config/vfpv3.h) \
    $(wildcard include/config/iwmmxt.h) \
  /home/armin/dev/gnublin/gnublin-lpc3131-2.6.33/linux-2.6.33-lpc313x/arch/arm/include/asm/domain.h \
    $(wildcard include/config/io/36.h) \
  include/linux/stringify.h \
  include/linux/bottom_half.h \
  include/linux/spinlock_types.h \
  include/linux/spinlock_types_up.h \
  include/linux/lockdep.h \
    $(wildcard include/config/lockdep.h) \
    $(wildcard include/config/lock/stat.h) \
    $(wildcard include/config/generic/hardirqs.h) \
  include/linux/rwlock_types.h \
  include/linux/spinlock_up.h \
  include/linux/rwlock.h \
  include/linux/spinlock_api_up.h \
  /home/armin/dev/gnublin/gnublin-lpc3131-2.6.33/linux-2.6.33-lpc313x/arch/arm/include/asm/atomic.h \
  include/asm-generic/atomic-long.h \
  include/linux/nfsd/debug.h \
  include/linux/sunrpc/debug.h \
    $(wildcard include/config/sysctl.h) \
  include/linux/nfsd/export.h \
  include/linux/nfsd/nfsfh.h \
    $(wildcard include/config/nfsd/v3.h) \
  include/linux/sunrpc/svc.h \
    $(wildcard include/config/nfs/v4/1.h) \
  include/linux/in.h \
  include/linux/socket.h \
    $(wildcard include/config/proc/fs.h) \
  /home/armin/dev/gnublin/gnublin-lpc3131-2.6.33/linux-2.6.33-lpc313x/arch/arm/include/asm/socket.h \
  /home/armin/dev/gnublin/gnublin-lpc3131-2.6.33/linux-2.6.33-lpc313x/arch/arm/include/asm/sockios.h \
  include/linux/sockios.h \
  include/linux/uio.h \
  include/linux/in6.h \
  include/linux/sunrpc/types.h \
  include/linux/timer.h \
    $(wildcard include/config/timer/stats.h) \
    $(wildcard include/config/debug/objects/timers.h) \
  include/linux/ktime.h \
    $(wildcard include/config/ktime/scalar.h) \
  include/linux/time.h \
    $(wildcard include/config/arch/uses/gettimeoffset.h) \
  include/linux/cache.h \
    $(wildcard include/config/arch/has/cache/line/size.h) \
  include/linux/seqlock.h \
  include/linux/math64.h \
  include/linux/jiffies.h \
  include/linux/timex.h \
  include/linux/param.h \
  /home/armin/dev/gnublin/gnublin-lpc3131-2.6.33/linux-2.6.33-lpc313x/arch/arm/include/asm/param.h \
    $(wildcard include/config/hz.h) \
  /home/armin/dev/gnublin/gnublin-lpc3131-2.6.33/linux-2.6.33-lpc313x/arch/arm/include/asm/timex.h \
  arch/arm/mach-lpc313x/include/mach/timex.h \
  arch/arm/mach-lpc313x/include/mach/constants.h \
  include/linux/debugobjects.h \
    $(wildcard include/config/debug/objects.h) \
    $(wildcard include/config/debug/objects/free.h) \
  include/linux/workqueue.h \
    $(wildcard include/config/debug/objects/work.h) \
  include/linux/sunrpc/xdr.h \
  /home/armin/dev/gnublin/gnublin-lpc3131-2.6.33/linux-2.6.33-lpc313x/arch/arm/include/asm/unaligned.h \
  include/linux/unaligned/le_byteshift.h \
  include/linux/unaligned/be_byteshift.h \
  include/linux/unaligned/generic.h \
  include/linux/scatterlist.h \
    $(wildcard include/config/debug/sg.h) \
  /home/armin/dev/gnublin/gnublin-lpc3131-2.6.33/linux-2.6.33-lpc313x/arch/arm/include/asm/scatterlist.h \
  /home/armin/dev/gnublin/gnublin-lpc3131-2.6.33/linux-2.6.33-lpc313x/arch/arm/include/asm/memory.h \
    $(wildcard include/config/page/offset.h) \
    $(wildcard include/config/thumb2/kernel.h) \
    $(wildcard include/config/dram/size.h) \
    $(wildcard include/config/dram/base.h) \
    $(wildcard include/config/zone/dma.h) \
    $(wildcard include/config/discontigmem.h) \
  include/linux/const.h \
  arch/arm/mach-lpc313x/include/mach/memory.h \
  /home/armin/dev/gnublin/gnublin-lpc3131-2.6.33/linux-2.6.33-lpc313x/arch/arm/include/asm/sizes.h \
  include/asm-generic/memory_model.h \
    $(wildcard include/config/flatmem.h) \
    $(wildcard include/config/sparsemem/vmemmap.h) \
    $(wildcard include/config/sparsemem.h) \
  include/linux/mm.h \
    $(wildcard include/config/stack/growsup.h) \
    $(wildcard include/config/ksm.h) \
    $(wildcard include/config/arch/populates/node/map.h) \
    $(wildcard include/config/have/arch/early/pfn/to/nid.h) \
    $(wildcard include/config/ia64.h) \
    $(wildcard include/config/debug/pagealloc.h) \
    $(wildcard include/config/hibernation.h) \
  include/linux/errno.h \
  /home/armin/dev/gnublin/gnublin-lpc3131-2.6.33/linux-2.6.33-lpc313x/arch/arm/include/asm/errno.h \
  include/asm-generic/errno.h \
  include/asm-generic/errno-base.h \
  include/linux/gfp.h \
    $(wildcard include/config/kmemcheck.h) \
    $(wildcard include/config/zone/dma32.h) \
    $(wildcard include/config/debug/vm.h) \
  include/linux/mmzone.h \
    $(wildcard include/config/force/max/zoneorder.h) \
    $(wildcard include/config/memory/hotplug.h) \
    $(wildcard include/config/flat/node/mem/map.h) \
    $(wildcard include/config/cgroup/mem/res/ctlr.h) \
    $(wildcard include/config/have/memory/present.h) \
    $(wildcard include/config/need/node/memmap/size.h) \
    $(wildcard include/config/need/multiple/nodes.h) \
    $(wildcard include/config/sparsemem/extreme.h) \
    $(wildcard include/config/nodes/span/other/nodes.h) \
    $(wildcard include/config/holes/in/zone.h) \
    $(wildcard include/config/arch/has/holes/memorymodel.h) \
  include/linux/wait.h \
  /home/armin/dev/gnublin/gnublin-lpc3131-2.6.33/linux-2.6.33-lpc313x/arch/arm/include/asm/current.h \
  include/linux/init.h \
    $(wildcard include/config/modules.h) \
    $(wildcard include/config/hotplug.h) \
  include/linux/pageblock-flags.h \
    $(wildcard include/config/hugetlb/page.h) \
    $(wildcard include/config/hugetlb/page/size/variable.h) \
  include/generated/bounds.h \
  /home/armin/dev/gnublin/gnublin-lpc3131-2.6.33/linux-2.6.33-lpc313x/arch/arm/include/asm/page.h \
    $(wildcard include/config/cpu/copy/v3.h) \
    $(wildcard include/config/cpu/copy/v4wt.h) \
    $(wildcard include/config/cpu/copy/v4wb.h) \
    $(wildcard include/config/cpu/copy/feroceon.h) \
    $(wildcard include/config/cpu/copy/fa.h) \
    $(wildcard include/config/cpu/xscale.h) \
    $(wildcard include/config/cpu/copy/v6.h) \
  /home/armin/dev/gnublin/gnublin-lpc3131-2.6.33/linux-2.6.33-lpc313x/arch/arm/include/asm/glue.h \
    $(wildcard include/config/cpu/arm610.h) \
    $(wildcard include/config/cpu/arm710.h) \
    $(wildcard include/config/cpu/abrt/lv4t.h) \
    $(wildcard include/config/cpu/abrt/ev4.h) \
    $(wildcard include/config/cpu/abrt/ev4t.h) \
    $(wildcard include/config/cpu/abrt/ev5tj.h) \
    $(wildcard include/config/cpu/abrt/ev5t.h) \
    $(wildcard include/config/cpu/abrt/ev6.h) \
    $(wildcard include/config/cpu/abrt/ev7.h) \
    $(wildcard include/config/cpu/pabrt/legacy.h) \
    $(wildcard include/config/cpu/pabrt/v6.h) \
    $(wildcard include/config/cpu/pabrt/v7.h) \
  include/asm-generic/getorder.h \
  include/linux/memory_hotplug.h \
    $(wildcard include/config/have/arch/nodedata/extension.h) \
    $(wildcard include/config/memory/hotremove.h) \
  include/linux/notifier.h \
  include/linux/mutex.h \
    $(wildcard include/config/debug/mutexes.h) \
  include/linux/rwsem.h \
    $(wildcard include/config/rwsem/generic/spinlock.h) \
  include/linux/rwsem-spinlock.h \
  include/linux/srcu.h \
  include/linux/topology.h \
    $(wildcard include/config/sched/smt.h) \
    $(wildcard include/config/sched/mc.h) \
  include/linux/cpumask.h \
    $(wildcard include/config/cpumask/offstack.h) \
    $(wildcard include/config/hotplug/cpu.h) \
    $(wildcard include/config/debug/per/cpu/maps.h) \
    $(wildcard include/config/disable/obsolete/cpumask/functions.h) \
  include/linux/smp.h \
    $(wildcard include/config/use/generic/smp/helpers.h) \
  /home/armin/dev/gnublin/gnublin-lpc3131-2.6.33/linux-2.6.33-lpc313x/arch/arm/include/asm/topology.h \
  include/asm-generic/topology.h \
  include/linux/mmdebug.h \
    $(wildcard include/config/debug/virtual.h) \
  include/linux/rbtree.h \
  include/linux/prio_tree.h \
  include/linux/debug_locks.h \
    $(wildcard include/config/debug/locking/api/selftests.h) \
  include/linux/mm_types.h \
    $(wildcard include/config/split/ptlock/cpus.h) \
    $(wildcard include/config/want/page/debug/flags.h) \
    $(wildcard include/config/aio.h) \
    $(wildcard include/config/mm/owner.h) \
    $(wildcard include/config/mmu/notifier.h) \
  include/linux/auxvec.h \
  /home/armin/dev/gnublin/gnublin-lpc3131-2.6.33/linux-2.6.33-lpc313x/arch/arm/include/asm/auxvec.h \
  include/linux/completion.h \
  include/linux/page-debug-flags.h \
    $(wildcard include/config/page/poisoning.h) \
    $(wildcard include/config/page/debug/something/else.h) \
  /home/armin/dev/gnublin/gnublin-lpc3131-2.6.33/linux-2.6.33-lpc313x/arch/arm/include/asm/mmu.h \
    $(wildcard include/config/cpu/has/asid.h) \
  /home/armin/dev/gnublin/gnublin-lpc3131-2.6.33/linux-2.6.33-lpc313x/arch/arm/include/asm/pgtable.h \
    $(wildcard include/config/highpte.h) \
  include/asm-generic/4level-fixup.h \
  /home/armin/dev/gnublin/gnublin-lpc3131-2.6.33/linux-2.6.33-lpc313x/arch/arm/include/asm/proc-fns.h \
    $(wildcard include/config/cpu/arm7tdmi.h) \
    $(wildcard include/config/cpu/arm720t.h) \
    $(wildcard include/config/cpu/arm740t.h) \
    $(wildcard include/config/cpu/arm9tdmi.h) \
    $(wildcard include/config/cpu/arm920t.h) \
    $(wildcard include/config/cpu/arm922t.h) \
    $(wildcard include/config/cpu/arm925t.h) \
    $(wildcard include/config/cpu/arm926t.h) \
    $(wildcard include/config/cpu/arm940t.h) \
    $(wildcard include/config/cpu/arm946e.h) \
    $(wildcard include/config/cpu/arm1020.h) \
    $(wildcard include/config/cpu/arm1020e.h) \
    $(wildcard include/config/cpu/arm1022.h) \
    $(wildcard include/config/cpu/arm1026.h) \
    $(wildcard include/config/cpu/mohawk.h) \
    $(wildcard include/config/cpu/feroceon.h) \
    $(wildcard include/config/cpu/v6.h) \
    $(wildcard include/config/cpu/v7.h) \
  /home/armin/dev/gnublin/gnublin-lpc3131-2.6.33/linux-2.6.33-lpc313x/arch/arm/include/asm/cpu-single.h \
  arch/arm/mach-lpc313x/include/mach/vmalloc.h \
  /home/armin/dev/gnublin/gnublin-lpc3131-2.6.33/linux-2.6.33-lpc313x/arch/arm/include/asm/pgtable-hwdef.h \
  include/asm-generic/pgtable.h \
  include/linux/page-flags.h \
    $(wildcard include/config/pageflags/extended.h) \
    $(wildcard include/config/arch/uses/pg/uncached.h) \
    $(wildcard include/config/memory/failure.h) \
    $(wildcard include/config/swap.h) \
    $(wildcard include/config/s390.h) \
  include/linux/vmstat.h \
    $(wildcard include/config/vm/event/counters.h) \
  include/linux/percpu.h \
    $(wildcard include/config/need/per/cpu/embed/first/chunk.h) \
    $(wildcard include/config/need/per/cpu/page/first/chunk.h) \
    $(wildcard include/config/have/setup/per/cpu/area.h) \
  include/linux/slab.h \
    $(wildcard include/config/slab/debug.h) \
    $(wildcard include/config/slub.h) \
    $(wildcard include/config/slob.h) \
    $(wildcard include/config/debug/slab.h) \
  include/linux/slab_def.h \
  include/linux/kmemtrace.h \
    $(wildcard include/config/kmemtrace.h) \
  include/trace/events/kmem.h \
  include/linux/tracepoint.h \
    $(wildcard include/config/tracepoints.h) \
  include/linux/rcupdate.h \
    $(wildcard include/config/tree/rcu.h) \
    $(wildcard include/config/tree/preempt/rcu.h) \
    $(wildcard include/config/tiny/rcu.h) \
  include/linux/rcutiny.h \
    $(wildcard include/config/no/hz.h) \
  include/trace/define_trace.h \
    $(wildcard include/config/event/tracing.h) \
  include/linux/kmalloc_sizes.h \
  include/linux/pfn.h \
  /home/armin/dev/gnublin/gnublin-lpc3131-2.6.33/linux-2.6.33-lpc313x/arch/arm/include/asm/percpu.h \
  include/asm-generic/percpu.h \
  include/linux/percpu-defs.h \
    $(wildcard include/config/debug/force/weak/per/cpu.h) \
  /home/armin/dev/gnublin/gnublin-lpc3131-2.6.33/linux-2.6.33-lpc313x/arch/arm/include/asm/io.h \
  arch/arm/mach-lpc313x/include/mach/io.h \
  arch/arm/mach-lpc313x/include/mach/hardware.h \
  arch/arm/mach-lpc313x/include/mach/cgu.h \
    $(wildcard include/config/regs.h) \
    $(wildcard include/config/soft/reset.h) \
  arch/arm/mach-lpc313x/include/mach/registers.h \
    $(wildcard include/config/reg.h) \
    $(wildcard include/config/pec.h) \
    $(wildcard include/config/ecgc.h) \
    $(wildcard include/config/8bit/ecc.h) \
    $(wildcard include/config/tl/528.h) \
    $(wildcard include/config/tl/516.h) \
    $(wildcard include/config/tl/512.h) \
    $(wildcard include/config/tl/mask.h) \
    $(wildcard include/config/eo.h) \
    $(wildcard include/config/dc.h) \
    $(wildcard include/config/m.h) \
    $(wildcard include/config/lc/0.h) \
    $(wildcard include/config/lc/1.h) \
    $(wildcard include/config/lc/2.h) \
    $(wildcard include/config/lc/mask.h) \
    $(wildcard include/config/es.h) \
    $(wildcard include/config/de.h) \
    $(wildcard include/config/ao.h) \
    $(wildcard include/config/wd.h) \
    $(wildcard include/config/ec.h) \
    $(wildcard include/config/cfg/io/drive.h) \
    $(wildcard include/config/cfg/data/def.h) \
    $(wildcard include/config/cfg/cle/1.h) \
    $(wildcard include/config/cfg/ale/1.h) \
    $(wildcard include/config/cfg/we/1.h) \
    $(wildcard include/config/cfg/re/1.h) \
  include/linux/sunrpc/auth.h \
  include/linux/sunrpc/sched.h \
  include/linux/sunrpc/msg_prot.h \
  include/linux/inet.h \
  include/linux/sunrpc/svcauth.h \
  include/linux/sunrpc/cache.h \
  include/linux/proc_fs.h \
    $(wildcard include/config/proc/devicetree.h) \
    $(wildcard include/config/proc/kcore.h) \
  include/linux/fs.h \
    $(wildcard include/config/dnotify.h) \
    $(wildcard include/config/sysfs.h) \
    $(wildcard include/config/quota.h) \
    $(wildcard include/config/fsnotify.h) \
    $(wildcard include/config/inotify.h) \
    $(wildcard include/config/security.h) \
    $(wildcard include/config/fs/posix/acl.h) \
    $(wildcard include/config/epoll.h) \
    $(wildcard include/config/debug/writecount.h) \
    $(wildcard include/config/file/locking.h) \
    $(wildcard include/config/auditsyscall.h) \
    $(wildcard include/config/block.h) \
    $(wildcard include/config/fs/xip.h) \
    $(wildcard include/config/migration.h) \
  include/linux/limits.h \
  include/linux/ioctl.h \
  /home/armin/dev/gnublin/gnublin-lpc3131-2.6.33/linux-2.6.33-lpc313x/arch/arm/include/asm/ioctl.h \
  include/asm-generic/ioctl.h \
  include/linux/kdev_t.h \
  include/linux/dcache.h \
  include/linux/rculist.h \
  include/linux/path.h \
  include/linux/stat.h \
  /home/armin/dev/gnublin/gnublin-lpc3131-2.6.33/linux-2.6.33-lpc313x/arch/arm/include/asm/stat.h \
  include/linux/kobject.h \
  include/linux/sysfs.h \
  include/linux/kref.h \
  include/linux/radix-tree.h \
  include/linux/pid.h \
  include/linux/capability.h \
  include/linux/semaphore.h \
  include/linux/fiemap.h \
  include/linux/quota.h \
    $(wildcard include/config/quota/netlink/interface.h) \
  include/linux/dqblk_xfs.h \
  include/linux/dqblk_v1.h \
  include/linux/dqblk_v2.h \
  include/linux/dqblk_qtree.h \
  include/linux/nfs_fs_i.h \
  include/linux/nfs.h \
  include/linux/fcntl.h \
  /home/armin/dev/gnublin/gnublin-lpc3131-2.6.33/linux-2.6.33-lpc313x/arch/arm/include/asm/fcntl.h \
  include/asm-generic/fcntl.h \
  include/linux/err.h \
  include/linux/magic.h \
  include/linux/hash.h \
  include/linux/nfsd/const.h \
  include/linux/nfs2.h \
  include/linux/nfs3.h \
  include/linux/nfs4.h \
  include/linux/nfsd/stats.h \
  fs/nfsd/nfsfh.h \
  fs/nfsd/auth.h \

fs/nfsd/nfsxdr.o: $(deps_fs/nfsd/nfsxdr.o)

$(deps_fs/nfsd/nfsxdr.o):
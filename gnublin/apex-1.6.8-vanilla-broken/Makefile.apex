# -*- Makefile -*-
# Makefile.apex, extra APEX specific targets for top-level directory of APEX
#

include/mach: FORCE
	$(Q)set -e; machlink=`readlink include/mach | cut -d '-' -f 2`;   \
	if [ -L include/mach ]; then                                     \
		if [ "$$machlink" != "$(CONFIG_MACH)" ]; then                \
			echo "ERROR: the symlink $@ points to mach-$$machlink but mach-$(CONFIG_MACH) was expected"; \
			exit 1;                                         \
		fi;                                                     \
	else                                                            \
		echo '  SYMLINK $@ -> include/mach-$(CONFIG_MACH)';     \
		if [ ! -d include ]; then                               \
			mkdir -p include;                               \
		fi;                                                     \
		ln -fsn ../src/mach-$(CONFIG_MACH) $@;                  \
	fi


#include/mach: include/.mach FORCE
#include/.mach: include/config/MARKER
#	@echo  '  SYMLINK include/mach -> src/mach-$(CONFIG_MACH)'
#	@ln -fsn ../src/mach-$(CONFIG_MACH) include/mach
#	@touch include/.mach


# every target builds all of the available configurations
# NOTE: We drop building debian configs because we use this target to
#       build viable binaries and forcing the compiler to ARM
#       littleendian won't build vaild binaries.  The debian configs
#       don't include an explicit compiler reference.

PHONY += every
every: #distclean
	@if [ -e .config ]; then rm .config ; fi
	@[ ! -d every ] || rm -rf every
	@mkdir every
	@for i in `find src/mach-*/ \
		   -name '*_config' -printf ' %f'` ; do \
	 o=apex-$(APEXVERSION)-`echo $$i | sed -s 's/_config//'`; \
	 [ ! -f makelog ] || rm makelog ;\
	 printf "  BUILD   %-32s ($$o)\n" $$i;\
	 $(MAKE) clean                       >> makelog 2>&1 || exit 1 ; \
	 $(MAKE) $$i                         >> makelog 2>&1 || exit 1 ; \
	 $(MAKE) oldconfig       < /dev/null >> makelog 2>&1 || exit 1 ; \
	 $(MAKE) `scripts/overrides $$i` \
	   apex.srec apex.elf apex.bin       >> makelog 2>&1 || exit 1 ; \
	 mkdir every/$$o ; \
	 mv src/arch-arm/rom/apex.{elf,bin,srec} makelog every/$$o ; \
	 cp .config every/$$o/config ; \
	 echo $(APEXVERSION) > every/$$o/APEXVERSION ; \
	 cp docs/Every every/$$o/README ; \
	 (cd every ; zip -rq $$o.zip $$o ) ; \
	done
	@rm .config

# every target builds all of the available configurations
PHONY += update-every
update-every:
	@for i in `find src/mach-*/ -name '*_config'` ; do \
	echo -- $$i;\
	cp $$i .config; \
	$(MAKE) oldconfig; \
	cp .config $$i; \
	done

PHONY += release-every
release-every: every
	mkdir -p ~ftp/pub/apex/apex-$(APEXVERSION)
	cp every/*.zip ~ftp/pub/apex/apex-$(APEXVERSION)


#PHONY += tgz
#tgz: FORCE
#	$(Q)$(MAKE) -f $(package-dir)/Makefile $@

PKGBASE := apex-$(APEXVERSION)

PHONY += tgz.pkg
tgz.pkg tgz:
ifneq "$(wildcard $(PKGBASE))" ""
	-rm -rf $(PKGBASE)
endif
	@git-clone -l ./ $(PKGBASE)
	find $(PKGBASE) -depth -type d -name '_intel' -exec rm -rf \{\} \;
	rm -rf $(PKGBASE)/include/splash
	tar zc\
	 --exclude='.git'\
	 --exclude='cc-png.h'\
	 --exclude='pcm*-*.h'\
	 --exclude='colorbars*.png'\
	 -f $(PKGBASE).tar.gz $(PKGBASE)
	@rm -rf $(PKGBASE)

PHONY += complete-release
complete-release: tgz release-every FORCE
	cp apex-$(APEXVERSION).tar.gz ~ftp/pub/apex
	git-tag -a $(APEXVERSION) -m "Release $(APEXVERSION)"
	-(cd ~ftp/pub/apex ; rm LATEST_IS* ; touch LATEST_IS_$(APEXVERSION) )


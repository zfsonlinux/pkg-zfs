initrddir = $(datarootdir)/initramfs-tools

initrd_SCRIPTS = conf-hooks.d/zfs hooks/zfs scripts/zfs

EXTRA_DIST = \
	$(top_srcdir)/initramfs/conf-hooks.d/zfs \
	$(top_srcdir)/initramfs/hooks/zfs \
	$(top_srcdir)/initramfs/scripts/zfs

install-initrd_SCRIPTS: $(EXTRA_DIST)
	for d in conf-hooks.d hooks scripts; do \
	  $(MKDIR_P) $(DESTDIR)$(initrddir)/$$d; \
	  cp $(top_srcdir)/initramfs/$$d/zfs \
	    $(DESTDIR)$(initrddir)/$$d/; \
	done
	if [ -f etc/init.d/zfs ]; then \
	  $(MKDIR_P) $(DESTDIR)$(DEFAULT_INITCONF_DIR); \
	  cp $(top_srcdir)/etc/init.d/zfs \
	    $(DESTDIR)$(DEFAULT_INITCONF_DIR)/; \
	fi

distclean-local::
	-$(RM) $(top_srcdir)/initramfs/scripts/zfs

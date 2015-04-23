initrddir = $(datarootdir)/initramfs-tools

initrd_SCRIPTS = \
	$(top_srcdir)/scripts/zfs-initramfs/conf-hooks.d/zfs \
	$(top_srcdir)/scripts/zfs-initramfs/hooks/zfs \
	$(top_srcdir)/scripts/zfs-initramfs/scripts/zfs

EXTRA_DIST = $(initrd_SCRIPTS)

install-initrdSCRIPTS:
	for d in conf-hooks.d hooks scripts; do \
	  $(MKDIR_P) $(DESTDIR)$(initrddir)/$$d; \
	  cp $(top_srcdir)/scripts/zfs-initramfs/$$d/zfs \
	    $(DESTDIR)$(initrddir)/$$d/; \
	done

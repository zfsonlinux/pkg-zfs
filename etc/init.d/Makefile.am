initdir = $(DEFAULT_INIT_DIR)
init_SCRIPTS = zfs-common zfs-mount zfs-share

$(init_SCRIPTS):
	-(if [ -e /etc/debian_version ]; then \
		NFS_SRV=nfs-kernel-server; \
	  else \
		NFS_SRV=nfs; \
	  fi; \
	  $(SED) -e 's,@bindir\@,$(bindir),g' \
		-e 's,@sbindir\@,$(sbindir),g' \
		-e 's,@udevdir\@,$(udevdir),g' \
		-e 's,@udevruledir\@,$(udevruledir),g' \
		-e 's,@sysconfdir\@,$(sysconfdir),g' \
		-e 's,@initdir\@,$(initdir),g' \
		-e 's,@runstatedir\@,$(runstatedir),g' \
		'$@.$(DEFAULT_INIT_SCRIPT).in' >'$@'

distclean-local::
	-$(RM) $(init_SCRIPTS)

dnl #
dnl # 2.6.36 API change
dnl # BIO_RW_SYNCIO renamed to REQ_SYNC
dnl #
AC_DEFUN([ZFS_AC_KERNEL_REQ_SYNC], [
	AC_MSG_CHECKING([whether REQ_SYNC is defined])
	ZFS_LINUX_TRY_COMPILE([
		#include <linux/bio.h>
	],[
		int flags;
		flags = REQ_SYNC;
	],[
		AC_MSG_RESULT(yes)
		AC_DEFINE(HAVE_REQ_SYNC, 1,
		          [REQ_SYNC is defined])
	],[
		AC_MSG_RESULT(no)
	])
])
